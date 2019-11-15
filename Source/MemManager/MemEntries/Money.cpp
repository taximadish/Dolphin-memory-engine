#include "Money.h"

#define MAX_MONEY (999999999)

Money::Money(bool serverMode)
{ 
  if (serverMode)
  {
	m_hostValue = 0;
  }
  else
  {
	m_watch = new MemWatchEntry(Name().c_str(), MONEY_ADDR, Common::MemType::type_word);
  }
}

std::string Money::Name()
{
  return "Money";
}

std::string Money::setValue(std::string value)
{
  m_watch->writeMemoryFromString(value);
  return value;
}

std::string Money::hostGetValue()
{
  return std::to_string(m_hostValue);
}

std::string Money::getUpdate(std::string hostVal)
{
  std::string currentValS = m_watch->getStringFromMemory();

  if (currentValS == "???")
  {
    return NO_UPDATE;
  }

  int32_t currentVal = atoi(currentValS.c_str());
  int32_t newVal = atoi(hostVal.c_str());

  if (currentVal == newVal)
  {
    return NO_UPDATE;
  }

  if (currentVal == MAX_MONEY && newVal < 899999999)
  {
    currentVal = 0; // We underflowed
  }

  return std::to_string(currentVal - newVal);
}

void Money::hostHandleUpdate(int id, std::string updateString)
{
  int32_t valToAdd = atoi(updateString.c_str());

  m_hostValue = m_hostValue + valToAdd;
  if (m_hostValue > MAX_MONEY)
    m_hostValue = MAX_MONEY;
  
  if (m_hostValue < 0)
    m_hostValue = 0;
}