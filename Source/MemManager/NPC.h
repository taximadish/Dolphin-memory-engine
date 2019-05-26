#pragma once

#include "../MemoryWatch/MemWatchEntry.h"

class NPC
{
public:
  NPC(int num);
  void update();
  void setPos(std::string x, std::string y, std::string z);
  void setAngle(std::string angle, std::string camAngle);

private:
  std::string getWidth(float camAngle);
  std::string getDepth(float camAngle);

  MemWatchEntry* m_entireNpcWatch;

  MemWatchEntry* m_flags1Watch;
  MemWatchEntry* m_flags2Watch;
  MemWatchEntry* m_animPtrWatch;

  MemWatchEntry* m_modelNumWatch;

  MemWatchEntry* m_xWatch;
  MemWatchEntry* m_yWatch;
  MemWatchEntry* m_zWatch;

  MemWatchEntry* m_angle;

  MemWatchEntry* m_widthWatch;
  MemWatchEntry* m_heightWatch;
  MemWatchEntry* m_depthWatch;

  MemWatchEntry* m_colorRedWatch;
  MemWatchEntry* m_colorGreenWatch;
  MemWatchEntry* m_colorBlueWatch;
  MemWatchEntry* m_colorAlphaWatch;

  MemWatchEntry* m_talkTextPtrWatch;

  MemWatchEntry* m_renderPtrWatch;
};
