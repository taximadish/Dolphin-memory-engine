#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class StrangeSack : public IMemEntry
{
public:
  StrangeSack(bool serverMode);

  std::string Name() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(std::string updateString) override;

private:
  bool IsPaused();
  bool UpdateKeyItems(bool givePeekaboo);
  std::string m_hostValue;
  std::vector<MemWatchEntry*> m_watches;
  MemWatchEntry* m_tattlesWatch;
  MemWatchEntry* m_pausedWatch;
};