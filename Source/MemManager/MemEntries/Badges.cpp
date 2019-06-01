#include "Badges.h"

#define MAX_BADGES			(200)
#define FIRST_BADGE_OFFSET	(0x1FA)
#define BADGE_SIZE_BYTES		(2)

Badges::Badges(bool serverMode)
{
  if (serverMode)
  {
    m_hostValue = "0";
  }
  else
  {
	  for (int i = 0; i < MAX_BADGES; i++)
	  {
		MemWatchEntry* watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);
		watch->setBoundToPointer(true);
		watch->addOffset(FIRST_BADGE_OFFSET + (i * BADGE_SIZE_BYTES));

		m_watches.push_back(watch);
	  }
  
	  m_pausedWatch = new MemWatchEntry("Paused", 0x8041E67b, Common::MemType::type_byte);
  }
}

std::string Badges::Name()
{
  return "Badges";
}

std::string Badges::setValue(std::string value)
{
  if (IsPaused())
    return COULD_NOT_SET;

  std::map<std::string, int8_t> hostCounts = badgeCounts(value);

  std::string myNewBadges = "";
  for (int i = 0; i < MAX_BADGES; i++)
  {
    std::string badge = m_watches[i]->getStringFromMemory();

    if (badge == "0")
    {
      for (std::map<std::string, int8_t>::iterator it = hostCounts.begin(); it != hostCounts.end(); ++it)
      {
        if (it->second > 0)
        {
          myNewBadges.append(it->first + ",");
          hostCounts[it->first]--;
          break;
        }
      }
    }
    else
    {
      if (hostCounts[badge] > 0)
      {
        myNewBadges.append(badge + ",");
        hostCounts[badge]--;
      }
    }
  }

  myNewBadges.append("0");

  // Set new badges
  std::vector<std::string> parts = customSplit(myNewBadges, ",");
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
  return value;
}

std::string Badges::hostGetValue()
{
  return m_hostValue;
}

std::string Badges::getUpdate(std::string hostVal)
{
  std::map<std::string, int8_t> hostCounts = badgeCounts(hostVal);
  
  std::string myBadges = "";
  for (int i = 0; i < MAX_BADGES; i++)
  {
    std::string badge = m_watches[i]->getStringFromMemory();
    if (badge == "0")
      break;
    myBadges.append(badge + ",");
  }

  myBadges.append("0");

  std::map<std::string, int8_t> myCounts = badgeCounts(myBadges);

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

void Badges::hostHandleUpdate(int id, std::string updateString)
{
  std::map<std::string, int8_t> diffs;
  std::vector<std::string> updates = customSplit(updateString, ",");
  for (int i = 0; i < updates.size(); i++)
  {
    std::vector<std::string> parts = customSplit(updates[i], "~");
    diffs[parts[0]] = atoi(parts[1].c_str());
  }

  std::vector<std::string> currentBadges = customSplit(hostGetValue(), ",");

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
  m_hostValue = newBadges;
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

bool Badges::IsPaused()
{
  std::string paused = m_pausedWatch->getStringFromMemory();
  return paused == "1";
}