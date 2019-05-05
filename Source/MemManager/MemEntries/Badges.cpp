#include "Badges.h"

#define MAX_BADGES			(200)
#define FIRST_BADGE_OFFSET	(0x1FA)
#define BADGE_SIZE_BYTES		(2)

Badges::Badges()
{ 
  for (int i = 0; i < MAX_BADGES; i++)
  {
    MemWatchEntry* watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);
    watch->setBoundToPointer(true);
    watch->addOffset(FIRST_BADGE_OFFSET + (i * BADGE_SIZE_BYTES));

	m_watches.push_back(watch);
  }
}

std::string Badges::Name()
{
  return "Badges";
}

void Badges::setValue(std::string value)
{
  std::vector<std::string> parts = customSplit(value, ",");
  for (int i = 0; i < MAX_BADGES; i++)
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
}

std::string Badges::getValue()
{
  std::string value = "";
  for (int i = 0; i < MAX_BADGES; i++)
  {
    std::string item = m_watches[i]->getStringFromMemory();
    if (item == "0")
      break;
    value.append(item+",");
  }

  value.append("0");

  return value;
}

std::string Badges::getUpdate(std::string hostVal)
{
  std::map<std::string, int8_t> hostCounts = badgeCounts(hostVal);
  std::map<std::string, int8_t> myCounts = badgeCounts(getValue());

  std::string diffs = "";
  // Host Badges
  for (std::map<std::string, int8_t>::iterator it = hostCounts.begin(); it != hostCounts.end(); ++it)
  {
    int8_t diff = myCounts[it->first] - it->second;
	
	if (diff != 0)
      diffs.append(it->first + "~" + std::to_string(diff) + ",");
  }

  // My new badges (badges that aren't in host inventory, so aren't checked by previous loop)
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

void Badges::handleUpdate(std::string updateString)
{
  std::map<std::string, int8_t> diffs;
  std::vector<std::string> updates = customSplit(updateString, ",");
  for (int i = 0; i < updates.size(); i++)
  {
    std::vector<std::string> parts = customSplit(updates[i], "~");
    diffs[parts[0]] = atoi(parts[1].c_str());
  }

  std::vector<std::string> currentBadges = customSplit(getValue(), ",");

  std::string newBadges = "";
  for (int i = 0; i < currentBadges.size(); i++) // Keep or discard current badges appropriately
  {
    std::string item = currentBadges[i];
    if (item == "0")
      break;

    if (diffs[item] < 0)
	{
      diffs[item]++;
	}
	else
	{
          newBadges.append(item + ",");
	}
  }

  // Add any new badges to end
  for (std::map<std::string, int8_t>::iterator it = diffs.begin(); it != diffs.end(); ++it)
  {
	for (int i = 0; i < it->second; i++)
	{
          newBadges.append(it->first + ",");
	}
  }

 newBadges.append("0");
  setValue(newBadges);
}


std::vector<std::string> Badges::customSplit(std::string s, std::string delim)
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

std::map<std::string, int8_t> Badges::badgeCounts(std::string badgesString)
{
  std::vector<std::string> badges = Badges::customSplit(badgesString, ",");
  
  std::map<std::string, int8_t> itemCounts;
  for (int i = 0; i < badges.size(); i++)
  {
    if (badges[i] != "0")
      itemCounts[badges[i]]++;
  }
  return itemCounts;
}