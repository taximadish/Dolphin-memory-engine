#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

class StarPieces : public IMemEntry
{
public:
  StarPieces();

  std::string Name() override;
  void setValue(std::string value) override;
  std::string getValue() override;

  std::string getUpdate(std::string hostVal) override;
  void handleUpdate(std::string updateString) override;

private:
  MemWatchEntry* m_watch;
};