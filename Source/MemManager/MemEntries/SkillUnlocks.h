#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

#define SKILL_UNLOCKS_ADDR (0x80572A25)

class SkillUnlocks : public IMemEntry
{
public:
  SkillUnlocks(bool serverMode);

  std::string Name() override;
  int Priority() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  MemWatchEntry* m_watch;
  std::string m_hostValue;
};