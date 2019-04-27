#pragma once

#include <string>
#include <map>

#include <QString>

#include "../MemoryWatch/MemWatchEntry.h"

class MemManager
{
public:
  MemManager();

  void createEntry(std::string name, Common::MemType type, int32_t address,
                   std::vector<int32_t> offsets = {});
  void addEntry(std::string name, MemWatchEntry* entry);
  void setEntryValue(std::string name, int32_t value);
  void setEntryValue(std::string name, std::string value);
  std::string readEntryValue(std::string name);

private:
  std::map<std::string, MemWatchEntry*> m_entries;
};
