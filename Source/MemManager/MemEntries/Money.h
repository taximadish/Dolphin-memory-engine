#pragma once

#include "../IMemEntry.h"
#include "../../MemoryWatch/MemWatchEntry.h"

#define MONEY_ADDR (0x9057A060)

class Money : public IMemEntry
{
public:
  Money(bool serverMode);

  std::string Name() override;
  int Priority() override;
  std::string setValue(std::string value) override;
  std::string hostGetValue() override;

  std::string getUpdate(std::string hostVal) override;
  void hostHandleUpdate(int id, std::string updateString) override;

private:
  MemWatchEntry* m_watch;
  int32_t m_hostValue;
};