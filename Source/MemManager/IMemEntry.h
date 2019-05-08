#pragma once

#include <map>
#include <string>

#include <QString>

#include "../MemoryWatch/MemWatchEntry.h"

#define POUCH_PTR	0x8041EB00
#define NO_UPDATE	""

class IMemEntry
{
public:
  virtual std::string Name() = 0;
  virtual bool setValue(std::string value) = 0;
  virtual std::string getValue() = 0;

  virtual std::string getUpdate(std::string hostVal) = 0;
  virtual void handleUpdate(std::string updateString) = 0;
};
