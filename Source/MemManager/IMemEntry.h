#pragma once

#include <map>
#include <string>

#include <QString>

#include "../MemoryWatch/MemWatchEntry.h"

#define POUCH_PTR		0x8041EB00
#define NO_UPDATE		""
#define COULD_NOT_SET	"__NOT_SET__"

class IMemEntry
{
public:
  virtual std::string Name() = 0;
  virtual std::string setValue(std::string value) = 0;
  virtual std::string hostGetValue() = 0;

  virtual std::string getUpdate(std::string hostVal) = 0;
  virtual void hostHandleUpdate(std::string updateString) = 0;
};
