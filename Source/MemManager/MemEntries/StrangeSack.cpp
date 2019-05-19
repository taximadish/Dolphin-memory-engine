#include "StrangeSack.h"

#define MAX_KEYITEMS			(120)
#define FIRST_KEYITEM_OFFSET	(0xA0)
#define KEYITEM_SIZE_BYTES		(2)

#define STRANGE_SACK	"1"
#define PEEKABOO		"309"

StrangeSack::StrangeSack(bool serverMode)
{ 
	if (serverMode)
	{
      m_hostValue = "N";
	}
	else
	{
	  for (int i = 0; i < MAX_KEYITEMS; i++)
	  {
		MemWatchEntry* watch = new MemWatchEntry(Name().c_str(), POUCH_PTR, Common::MemType::type_halfword);
		watch->setBoundToPointer(true);
		watch->addOffset(FIRST_KEYITEM_OFFSET + (i * KEYITEM_SIZE_BYTES));

		m_watches.push_back(watch);
	  }
	  m_tattlesWatch = new MemWatchEntry("Tattles", 0x803DAFBC, Common::MemType::type_byteArray,
										 Common::MemBase::base_hexadecimal, true, 28);

	  m_pausedWatch = new MemWatchEntry("Paused", 0x8041E67b, Common::MemType::type_byte);
	}
}

std::string StrangeSack::Name()
{
  return "StrangeSack";
}

std::string StrangeSack::setValue(std::string value)
{
  if (IsPaused())
    return COULD_NOT_SET;

  UpdateKeyItems(false);
  return value;
}

std::string StrangeSack::hostGetValue()
{
  return m_hostValue;
}

std::string StrangeSack::getUpdate(std::string hostVal)
{
  if (IsPaused())
    return NO_UPDATE;

  bool havePeekaboo = UpdateKeyItems(false);

  if (hostVal != "P" && havePeekaboo)
    return "P";
  else
    return NO_UPDATE;
}

void StrangeSack::hostHandleUpdate(int id, std::string updateString)
{
  if (updateString == "P")
    m_hostValue = "P";
}

bool StrangeSack::UpdateKeyItems(bool givePeekaboo)
{
  bool seenStrangeSack = false;
  bool seenPeekaboo = false;
  std::vector<std::string> keyItems;

  int numItems = 0;

  for (int i = 0; i < MAX_KEYITEMS; i++)
  {
    std::string item = m_watches[i]->getStringFromMemory();
	if (item == STRANGE_SACK)
	{
		if (seenStrangeSack && !seenPeekaboo) // Replace the first duplicate Strange Sack with Peekaboo
		{
		  keyItems.push_back(PEEKABOO);
		  seenPeekaboo = true;
		}
		if (!seenStrangeSack) // Ignore (and thus delete) further multiples of Strange Sack
		{
		  keyItems.push_back(STRANGE_SACK);
		}
		seenStrangeSack = true;
	}
	else if (item == PEEKABOO && !seenPeekaboo) // Ignore (and thus delete) any multiples of peekaboo
	{
      keyItems.push_back(PEEKABOO);
	  seenPeekaboo = true;
	}
	else if (item == "0")
      break;
	else
	  keyItems.push_back(item); // Always keep other key items

	numItems++;
  }

  if (!seenStrangeSack)
    keyItems.push_back(STRANGE_SACK); // Give Strange Sack if we don't have it
  if (!seenPeekaboo && givePeekaboo)
  {
    keyItems.push_back(PEEKABOO);
    seenPeekaboo = true;
  }

  size_t max = (numItems > keyItems.size()) ? numItems : keyItems.size();
  // Now to reinstate the items we just kept/gained
  for (int i = 0; i < max; i++)
  {
    if (i < keyItems.size())
	    m_watches[i]->writeMemoryFromString(keyItems[i]);
    else
		m_watches[i]->writeMemoryFromString("0");
  }

  // finally, if we have peekaboo, fill out the tattle log
  if (seenPeekaboo)
	m_tattlesWatch->writeMemoryFromString("FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"); // Enough F's to fill the 28 bytes

  return seenPeekaboo;
}

bool StrangeSack::IsPaused()
{
  return m_pausedWatch->getStringFromMemory() == "1";
}