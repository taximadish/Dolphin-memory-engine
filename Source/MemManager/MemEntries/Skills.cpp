#include "Skills.h"

Skills::Skills(bool serverMode)
{
  if (serverMode)
  {
	std::string value = "";
    for (int chara = 0; chara < NUM_CHARACTERS; chara++)
    {
      std::string trees = "";
      for (int tree = 0; tree < NUM_TREES; tree++)
      {
        trees.append("0,");
	  }

      trees = trees.substr(0, trees.length() - 1); // remove trailing comma
      value.append(trees + "!");
	}

    value = value.substr(0, value.length() - 1); // remove trailing '!'

	m_hostValue = value;
  }
  else
  {
	// Init Watches
    for (int chara = 0; chara < NUM_CHARACTERS; chara++)
    {
      u32 charBase = BASE_RESERVE_ADDRESS + (chara * CHARACTER_SIZE);
      std::vector<std::vector<MemWatchEntry*>> trees;
      std::vector<MemWatchEntry*> treeProgresses;
      for (int tree = 0; tree < NUM_TREES; tree++)
      {
        u32 treeBase = charBase + SKILLS_OFFSET + (tree * SKILL_TREE_PERIOD);
        std::vector<MemWatchEntry*> skills;
        for (int skill = 0; skill < NUM_SKILLS; skill++)
        {
          u32 skillAddress = treeBase + (skill * SKILL_PERIOD);
          MemWatchEntry* m = new MemWatchEntry("Skill", skillAddress, Common::MemType::type_word);
          std::string skilly = m->getStringFromMemory();
          skills.push_back(m);
        }
        trees.push_back(skills);
        u32 treeProgressAddress = charBase + TREE_PROGRESS_OFFSET + (tree * TREE_PROGRESS_SIZE);
        treeProgresses.push_back(new MemWatchEntry("Tree Progress", treeProgressAddress, Common::MemType::type_word));
      }
      m_SkillWatches.push_back(trees);
      m_SkillProgressWatches.push_back(treeProgresses);
    }

	// Init SkillSpNeeded
    m_skillSpNeeded.push_back({0, 700, 1000, 2000, 3500}); // Tree 0 (Middle)
    m_skillSpNeeded.push_back({300, 700, 1000, 2000, 3500}); // Tree 1 (Mid Left)
    m_skillSpNeeded.push_back(m_skillSpNeeded[1]); // Tree 2 (Mid Right)
    m_skillSpNeeded.push_back({200, 500, 800, 1000, 1500}); // Tree 3 (Left)
    m_skillSpNeeded.push_back(m_skillSpNeeded[3]); // Tree 4 (Right)
  }
}

std::string Skills::Name()
{
  return "Skills";
}

int Skills::Priority()
{
  return LOW;
}

std::string Skills::setValue(std::string value)
{
  //std::string out = "";
  int skillNo = 0;
  std::vector<std::string> charas = customSplit(value, "!");
  for (int chara = 0; chara < NUM_CHARACTERS; chara++)
  {
    //out.append(std::to_string(chara) + " Char: {");
    std::vector<std::string> trees = customSplit(charas[chara], ",");
    for (int tree = 0; tree < NUM_TREES; tree++)
    {
      int myTreeValue = 0;
      int hostTreeValue = atoi(trees[tree].c_str());
      //out.append("{");
      for (int skill = 0; skill < NUM_SKILLS; skill++)
      {
        skillNo++;
        int pointsLeft = hostTreeValue - myTreeValue;
        int pointsNeeded = m_skillSpNeeded[tree][skill];
        if (pointsLeft >= pointsNeeded)
        {
          int num = (skillNo == 176 ? 51 : skillNo); // There's no skill 176 (M-Fiora's 1st = H-Fiora's 1st (51))
          m_SkillWatches[chara][tree][skill]->writeMemoryFromString(std::to_string(num));
          myTreeValue += pointsNeeded;
        }
        else
        {
          m_SkillWatches[chara][tree][skill]->writeMemoryFromString("0");
		}
        //std::string m = m_SkillWatches[chara][tree][skill]->getStringFromMemory();
        //out.append(m + ",");
	  }
      int pointsLeft = hostTreeValue - myTreeValue;
      m_SkillProgressWatches[chara][tree]->writeMemoryFromString(std::to_string(pointsLeft));
      //out.append("}");
	}
    //out.append("}\n");
  }

  return value;
}

std::string Skills::hostGetValue()
{
  return m_hostValue;
}

std::string Skills::getUpdate(std::string hostVal)
{
  std::string updates = "";
  std::vector<std::string> charas = customSplit(hostVal, "!");
  for (int chara = 0; chara < NUM_CHARACTERS; chara++)
  {
    std::vector<std::string> trees = customSplit(charas[chara], ",");
    for (int tree = 0; tree < NUM_TREES; tree++)
    {
      int myTreeValue = 0;
      for (int skill = 0; skill < NUM_SKILLS; skill++)
      {
        int mySkill = atoi(m_SkillWatches[chara][tree][skill]->getStringFromMemory().c_str());
        if (mySkill != 0)
        { // We have the skill
          myTreeValue += m_skillSpNeeded[tree][skill];
		}
      }
      myTreeValue += atoi(m_SkillProgressWatches[chara][tree]->getStringFromMemory().c_str());
      int hostTreeValue = atoi(trees[tree].c_str());
      int pointsDiff = myTreeValue - hostTreeValue;
      updates.append(std::to_string(pointsDiff) + ",");
    }
    updates = updates.substr(0, updates.length() - 1); // remove trailing comma
    updates.append("!");
  }
  updates = updates.substr(0, updates.length() - 1); // remove trailing '!'
  return updates;
}

void Skills::hostHandleUpdate(int id, std::string updateString)
{
  std::string newValue = "";
  std::vector<std::string> charas = customSplit(updateString, "!");
  std::vector<std::string> hostCharas = customSplit(m_hostValue, "!");
  for (int chara = 0; chara < NUM_CHARACTERS; chara++)
  {
    std::vector<std::string> trees = customSplit(charas[chara], ",");
    std::vector<std::string> hostTrees = customSplit(hostCharas[chara], ",");
    for (int tree = 0; tree < NUM_TREES; tree++)
    {
      int valAdded = atoi(trees[tree].c_str());
      int hostVal = atoi(hostTrees[tree].c_str());
      newValue.append(std::to_string(hostVal + valAdded) + ",");
    }
    newValue = newValue.substr(0, newValue.length() - 1); // remove trailing comma
    newValue.append("!");
  }
  newValue = newValue.substr(0, newValue.length() - 1); // remove trailing '!'

  m_hostValue = newValue;
}


std::vector<std::string> Skills::customSplit(std::string s, std::string delim)
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