#pragma once

#include <string>
#include <map>

#include <QString>

#include "IMemEntry.h"

class MemManager
{
public:
  MemManager(bool serverMode);

  void addEntry(IMemEntry* entry);
  int GetPriority(std::string name);
  std::string setEntryValue(std::string name, std::string value);
  std::string hostGetEntryValue(std::string name);

  std::string getUpdate(std::string name, std::string hostValue);
  void hostHandleUpdate(std::string name, int id, std::string updateString);

  std::vector<std::string> Keys();

private:
  std::map<std::string, IMemEntry*> m_entries;
};
