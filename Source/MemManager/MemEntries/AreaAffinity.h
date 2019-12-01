#pragma once

#include "../../MemoryWatch/MemWatchEntry.h"
#include "../IMemEntry.h"

#define BASE_AFFINITIES_ADDR (0x805718CE)
#define AFFINITY_SIZE (0x02)

#define NUM_AFFINITIES (6) // Not 5 - one unused in the middle, just easier to include it

class AreaAffinity : public IMemEntry
{
public:
  AreaAffinity(bool serverMode);

  std::string Name() override;
  int Priority() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::vector<MemWatchEntry*> m_AreaAffinityWatches;

  std::string m_hostValue;
};