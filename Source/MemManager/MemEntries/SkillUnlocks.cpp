#include "SkillUnlocks.h"

#define BINARY_0_16BIT	"0000000000000000"

SkillUnlocks::SkillUnlocks(bool serverMode)
{ 
  if (serverMode)
  {
	m_hostValue = BINARY_0_16BIT;
  }
  else
  {
	m_watch = new MemWatchEntry(Name().c_str(), SKILL_UNLOCKS_ADDR, Common::MemType::type_halfword, Common::MemBase::base_binary);
  }
}

std::string SkillUnlocks::Name()
{
  return "SkillUnlocks";
}

std::string SkillUnlocks::setValue(std::string value)
{
  m_watch->writeMemoryFromString(value);
  return value;
}

std::string SkillUnlocks::hostGetValue()
{
  return m_hostValue;
}

std::string SkillUnlocks::getUpdate(std::string hostVal)
{
	std::string currentVal = m_watch->getStringFromMemory();

	if (currentVal == "???")
	{
		return NO_UPDATE;
	}

	bool changed = false;
	for(int i = 0; i < hostVal.length(); i++)
	{
		if (hostVal[i] == '0' && currentVal[i] == '1')
		{
			changed = true;
			hostVal[i] = '1';
		}
	}

	if (changed)
	{
		return hostVal;
	}
	else
	{
		return NO_UPDATE;
	}
}

void SkillUnlocks::hostHandleUpdate(int id, std::string updateString)
{
  m_hostValue = updateString;
}