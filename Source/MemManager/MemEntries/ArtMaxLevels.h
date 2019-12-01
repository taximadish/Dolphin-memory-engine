#pragma once

#include "../../MemoryWatch/MemWatchEntry.h"
#include "../IMemEntry.h"


#define NUM_ARTS (16)
#define ART_SIZE (0x02)

#define ARTMAXLEVELS_OFFSET (-0x1693)
#define SHULK_EXTRAS_OFFSET (-0x49)

class ArtMaxLevels : public IMemEntry
{
public:
  ArtMaxLevels(bool serverMode);

  std::string Name() override;
  int Priority() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::vector<std::vector<MemWatchEntry*>> m_ArtMaxLevelsWatches;
  std::vector<MemWatchEntry*> m_ShulkExtraWatches;

  std::string m_hostValue;
};