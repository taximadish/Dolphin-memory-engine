#pragma once

#include "../../MemoryWatch/MemWatchEntry.h"
#include "../IMemEntry.h"

#define NUM_CHARACTERS (8)
#define NUM_TREES (5)
#define NUM_SKILLS (5)

#define BASE_RESERVE_ADDRESS (0x80CE437C)
#define CHARACTER_SIZE (0x3DD4)
#define SKILLS_OFFSET (0x1DD8)
#define SKILL_TREE_PERIOD (0xC4)
#define SKILL_PERIOD (0x20)

#define TREE_PROGRESS_OFFSET (0x2640)
#define TREE_PROGRESS_SIZE (0x04)

class Skills : public IMemEntry
{
public:
  Skills(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::vector<std::vector<MemWatchEntry*>> m_SkillProgressWatches;
  std::vector<std::vector<std::vector<MemWatchEntry*>>> m_SkillWatches;
  std::vector<std::vector<int>> m_skillSpNeeded;

  std::string m_hostValue;
};