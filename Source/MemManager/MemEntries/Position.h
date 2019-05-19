#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class Position : public IMemEntry
{
public:
  Position(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  void addMarioPosWatch();
  void addNPCPosWatches();

  std::vector<std::string> customSplit(std::string s, std::string delim);

  std::vector<MemWatchEntry*> m_watchesX;
  std::vector<MemWatchEntry*> m_watchesY;
  std::vector<MemWatchEntry*> m_watchesZ;
  std::map<int32_t, std::string> m_hostValues;
  MemWatchEntry* m_mapWatch;
};