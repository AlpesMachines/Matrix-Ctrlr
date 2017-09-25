#include <EEPROM.h>
#include "oner.h"


///////////////////////////////////////////////////////////
// ZONE : Split a single channel into 4 channels with Limits LM_Z
// (could be assigned to ui_variables)
///////////////////////////////////////////////////////////
const unsigned char defaultZONE[4][5] PROGMEM =
{ //rCH, Lo, transpose, Hi, tCH
  {1, 0, 5, 127, 1}, // zone A
  {2, 0, 5, 127, 2}, // zone B
  {3, 0, 5, 127, 3}, // zone C
  {4, 0, 5, 127, 4} // zone D
};

// zone arrays
unsigned char ZONE[4][5];

bool zActive;
unsigned char ui_Zone;

///////////////////////////////////////////////////////////
// initialise zone parameters with default value (PROGMEM of an array)
///////////////////////////////////////////////////////////
void ZONE_Init(void)
{
#if DEBUG_router
  Serial.println(F("ZONE_Init()"));
#endif

  for (unsigned char i = 0; i < 4; i++) {
    for (unsigned char j = 0; j < 5; j++) {
      ZONE[i][j] = pgm_read_byte_near(&defaultZONE[i][j]);
    }
  }

  zActive = false;

  ZONE_Save();
}

///////////////////////////////////////////////////////////
// load zone parameters
///////////////////////////////////////////////////////////
void ZONE_Load(void)
{
#if DEBUG_router
  Serial.println(F("ZONE_Load()"));
#endif

  // read EEPROM values of ZONE[][]:
  for (unsigned char i = 0; i < 4; i++)
  {
    for (unsigned char j = 0; i < 5; j++)
    {
      ZONE[i][j] = EEPROM.read(EEPROM_ZONE + 5 * i + j); // addr + 4x5
    }
  }

  // if ZONE[][] is outbound limits, get default value :
  for (unsigned char i = 0; i < 4; i++)
  {
    if (ZONE[i][0] < 1 || ZONE[i][0] > 16) { // midi channel 1- 16
      ZONE[i][0] = pgm_read_byte_near(&defaultZONE[i][0]);
    }
    if (ZONE[i][1] < 0 || ZONE[i][1] > 127) { // lo limit 0
      ZONE[i][1] = pgm_read_byte_near(&defaultZONE[i][1]);
    }
    if (ZONE[i][2] < 0 || ZONE[i][2] > 5) { // transpose +/-5
      ZONE[i][2] = pgm_read_byte_near(&defaultZONE[i][2]);
    }
    if (ZONE[i][3] < 0 || ZONE[i][3] > 127) { // transpose +/-5
      ZONE[i][3] = pgm_read_byte_near(&defaultZONE[i][3]);
    }
    if (ZONE[i][4] < 1 || ZONE[i][4] > 16) { // midi channel 1- 16
      ZONE[i][4] = pgm_read_byte_near(&defaultZONE[i][4]);
    }
  }

  zActive = EEPROM.read(EEPROM_Z_ACTIVE);
}

///////////////////////////////////////////////////////////
// save zone parameters into arduino EEPROM
///////////////////////////////////////////////////////////
void ZONE_Save(void)
{
#if DEBUG_router
  Serial.println(F("ZONE_Save()"));
#endif

  for (unsigned char i = 0; i < 4; i++) {
    for (unsigned char j = 0; j < 5; j++) {
      EEPROM.update(EEPROM_ZONE + 5 * i + j, ZONE[i][j] ); // 20 bytes
    }
  }

  EEPROM.update(EEPROM_Z_ACTIVE, zActive);
}

///////////////////////////////////////////////////////////
// assign and route a noteOff to a zone
///////////////////////////////////////////////////////////
void ZoneNoteOff(byte channel, byte pitch, byte velocity)
{
#if DEBUG_router
  Serial.println(F("ZoneNoteOff()"));
#endif

  MIDI_Incoming = true; // show that we receive midi message

  if (zActive == true)
  {
    for (unsigned char z = ZONEA; z <= ZONED; ++z) {
      if ((channel == ZONE[z][Z_RCH]) && (pitch >= ZONE[z][Z_LOW]) && (pitch <= ZONE[z][Z_HIGH]) )
      {
        HandleNoteOff(ZONE[z][Z_TCH], pitch + (ZONE[z][Z_TRANSPOSE] - Z_TRANSPOSE_MIDDLE) * 12, velocity);
      }
    }
  }
  else
    HandleNoteOff(channel, pitch, velocity);
}

///////////////////////////////////////////////////////////
// assign and route a noteOff to a zone
///////////////////////////////////////////////////////////
void ZoneNoteOn(byte channel, byte pitch, byte velocity)
{
#if DEBUG_router
  Serial.println(F("ZoneNoteOn()"));
#endif

  MIDI_Incoming = true; // show that we receive midi message

  if (zActive == true) {
    for (unsigned char z = ZONEA; z <= ZONED; ++z)
    {
      if ((channel == ZONE[z][Z_RCH]) && (pitch >= ZONE[z][Z_LOW]) && (pitch <= ZONE[z][Z_HIGH]) ) { // rCH ZONEA, pitch between bounds et Zone activated
        HandleNoteOn(ZONE[z][Z_TCH], pitch + (ZONE[z][Z_TRANSPOSE] - Z_TRANSPOSE_MIDDLE) * 12, velocity);
      }
    }
  }
  else
    HandleNoteOn(channel, pitch, velocity);

}

