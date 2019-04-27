#include "MemManager.h"

MemManager::MemManager()
{
  m_entries = {};
  createEntry("Coins", Common::MemBase::base_decimal, Common::MemType::type_halfword,
	  0x8041EB00, std::vector<int32_t> { 0x78 });
}

void MemManager::createEntry(const char* name, Common::MemBase base, Common::MemType type, int32_t address,
                           std::vector<int32_t> offsets)
{
  MemWatchEntry* entry = new MemWatchEntry(name, address, type, base);
  
	for(int i = 0; i < offsets.size(); i++)
	{
          entry->setBoundToPointer(true);
		entry->addOffset(offsets[i]);
	}
 
  addEntry(name, entry);
}

void MemManager::addEntry(const char* name, MemWatchEntry* entry)
{
  m_entries[name] = entry;
}

void MemManager::setEntryValue(const char* name, int32_t value)
{
  MemWatchEntry entry = m_entries[name];
  entry.writeMemoryFromInt(value);
}

void MemManager::setEntryValue(const char* name, char* value)
{
  MemWatchEntry entry = m_entries[name];
  entry.writeMemoryFromString(value);
}

std::string MemManager::readEntryValue(const char* name)
{
  MemWatchEntry entry = m_entries[name];
  return entry.getStringFromMemory();
}