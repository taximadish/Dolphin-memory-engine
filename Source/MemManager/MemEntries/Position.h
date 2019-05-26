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
  void initMarioWatches();
  std::string getAngle();

  std::vector<std::string> customSplit(std::string s, std::string delim);

  MemWatchEntry* m_marioX;
  MemWatchEntry* m_marioY;
  MemWatchEntry* m_marioZ;
  MemWatchEntry* m_marioAngle;
  MemWatchEntry* m_cameraAngle;

  std::map<int32_t, std::string> m_hostValues;
  MemWatchEntry* m_mapWatch;

  std::map<uint8_t, NPC*> m_npcMap;

  void prepareNPC(uint8_t num);
};