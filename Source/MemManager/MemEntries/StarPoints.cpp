#include "StarPoints.h"

StarPoints::StarPoints()
{ 
  m_watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);
  m_watch->setBoundToPointer(true);
  m_watch->addOffset(0x96);

  m_levelWatch = new MemWatchEntry("Level", POUCH_PTR, Common::MemType::type_halfword);
  m_levelWatch->setBoundToPointer(true);
  m_levelWatch->addOffset(0x8A);

  m_battleWatch = new MemWatchEntry("BattleBool", 0x80415DC0, Common::MemType::type_word);
  m_battleWatch->setBoundToPointer(true);
  m_battleWatch->addOffset(0x10C);
}

std::string StarPoints::Name()
{
  return "StarPoints";
}

bool StarPoints::setValue(std::string value)
{
  if (InBattle())
    return false;

  int32_t intVal = atoi(value.c_str());
  if (intVal > 99)
    value = "99";
  if (intVal < 0)
    value = "0";
  m_watch->writeMemoryFromString(value);
  return true;
}

std::string StarPoints::getValue()
{
  if (InBattle())
    return NO_UPDATE;

  return m_watch->getStringFromMemory();
}

std::string StarPoints::getUpdate(std::string hostVal)
{
  if (InBattle())
    return NO_UPDATE;

  int32_t currentVal = atoi(m_watch->getStringFromMemory().c_str());
  int32_t newVal = atoi(hostVal.c_str());

  if (currentVal == newVal)
  {
    return NO_UPDATE;
  }

  return std::to_string(currentVal - newVal);
}

void StarPoints::handleUpdate(std::string updateString)
{
  int32_t currentVal = atoi(m_watch->getStringFromMemory().c_str());
  int32_t valToAdd = atoi(updateString.c_str());

  setValue(std::to_string(currentVal + valToAdd));
}

bool StarPoints::InBattle()
{
  std::string inBattle = m_battleWatch->getStringFromMemory();
  return inBattle == "1";
}