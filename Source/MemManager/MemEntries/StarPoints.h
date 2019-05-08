#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class StarPoints : public IMemEntry
{
public:
  StarPoints();

  std::string Name() override;
  bool setValue(std::string value) override;
  std::string getValue() override;

  std::string getUpdate(std::string hostVal) override;
  void handleUpdate(std::string updateString) override;

private:
  bool InBattle();
  MemWatchEntry* m_watch;
  MemWatchEntry* m_levelWatch;
  MemWatchEntry* m_battleWatch;
};