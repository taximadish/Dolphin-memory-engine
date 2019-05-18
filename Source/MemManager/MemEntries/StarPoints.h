#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class StarPoints : public IMemEntry
{
public:
  StarPoints(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(std::string updateString) override;

private:
  bool InBattle();
  MemWatchEntry* m_pointsWatch;
  MemWatchEntry* m_levelWatch;
  MemWatchEntry* m_battleWatch;

  std::string m_storedClientVal;
  std::string m_hostValue;
};