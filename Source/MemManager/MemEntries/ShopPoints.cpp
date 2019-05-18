#include "ShopPoints.h"

ShopPoints::ShopPoints(bool serverMode)
{
  if (serverMode)
  {
    m_hostValue = "0";
  }
  else
  {
    m_watch = new MemWatchEntry(Name().c_str(), 0x80B08110, Common::MemType::type_halfword);
  }
}

std::string ShopPoints::Name()
{
  return "ShopPoints";
}

std::string ShopPoints::setValue(std::string value)
{
  int32_t intVal = atoi(value.c_str());
  if (intVal < 0)
    value = "0";
  m_watch->writeMemoryFromString(value);
  return value;
}

std::string ShopPoints::hostGetValue()
{
  return m_hostValue;
}

std::string ShopPoints::getUpdate(std::string hostVal)
{
  int32_t currentVal = atoi(m_watch->getStringFromMemory().c_str());
  int32_t newVal = atoi(hostVal.c_str());

  if (currentVal == newVal)
  {
    return NO_UPDATE;
  }

  return std::to_string(currentVal - newVal);
}

void ShopPoints::hostHandleUpdate(std::string updateString)
{
  int32_t currentVal = atoi(m_hostValue.c_str());
  int32_t valToAdd = atoi(updateString.c_str());

  m_hostValue = std::to_string(currentVal + valToAdd);
}