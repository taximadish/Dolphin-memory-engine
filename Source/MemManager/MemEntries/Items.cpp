#include "Items.h"

#define MAX_ITEMS			(20)
#define FIRST_ITEM_OFFSET	(0x192)
#define ITEM_SIZE_BYTES		(2)

Items::Items(bool serverMode)
{
  if (serverMode)
  {
    m_hostValue = "0";
  }
  else
  {
	  for (int i = 0; i < MAX_ITEMS; i++)
	  {
		MemWatchEntry* watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);
		watch->setBoundToPointer(true);
		watch->addOffset(FIRST_ITEM_OFFSET + (i * ITEM_SIZE_BYTES));

		m_watches.push_back(watch);
	  }
  
	  m_pausedWatch = new MemWatchEntry("Paused", 0x8041E67b, Common::MemType::type_byte);
  }
}

std::string Items::Name()
{
  return "Items";
}

std::string Items::setValue(std::string value)
{
  if (IsPaused())
    return COULD_NOT_SET;

  std::vector<std::string> parts = customSplit(value, ",");
  for (int i = 0; i < MAX_ITEMS; i++)
  {
    if (i < parts.size())
    {
      m_watches[i]->writeMemoryFromString(parts[i]);
    }
	else
	{
      m_watches[i]->writeMemoryFromString("0");
	}
  }
  return value;
}

std::string Items::hostGetValue()
{
  return m_hostValue;
}

std::string Items::getUpdate(std::string hostVal)
{
  std::map<std::string, int8_t> hostCounts = itemCounts(hostVal);

  std::string myItems = "";
  for (int i = 0; i < MAX_ITEMS; i++)
  {
    std::string item = m_watches[i]->getStringFromMemory();
    if (item == "0")
      break;
    myItems.append(item + ",");
  }

  myItems.append("0");

  std::map<std::string, int8_t> myCounts = itemCounts(myItems);

  std::string diffs = "";
  // Host Items
  for (std::map<std::string, int8_t>::iterator it = hostCounts.begin(); it != hostCounts.end(); ++it)
  {
    int8_t diff = myCounts[it->first] - it->second;
	
	if (diff != 0)
      diffs.append(it->first + "~" + std::to_string(diff) + ",");
  }

  // My new items (items that aren't in host inventory, so aren't checked by previous loop)
  for (std::map<std::string, int8_t>::iterator it = myCounts.begin(); it != myCounts.end(); ++it)
  {
    int8_t hostCount = hostCounts[it->first];
    if (hostCount == 0)
	{
		diffs.append(it->first + "~" + std::to_string(it->second) + ",");
	}
  }

  if (diffs == "")
    diffs = NO_UPDATE;
  else
    diffs = diffs.substr(0, diffs.length() - 1); // remove trailing comma

  return diffs;
}

void Items::hostHandleUpdate(std::string updateString)
{
  std::map<std::string, int8_t> diffs;
  std::vector<std::string> updates = customSplit(updateString, ",");
  for (int i = 0; i < updates.size(); i++)
  {
    std::vector<std::string> parts = customSplit(updates[i], "~");
    diffs[parts[0]] = atoi(parts[1].c_str());
  }

  std::vector<std::string> currentItems = customSplit(hostGetValue(), ",");

  std::string newItems = "";
  for (int i = 0; i < currentItems.size(); i++) // Keep or discard current items appropriately
  {
    std::string item = currentItems[i];
    if (item == "0")
      break;

    if (diffs[item] < 0)
	{
      diffs[item]++;
	}
	else
	{
      newItems.append(item + ",");
	}
  }

  // Add any new items to end
  for (std::map<std::string, int8_t>::iterator it = diffs.begin(); it != diffs.end(); ++it)
  {
	for (int i = 0; i < it->second; i++)
	{
	  newItems.append(it->first + ",");
	}
  }

 newItems.append("0");
 m_hostValue = newItems;
}


std::vector<std::string> Items::customSplit(std::string s, std::string delim)
{
  std::vector<std::string> parts = {};
  std::string delimiter = delim;

  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    parts.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  if (s.length() > 0)
  {
    parts.push_back(s);
  }
  return parts;
}

std::map<std::string, int8_t> Items::itemCounts(std::string itemsString)
{
  std::vector<std::string> items = Items::customSplit(itemsString, ",");
  
  std::map<std::string, int8_t> itemCounts;
  for (int i = 0; i < items.size(); i++)
  {
    if (items[i] != "0")
      itemCounts[items[i]]++;
  }
  return itemCounts;
}

bool Items::IsPaused()
{
  std::string paused = m_pausedWatch->getStringFromMemory();
  return paused == "1";
}