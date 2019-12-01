#include "ArtMaxLevels.h"

ArtMaxLevels::ArtMaxLevels(bool serverMode)
{
  if (serverMode)
  {
    std::string value = "";
    for (int chara = 0; chara < NUM_CHARACTERS; chara++)
    {
      std::string arts = "";
      for (int art = 0; art < NUM_ARTS; art++)
      {
        arts.append("00,");
      }

      arts = arts.substr(0, arts.length() - 1); // remove trailing comma
      value.append(arts + "!");
    }

    value = value.substr(0, value.length() - 1); // remove trailing '!'

    m_hostValue = value;
  }
  else
  {
	// Reserve Watches
    for (int chara = 0; chara < NUM_CHARACTERS; chara++)
    {
      int baseArtAddress = BASE_RESERVE_ADDRESS + (chara * CHARACTER_SIZE) + ARTMAXLEVELS_OFFSET;
	  if (chara == 0)
	  {
        baseArtAddress = 0x80CE2D32;
	  }
      std::vector<MemWatchEntry*> arts;
      for (int art = 0; art < NUM_ARTS; art++)
      {
        arts.push_back(new MemWatchEntry(
			"ArtMaxLevel", baseArtAddress + (art * ART_SIZE), Common::MemType::type_byte, Common::MemBase::base_binary
		));

		if (chara == 0)
        {
          m_ShulkExtraWatches.push_back(new MemWatchEntry(
			  "ArtMaxLevel", baseArtAddress + SHULK_EXTRAS_OFFSET + (art * ART_SIZE),
			  Common::MemType::type_byte, Common::MemBase::base_binary
		  ));
		}
      }

      m_ArtMaxLevelsWatches.push_back(arts);
    }
  }
}

std::string ArtMaxLevels::Name()
{
  return "ArtMaxLevels";
}

int ArtMaxLevels::Priority()
{
  return LOWEST;
}

std::string ArtMaxLevels::setValue(std::string value)
{
  std::vector<std::string> charas = customSplit(value, "!");
  for (int chara = 0; chara < NUM_CHARACTERS; chara++)
  {
    std::vector<std::string> hostArts = customSplit(charas[chara], ",");
    for (int art = 0; art < NUM_ARTS; art++)
    {
      std::string hostArtValue = hostArts[art];
      std::string myArtValue = m_ArtMaxLevelsWatches[chara][art]->getStringFromMemory();
      if (hostArtValue[0] == '1')
      {
        myArtValue[0] = '1';
	  }
      if (hostArtValue[1] == '1')
      {
        myArtValue[1] = '1';
      }
      
	  m_ArtMaxLevelsWatches[chara][art]->writeMemoryFromString(myArtValue);
      if (chara == 0)
      {
        m_ShulkExtraWatches[art]->writeMemoryFromString(myArtValue);
	  }
    }
  }

  return value;
}

std::string ArtMaxLevels::hostGetValue()
{
  return m_hostValue;
}

std::string ArtMaxLevels::getUpdate(std::string hostVal)
{

  std::string updates = "";

  for (int chara = 0; chara < NUM_CHARACTERS; chara++)
  {
    for (int art = 0; art < NUM_ARTS; art++)
    {
      std::string myValue = m_ArtMaxLevelsWatches[chara][art]->getStringFromMemory().substr(0, 2);
	  if (chara == 0)
	  {
        std::string extraValue = m_ShulkExtraWatches[art]->getStringFromMemory();
        if (extraValue[0] == '1')
        {
          myValue[0] = '1';
		}
        if (extraValue[1] == '1')
        {
          myValue[1] = '1';
        }
	  }
      updates.append(myValue + ",");
    }
    updates = updates.substr(0, updates.length() - 1); // remove trailing comma
    updates.append("!");
  }

  updates = updates.substr(0, updates.length() - 1); // remove trailing '!'
  return updates;
}

void ArtMaxLevels::hostHandleUpdate(int id, std::string updateString)
{
  std::string newValues = "";
  
  std::vector<std::string> hostCharas = customSplit(m_hostValue, "!");
  std::vector<std::string> updateCharas = customSplit(updateString, "!");

  for (int chara = 0; chara < NUM_CHARACTERS; chara++)
  {
    std::vector<std::string> hostArts = customSplit(hostCharas[chara], ",");
    std::vector<std::string> updateArts = customSplit(updateCharas[chara], ",");
    for (int art = 0; art < NUM_ARTS; art++)
    {
      std::string hostArtValue = hostArts[art];
      std::string updateArtValue = updateArts[art];
      if (updateArtValue[0] == '1')
      {
        hostArtValue[0] = '1';
      }
      if (updateArtValue[1] == '1')
      {
        hostArtValue[1] = '1';
      }

      newValues.append(hostArtValue + ",");
    }
    newValues = newValues.substr(0, newValues.length() - 1); // remove trailing comma
    newValues.append("!");
  }
  newValues = newValues.substr(0, newValues.length() - 1); // remove trailing '!'
  m_hostValue = newValues;
}


std::vector<std::string> ArtMaxLevels::customSplit(std::string s, std::string delim)
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