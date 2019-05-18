#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class Badges : public IMemEntry
{
public:
  Badges(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(std::string updateString) override;

private:
  bool IsPaused();
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::map<std::string, int8_t> badgeCounts(std::string itemsString);
  std::vector<MemWatchEntry*> m_watches;
  MemWatchEntry* m_pausedWatch;
  std::string m_hostValue;
};