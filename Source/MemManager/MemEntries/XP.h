#pragma once

#include "../../MemoryWatch/MemWatchEntry.h"
#include "../IMemEntry.h"

#define NUM_CHARACTERS (8)

#define BASE_RESERVE_ADDRESS (0x80CE437C)
#define CHARACTER_SIZE (0x3DD4)
#define XP_UPTOLEVEL_OFFSET (0)
#define XP_PASTLEVEL_OFFSET (0x04)

#define BASE_SLOT_ADDR (0x80CDCBD8)
#define SLOT_SIZE (0x04)

#define BASE_PARTY_PTR (0x90567F4C)

class XP : public IMemEntry
{
public:
  XP(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::vector<MemWatchEntry*> m_ReserveXpUpToLevelWatches;
  std::vector<MemWatchEntry*> m_ReserveXpPastLevelWatches;
  std::vector<MemWatchEntry*> m_SlotWatches;
  std::vector<MemWatchEntry*> m_PartyXpWatches;

  std::string m_hostValue;
};