#include "Position.h"

#define MARIO_PTR			(0x8041E900)

#define MARIO_XVAL_OFFSET	(0x8C)

#define NPC1_XVAL_OFFSET	(0x80AF2EEC)
#define NPC_SIZE			(0x340)
#define POS_VAL_DIST		(0x04)

#define MARIO_FACING_OFFSET (0x1B0)

#define CAMERA_ANGLE_OFFSET (0x19C)

Position::Position(bool serverMode)
{ 
  if (serverMode)
  {
    return;
  }
  else
  {
    initMarioWatches();

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
      std::string angle = parts[5];

	  std::string myMap = m_mapWatch->getStringFromMemory();

	  prepareNPC(num); // Must be done before modifying NPC in any way

	  if (strcmp(myMap.c_str(), map.c_str()) != 0) // Not on same map
	  {
            m_npcMap[num]->setPos("0", "-1000", "0"); // hide character below map
            continue;
	  }

	  m_npcMap[num]->setPos(x, y, z);
      m_npcMap[num]->setAngle(angle, m_cameraAngle->getStringFromMemory());
  }
  return value;
}

std::string Position::hostGetValue()
{
  std::string myPosValue = "-1,_PLH_,0,0,0,0#";
  
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

  std::string x = m_marioX->getStringFromMemory();
  std::string y = m_marioY->getStringFromMemory();
  std::string z = m_marioZ->getStringFromMemory();

  std::string angle = getAngle();

  std::string value = map + "," + x + "," + y + "," + z + "," + angle;
  return value;
}

void Position::hostHandleUpdate(int id, std::string updateString)
{
  m_hostValues[id] = updateString;
}

void Position::initMarioWatches()
{
  m_marioX = new MemWatchEntry("Mario X", MARIO_PTR, Common::MemType::type_float);
  m_marioX->setBoundToPointer(true);
  m_marioX->addOffset(MARIO_XVAL_OFFSET);

  m_marioY = new MemWatchEntry("Mario Y", MARIO_PTR, Common::MemType::type_float);
  m_marioY->setBoundToPointer(true);
  m_marioY->addOffset(MARIO_XVAL_OFFSET + POS_VAL_DIST);

  m_marioZ = new MemWatchEntry("Mario Z", MARIO_PTR, Common::MemType::type_float);
  m_marioZ->setBoundToPointer(true);
  m_marioZ->addOffset(MARIO_XVAL_OFFSET + (2 * POS_VAL_DIST));

  m_marioAngle = new MemWatchEntry("Mario Angle", MARIO_PTR, Common::MemType::type_float);
  m_marioAngle->setBoundToPointer(true);
  m_marioAngle->addOffset(MARIO_FACING_OFFSET);

  m_cameraAngle = new MemWatchEntry("Camera Angle", MARIO_PTR, Common::MemType::type_float);
  m_cameraAngle->setBoundToPointer(true);
  m_cameraAngle->addOffset(CAMERA_ANGLE_OFFSET);
}

std::string Position::getAngle()
{
  std::string facing = m_marioAngle->getStringFromMemory();
  std::string camera = m_cameraAngle->getStringFromMemory();

  float angle = atof(facing.c_str()) + atof(camera.c_str());

  angle = fmod(angle, 360.0);

  return std::to_string(angle);
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

void Position::prepareNPC(uint8_t num)
{
  if (m_npcMap.count(num) == 0)
  {
    m_npcMap[num] = new NPC(num + 23);
  }

  m_npcMap[num]->update();
}