#pragma once

#include <string>
#include <map>

#include <QString>

#include "../MemoryWatch/MemWatchEntry.h"

class MemManager
{
public:
  MemManager();

  void createEntry(const char* name, Common::MemBase base, Common::MemType type, int32_t address, std::vector<int32_t> offsets = {});
  void addEntry(const char* name, MemWatchEntry* entry);
  void setEntryValue(const char* name, int32_t value);
  void setEntryValue(const char* name, char* value);
  std::string readEntryValue(const char* name);

private:
  std::map<const char*, MemWatchEntry*> m_entries;
};
