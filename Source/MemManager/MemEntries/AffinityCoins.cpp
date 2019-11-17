#include "AffinityCoins.h"

AffinityCoins::AffinityCoins(bool serverMode)
{
  if (serverMode)
  {
	  m_hostValue = "0,0,0,0,0,0,0,0";
  }
  else
  {
	// Reserve Watches
    for (int i = 0; i < NUM_CHARACTERS; i++)
    {
      int char_base_addr = BASE_RESERVE_ADDRESS + (i * CHARACTER_SIZE);
      m_AffinityCoinsWatches.push_back(new MemWatchEntry(
		  "Affinity Coins", char_base_addr + AFFINITYCOINS_OFFSET, Common::MemType::type_word
	  ));
	}
  }
}

std::string AffinityCoins::Name()
{
  return "AffinityCoins";
}

std::string AffinityCoins::setValue(std::string value)
{
  std::vector<std::string> hostAffinityCoins = customSplit(value, ",");
  std::string newValues = "";

  for (int i = 0; i < NUM_CHARACTERS; i++)
  {
    int myAffinityCoins = atoi(m_AffinityCoinsWatches[i]->getStringFromMemory().c_str());
    if (myAffinityCoins == 0)
    {
      newValues.append("0,");
      continue; // We haven't gotten this party member yet, skip since this _WOULD_ triple party member starting level
	}

	int hostValue = atoi(hostAffinityCoins[i].c_str());

	if (hostValue < myAffinityCoins)
    {
      newValues.append(std::to_string(myAffinityCoins) + ","); // Don't ever decrease affinity coins
      continue;
	}

	m_AffinityCoinsWatches[i]->writeMemoryFromString(std::to_string(hostValue));
    newValues.append(hostAffinityCoins[i] + ",");
  }

  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma

  return newValues;
}

std::string AffinityCoins::hostGetValue()
{
  return m_hostValue;
}

std::string AffinityCoins::getUpdate(std::string hostVal)
{
  std::vector<std::string> hostAffinityCoins = customSplit(hostVal, ",");

  std::string updates = "";

  for (int i = 0; i < NUM_CHARACTERS; i++)
  {
    int hostValue = atoi(hostAffinityCoins[i].c_str());

    int myAffinityCoins = atoi(m_AffinityCoinsWatches[i]->getStringFromMemory().c_str());
    if (myAffinityCoins == 0)
    {
      // We haven't gotten this party member yet, don't update
      updates.append("0,");
      continue;
    }

	updates.append(std::to_string(myAffinityCoins - hostValue) + ",");
  }

  updates = updates.substr(0, updates.length() - 1); // remove trailing comma
  return updates;
}

void AffinityCoins::hostHandleUpdate(int id, std::string updateString)
{
  std::vector<std::string> AffinityCoinsUpdates = customSplit(updateString, ",");
  std::vector<std::string> AffinityCoinsTotals = customSplit(m_hostValue, ",");

  std::string newValues = "";
  for (int i = 0; i < NUM_CHARACTERS; i++)
  {
    int addedAffinityCoins = atoi(AffinityCoinsUpdates[i].c_str());
    int currentTotal = atoi(AffinityCoinsTotals[i].c_str());
    newValues.append(std::to_string(currentTotal + addedAffinityCoins) + ",");
  }

  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma

  m_hostValue = newValues;
}


std::vector<std::string> AffinityCoins::customSplit(std::string s, std::string delim)
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