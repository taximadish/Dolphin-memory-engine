#include "MemManager.h"
#include "MemEntries/Coins.h"

#define POUCH_PTR 0x8041EB00

MemManager::MemManager()
{
  m_entries = {};

  addEntry(new Coins());
  /*
  createEntry("Coins", Common::MemType::type_halfword, POUCH_PTR, std::vector<int32_t>{0x78});

  createEntry("Level", Common::MemType::type_halfword, POUCH_PTR,
              std::vector<int32_t>{0x8A});

  createEntry("Star Points", Common::MemType::type_halfword, POUCH_PTR,
              std::vector<int32_t>{0x96});
			  */
}

void MemManager::addEntry(IMemEntry* entry)
{
  m_entries[entry->Name()] = entry;
}

void MemManager::setEntryValue(std::string name, int32_t value)
{
  m_entries[name]->setValue(value);
}

void MemManager::setEntryValue(std::string name, std::string value)
{
  m_entries[name]->setValue(value);
}

std::string MemManager::readEntryValueAsString(std::string name)
{
  return m_entries[name]->getValueAsString();
}

int32_t MemManager::readEntryValueAsInt(std::string name)
{
  return m_entries[name]->getValueAsInt();
}

std::vector<std::string> MemManager::Keys()
{
  std::vector<std::string> keys;
  for (std::map<std::string, IMemEntry*>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
  {
    keys.push_back(it->first);
  }

  return keys;
}