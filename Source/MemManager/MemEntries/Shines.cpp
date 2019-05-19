#include "Shines.h"

Shines::Shines(bool serverMode)
{
  if (serverMode)
  {
    m_hostValue = "0";
  }
  else
  {
	  m_watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);

	  m_watch->setBoundToPointer(true);
	  m_watch->addOffset(0x9C);
  }
}

std::string Shines::Name()
{
  return "Shines";
}

std::string Shines::setValue(std::string value)
{
  int32_t intVal = atoi(value.c_str());
  if (intVal > 999)
    value = "999";
  if (intVal < 0)
    value = "0";
  m_watch->writeMemoryFromString(value);
  return value;
}

std::string Shines::hostGetValue()
{
  return m_hostValue;
}

std::string Shines::getUpdate(std::string hostVal)
{
  int32_t currentVal = atoi(m_watch->getStringFromMemory().c_str());
  int32_t newVal = atoi(hostVal.c_str());

  if (currentVal == newVal)
  {
    return NO_UPDATE;
  }

  return std::to_string(currentVal - newVal);
}

void Shines::hostHandleUpdate(int id, std::string updateString)
{
  int32_t currentVal = atoi(m_hostValue.c_str());
  int32_t valToAdd = atoi(updateString.c_str());

  m_hostValue = std::to_string(currentVal + valToAdd);
}