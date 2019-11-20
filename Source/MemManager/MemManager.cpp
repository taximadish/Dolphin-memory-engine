#include "MemManager.h"

#include "MemEntries/Position.h"
#include "MemEntries/Money.h"
#include "MemEntries/SkillUnlocks.h"
#include "MemEntries/XP.h";
#include "MemEntries/PartyAffinity.h"
#include "MemEntries/AP.h"
#include "MemEntries/AffinityCoins.h"
#include "MemEntries/Skills.h"

MemManager::MemManager(bool serverMode)
{
  m_entries = {};

  // Shared things
  addEntry(new Position(serverMode));
  addEntry(new Money(serverMode));
  addEntry(new SkillUnlocks(serverMode));
  addEntry(new XP(serverMode));
  addEntry(new PartyAffinity(serverMode));
  addEntry(new AP(serverMode));
  addEntry(new AffinityCoins(serverMode));
  addEntry(new Skills(serverMode));
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