#pragma once

#include <string>
#include <map>

#include <QString>

#include "IMemEntry.h"

class MemManager
{
public:
  MemManager();

  void addEntry(IMemEntry* entry);
  bool setEntryValue(std::string name, std::string value);
  std::string readEntryValue(std::string name);

  std::string getUpdate(std::string name, std::string hostValue);
  void handleUpdate(std::string name, std::string updateString);

  std::vector<std::string> Keys();

private:
  std::map<std::string, IMemEntry*> m_entries;
};
