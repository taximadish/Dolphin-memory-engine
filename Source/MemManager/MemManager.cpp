#include "MemManager.h"

#include "MemEntries/Coins.h"
#include "MemEntries/Shines.h"
#include "MemEntries/StarPieces.h"
#include "MemEntries/Items.h"
#include "MemEntries/StoredItems.h"
#include "MemEntries/Badges.h"
#include "MemEntries/StrangeSack.h"
#include "MemEntries/StarPoints.h"
#include "MemEntries/ShopPoints.h"
#include "MemEntries/Position.h"

#define POUCH_PTR 0x8041EB00

MemManager::MemManager(bool serverMode)
{
  m_entries = {};

  // Shared things
  addEntry(new Coins(serverMode));
  addEntry(new Shines(serverMode));
  addEntry(new StarPieces(serverMode));
  addEntry(new Items(serverMode));
  addEntry(new StoredItems(serverMode));
  addEntry(new Badges(serverMode));
  addEntry(new StrangeSack(serverMode));
  addEntry(new StarPoints(serverMode));
  addEntry(new ShopPoints(serverMode));
  addEntry(new Position(serverMode));
}

void MemManager::addEntry(IMemEntry* entry)
{
  m_entries[entry->Name()] = entry;
}

std::string MemManager::setEntryValue(std::string name, std::string value)
{
  return m_entries[name]->setValue(value);
}

std::string MemManager::hostGetEntryValue(std::string name)
{
  return m_entries[name]->hostGetValue();
}

std::string MemManager::getUpdate(std::string name, std::string hostVal)
{
  return m_entries[name]->getUpdate(hostVal);
}

void MemManager::hostHandleUpdate(std::string name, int id, std::string updateString)
{
  m_entries[name]->hostHandleUpdate(id, updateString);
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