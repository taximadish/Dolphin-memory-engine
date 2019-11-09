#include "Position.h"

#define POS_BASE (0x805717EC)

#define OFFSET_1 (0x38)
#define OFFSET_2 (0x18)
#define OFFSET_3 (0x3F60)
#define OFFSET_4 (0x3A8)

Position::Position(bool serverMode)
{
  if (serverMode)
  {
    return;
  }
  else
  {
    initMainPosWatches();

    // m_mapWatch = new MemWatchEntry("Map", 0x80415FD0, Common::MemType::type_string,
    // Common::MemBase::base_decimal, false, 16);
    // m_mapWatch->setBoundToPointer(true);
    // m_mapWatch->addOffset(0x12C);
  }
}

std::string Position::Name()
{
  return "Pos";
}

std::string Position::setValue(std::string value)
{
  bool nowInBattle = inBattle();
  if (nowInBattle && m_wasPreviouslyInBattle)
  {
    return value; // Let them fight
  }
  else if (nowInBattle && !m_wasPreviouslyInBattle)
  {
    // Battle just started, return control of party members (tp them very far away so they get
    // warped back to player)
    m_secondY->writeMemoryFromString("-10000");
    m_thirdY->writeMemoryFromString("-10000");

    m_wasPreviouslyInBattle = true;
    return value;
  }

  m_wasPreviouslyInBattle = false;

  // Format "NUM,MAP,X,Y,Z#NUM,MAP,X,Y,Z"		Num is 0-indexed, 0 = first NPC
  std::vector<std::string> players = customSplit(value, "#");

  
  int32_t my_num = INT_MAX;
  for (int i = 0; i < players.size(); i++)
  {
    std::vector<std::string> parts = customSplit(players[i], ",");
    if (parts[0].find("$YOU_ARE") != std::string::npos)
    {
      my_num = atoi(parts[0].substr(parts[0].length() - 1).c_str());
	}
  }

  for (int i = 0; i < players.size(); i++)
  {
    std::vector<std::string> parts = customSplit(players[i], ",");

    if (parts[0] == "-1") // skip placeholder
      continue;

    int32_t num = atoi(parts[0].substr(parts[0].length() - 1).c_str());
    if (num == my_num)
    {
      continue;
    }
    else if (num > my_num)
    {
      num--;
	}
    std::string map = parts[1];
    std::string x = parts[2];
    std::string y = parts[3];
    std::string z = parts[4];

    std::string myMap = "_NOT_IMPL_"; // m_mapWatch->getStringFromMemory();

    if (strcmp(myMap.c_str(), map.c_str()) != 0) // Not on same map
    {
      // m_npcMap[num]->setPos("0", "-1000", "0"); // hide character below map
      continue;
    }

    if (num == 0)
    {
      m_secondX->writeMemoryFromString(x);
      m_secondY->writeMemoryFromString(y);
      m_secondZ->writeMemoryFromString(z);
    }
    else if (num == 1)
    {
      m_thirdX->writeMemoryFromString(x);
      m_thirdY->writeMemoryFromString(y);
      m_thirdZ->writeMemoryFromString(z);
    }

    // m_npcMap[num]->setPos(x, y, z);
    // m_npcMap[num]->setAngle(angle, m_cameraAngle->getStringFromMemory());
  }

  return value;
}

std::string Position::hostGetValue()
{
  std::string myPosValue = "-1,_PLH_,0,0,0,0#";

  // Add any new items to end
  for (std::map<int32_t, std::string>::iterator it = m_hostValues.begin(); it != m_hostValues.end();
       ++it)
  {
    int32_t key = it->first;
    std::string value = it->second;

    if (value != "")
    {
      myPosValue.append("$USER" + std::to_string(key) + "," + value + "#");
    }
  }

  if (myPosValue.length() > 0)
    myPosValue = myPosValue.substr(0, myPosValue.length() - 1); // strip last "#"

  return myPosValue;
}

std::string Position::getUpdate(std::string hostVal)
{
  std::string map = "_NOT_IMPL_"; // m_mapWatch->getStringFromMemory();

  std::string x = m_mainX->getStringFromMemory();
  std::string y = m_mainY->getStringFromMemory();
  std::string z = m_mainZ->getStringFromMemory();

  std::string value = map + "," + x + "," + y + "," + z;
  return value;
}

void Position::hostHandleUpdate(int id, std::string updateString)
{
  m_hostValues[id] = updateString;
}

bool Position::inBattle()
{
  return m_battle->getStringFromMemory() == "1";
}

void Position::initMainPosWatches()
{
  m_battle = new MemWatchEntry("Battle Bool", 0x80C45EF4, Common::MemType::type_byte);
  m_battle->setBoundToPointer(true);
  m_battle->addOffset(0x78);
  m_battle->addOffset(0x18);
  m_battle->addOffset(0x0E);

  m_mainX = new MemWatchEntry("Main X", POS_BASE, Common::MemType::type_float);
  m_mainX->setBoundToPointer(true);
  m_mainX->addOffset(OFFSET_1);
  m_mainX->addOffset(OFFSET_2);
  m_mainX->addOffset(OFFSET_3);
  m_mainX->addOffset(OFFSET_4);

  m_mainY = new MemWatchEntry("Main Y", POS_BASE, Common::MemType::type_float);
  m_mainY->setBoundToPointer(true);
  m_mainY->addOffset(OFFSET_1);
  m_mainY->addOffset(OFFSET_2);
  m_mainY->addOffset(OFFSET_3);
  m_mainY->addOffset(OFFSET_4 + 0x04);

  m_mainZ = new MemWatchEntry("Main Z", POS_BASE, Common::MemType::type_float);
  m_mainZ->setBoundToPointer(true);
  m_mainZ->addOffset(OFFSET_1);
  m_mainZ->addOffset(OFFSET_2);
  m_mainZ->addOffset(OFFSET_3);
  m_mainZ->addOffset(OFFSET_4 + 0x08);

  m_secondX = new MemWatchEntry("2 X", POS_BASE + 0x4, Common::MemType::type_float);
  m_secondX->setBoundToPointer(true);
  m_secondX->addOffset(OFFSET_1);
  m_secondX->addOffset(OFFSET_2);
  m_secondX->addOffset(OFFSET_3);
  m_secondX->addOffset(OFFSET_4);

  m_secondY = new MemWatchEntry("2 Y", POS_BASE + 0x04, Common::MemType::type_float);
  m_secondY->setBoundToPointer(true);
  m_secondY->addOffset(OFFSET_1);
  m_secondY->addOffset(OFFSET_2);
  m_secondY->addOffset(OFFSET_3);
  m_secondY->addOffset(OFFSET_4 + 0x04);

  m_secondZ = new MemWatchEntry("2 Z", POS_BASE + 0x04, Common::MemType::type_float);
  m_secondZ->setBoundToPointer(true);
  m_secondZ->addOffset(OFFSET_1);
  m_secondZ->addOffset(OFFSET_2);
  m_secondZ->addOffset(OFFSET_3);
  m_secondZ->addOffset(OFFSET_4 + 0x08);

  m_thirdX = new MemWatchEntry("3 X", POS_BASE + 0x8, Common::MemType::type_float);
  m_thirdX->setBoundToPointer(true);
  m_thirdX->addOffset(OFFSET_1);
  m_thirdX->addOffset(OFFSET_2);
  m_thirdX->addOffset(OFFSET_3);
  m_thirdX->addOffset(OFFSET_4);

  m_thirdY = new MemWatchEntry("3 Y", POS_BASE + 0x08, Common::MemType::type_float);
  m_thirdY->setBoundToPointer(true);
  m_thirdY->addOffset(OFFSET_1);
  m_thirdY->addOffset(OFFSET_2);
  m_thirdY->addOffset(OFFSET_3);
  m_thirdY->addOffset(OFFSET_4 + 0x04);

  m_thirdZ = new MemWatchEntry("3 Z", POS_BASE + 0x08, Common::MemType::type_float);
  m_thirdZ->setBoundToPointer(true);
  m_thirdZ->addOffset(OFFSET_1);
  m_thirdZ->addOffset(OFFSET_2);
  m_thirdZ->addOffset(OFFSET_3);
  m_thirdZ->addOffset(OFFSET_4 + 0x08);
}

std::vector<std::string> Position::customSplit(std::string s, std::string delim)
{
  std::vector<std::string> parts = {};
  std::string delimiter = delim;

  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    parts.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  if (s.length() > 0)
  {
    parts.push_back(s);
  }
  return parts;
}