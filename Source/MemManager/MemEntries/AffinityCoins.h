#pragma once

#include "../../MemoryWatch/MemWatchEntry.h"
#include "../IMemEntry.h"

#define NUM_CHARACTERS (8)

#define BASE_RESERVE_ADDRESS (0x80CE437C)
#define CHARACTER_SIZE (0x3DD4)
#define AFFINITYCOINS_OFFSET (0x263C)

class AffinityCoins : public IMemEntry
{
public:
  AffinityCoins(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::vector<MemWatchEntry*> m_AffinityCoinsWatches;

  std::string m_hostValue;
};