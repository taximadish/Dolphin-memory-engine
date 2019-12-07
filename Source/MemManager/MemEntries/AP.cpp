#include "AP.h"

AP::AP(bool serverMode)
{
  if (serverMode)
  {
    m_hostValue = "0,0,0,0,0,0,0,0";
  }
  else
  {
    m_lastHostValues = "";

    for (int i = 0; i < NUM_CHARACTERS; i++)
    {
      m_myLastValues.push_back(-1);

      // Reserve Watches
      int char_base_addr = BASE_RESERVE_ADDRESS + (i * CHARACTER_SIZE);
      m_APWatches.push_back(new MemWatchEntry("AP", char_base_addr + AP_OFFSET, Common::MemType::type_word));
	}
  }
}

std::string AP::Name()
{
  return "AP";
}

int AP::Priority()
{
  return AVERAGE;
}

std::string AP::setValue(std::string value)
{
  if (m_lastHostValues == "")
  {
    m_lastHostValues = value;
    return value;
  }

  if (value == m_lastHostValues)
  {
    return value;
  }

  std::vector<std::string> oldHostValues = customSplit(m_lastHostValues, ",");
  std::vector<std::string> newHostValues = customSplit(value, ",");

  for (int i = 0; i < NUM_CHARACTERS; i++)
  {
    int oldValue = atoi(oldHostValues[i].c_str());
    int newValue = atoi(newHostValues[i].c_str());

	int apAdded = newValue - oldValue;

	int currentAP = atoi(m_APWatches[i]->getStringFromMemory().c_str());
    if (m_myLastValues[i] == -1)
    {
        m_myLastValues[i] = currentAP;
	}

	m_APWatches[i]->writeMemoryFromString(std::to_string(currentAP + apAdded));

	m_myLastValues[i] += apAdded;
  }

  m_lastHostValues = value;
  return value;
}

std::string AP::hostGetValue()
{
  return m_hostValue;
}

std::string AP::getUpdate(std::string hostVal)
{
  if (m_lastHostValues == "")
  {
    m_lastHostValues = hostVal;
  }

  std::string updates = "";

  std::vector<std::string> lastHostValues = customSplit(m_lastHostValues, ",");
  std::string adjustedHostValues = "";

  for (int i = 0; i < 8; i++)
  {
    int currentAP = atoi(m_APWatches[i]->getStringFromMemory().c_str());

	if (m_myLastValues[i] == -1)
	{
      m_myLastValues[i] = currentAP;
	}

    int oldAP = m_myLastValues[i];

	int diff = currentAP - oldAP;

	if (diff < 0 || oldAP == 0) // Don't share initial AP for recruiting character (eg. from 0 -> 8000)
    {
      diff = 0;
	}

	updates.append(std::to_string(diff) + ",");
    int adjustedHostValue = atoi(lastHostValues[i].c_str()) + diff;
    adjustedHostValues.append(std::to_string(adjustedHostValue) + ",");

	m_myLastValues[i] = currentAP;
  }

  adjustedHostValues = adjustedHostValues.substr(0, adjustedHostValues.length() - 1); // remove trailing comma
  m_lastHostValues = adjustedHostValues;

  updates = updates.substr(0, updates.length() - 1); // remove trailing comma
  return updates;
}

void AP::hostHandleUpdate(int id, std::string updateString)
{
  std::vector<std::string> APUpdates = customSplit(updateString, ",");
  std::vector<std::string> APTotals = customSplit(m_hostValue, ",");

  std::string newValues = "";
  for (int i = 0; i < 8; i++)
  {
    int addedAP = atoi(APUpdates[i].c_str());
    if (addedAP < 0)
    {
      addedAP = 0; // This should never happen, but just in case
	}
    int currentTotal = atoi(APTotals[i].c_str());
    newValues.append(std::to_string(currentTotal + addedAP) + ",");
  }

  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma

  m_hostValue = newValues;
}


std::vector<std::string> AP::customSplit(std::string s, std::string delim)
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