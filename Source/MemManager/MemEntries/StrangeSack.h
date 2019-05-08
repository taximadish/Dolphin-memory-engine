#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class StrangeSack : public IMemEntry
{
public:
  StrangeSack();

  std::string Name() override;
  bool setValue(std::string value) override;
  std::string getValue() override;

  std::string getUpdate(std::string hostVal) override;
  void handleUpdate(std::string updateString) override;

private:
  bool UpdateKeyItems();
  std::vector<MemWatchEntry*> m_watches;
  MemWatchEntry* m_tattlesWatch;
  MemWatchEntry* m_pausedWatch;
  bool m_givePeekaboo;
};