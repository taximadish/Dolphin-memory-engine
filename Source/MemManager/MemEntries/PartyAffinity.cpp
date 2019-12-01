#include "PartyAffinity.h"

PartyAffinity::PartyAffinity(bool serverMode)
{
  if (serverMode)
  {
	m_hostValue = "";
	for (int i = 0; i < NUM_AFFINITIES; i++)
	{
	  m_hostValue += "0,";
	}

    m_hostValue = m_hostValue.substr(0, m_hostValue.length() - 1); // remove trailing comma
  }
  else
  {
	// Set Up Watches
    for (int i = 0; i < NUM_AFFINITIES; i++)
    {
      m_PartyAffinityWatches.push_back(new MemWatchEntry(
		  "Party Affinity", BASE_AFFINITIES_ADDR + (i * AFFINITY_SIZE), Common::MemType::type_halfword
	  ));
	}
  }
}

std::string PartyAffinity::Name()
{
  return "PartyAffinity";
}

int PartyAffinity::Priority()
{
  return LOW;
}

std::string PartyAffinity::setValue(std::string value)
{
  std::vector<std::string> hostAffinities = customSplit(value, ",");

  for (int i = 0; i < NUM_AFFINITIES; i++)
  {
    int newAffinity = atoi(hostAffinities[i].c_str());

	if (newAffinity < 0)
	{
      newAffinity = 0;
	}
	
	m_PartyAffinityWatches[i]->writeMemoryFromString(std::to_string(newAffinity));
  }

  return value;
}

std::string PartyAffinity::hostGetValue()
{
  return m_hostValue;
}

std::string PartyAffinity::getUpdate(std::string hostVal)
{
  std::vector<std::string> hostAffinities = customSplit(hostVal, ",");

  std::string updates = "";

  for (int i = 0; i < NUM_AFFINITIES; i++)
  {
    int hostAffinity = atoi(hostAffinities[i].c_str());
    int myAffinity = atoi(m_PartyAffinityWatches[i]->getStringFromMemory().c_str());

	updates.append(std::to_string(myAffinity - hostAffinity) + ",");
  }

  updates = updates.substr(0, updates.length() - 1); // remove trailing comma
  return updates;
}

void PartyAffinity::hostHandleUpdate(int id, std::string updateString)
{
  std::vector<std::string> hostAffinities = customSplit(m_hostValue, ",");
  std::vector<std::string> updates = customSplit(updateString, ",");
  
  std::string newValues = "";

  for (int i = 0; i < NUM_AFFINITIES; i++)
  {
    int hostAffinity = atoi(hostAffinities[i].c_str());
    int affinityAdded = atoi(updates[i].c_str());

	int newAffinity = hostAffinity + affinityAdded;
    if (newAffinity < 0)
    {
      newAffinity = 0;
	}

    newValues.append(std::to_string(newAffinity) + ",");
  }

  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma

  m_hostValue = newValues;
}


std::vector<std::string> PartyAffinity::customSplit(std::string s, std::string delim)
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