#pragma once

#include <map>
#include <string>

#include <QString>

#include "../MemoryWatch/MemWatchEntry.h"

#define POUCH_PTR 0x8041EB00

class IMemEntry
{
public:
  virtual std::string Name() = 0;
  void setValue(std::string value)
  {
    setValue(atoi(value.c_str()));
  }
  virtual void setValue(int32_t value) = 0;
  virtual int32_t getValueAsInt() = 0;
  virtual std::string getValueAsString() = 0;
};
