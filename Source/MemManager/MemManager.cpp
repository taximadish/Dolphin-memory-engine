#include "MemManager.h"

#define POUCH_PTR	0x8041EB00

MemManager::MemManager()
{
  m_entries = {};

  createEntry("Coins", Common::MemType::type_halfword, POUCH_PTR, std::vector<int32_t>{0x78});

  createEntry("Level", Common::MemType::type_halfword, POUCH_PTR, std::vector<int32_t>{0x8A});

  createEntry("Star Points", Common::MemType::type_halfword, POUCH_PTR, std::vector<int32_t>{0x96});
}

void MemManager::createEntry(std::string name, Common::MemType type, int32_t address,
                           std::vector<int32_t> offsets)
{
  MemWatchEntry* entry = new MemWatchEntry(name.c_str(), address, type);
  
	for(int i = 0; i < offsets.size(); i++)
	{
          entry->setBoundToPointer(true);
		entry->addOffset(offsets[i]);
	}
 
  addEntry(name, entry);
}

void MemManager::addEntry(std::string name, MemWatchEntry* entry)
{
  m_entries[name] = entry;
}

void MemManager::setEntryValue(std::string name, int32_t value)
{
  MemWatchEntry entry = m_entries[name];
  entry.writeMemoryFromInt(value);
}

void MemManager::setEntryValue(std::string name, std::string value)
{
  MemWatchEntry entry = m_entries[name];
  entry.writeMemoryFromString(value);
}

std::string MemManager::readEntryValue(std::string name)
{
  MemWatchEntry entry = m_entries[name];
  return entry.getStringFromMemory();
}