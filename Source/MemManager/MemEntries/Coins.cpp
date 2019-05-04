#include "Coins.h"

Coins::Coins()
{ 
  m_watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);

  m_watch->setBoundToPointer(true);
  m_watch->addOffset(0x78);
}

std::string Coins::Name()
{
  return "Coins";
}

void Coins::setValue(int32_t value)
{
  if (value > 999)
    value = 999;
  if (value < 0)
    value = 0;
  m_watch->writeMemoryFromInt(value);
}

int32_t Coins::getValueAsInt()
{
  return atoi(m_watch->getStringFromMemory().c_str());
}

std::string Coins::getValueAsString()
{
  return m_watch->getStringFromMemory();
}