#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"
#include "../NPC.h"

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
  void initMainPosWatches();

  std::vector<std::string> customSplit(std::string s, std::string delim);

  MemWatchEntry* m_mainX;
  MemWatchEntry* m_mainY;
  MemWatchEntry* m_mainZ;

  MemWatchEntry* m_secondX;
  MemWatchEntry* m_secondY;
  MemWatchEntry* m_secondZ;

  MemWatchEntry* m_thirdX;
  MemWatchEntry* m_thirdY;
  MemWatchEntry* m_thirdZ;

  std::map<int32_t, std::string> m_hostValues;
  MemWatchEntry* m_mapWatch;
};