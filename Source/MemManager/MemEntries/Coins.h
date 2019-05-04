#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class Coins : public IMemEntry
{
public:
  Coins();

  std::string Name() override;

  void setValue(int32_t value) override;
  int32_t getValueAsInt() override;
  std::string getValueAsString() override;

private:
  MemWatchEntry* m_watch;
};