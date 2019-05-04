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
  void setEntryValue(std::string name, int32_t value);
  void setEntryValue(std::string name, std::string value);
  std::string readEntryValueAsString(std::string name);
  int32_t readEntryValueAsInt(std::string name);
  std::vector<std::string> Keys();

private:
  std::map<std::string, IMemEntry*> m_entries;
};
