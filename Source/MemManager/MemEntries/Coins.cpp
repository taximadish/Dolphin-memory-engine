#include "Coins.h"

Coins::Coins(bool serverMode)
{ 
  if (serverMode)
  {
	m_hostValue = 0;
  }
  else
  {
	m_watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);

	m_watch->setBoundToPointer(true);
	m_watch->addOffset(0x78);
  }
}

std::string Coins::Name()
{
  return "Coins";
}

std::string Coins::setValue(std::string value)
{
  int32_t intVal = atoi(value.c_str());
  m_watch->writeMemoryFromString(value);
  return value;
}

std::string Coins::hostGetValue()
{
  return std::to_string(m_hostValue);
}

std::string Coins::getUpdate(std::string hostVal)
{
  int32_t currentVal = atoi(m_watch->getStringFromMemory().c_str());
  int32_t newVal = atoi(hostVal.c_str());

  if (currentVal == newVal)
  {
    return NO_UPDATE;
  }

  return std::to_string(currentVal - newVal);
}

void Coins::hostHandleUpdate(std::string updateString)
{
  int32_t valToAdd = atoi(updateString.c_str());

  m_hostValue = m_hostValue + valToAdd;
  if (m_hostValue > 999)
    m_hostValue = 999;
  
  if (m_hostValue < 0)
    m_hostValue = 0;
}