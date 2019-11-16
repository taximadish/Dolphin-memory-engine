#include "XP.h"

XP::XP(bool serverMode)
{
  if (serverMode)
  {
	  m_hostValue = "0,0,0,0,0,0,0,0"; // Start at level 1, no xp
  }
  else
  {
	// Reserve Watches
    for (int i = 0; i < NUM_CHARACTERS; i++)
    {
      int char_base_addr = BASE_RESERVE_ADDRESS + (i * CHARACTER_SIZE);
      m_ReserveXpUpToLevelWatches.push_back(new MemWatchEntry(
		  "XP Up To Level", char_base_addr + XP_UPTOLEVEL_OFFSET, Common::MemType::type_word
	  ));

      m_ReserveXpPastLevelWatches.push_back(new MemWatchEntry(
		  "XP Past Level", char_base_addr + XP_PASTLEVEL_OFFSET, Common::MemType::type_word
	  ));
	}

	// Slot Watches
    for (int i = 0; i < 3; i++)
    {
      m_SlotWatches.push_back(new MemWatchEntry(
		  "Character Slot", BASE_SLOT_ADDR + (i * SLOT_SIZE), Common::MemType::type_word
	  ));
	}

	// Party Watches
    MemWatchEntry* party1Xp = new MemWatchEntry("Party 1 XP", BASE_PARTY_PTR, Common::MemType::type_word);
    party1Xp->setBoundToPointer(true);
    party1Xp->addOffset(0x1604);
    m_PartyXpWatches.push_back(party1Xp);

    MemWatchEntry* party2Xp = new MemWatchEntry("Party 2 XP", BASE_PARTY_PTR, Common::MemType::type_word);
    party2Xp->setBoundToPointer(true);
    party2Xp->addOffset(0x654C);
    m_PartyXpWatches.push_back(party2Xp);

    MemWatchEntry* party3Xp = new MemWatchEntry("Party 3 XP", BASE_PARTY_PTR, Common::MemType::type_word);
    party3Xp->setBoundToPointer(true);
    party3Xp->addOffset(0xAED4);
    m_PartyXpWatches.push_back(party3Xp);
  }
}

std::string XP::Name()
{
  return "XP";
}

std::string XP::setValue(std::string value)
{
  if (m_PartyXpWatches[0]->getStringFromMemory() == "???")
  {
    return COULD_NOT_SET; // Can't set safely since we're likely mid-load
  }

  std::vector<std::string> xpCounts = customSplit(value, ",");
  std::string newValues = "";

  for (int xpCountNum = 0; xpCountNum < 8; xpCountNum++)
  {
    int targetXp = atoi(xpCounts[xpCountNum].c_str());

    int xpToLevel = atoi(m_ReserveXpUpToLevelWatches[xpCountNum]->getStringFromMemory().c_str());
    if (xpToLevel == 0)
    {
      newValues.append("0,");
      continue; // We haven't gotten this party member yet, skip since this _WOULD_ triple party member starting level
	}

	int newXpPastLevel = targetXp - xpToLevel;
    if (newXpPastLevel < 0)
    {
      int xpPastLevel = atoi(m_ReserveXpPastLevelWatches[xpCountNum]->getStringFromMemory().c_str());
      newValues.append(std::to_string(xpPastLevel + xpToLevel) + ",");
	  continue; // This shouldn't really happen after initial sync, but just in case
	}

	m_ReserveXpPastLevelWatches[xpCountNum]->writeMemoryFromString(std::to_string(newXpPastLevel));
	newValues.append(std::to_string(newXpPastLevel + xpToLevel) + ",");

    int charNum = xpCountNum + 1;
	for (int partySlot = 0; partySlot < 3; partySlot++)
    {
	  if (m_SlotWatches[partySlot]->getStringFromMemory() == std::to_string(charNum))
	  {
        m_PartyXpWatches[partySlot]->writeMemoryFromString(std::to_string(newXpPastLevel));
      }
    }
  }

  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma

  return newValues;
}

std::string XP::hostGetValue()
{
  return m_hostValue;
}

std::string XP::getUpdate(std::string hostVal)
{
  std::vector<std::string> xpTotals = customSplit(hostVal, ",");

  std::string updates = "";

  for (int i = 0; i < 8; i++)
  {
    int hostXp = atoi(xpTotals[i].c_str());

    int xpToLevel = atoi(m_ReserveXpUpToLevelWatches[i]->getStringFromMemory().c_str());
    if (xpToLevel == 0)
    {
      // We haven't gotten this party member yet, don't update
      updates.append("0,");
      continue;
    }

	int xpPastLevel = atoi(m_ReserveXpPastLevelWatches[i]->getStringFromMemory().c_str());

    int myTotal = xpToLevel + xpPastLevel;

	int xpAdded = myTotal - hostXp;

    if (xpAdded < 0)
    {
      // This can happen, but should be ignored to avoid decreasing total xp
      updates.append("0,");
      continue;
    }

	updates.append(std::to_string(xpAdded) + ",");
  }

  updates = updates.substr(0, updates.length() - 1); // remove trailing comma
  return updates;
}

void XP::hostHandleUpdate(int id, std::string updateString)
{
  std::vector<std::string> xpUpdates = customSplit(updateString, ",");
  std::vector<std::string> xpTotals = customSplit(m_hostValue, ",");

  std::string newValues = "";
  for (int i = 0; i < 8; i++)
  {
    int addedXp = atoi(xpUpdates[i].c_str());
    int currentTotal = atoi(xpTotals[i].c_str());
    newValues.append(std::to_string(currentTotal + addedXp) + ",");
  }

  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma

  m_hostValue = newValues;
}


std::vector<std::string> XP::customSplit(std::string s, std::string delim)
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