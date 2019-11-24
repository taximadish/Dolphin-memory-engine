#pragma once

#include "../../MemoryWatch/MemWatchEntry.h"
#include "../IMemEntry.h"

#define PARTY_SIZE (3)
#define SLOT_SIZE (0x04)

#define POS_BASE (0x805717EC)

#define OFFSET_1 (0x38)
#define OFFSET_2 (0x18)
#define OFFSET_3 (0x3F60)
#define OFFSET_4 (0x3A8)

#define X_OFFSET (0x00)
#define Y_OFFSET (0x04)
#define Z_OFFSET (0x08)

#define Y_VEL_OFFSET (0x3C4)
#define TARGET_ANGLE_OFFSET (0x440)
#define CURRENT_ANGLE_OFFSET (0x444)
#define FALL_PEAK_OFFSET (0x488)

#define TARGET_2_OFFSET (-0x456C)
#define TARGET_3_OFFSET (0xAFC)
#define TELEBLOCKER_OFFSET (0xF3)

#define TARGET_MULTI (0.00)

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
  bool inBattle();

  bool m_wasPreviouslyInBattle;

  std::vector<std::string> customSplit(std::string s, std::string delim);

  MemWatchEntry* m_battle;

  std::vector<MemWatchEntry*> m_XWatches;
  std::vector<MemWatchEntry*> m_YWatches;
  std::vector<MemWatchEntry*> m_ZWatches;

  std::vector<MemWatchEntry*> m_YVelWatches;

  std::vector<MemWatchEntry*> m_TargetAngleWatches;
  std::vector<MemWatchEntry*> m_CurrentAngleWatches;

  std::vector<MemWatchEntry*> m_FallPeakWatches;

  std::vector<MemWatchEntry*> m_TeleblockerWatches;

  std::vector<MemWatchEntry*> m_TargetXWatches;
  std::vector<MemWatchEntry*> m_TargetYWatches;
  std::vector<MemWatchEntry*> m_TargetZWatches;

  std::vector<std::string> m_prevXValues;
  std::vector<std::string> m_prevYValues;
  std::vector<std::string> m_prevZValues;

  std::vector<std::string> m_prevAngles;

  std::string GetTarget(std::string prevVal, std::string newVal);

  std::map<int32_t, std::string> m_hostValues;
  MemWatchEntry* m_mapWatch;
};