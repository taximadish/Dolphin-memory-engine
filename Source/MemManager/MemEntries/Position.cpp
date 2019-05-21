#include "Position.h"

#define MARIO_PTR			(0x8041E900)

#define MARIO_XVAL_OFFSET	(0x8C)

#define NPC1_XVAL_OFFSET	(0x80AF2EEC)
#define NPC_SIZE			(0x340)
#define POS_VAL_DIST		(0x04)

Position::Position(bool serverMode)
{ 
  if (serverMode)
  {
    return;
  }
  else
  {
    addMarioPosWatch();
    addNPCPosWatches();

	m_mapWatch = new MemWatchEntry("Map", 0x80415FD0, Common::MemType::type_string,
		Common::MemBase::base_decimal, false, 16);
    m_mapWatch->setBoundToPointer(true);
    m_mapWatch->addOffset(0x12C);
  }
}

std::string Position::Name()
{
  return "Pos";
}

std::string Position::setValue(std::string value)
{
	// Format "NUM,MAP,X,Y,Z#NUM,MAP,X,Y,Z"		Num is 0-indexed, 0 = first NPC
  std::vector<std::string> players = customSplit(value, "#");

  for (int i = 0; i < players.size(); i++)
  {
	  std::vector<std::string> parts = customSplit(players[i], ",");

	  if (parts[0] == "-1") // skip placeholder
            continue;

	  int32_t num = atoi(parts[0].substr(parts[0].length() - 1).c_str());
	  std::string map = parts[1];
	  std::string x = parts[2];
	  std::string y = parts[3];
	  std::string z = parts[4];

	  std::string myMap = m_mapWatch->getStringFromMemory();

	  if (strcmp(myMap.c_str(), map.c_str()) != 0) // Not on same map
	  {
		continue;
	  }

	  m_watchesX[num + 1]->writeMemoryFromString(x);
	  m_watchesY[num + 1]->writeMemoryFromString(y);
	  m_watchesZ[num + 1]->writeMemoryFromString(z);
  }
  return value;
}

std::string Position::hostGetValue()
{
  std::string myPosValue = "-1,_PLH_,0,0,0#";
  
    // Add any new items to end
  for (std::map<int32_t, std::string>::iterator it = m_hostValues.begin(); it != m_hostValues.end(); ++it)
  {
    int32_t key = it->first;
    std::string value = it->second;

	if (value != "")
      myPosValue.append("$USER" + std::to_string(key) + "," + value + "#");
  }

  if (myPosValue.length() > 0)
    myPosValue = myPosValue.substr(0, myPosValue.length() - 1); // strip last "#"

  return myPosValue;
}

std::string Position::getUpdate(std::string hostVal)
{
  std::string map = m_mapWatch->getStringFromMemory();
  std::string x = m_watchesX[0]->getStringFromMemory();
  std::string y = m_watchesY[0]->getStringFromMemory();
  std::string z = m_watchesZ[0]->getStringFromMemory();

  std::string value = map + "," + x + "," + y + "," + z;
  return value;
}

void Position::hostHandleUpdate(int id, std::string updateString)
{
  m_hostValues[id] = updateString;
}

void Position::addMarioPosWatch()
{
  MemWatchEntry* xWatch = new MemWatchEntry("Mario X", MARIO_PTR, Common::MemType::type_float);
  xWatch->setBoundToPointer(true);
  xWatch->addOffset(MARIO_XVAL_OFFSET);

  MemWatchEntry* yWatch = new MemWatchEntry("Mario Y", MARIO_PTR, Common::MemType::type_float);
  yWatch->setBoundToPointer(true);
  yWatch->addOffset(MARIO_XVAL_OFFSET + POS_VAL_DIST);

  MemWatchEntry* zWatch = new MemWatchEntry("Mario Z", MARIO_PTR, Common::MemType::type_float);
  zWatch->setBoundToPointer(true);
  zWatch->addOffset(MARIO_XVAL_OFFSET + (2 * POS_VAL_DIST));

  m_watchesX.push_back(xWatch);
  m_watchesY.push_back(yWatch);
  m_watchesZ.push_back(zWatch);
}

void Position::addNPCPosWatches()
{
  for (int i = 0; i < 10; i++)
  {
    int64_t xAddr = NPC1_XVAL_OFFSET + (NPC_SIZE * i);
	  MemWatchEntry* xWatch = new MemWatchEntry("NPC X", xAddr, Common::MemType::type_float);

	  MemWatchEntry* yWatch = new MemWatchEntry("NPC Y", xAddr + POS_VAL_DIST, Common::MemType::type_float);

	  MemWatchEntry* zWatch = new MemWatchEntry("NPC Z", xAddr + (POS_VAL_DIST * 2), Common::MemType::type_float);

	  m_watchesX.push_back(xWatch);
	  m_watchesY.push_back(yWatch);
	  m_watchesZ.push_back(zWatch);
  }
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