#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class StoredItems : public IMemEntry
{
public:
  StoredItems(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(std::string updateString) override;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::map<std::string, int8_t> itemCounts(std::string itemsString);
  std::vector<MemWatchEntry*> m_watches;

  std::string m_hostValue;
};