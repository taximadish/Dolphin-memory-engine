#include "NPC.h"

#define MIN_ANGLE		10

#define NPC_SIZE		0x340

#define MAX_NPC_NUM		30
/*Seems to dynamically grow, if something is in slot 30, then slot 99 enters existence*/

#define BASE_ADDRESS	0x80AF2E60

#define FLAGS_1_OFFSET	0x03
#define FLAGS_2_OFFSET	0x1F7

#define ANIM_PTR_OFFSET	0x28

#define X_POS_OFFSET	0x8C
#define Y_POS_OFFSET	0x90
#define Z_POS_OFFSET	0x94

#define WIDTH_OFFSET	0xE0
#define HEIGHT_OFFSET	0xE4
#define DEPTH_OFFSET	0xE8

#define MODEL_NUM_OFFSET	0x107

#define RENDER_PTR_OFFSET	0x110

#define RED_OFFSET			0x114
#define GREEN_OFFSET		0x115
#define BLUE_OFFSET			0x116
#define ALPHA_OFFSET		0x117

#define TALK_PTR_OFFSET		0x128

#define ANGLE_OFFSET		0x144

#define HITBOX_RADIUS_OFFSET	0x14C
#define HITBOX_HEIGHT_OFFSET	0x150
#define SHADOW_RADIUS_OFFSET	0x154

#define STAND_STILL_OFFSET		0x1D5

NPC::NPC(int num)
{
  int startAddress = BASE_ADDRESS + (NPC_SIZE * num);

  m_entireNpcWatch = new MemWatchEntry("NPC", startAddress, Common::MemType::type_byteArray,
										Common::MemBase::base_hexadecimal, true, NPC_SIZE);

  m_flags1Watch = new MemWatchEntry("NPC Flags 1", startAddress + FLAGS_1_OFFSET, Common::MemType::type_byte);
  m_flags2Watch = new MemWatchEntry("NPC Flags 2", startAddress + FLAGS_2_OFFSET, Common::MemType::type_byte);

  m_xWatch = new MemWatchEntry("NPC X", startAddress + X_POS_OFFSET, Common::MemType::type_float);
  m_yWatch = new MemWatchEntry("NPC Y", startAddress + Y_POS_OFFSET, Common::MemType::type_float);
  m_zWatch = new MemWatchEntry("NPC Z", startAddress + Z_POS_OFFSET, Common::MemType::type_float);

  m_angle = new MemWatchEntry("NPC Angle", startAddress + ANGLE_OFFSET, Common::MemType::type_float);

  m_widthWatch = new MemWatchEntry("NPC Width", startAddress + WIDTH_OFFSET, Common::MemType::type_float);
  m_heightWatch = new MemWatchEntry("NPC Height", startAddress + HEIGHT_OFFSET, Common::MemType::type_float);
  m_depthWatch = new MemWatchEntry("NPC Depth", startAddress + DEPTH_OFFSET, Common::MemType::type_float);

  m_colorRedWatch = new MemWatchEntry("Red Lighting", startAddress + RED_OFFSET, Common::MemType::type_byte);
  m_colorGreenWatch = new MemWatchEntry("Green Lighting", startAddress + GREEN_OFFSET, Common::MemType::type_byte);
  m_colorBlueWatch = new MemWatchEntry("Blue Lighting", startAddress + BLUE_OFFSET, Common::MemType::type_byte);
  m_colorAlphaWatch = new MemWatchEntry("Alpha", startAddress + ALPHA_OFFSET, Common::MemType::type_byte);

  m_modelNumWatch = new MemWatchEntry("NPC Model Num", startAddress + MODEL_NUM_OFFSET, Common::MemType::type_byte);

  m_renderPtrWatch = new MemWatchEntry("NPC Renderer", startAddress + RENDER_PTR_OFFSET, Common::MemType::type_word);

  m_talkTextPtrWatch = new MemWatchEntry("NPC Talk Text", startAddress + TALK_PTR_OFFSET, Common::MemType::type_word);

  m_animPtrWatch = new MemWatchEntry("NPC AnimPtr", startAddress + ANIM_PTR_OFFSET, Common::MemType::type_word);
}

void NPC::update() // Maintenance function, to be called every cycle
{
  m_animPtrWatch->writeMemoryFromString("0"); // seems to prevent crashes

  m_flags1Watch->writeMemoryFromString("39"); // 2^0 + 2^1 + 2^2 + 2^6 (loaded + exists + NPCtype + noHitbox)
  m_flags2Watch->writeMemoryFromString("4"); // 2^2 (unknown, doesn't work without)
  m_modelNumWatch->writeMemoryFromString("1"); // Mario facing forward+left

  m_depthWatch->writeMemoryFromString("1");
  m_heightWatch->writeMemoryFromString("1");

  m_talkTextPtrWatch->writeMemoryFromString("0"); // No talk text

  m_colorRedWatch->writeMemoryFromString("-1"); // =255, max lighting
  m_colorGreenWatch->writeMemoryFromString("-1"); // =255, max lighting
  m_colorBlueWatch->writeMemoryFromString("-1"); // =255, max lighting
  m_colorAlphaWatch->writeMemoryFromString("-1");   // =255, fully opaque

  m_renderPtrWatch->writeMemoryFromString("-1"); // Required, unsure why
}

void NPC::setPos(std::string x, std::string y, std::string z)
{
  m_xWatch->writeMemoryFromString(x);
  m_yWatch->writeMemoryFromString(y);
  m_zWatch->writeMemoryFromString(z);
}

void NPC::setAngle(std::string angle, std::string camAngle)
{
  float fAngle = atof(angle.c_str());
  float fCamAngle = atof(camAngle.c_str());

  float relAngle = fAngle - fCamAngle;
  relAngle = fmod(relAngle, 360.0);

  if (relAngle < 0)
    relAngle += 360.0; // always want positive value

  // Adjust so never completely perpendicular to camera
  if (relAngle < 90 + MIN_ANGLE && relAngle > 90 - MIN_ANGLE)
  {
    fAngle -= MIN_ANGLE;
    relAngle -= MIN_ANGLE;
  }
  else if (relAngle < 270 + MIN_ANGLE && relAngle > 270 - MIN_ANGLE)
  {
    fAngle += MIN_ANGLE;
    relAngle += MIN_ANGLE;
  }

  if (relAngle > 90 && relAngle < 270) // facing away from camera
  {
    fAngle = fmod((360 - (fAngle + 90)) - 90, 360); // Do some maths to make mario face camera
    m_widthWatch->writeMemoryFromString("-1");
  }
  else
  {
    m_widthWatch->writeMemoryFromString("1");
  }

  m_angle->writeMemoryFromString(std::to_string(fAngle));
}