#include "Position.h"

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

int Position::Priority()
{
  return REALTIME;
}

std::string Position::setValue(std::string value)
{
  bool nowInBattle = inBattle();
  if (nowInBattle && m_wasPreviouslyInBattle)
  {
    return value; // Don't interfere with anything - let them fight
  }
  else if (nowInBattle && !m_wasPreviouslyInBattle)
  {
    // Battle just started, return control of party members (tp them very far away so they get
    // warped back to player)
    for (int slot = 1; slot < 2; slot++)
    {
      m_XWatches[slot]->writeMemoryFromString("-10000");
      m_YWatches[slot]->writeMemoryFromString(m_YWatches[0]->getStringFromMemory());
      m_ZWatches[slot]->writeMemoryFromString("-10000");
      m_FallPeakWatches[slot]->writeMemoryFromString("-10000");
      m_TeleblockerWatches[slot]->writeMemoryFromString("1");
    }

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
    std::string angle = parts[5];

    std::string myMap = "_NOT_IMPL_"; // m_mapWatch->getStringFromMemory();

    if (strcmp(myMap.c_str(), map.c_str()) != 0) // Not on same map
    {
      // m_npcMap[num]->setPos("0", "-1000", "0"); // hide character below map
      // continue;
    }

    if (num <= 1) // Only have enough party members to show nums 0 & 1
    {
      int slot = num + 1;
      m_XWatches[slot]->writeMemoryFromString(x); // m_prevXValues[slot]);
      m_TargetXWatches[slot]->writeMemoryFromString(x); // GetTarget(m_prevXValues[slot], x));

      m_YWatches[slot]->writeMemoryFromString(y);   // m_prevYValues[slot]);
      m_TargetYWatches[slot]->writeMemoryFromString(y); // GetTarget(m_prevYValues[slot], y));

      m_ZWatches[slot]->writeMemoryFromString(z);       // m_prevZValues[slot]);
      m_TargetZWatches[slot]->writeMemoryFromString(z); // GetTarget(m_prevZValues[slot], z));

      m_TargetAngleWatches[slot]->writeMemoryFromString(angle);
      m_CurrentAngleWatches[slot]->writeMemoryFromString(angle); // m_prevAngles[slot]);

      m_FallPeakWatches[slot]->writeMemoryFromString("-10000");
      m_YVelWatches[slot]->writeMemoryFromString("0");
      m_TeleblockerWatches[slot]->writeMemoryFromString("-1"); // signed equivalent of 255

      m_prevXValues[slot] = x;
      m_prevYValues[slot] = y;
      m_prevZValues[slot] = z;
      m_prevAngles[slot] = angle;
    }
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

  std::string x = m_XWatches[0]->getStringFromMemory();
  std::string y = m_YWatches[0]->getStringFromMemory();
  std::string z = m_ZWatches[0]->getStringFromMemory();
  std::string angle = m_TargetAngleWatches[0]->getStringFromMemory();

  if (z == "???") // We don't exist (eg. load transition)
  {
    x = "10000"; // Hide char
    y = "-10000";
    z = "10000";
  }

  std::string value = map + "," + x + "," + y + "," + z + "," + angle;
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

std::string Position::GetTarget(std::string prevVal, std::string newVal)
{
  float prevF = atof(prevVal.c_str());
  float newF = atof(newVal.c_str());
  float diffF = newF - prevF;
  float extraDiff = diffF * TARGET_MULTI;

  return std::to_string(prevF + extraDiff);
}

void Position::initMainPosWatches()
{
  m_battle = new MemWatchEntry("Battle Bool", 0x80C45EF4, Common::MemType::type_byte);
  m_battle->setBoundToPointer(true);
  m_battle->addOffset(0x78);
  m_battle->addOffset(0x18);
  m_battle->addOffset(0x0E);

  for (int slot = 0; slot < PARTY_SIZE; slot++)
  {
    MemWatchEntry* x =
        new MemWatchEntry("PosX", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    x->setBoundToPointer(true);
    x->addOffset(OFFSET_1);
    x->addOffset(OFFSET_2);
    x->addOffset(OFFSET_3);
    x->addOffset(OFFSET_4 + X_OFFSET);

    m_XWatches.push_back(x);

    MemWatchEntry* y =
        new MemWatchEntry("PosY", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    y->setBoundToPointer(true);
    y->addOffset(OFFSET_1);
    y->addOffset(OFFSET_2);
    y->addOffset(OFFSET_3);
    y->addOffset(OFFSET_4 + Y_OFFSET);

    m_YWatches.push_back(y);

    MemWatchEntry* z =
        new MemWatchEntry("PosZ", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    z->setBoundToPointer(true);
    z->addOffset(OFFSET_1);
    z->addOffset(OFFSET_2);
    z->addOffset(OFFSET_3);
    z->addOffset(OFFSET_4 + Z_OFFSET);

    m_ZWatches.push_back(z);

	MemWatchEntry* targetAngle =
        new MemWatchEntry("Target Angle", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    targetAngle->setBoundToPointer(true);
    targetAngle->addOffset(OFFSET_1);
    targetAngle->addOffset(OFFSET_2);
    targetAngle->addOffset(OFFSET_3);
    targetAngle->addOffset(TARGET_ANGLE_OFFSET);

    m_TargetAngleWatches.push_back(targetAngle);

	MemWatchEntry* currentAngle =
        new MemWatchEntry("Current Angle", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    currentAngle->setBoundToPointer(true);
    currentAngle->addOffset(OFFSET_1);
    currentAngle->addOffset(OFFSET_2);
    currentAngle->addOffset(OFFSET_3);
    currentAngle->addOffset(CURRENT_ANGLE_OFFSET);

    m_CurrentAngleWatches.push_back(currentAngle);

    MemWatchEntry* yVel =
        new MemWatchEntry("PosYVel", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    yVel->setBoundToPointer(true);
    yVel->addOffset(OFFSET_1);
    yVel->addOffset(OFFSET_2);
    yVel->addOffset(OFFSET_3);
    yVel->addOffset(Y_VEL_OFFSET);

    m_YVelWatches.push_back(yVel);

    MemWatchEntry* fallPeak =
		new MemWatchEntry("PosFallPeak", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    fallPeak->setBoundToPointer(true);
    fallPeak->addOffset(OFFSET_1);
    fallPeak->addOffset(OFFSET_2);
    fallPeak->addOffset(OFFSET_3);
    fallPeak->addOffset(FALL_PEAK_OFFSET);

    m_FallPeakWatches.push_back(fallPeak);
  }

  m_prevXValues = {"0", "0", "0"};
  m_prevYValues = {"0", "0", "0"};
  m_prevZValues = {"0", "0", "0"};
  m_prevAngles = {"0", "0", "0"};

  // These are invalid/unused for char 0 (Main char)
  m_TeleblockerWatches.push_back(NULL);
  m_TargetXWatches.push_back(NULL);
  m_TargetYWatches.push_back(NULL);
  m_TargetZWatches.push_back(NULL);


  for (int slot = 1; slot < 3; slot++)
  {
    int targetBaseOffset = TARGET_2_OFFSET;
    if (slot == 2)
    {
      targetBaseOffset = TARGET_3_OFFSET;
    }
    MemWatchEntry* teleBlocker =
        new MemWatchEntry("Teleblocker", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_byte);
    teleBlocker->setBoundToPointer(true);
    teleBlocker->addOffset(OFFSET_1);
    teleBlocker->addOffset(OFFSET_2);
    teleBlocker->addOffset(OFFSET_3);
    teleBlocker->addOffset(targetBaseOffset + TELEBLOCKER_OFFSET);

    m_TeleblockerWatches.push_back(teleBlocker);

    MemWatchEntry* targetX =
        new MemWatchEntry("PosX", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    targetX->setBoundToPointer(true);
    targetX->addOffset(OFFSET_1);
    targetX->addOffset(OFFSET_2);
    targetX->addOffset(OFFSET_3);
    targetX->addOffset(targetBaseOffset + X_OFFSET);

    m_TargetXWatches.push_back(targetX);

    MemWatchEntry* targetY =
        new MemWatchEntry("PosY", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    targetY->setBoundToPointer(true);
    targetY->addOffset(OFFSET_1);
    targetY->addOffset(OFFSET_2);
    targetY->addOffset(OFFSET_3);
    targetY->addOffset(targetBaseOffset + Y_OFFSET);

    m_TargetYWatches.push_back(targetY);

    MemWatchEntry* targetZ =
        new MemWatchEntry("PosZ", POS_BASE + (slot * SLOT_SIZE), Common::MemType::type_float);
    targetZ->setBoundToPointer(true);
    targetZ->addOffset(OFFSET_1);
    targetZ->addOffset(OFFSET_2);
    targetZ->addOffset(OFFSET_3);
    targetZ->addOffset(targetBaseOffset + Z_OFFSET);

    m_TargetZWatches.push_back(targetZ);
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