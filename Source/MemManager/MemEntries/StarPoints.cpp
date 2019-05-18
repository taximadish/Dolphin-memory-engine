#include "StarPoints.h"

StarPoints::StarPoints(bool serverMode)
{
  if (serverMode)
  {
    m_hostValue = "100"; //Start at level 1, no xp
  }
  else
  {
	  m_pointsWatch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);
	  m_pointsWatch->setBoundToPointer(true);
	  m_pointsWatch->addOffset(0x96);

	  m_levelWatch = new MemWatchEntry("Level", POUCH_PTR, Common::MemType::type_halfword);
	  m_levelWatch->setBoundToPointer(true);
	  m_levelWatch->addOffset(0x8A);

	  m_battleWatch = new MemWatchEntry("BattleBool", 0x80415DC0, Common::MemType::type_word);
	  m_battleWatch->setBoundToPointer(true);
	  m_battleWatch->addOffset(0x10C);

	  int32_t currentLevel = atoi(m_levelWatch->getStringFromMemory().c_str());
	  int32_t currentPoints = atoi(m_pointsWatch->getStringFromMemory().c_str());

	  int32_t currentVal = (currentLevel * 100) + currentPoints;
	  m_storedClientVal = std::to_string(currentVal);
  }
}

std::string StarPoints::Name()
{
  return "StarPoints";
}

std::string StarPoints::setValue(std::string value)
{
  if (InBattle())
    return COULD_NOT_SET; // Can't set safely since we may be mid-lvlup

  int32_t currentLevel = atoi(m_levelWatch->getStringFromMemory().c_str());
  int32_t currentPoints = atoi(m_pointsWatch->getStringFromMemory().c_str());

  int32_t newValue = atoi(value.c_str());
  
  if (newValue >= (currentLevel + 1) * 100) // Higher level
  {
    m_pointsWatch->writeMemoryFromString("99");
    return std::to_string((currentLevel*100)+99);
  }

  m_pointsWatch->writeMemoryFromString(std::to_string(newValue % 100));
  return value;
}

std::string StarPoints::hostGetValue()
{
  return m_hostValue;
}

std::string StarPoints::getUpdate(std::string hostVal)
{
  if (InBattle())
    return NO_UPDATE;

  int32_t currentLevel = atoi(m_levelWatch->getStringFromMemory().c_str());
  int32_t currentPoints = atoi(m_pointsWatch->getStringFromMemory().c_str());

  int32_t currentVal = (currentLevel * 100) + currentPoints;
  int32_t newVal = atoi(hostVal.c_str());

  if (currentVal <= newVal)
  {
    return NO_UPDATE;
  }

  return std::to_string(currentVal - newVal);
}

void StarPoints::hostHandleUpdate(std::string updateString)
{
  int32_t currentVal = atoi(m_hostValue.c_str());
  int32_t valToAdd = atoi(updateString.c_str());

  m_hostValue = std::to_string(currentVal + valToAdd);
}

bool StarPoints::InBattle()
{
  std::string inBattle = m_battleWatch->getStringFromMemory();
  return inBattle == "1";
}