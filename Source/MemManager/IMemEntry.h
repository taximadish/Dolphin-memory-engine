#pragma once

#include <map>
#include <string>

#include <QString>

#include "../MemoryWatch/MemWatchEntry.h"

#define NO_UPDATE ""
#define COULD_NOT_SET "__NOT_SET__"

#define REALTIME 1
#define HIGH 5
#define AVERAGE 15
#define LOW 30
#define LOWEST 90

#define NUM_CHARACTERS (8)
#define BASE_RESERVE_ADDRESS (0x80CE437C)
#define CHARACTER_SIZE (0x3DD4)

class IMemEntry
{
public:
  virtual std::string Name() = 0;
  virtual int Priority() = 0;
  virtual std::string setValue(std::string value) = 0;
  virtual std::string hostGetValue() = 0;

  virtual std::string getUpdate(std::string hostVal) = 0;
  virtual void hostHandleUpdate(int id, std::string updateString) = 0;
};
