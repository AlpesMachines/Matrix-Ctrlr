#include "chaosmatrix.h"
#include <avr/pgmspace.h>
#include "lcd.h"

unsigned char EditBuffer[4][134];
unsigned char EditBufferOrig [134];

unsigned char UnisonDetune[4];
unsigned char UnisonDetuneOrig;

unsigned char GlobalParameters[172];
//unsigned char GlobalParameters_6[236]; // NOT IMPLEMENTED (too complicated, use too much RAM)
byte SystemConfig[47];

unsigned char ProgramNumber; // patches
unsigned char ProgramNumberEdit; // patches
unsigned char BankNumber;
unsigned char BankNumberEdit;
//unsigned char Osc_Pulse_Modulate[NUM_OSCILLATORS]; // keey track of pulse led, blink enabled
unsigned char FilterCutoffMode; // 0=sysex, 1=lever 3 (continuous controller)
unsigned char FilterSustainMode;
//unsigned char Osc1_ClickMode; // DCO1 click button toggles click . 0= off, 1=click
unsigned char Osc2_ClickMode; // DCO2 click button toggles click and noise waveform. 0=both off, 1=click, 2=noise


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constantes
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte DefaultSystemConfig[47 ] PROGMEM = {
  1, // midi channel
  0, // filter sustain hack
  0, // last bankA
  0, // last patchA
  0, // last device
  1, // last matrixmodel A
  0, // last matrix modelB
  1, // last matrix modelC
  1, // last matrix modelD
  0, // encoder inverted
  0, // mthru xcc
  0, // internal clock
  0, // last bankB
  1, // last patchB
  0, // last bankC
  2, // last patchC
  0, // last bankD
  3, // last patchD

  1, // EEPROM ZoneA
  0, // EEPROM ZoneA
  5, // EEPROM ZoneA
  127, // EEPROM ZoneA
  1, // EEPROM ZoneA

  2, // EEPROM ZoneB
  0, // EEPROM ZoneB
  5, // EEPROM ZoneB
  127, // EEPROM ZoneB
  2, // EEPROM ZoneB

  3, // EEPROM ZoneC
  0, // EEPROM ZoneC
  5, // EEPROM ZoneC
  127, // EEPROM ZoneC
  3, // EEPROM ZoneC

  4, // EEPROM ZoneD
  0, // EEPROM ZoneD
  5, // EEPROM ZoneD
  127, // EEPROM ZoneD
  4, // EEPROM ZoneD

  0, // EEPROM_Z_ACTIVE
  1, // localControl
  0, 0, 0, 0, 0, 0, 0 // 7 free parameters

};


const unsigned char singlePatchDataFormatCC[100][9] PROGMEM = {
  // big excel table TO DO
  //{ID, Name, Param, EB, CC nbr, bits, Value min, Value max, CC_Complement},
  //  {1, SX_OSC1_FREQ, 0, 9, 9, 6, 0, 63, 0},
  //  {78, SX_OSC1_FREQBYLFO1AMOUNT, 1, 86, 70, SIGNED7, 0, 127, 64},
  //  {17, SX_OSC_SYNCMODE, 2, 25, 104, 2, 0, 3, 0},
  //  {3, SX_OSC1_PULSEWIDTH, 3, 11, 11, 6, 0, 63, 0},
  //  {79, SX_OSC1_PWBYLFO2AMOUNT, 4, 87, 71, SIGNED7, 0, 127, 64},
  //  {2, SX_OSC1_WAVESHAPE, 5, 10, 10, 6, 0, 63, 0},
  //  {5, SX_OSC1_WAVEFORM, 6, 13, 13, 2, 0, 3, 0},
  //  {4, SX_OSC1_LEVERMOD, 7, 12, 12, 2, 0, 3, 0},
  //  {13, SX_OSC1_KEYBOARDMOD, 8, 21, 102, 2, 0, 3, 0},
  //  {14, SX_OSC1_CLICK, 9, 22, 80, 1, 0, 1, 0},
  //  {6, SX_OSC2_FREQ, 10, 14, 14, 6, 0, 63, 0},
  //  {80, SX_OSC2_FREQBYLFO1AMOUNT, 11, 88, 72, SIGNED7, 0, 127, 64},
  //  {11, SX_OSC2_DETUNE, 12, 19, 19, 6, 0, 63, 0},
  //  {8, SX_OSC2_PULSEWIDTH, 13, 16, 16, 6, 0, 63, 0},
  //  {81, SX_OSC2_PWBYLFO2AMOUNT, 14, 89, 73, SIGNED7, 0, 127, 64},
  //  {7, SX_OSC2_WAVESHAPE, 15, 15, 15, 6, 0, 63, 0},
  //  {10, SX_OSC2_WAVEFORM, 16, 18, 18, 3, 0, 3, 0},
  //  {9, SX_OSC2_LEVERMOD, 17, 17, 17, 2, 0, 3, 0},
  //  {15, SX_OSC2_KEYBOARDMOD, 18, 23, 103, 2, 0, 3, 0},
  //  {16, SX_OSC2_CLICK, 19, 24, 81, 1, 0, 1, 0},
  //  {12, SX_MIX, 20, 20, 8, 6, 0, 63, 0},
  //  {18, SX_FILTER_FREQ, 21, 26, 105, 7, 0, 127, 0},
  //  {82, SX_FILTER_FREQBYENV1AMOUNT, 22, 90, 74, SIGNED7, 0, 127, 64},
  //  {83, SX_FILTER_FREQBYPRESSURE, 23, 91, 75, SIGNED7, 0, 127, 64},
  //  {19, SX_FILTER_RESONANCE, 24, 27, 106, 6, 0, 63, 0},
  //  {20, SX_FILTER_LEVERMOD, 25, 28, 107, 2, 0, 3, 0},
  //  {21, SX_FILTER_KEYBOARDMOD, 26, 29, 108, 2, 0, 3, 0},
  //  {23, SX_VCA1_AMOUNT, 27, 31, 110, 6, 0, 63, 0},
  //  {84, SX_VCA1_AMPBYVELOCITY, 28, 92, 76, SIGNED7, 0, 127, 64},
  //  {85, SX_VCA2_AMPBYENV2, 29, 93, 77, SIGNED7, 0, 127, 64},
  //  {22, SX_FILTER_FM_AMOUNT, 30, 30, 109, 6, 0, 63, 0},
  //  {92, SX_FILTER_FMBYENV3, 31, 100, 88, SIGNED7, 0, 127, 64},
  //  {93, SX_FILTER_FMBYPRESSURE, 32, 101, 89, SIGNED7, 0, 127, 64},
  //  {68, SX_TRACK_INPUT, 33, 76, 57, 5, 0, 31, 0},
  //  {69, SX_TRACK_POINT1, 34, 77, 58, 6, 0, 63, 0},
  //  {70, SX_TRACK_POINT2, 35, 78, 59, 6, 0, 63, 0},
  //  {71, SX_TRACK_POINT3, 36, 79, 60, 6, 0, 63, 0},
  //  {72, SX_TRACK_POINT4, 37, 80, 61, 6, 0, 63, 0},
  //  {73, SX_TRACK_POINT5, 38, 81, 62, 6, 0, 63, 0},
  //  {74, SX_RAMP1_RATE, 40, 82, 63, 6, 0, 63, 0},
  //  {75, SX_RAMP1_MODE, 41, 83, 38, 2, 0, 3, 0},
  //  {76, SX_RAMP2_RATE, 42, 84, 39, 6, 0, 63, 0},
  //  {77, SX_RAMP2_MODE, 43, 85, 40, 2, 0, 3, 0},
  //  {24, SX_PORTAMENTO_RATE, 44, 32, 5, 6, 0, 63, 0},
  //  {91, SX_PORTAMENTO_SPEEDBYVELOCITY, 45, 99, 87, SIGNED7, 0, 127, 64},
  //  {25, SX_PORTAMENTO_MODE, 46, 33, 37, 2, 0, 3, 0},
  //  {26, SX_PORTAMENTO_LEGATOENABLE, 47, 34, 65, 1, 0, 1, 0},
  //  {0, SX_KEYBOARD_MODE, 48, 8, 95, 2, 0, 3, 0},
  //  {42, SX_ENV1_DELAY, 50, 50, 21, 6, 0, 63, 0},
  //  {43, SX_ENV1_ATTACK, 51, 51, 22, 6, 0, 63, 0},
  //  {44, SX_ENV1_DECAY, 52, 52, 23, 6, 0, 63, 0},
  //  {45, SX_ENV1_SUSTAIN, 53, 53, 24, 6, 0, 63, 0},
  //  {46, SX_ENV1_RELEASE, 54, 54, 25, 6, 0, 63, 0},
  //  {47, SX_ENV1_AMPLITUDE, 55, 55, 26, 6, 0, 63, 0},
  //  {86, SX_ENV1_AMPBYVELO, 56, 94, 78, SIGNED7, 0, 127, 64},
  //  {41, SX_ENV1_TRIGGERMODE, 57, 49, 20, 3, 0, 7, 0},
  //  {49, SX_ENV1_RELEASEMODE, 58, 57, 28, 2, 0, 3, 0},
  //  {48, SX_ENV1_LFOTRIGGERMODE, 59, 56, 27, 2, 0, 3, 0},
  //  {51, SX_ENV2_DELAY, 60, 59, 30, 6, 0, 63, 0},
  //  {52, SX_ENV2_ATTACK, 61, 60, 41, 6, 0, 63, 0},
  //  {53, SX_ENV2_DECAY, 62, 61, 42, 6, 0, 63, 0},
  //  {54, SX_ENV2_SUSTAIN, 63, 62, 43, 6, 0, 63, 0},
  //  {55, SX_ENV2_RELEASE, 64, 63, 44, 6, 0, 63, 0},
  //  {56, SX_ENV2_AMPLITUDE, 65, 64, 45, 6, 0, 63, 0},
  //  {87, SX_ENV2_AMPBYVELO, 66, 95, 79, SIGNED7, 0, 127, 64},
  //  {50, SX_ENV2_TRIGGERMODE, 67, 58, 29, 3, 0, 7, 0},
  //  {58, SX_ENV2_RELEASEMODE, 68, 66, 47, 2, 0, 3, 0},
  //  {57, SX_ENV2_LFOTRIGGERMODE, 69, 65, 46, 2, 0, 3, 0},
  //  {60, SX_ENV3_DELAY, 70, 68, 49, 6, 0, 63, 0},
  //  {61, SX_ENV3_ATTACK, 71, 69, 50, 6, 0, 63, 0},
  //  {62, SX_ENV3_DECAY, 72, 70, 51, 6, 0, 63, 0},
  //  {63, SX_ENV3_SUSTAIN, 73, 71, 52, 6, 0, 63, 0},
  //  {64, SX_ENV3_RELEASE, 74, 72, 53, 6, 0, 63, 0},
  //  {65, SX_ENV3_AMPLITUDE, 75, 73, 54, 6, 0, 63, 0},
  //  {88, SX_ENV3_AMPBYVELO, 76, 96, 84, SIGNED7, 0, 127, 64},
  //  {59, SX_ENV3_TRIGGERMODE, 77, 67, 48, 3, 0, 7, 0},
  //  {67, SX_ENV3_RELEASEMODE, 78, 75, 56, 2, 0, 3, 0},
  //  {66, SX_ENV3_LFOTRIGGERMODE, 79, 74, 55, 2, 0, 3, 0},
  //  {27, SX_LFO1_SPEED, 80, 35, 111, 6, 0, 63, 0},
  //  {94, SX_LFO1_SPEEDMODULATION, 81, 102, 90, SIGNED7, 0, 127, 64},
  //  {30, SX_LFO1_WAVESHAPE, 82, 38, 113, 3, 0, 7, 0},
  //  {31, SX_LFO1_RETRIGGERPOINT, 83, 39, 114, 5, 0, 31, 0},
  //  {33, SX_LFO1_AMPLITUDE, 84, 41, 116, 6, 0, 63, 0},
  //  {89, SX_LFO1_AMPMODULATION, 85, 97, 85, SIGNED7, 0, 127, 64},
  //  {28, SX_LFO1_TRIGGERMODE, 86, 36, 112, 2, 0, 3, 0},
  //  {29, SX_LFO1_LAGENABLE, 87, 37, 82, 1, 0, 1, 0},
  //  {32, SX_LFO1_SAMPLEDSOURCE, 88, 40, 115, 5, 0, 31, 0},
  //  {34, SX_LFO2_SPEED, 90, 42, 117, 6, 0, 63, 0},
  //  {95, SX_LFO2_SPEEDMODULATION, 91, 103, 91, SIGNED7, 0, 127, 64},
  //  {37, SX_LFO2_WAVESHAPE, 92, 45, 33, 3, 0, 7, 0},
  //  {38, SX_LFO2_RETRIGGERPOINT, 93, 46, 34, 5, 0, 31, 0},
  //  {40, SX_LFO2_AMPLITUDE, 94, 48, 36, 6, 0, 63, 0},
  //  {90, SX_LFO2_AMPMODULATION, 95, 98, 86, SIGNED7, 0, 127, 64},
  //  {35, SX_LFO2_TRIGGERMODE, 96, 43, 118, 2, 0, 3, 0},
  //  {96, SX_UNISON_DETUNE, 96, 104, 94, 7, 0, 127, 0},
  //  {36, SX_LFO2_LAGENABLE, 97, 44, 83, 1, 0, 1, 0},
  //  {39, SX_LFO2_SAMPLEDSOURCE, 98, 47, 35, 5, 0, 31, 0}
  {1, SX_OSC1_FREQ, 0, 9, 9, 6, 0, 63, 0},
  {78, SX_OSC1_FREQBYLFO1AMOUNT, 1, 86, 70, SIGNED7, 0, 127, 64},
  {17, SX_OSC_SYNCMODE, 2, 25, 104, 2, 0, 3, 0},
  {3, SX_OSC1_PULSEWIDTH, 3, 11, 11, 6, 0, 63, 0},
  {79, SX_OSC1_PWBYLFO2AMOUNT, 4, 87, 71, SIGNED7, 0, 127, 64},
  {2, SX_OSC1_WAVESHAPE, 5, 10, 10, 6, 0, 63, 0},
  {5, SX_OSC1_WAVEFORM, 6, 13, 13, 2, 0, 3, 0},
  {4, SX_OSC1_LEVERMOD, 7, 12, 12, 2, 0, 3, 0},
  {13, SX_OSC1_KEYBOARDMOD, 8, 21, 102, 2, 0, 3, 0},
  {14, SX_OSC1_CLICK, 9, 22, 80, 1, 0, 1, 0},
  {6, SX_OSC2_FREQ, 10, 14, 14, 6, 0, 63, 0},
  {80, SX_OSC2_FREQBYLFO1AMOUNT, 11, 88, 72, SIGNED7, 0, 127, 64},
  {11, SX_OSC2_DETUNE, 12, 19, 19, 6, 0, 63, 0},
  {8, SX_OSC2_PULSEWIDTH, 13, 16, 16, 6, 0, 63, 0},
  {81, SX_OSC2_PWBYLFO2AMOUNT, 14, 89, 73, SIGNED7, 0, 127, 64},
  {7, SX_OSC2_WAVESHAPE, 15, 15, 15, 6, 0, 63, 0},
  {10, SX_OSC2_WAVEFORM, 16, 18, 18, 3, 0, 3, 0},
  {9, SX_OSC2_LEVERMOD, 17, 17, 17, 2, 0, 3, 0},
  {15, SX_OSC2_KEYBOARDMOD, 18, 23, 103, 2, 0, 3, 0},
  {16, SX_OSC2_CLICK, 19, 24, 81, 1, 0, 1, 0},
  {12, SX_MIX, 20, 20, 8, 6, 0, 63, 0},
  {18, SX_FILTER_FREQ, 21, 26, 105, 7, 0, 127, 0},
  {82, SX_FILTER_FREQBYENV1AMOUNT, 22, 90, 74, SIGNED7, 0, 127, 64},
  {83, SX_FILTER_FREQBYPRESSURE, 23, 91, 75, SIGNED7, 0, 127, 64},
  {19, SX_FILTER_RESONANCE, 24, 27, 106, 6, 0, 63, 0},
  {20, SX_FILTER_LEVERMOD, 25, 28, 107, 2, 0, 3, 0},
  {21, SX_FILTER_KEYBOARDMOD, 26, 29, 108, 2, 0, 3, 0},
  {23, SX_VCA1_AMOUNT, 27, 31, 110, 6, 0, 63, 0},
  {84, SX_VCA1_AMPBYVELOCITY, 28, 92, 76, SIGNED7, 0, 127, 64},
  {85, SX_VCA2_AMPBYENV2, 29, 93, 77, SIGNED7, 0, 127, 64},
  {22, SX_FILTER_FM_AMOUNT, 30, 30, 109, 6, 0, 63, 0},
  {92, SX_FILTER_FMBYENV3, 31, 100, 88, SIGNED7, 0, 127, 64},
  {93, SX_FILTER_FMBYPRESSURE, 32, 101, 89, SIGNED7, 0, 127, 64},
  {68, SX_TRACK_INPUT, 33, 76, 57, 5, 0, 31, 0},
  {69, SX_TRACK_POINT1, 34, 77, 58, 6, 0, 63, 0},
  {70, SX_TRACK_POINT2, 35, 78, 59, 6, 0, 63, 0},
  {71, SX_TRACK_POINT3, 36, 79, 60, 6, 0, 63, 0},
  {72, SX_TRACK_POINT4, 37, 80, 61, 6, 0, 63, 0},
  {73, SX_TRACK_POINT5, 38, 81, 62, 6, 0, 63, 0},
  {127, SX_NULL, 39, 0, 0, 0, 0, 63, 0}, // param 39 don't exist
  {74, SX_RAMP1_RATE, 40, 82, 63, 6, 0, 63, 0},
  {75, SX_RAMP1_MODE, 41, 83, 38, 2, 0, 3, 0},
  {76, SX_RAMP2_RATE, 42, 84, 39, 6, 0, 63, 0},
  {77, SX_RAMP2_MODE, 43, 85, 40, 2, 0, 3, 0},
  {24, SX_PORTAMENTO_RATE, 44, 32, 5, 6, 0, 63, 0},
  {91, SX_PORTAMENTO_SPEEDBYVELOCITY, 45, 99, 87, SIGNED7, 0, 127, 64},
  {25, SX_PORTAMENTO_MODE, 46, 33, 37, 2, 0, 3, 0},
  {26, SX_PORTAMENTO_LEGATOENABLE, 47, 34, 65, 1, 0, 1, 0},
  {0, SX_KEYBOARD_MODE, 48, 8, 95, 2, 0, 3, 0},
  {127, SX_NULL, 49, 0, 0, 0, 0, 63, 0}, // param 49 don't exist
  {42, SX_ENV1_DELAY, 50, 50, 21, 6, 0, 63, 0},
  {43, SX_ENV1_ATTACK, 51, 51, 22, 6, 0, 63, 0},
  {44, SX_ENV1_DECAY, 52, 52, 23, 6, 0, 63, 0},
  {45, SX_ENV1_SUSTAIN, 53, 53, 24, 6, 0, 63, 0},
  {46, SX_ENV1_RELEASE, 54, 54, 25, 6, 0, 63, 0},
  {47, SX_ENV1_AMPLITUDE, 55, 55, 26, 6, 0, 63, 0},
  {86, SX_ENV1_AMPBYVELO, 56, 94, 78, SIGNED7, 0, 127, 64},
  {41, SX_ENV1_TRIGGERMODE, 57, 49, 20, 3, 0, 7, 0},
  {49, SX_ENV1_RELEASEMODE, 58, 57, 28, 2, 0, 3, 0},
  {48, SX_ENV1_LFOTRIGGERMODE, 59, 56, 27, 2, 0, 3, 0},
  {51, SX_ENV2_DELAY, 60, 59, 30, 6, 0, 63, 0},
  {52, SX_ENV2_ATTACK, 61, 60, 41, 6, 0, 63, 0},
  {53, SX_ENV2_DECAY, 62, 61, 42, 6, 0, 63, 0},
  {54, SX_ENV2_SUSTAIN, 63, 62, 43, 6, 0, 63, 0},
  {55, SX_ENV2_RELEASE, 64, 63, 44, 6, 0, 63, 0},
  {56, SX_ENV2_AMPLITUDE, 65, 64, 45, 6, 0, 63, 0},
  {87, SX_ENV2_AMPBYVELO, 66, 95, 79, SIGNED7, 0, 127, 64},
  {50, SX_ENV2_TRIGGERMODE, 67, 58, 29, 3, 0, 7, 0},
  {58, SX_ENV2_RELEASEMODE, 68, 66, 47, 2, 0, 3, 0},
  {57, SX_ENV2_LFOTRIGGERMODE, 69, 65, 46, 2, 0, 3, 0},
  {60, SX_ENV3_DELAY, 70, 68, 49, 6, 0, 63, 0},
  {61, SX_ENV3_ATTACK, 71, 69, 50, 6, 0, 63, 0},
  {62, SX_ENV3_DECAY, 72, 70, 51, 6, 0, 63, 0},
  {63, SX_ENV3_SUSTAIN, 73, 71, 52, 6, 0, 63, 0},
  {64, SX_ENV3_RELEASE, 74, 72, 53, 6, 0, 63, 0},
  {65, SX_ENV3_AMPLITUDE, 75, 73, 54, 6, 0, 63, 0},
  {88, SX_ENV3_AMPBYVELO, 76, 96, 84, SIGNED7, 0, 127, 64},
  {59, SX_ENV3_TRIGGERMODE, 77, 67, 48, 3, 0, 7, 0},
  {67, SX_ENV3_RELEASEMODE, 78, 75, 56, 2, 0, 3, 0},
  {66, SX_ENV3_LFOTRIGGERMODE, 79, 74, 55, 2, 0, 3, 0},
  {27, SX_LFO1_SPEED, 80, 35, 111, 6, 0, 63, 0},
  {94, SX_LFO1_SPEEDMODULATION, 81, 102, 90, SIGNED7, 0, 127, 64},
  {30, SX_LFO1_WAVESHAPE, 82, 38, 113, 3, 0, 7, 0},
  {31, SX_LFO1_RETRIGGERPOINT, 83, 39, 114, 5, 0, 31, 0},
  {33, SX_LFO1_AMPLITUDE, 84, 41, 116, 6, 0, 63, 0},
  {89, SX_LFO1_AMPMODULATION, 85, 97, 85, SIGNED7, 0, 127, 64},
  {28, SX_LFO1_TRIGGERMODE, 86, 36, 112, 2, 0, 3, 0},
  {29, SX_LFO1_LAGENABLE, 87, 37, 82, 1, 0, 1, 0},
  {32, SX_LFO1_SAMPLEDSOURCE, 88, 40, 115, 5, 0, 31, 0},
  {127, SX_NULL, 89, 0, 0, 0, 0, 63, 0}, // param 89 don't exist
  {34, SX_LFO2_SPEED, 90, 42, 117, 6, 0, 63, 0},
  {95, SX_LFO2_SPEEDMODULATION, 91, 103, 91, SIGNED7, 0, 127, 64},
  {37, SX_LFO2_WAVESHAPE, 92, 45, 33, 3, 0, 7, 0},
  {38, SX_LFO2_RETRIGGERPOINT, 93, 46, 34, 5, 0, 31, 0},
  {40, SX_LFO2_AMPLITUDE, 94, 48, 36, 6, 0, 63, 0},
  {90, SX_LFO2_AMPMODULATION, 95, 98, 86, SIGNED7, 0, 127, 64},
  {35, SX_LFO2_TRIGGERMODE, 96, 43, 118, 2, 0, 3, 0},
  {36, SX_LFO2_LAGENABLE, 97, 44, 83, 1, 0, 1, 0},
  {39, SX_LFO2_SAMPLEDSOURCE, 98, 47, 35, 5, 0, 31, 0},
  {96, SX_UNISON_DETUNE, 99, 134, 94, 7, 0, 127, 0}
};

const unsigned char singlePatchDataFormatX[128][9] PROGMEM = {
  //{ID, Name, Param, EB, CC nbr, bits, Value min, Value max, CC_Complement},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255}, //CC0
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255}, //CC1
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {24, SX_PORTAMENTO_RATE, 44, 32, 5, 6, 0, 63, 0},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255}, // CC6
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255}, // CC7 volume
  {12, SX_MIX, 20, 20, 8, 6, 0, 63, 0},
  {1, SX_OSC1_FREQ, 0, 9, 9, 6, 0, 63, 0},
  {2, SX_OSC1_WAVESHAPE, 5, 10, 10, 6, 0, 63, 0},
  {3, SX_OSC1_PULSEWIDTH, 3, 11, 11, 6, 0, 63, 0},
  {4, SX_OSC1_LEVERMOD, 7, 12, 12, 2, 0, 3, 0},
  {5, SX_OSC1_WAVEFORM, 6, 13, 13, 2, 0, 3, 0},
  {6, SX_OSC2_FREQ, 10, 14, 14, 6, 0, 63, 0},
  {7, SX_OSC2_WAVESHAPE, 15, 15, 15, 6, 0, 63, 0},
  {8, SX_OSC2_PULSEWIDTH, 13, 16, 16, 6, 0, 63, 0},
  {9, SX_OSC2_LEVERMOD, 17, 17, 17, 2, 0, 3, 0},
  {10, SX_OSC2_WAVEFORM, 16, 18, 18, 3, 0, 3, 0},
  {11, SX_OSC2_DETUNE, 12, 19, 19, 6, 0, 63, 0},
  {41, SX_ENV1_TRIGGERMODE, 57, 49, 20, 3, 0, 7, 0},
  {42, SX_ENV1_DELAY, 50, 50, 21, 6, 0, 63, 0},
  {43, SX_ENV1_ATTACK, 51, 51, 22, 6, 0, 63, 0},
  {44, SX_ENV1_DECAY, 52, 52, 23, 6, 0, 63, 0},
  {45, SX_ENV1_SUSTAIN, 53, 53, 24, 6, 0, 63, 0},
  {46, SX_ENV1_RELEASE, 54, 54, 25, 6, 0, 63, 0},
  {47, SX_ENV1_AMPLITUDE, 55, 55, 26, 6, 0, 63, 0},
  {48, SX_ENV1_LFOTRIGGERMODE, 59, 56, 27, 2, 0, 3, 0},
  {49, SX_ENV1_RELEASEMODE, 58, 57, 28, 2, 0, 3, 0},
  {50, SX_ENV2_TRIGGERMODE, 67, 58, 29, 3, 0, 7, 0},
  {51, SX_ENV2_DELAY, 60, 59, 30, 6, 0, 63, 0},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {37, SX_LFO2_WAVESHAPE, 92, 45, 33, 3, 0, 7, 0},
  {38, SX_LFO2_RETRIGGERPOINT, 93, 46, 34, 5, 0, 31, 0},
  {39, SX_LFO2_SAMPLEDSOURCE, 98, 47, 35, 5, 0, 31, 0},
  {40, SX_LFO2_AMPLITUDE, 94, 48, 36, 6, 0, 63, 0},
  {25, SX_PORTAMENTO_MODE, 46, 33, 37, 2, 0, 3, 0},
  {75, SX_RAMP1_MODE, 41, 83, 38, 2, 0, 3, 0},
  {76, SX_RAMP2_RATE, 42, 84, 39, 6, 0, 63, 0},
  {77, SX_RAMP2_MODE, 43, 85, 40, 2, 0, 3, 0},
  {52, SX_ENV2_ATTACK, 61, 60, 41, 6, 0, 63, 0},
  {53, SX_ENV2_DECAY, 62, 61, 42, 6, 0, 63, 0},
  {54, SX_ENV2_SUSTAIN, 63, 62, 43, 6, 0, 63, 0},
  {55, SX_ENV2_RELEASE, 64, 63, 44, 6, 0, 63, 0},
  {56, SX_ENV2_AMPLITUDE, 65, 64, 45, 6, 0, 63, 0},
  {57, SX_ENV2_LFOTRIGGERMODE, 69, 65, 46, 2, 0, 3, 0},
  {58, SX_ENV2_RELEASEMODE, 68, 66, 47, 2, 0, 3, 0},
  {59, SX_ENV3_TRIGGERMODE, 77, 67, 48, 3, 0, 7, 0},
  {60, SX_ENV3_DELAY, 70, 68, 49, 6, 0, 63, 0},
  {61, SX_ENV3_ATTACK, 71, 69, 50, 6, 0, 63, 0},
  {62, SX_ENV3_DECAY, 72, 70, 51, 6, 0, 63, 0},
  {63, SX_ENV3_SUSTAIN, 73, 71, 52, 6, 0, 63, 0},
  {64, SX_ENV3_RELEASE, 74, 72, 53, 6, 0, 63, 0},
  {65, SX_ENV3_AMPLITUDE, 75, 73, 54, 6, 0, 63, 0},
  {66, SX_ENV3_LFOTRIGGERMODE, 79, 74, 55, 2, 0, 3, 0},
  {67, SX_ENV3_RELEASEMODE, 78, 75, 56, 2, 0, 3, 0},
  {68, SX_TRACK_INPUT, 33, 76, 57, 5, 0, 31, 0},
  {69, SX_TRACK_POINT1, 34, 77, 58, 6, 0, 63, 0},
  {70, SX_TRACK_POINT2, 35, 78, 59, 6, 0, 63, 0},
  {71, SX_TRACK_POINT3, 36, 79, 60, 6, 0, 63, 0},
  {72, SX_TRACK_POINT4, 37, 80, 61, 6, 0, 63, 0},
  {73, SX_TRACK_POINT5, 38, 81, 62, 6, 0, 63, 0},
  {74, SX_RAMP1_RATE, 40, 82, 63, 6, 0, 63, 0},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {26, SX_PORTAMENTO_LEGATOENABLE, 47, 34, 65, 1, 0, 1, 0},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {78, SX_OSC1_FREQBYLFO1AMOUNT, 1, 86, 70, SIGNED7, 0, 127, 64},
  {79, SX_OSC1_PWBYLFO2AMOUNT, 4, 87, 71, SIGNED7, 0, 127, 64},
  {80, SX_OSC2_FREQBYLFO1AMOUNT, 11, 88, 72, SIGNED7, 0, 127, 64},
  {81, SX_OSC2_PWBYLFO2AMOUNT, 14, 89, 73, SIGNED7, 0, 127, 64},
  {82, SX_FILTER_FREQBYENV1AMOUNT, 22, 90, 74, SIGNED7, 0, 127, 64},
  {83, SX_FILTER_FREQBYPRESSURE, 23, 91, 75, SIGNED7, 0, 127, 64},
  {84, SX_VCA1_AMPBYVELOCITY, 28, 92, 76, SIGNED7, 0, 127, 64},
  {85, SX_VCA2_AMPBYENV2, 29, 93, 77, SIGNED7, 0, 127, 64},
  {86, SX_ENV1_AMPBYVELO, 56, 94, 78, SIGNED7, 0, 127, 64},
  {87, SX_ENV2_AMPBYVELO, 66, 95, 79, SIGNED7, 0, 127, 64},
  {14, SX_OSC1_CLICK, 9, 22, 80, 1, 0, 1, 0},
  {16, SX_OSC2_CLICK, 19, 24, 81, 1, 0, 1, 0},
  {29, SX_LFO1_LAGENABLE, 87, 37, 82, 1, 0, 1, 0},
  {36, SX_LFO2_LAGENABLE, 97, 44, 83, 1, 0, 1, 0},
  {88, SX_ENV3_AMPBYVELO, 76, 96, 84, SIGNED7, 0, 127, 64},
  {89, SX_LFO1_AMPMODULATION, 85, 97, 85, SIGNED7, 0, 127, 64},
  {90, SX_LFO2_AMPMODULATION, 95, 98, 86, SIGNED7, 0, 127, 64},
  {91, SX_PORTAMENTO_SPEEDBYVELOCITY, 45, 99, 87, SIGNED7, 0, 127, 64},
  {92, SX_FILTER_FMBYENV3, 31, 100, 88, SIGNED7, 0, 127, 64},
  {93, SX_FILTER_FMBYPRESSURE, 32, 101, 89, SIGNED7, 0, 127, 64},
  {94, SX_LFO1_SPEEDMODULATION, 81, 102, 90, SIGNED7, 0, 127, 64},
  {95, SX_LFO2_SPEEDMODULATION, 91, 103, 91, SIGNED7, 0, 127, 64},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {96, SX_UNISON_DETUNE, 96, 104, 94, 7, 0, 127, 0},
  {0, SX_KEYBOARD_MODE, 48, 8, 95, 2, 0, 3, 0},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {13, SX_OSC1_KEYBOARDMOD, 8, 21, 102, 2, 0, 3, 0},
  {15, SX_OSC2_KEYBOARDMOD, 18, 23, 103, 2, 0, 3, 0},
  {17, SX_OSC_SYNCMODE, 2, 25, 104, 2, 0, 3, 0},
  {18, SX_FILTER_FREQ, 21, 26, 105, 7, 0, 127, 0},
  {19, SX_FILTER_RESONANCE, 24, 27, 106, 6, 0, 63, 0},
  {20, SX_FILTER_LEVERMOD, 25, 28, 107, 2, 0, 3, 0},
  {21, SX_FILTER_KEYBOARDMOD, 26, 29, 108, 2, 0, 3, 0},
  {22, SX_FILTER_FM_AMOUNT, 30, 30, 109, 6, 0, 63, 0},
  {23, SX_VCA1_AMOUNT, 27, 31, 110, 6, 0, 63, 0},
  {27, SX_LFO1_SPEED, 80, 35, 111, 6, 0, 63, 0},
  {28, SX_LFO1_TRIGGERMODE, 86, 36, 112, 2, 0, 3, 0},
  {30, SX_LFO1_WAVESHAPE, 82, 38, 113, 3, 0, 7, 0},
  {31, SX_LFO1_RETRIGGERPOINT, 83, 39, 114, 5, 0, 31, 0},
  {32, SX_LFO1_SAMPLEDSOURCE, 88, 40, 115, 5, 0, 31, 0},
  {33, SX_LFO1_AMPLITUDE, 84, 41, 116, 6, 0, 63, 0},
  {34, SX_LFO2_SPEED, 90, 42, 117, 6, 0, 63, 0},
  {35, SX_LFO2_TRIGGERMODE, 96, 43, 118, 2, 0, 3, 0},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255}, // CC119
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255},
  {255, SX_NULL, 255, 255, 255, 255, 255, 255, 255}
};

const unsigned char singlePatchDataFormatEB[97][9] PROGMEM = {
  //{ID, Name, Param, EB, CC nbr, bits, Value min, Value max, CC_Complement},
  {0, SX_KEYBOARD_MODE, 48, 8, 95, 2, 0, 3, 0},
  {1, SX_OSC1_FREQ, 0, 9, 9, 6, 0, 63, 0},
  {2, SX_OSC1_WAVESHAPE, 5, 10, 10, 6, 0, 63, 0},
  {3, SX_OSC1_PULSEWIDTH, 3, 11, 11, 6, 0, 63, 0},
  {4, SX_OSC1_LEVERMOD, 7, 12, 12, 2, 0, 3, 0},
  {5, SX_OSC1_WAVEFORM, 6, 13, 13, 2, 0, 3, 0},
  {6, SX_OSC2_FREQ, 10, 14, 14, 6, 0, 63, 0},
  {7, SX_OSC2_WAVESHAPE, 15, 15, 15, 6, 0, 63, 0},
  {8, SX_OSC2_PULSEWIDTH, 13, 16, 16, 6, 0, 63, 0},
  {9, SX_OSC2_LEVERMOD, 17, 17, 17, 2, 0, 3, 0},
  {10, SX_OSC2_WAVEFORM, 16, 18, 18, 3, 0, 3, 0},
  {11, SX_OSC2_DETUNE, 12, 19, 19, 6, 0, 63, 0},
  {12, SX_MIX, 20, 20, 8, 6, 0, 63, 0},
  {13, SX_OSC1_KEYBOARDMOD, 8, 21, 102, 2, 0, 3, 0},
  {14, SX_OSC1_CLICK, 9, 22, 80, 1, 0, 1, 0},
  {15, SX_OSC2_KEYBOARDMOD, 18, 23, 103, 2, 0, 3, 0},
  {16, SX_OSC2_CLICK, 19, 24, 81, 1, 0, 1, 0},
  {17, SX_OSC_SYNCMODE, 2, 25, 104, 2, 0, 3, 0},
  {18, SX_FILTER_FREQ, 21, 26, 105, 7, 0, 127, 0},
  {19, SX_FILTER_RESONANCE, 24, 27, 106, 6, 0, 63, 0},
  {20, SX_FILTER_LEVERMOD, 25, 28, 107, 2, 0, 3, 0},
  {21, SX_FILTER_KEYBOARDMOD, 26, 29, 108, 2, 0, 3, 0},
  {22, SX_FILTER_FM_AMOUNT, 30, 30, 109, 6, 0, 63, 0},
  {23, SX_VCA1_AMOUNT, 27, 31, 110, 6, 0, 63, 0},
  {24, SX_PORTAMENTO_RATE, 44, 32, 5, 6, 0, 63, 0},
  {25, SX_PORTAMENTO_MODE, 46, 33, 37, 2, 0, 3, 0},
  {26, SX_PORTAMENTO_LEGATOENABLE, 47, 34, 65, 1, 0, 1, 0},
  {27, SX_LFO1_SPEED, 80, 35, 111, 6, 0, 63, 0},
  {28, SX_LFO1_TRIGGERMODE, 86, 36, 112, 2, 0, 3, 0},
  {29, SX_LFO1_LAGENABLE, 87, 37, 82, 1, 0, 1, 0},
  {30, SX_LFO1_WAVESHAPE, 82, 38, 113, 3, 0, 7, 0},
  {31, SX_LFO1_RETRIGGERPOINT, 83, 39, 114, 5, 0, 31, 0},
  {32, SX_LFO1_SAMPLEDSOURCE, 88, 40, 115, 5, 0, 31, 0},
  {33, SX_LFO1_AMPLITUDE, 84, 41, 116, 6, 0, 63, 0},
  {34, SX_LFO2_SPEED, 90, 42, 117, 6, 0, 63, 0},
  {35, SX_LFO2_TRIGGERMODE, 96, 43, 118, 2, 0, 3, 0},
  {36, SX_LFO2_LAGENABLE, 97, 44, 83, 1, 0, 1, 0},
  {37, SX_LFO2_WAVESHAPE, 92, 45, 33, 3, 0, 7, 0},
  {38, SX_LFO2_RETRIGGERPOINT, 93, 46, 34, 5, 0, 31, 0},
  {39, SX_LFO2_SAMPLEDSOURCE, 98, 47, 35, 5, 0, 31, 0},
  {40, SX_LFO2_AMPLITUDE, 94, 48, 36, 6, 0, 63, 0},
  {41, SX_ENV1_TRIGGERMODE, 57, 49, 20, 3, 0, 7, 0},
  {42, SX_ENV1_DELAY, 50, 50, 21, 6, 0, 63, 0},
  {43, SX_ENV1_ATTACK, 51, 51, 22, 6, 0, 63, 0},
  {44, SX_ENV1_DECAY, 52, 52, 23, 6, 0, 63, 0},
  {45, SX_ENV1_SUSTAIN, 53, 53, 24, 6, 0, 63, 0},
  {46, SX_ENV1_RELEASE, 54, 54, 25, 6, 0, 63, 0},
  {47, SX_ENV1_AMPLITUDE, 55, 55, 26, 6, 0, 63, 0},
  {48, SX_ENV1_LFOTRIGGERMODE, 59, 56, 27, 2, 0, 3, 0},
  {49, SX_ENV1_RELEASEMODE, 58, 57, 28, 2, 0, 3, 0},
  {50, SX_ENV2_TRIGGERMODE, 67, 58, 29, 3, 0, 7, 0},
  {51, SX_ENV2_DELAY, 60, 59, 30, 6, 0, 63, 0},
  {52, SX_ENV2_ATTACK, 61, 60, 41, 6, 0, 63, 0},
  {53, SX_ENV2_DECAY, 62, 61, 42, 6, 0, 63, 0},
  {54, SX_ENV2_SUSTAIN, 63, 62, 43, 6, 0, 63, 0},
  {55, SX_ENV2_RELEASE, 64, 63, 44, 6, 0, 63, 0},
  {56, SX_ENV2_AMPLITUDE, 65, 64, 45, 6, 0, 63, 0},
  {57, SX_ENV2_LFOTRIGGERMODE, 69, 65, 46, 2, 0, 3, 0},
  {58, SX_ENV2_RELEASEMODE, 68, 66, 47, 2, 0, 3, 0},
  {59, SX_ENV3_TRIGGERMODE, 77, 67, 48, 3, 0, 7, 0},
  {60, SX_ENV3_DELAY, 70, 68, 49, 6, 0, 63, 0},
  {61, SX_ENV3_ATTACK, 71, 69, 50, 6, 0, 63, 0},
  {62, SX_ENV3_DECAY, 72, 70, 51, 6, 0, 63, 0},
  {63, SX_ENV3_SUSTAIN, 73, 71, 52, 6, 0, 63, 0},
  {64, SX_ENV3_RELEASE, 74, 72, 53, 6, 0, 63, 0},
  {65, SX_ENV3_AMPLITUDE, 75, 73, 54, 6, 0, 63, 0},
  {66, SX_ENV3_LFOTRIGGERMODE, 79, 74, 55, 2, 0, 3, 0},
  {67, SX_ENV3_RELEASEMODE, 78, 75, 56, 2, 0, 3, 0},
  {68, SX_TRACK_INPUT, 33, 76, 57, 5, 0, 31, 0},
  {69, SX_TRACK_POINT1, 34, 77, 58, 6, 0, 63, 0},
  {70, SX_TRACK_POINT2, 35, 78, 59, 6, 0, 63, 0},
  {71, SX_TRACK_POINT3, 36, 79, 60, 6, 0, 63, 0},
  {72, SX_TRACK_POINT4, 37, 80, 61, 6, 0, 63, 0},
  {73, SX_TRACK_POINT5, 38, 81, 62, 6, 0, 63, 0},
  {74, SX_RAMP1_RATE, 40, 82, 63, 6, 0, 63, 0},
  {75, SX_RAMP1_MODE, 41, 83, 38, 2, 0, 3, 0},
  {76, SX_RAMP2_RATE, 42, 84, 39, 6, 0, 63, 0},
  {77, SX_RAMP2_MODE, 43, 85, 40, 2, 0, 3, 0},
  {78, SX_OSC1_FREQBYLFO1AMOUNT, 1, 86, 70, SIGNED7, 0, 127, 64},
  {79, SX_OSC1_PWBYLFO2AMOUNT, 4, 87, 71, SIGNED7, 0, 127, 64},
  {80, SX_OSC2_FREQBYLFO1AMOUNT, 11, 88, 72, SIGNED7, 0, 127, 64},
  {81, SX_OSC2_PWBYLFO2AMOUNT, 14, 89, 73, SIGNED7, 0, 127, 64},
  {82, SX_FILTER_FREQBYENV1AMOUNT, 22, 90, 74, SIGNED7, 0, 127, 64},
  {83, SX_FILTER_FREQBYPRESSURE, 23, 91, 75, SIGNED7, 0, 127, 64},
  {84, SX_VCA1_AMPBYVELOCITY, 28, 92, 76, SIGNED7, 0, 127, 64},
  {85, SX_VCA2_AMPBYENV2, 29, 93, 77, SIGNED7, 0, 127, 64},
  {86, SX_ENV1_AMPBYVELO, 56, 94, 78, SIGNED7, 0, 127, 64},
  {87, SX_ENV2_AMPBYVELO, 66, 95, 79, SIGNED7, 0, 127, 64},
  {88, SX_ENV3_AMPBYVELO, 76, 96, 84, SIGNED7, 0, 127, 64},
  {89, SX_LFO1_AMPMODULATION, 85, 97, 85, SIGNED7, 0, 127, 64},
  {90, SX_LFO2_AMPMODULATION, 95, 98, 86, SIGNED7, 0, 127, 64},
  {91, SX_PORTAMENTO_SPEEDBYVELOCITY, 45, 99, 87, SIGNED7, 0, 127, 64},
  {92, SX_FILTER_FMBYENV3, 31, 100, 88, SIGNED7, 0, 127, 64},
  {93, SX_FILTER_FMBYPRESSURE, 32, 101, 89, SIGNED7, 0, 127, 64},
  {94, SX_LFO1_SPEEDMODULATION, 81, 102, 90, SIGNED7, 0, 127, 64},
  {95, SX_LFO2_SPEEDMODULATION, 91, 103, 91, SIGNED7, 0, 127, 64},
  {96, SX_UNISON_DETUNE, 96, 104, 94, 7, 0, 127, 0}
};
////////// DCO wave pulse noise /////////////////////////////////////////////////////////
const char DCO_Waves_Name0[] PROGMEM = {" OFF            "};
const char DCO_Waves_Name1[] PROGMEM = {" PULSE          "};
const char DCO_Waves_Name2[] PROGMEM = {" WAVE           "};
const char DCO_Waves_Name3[] PROGMEM = {" WAVE & PULSE   "};
const char DCO_Waves_Name4[] PROGMEM = {" NOISE          "};
const char DCO_Waves_Name5[] PROGMEM = {" NOISE          "};
const char DCO_Waves_Name6[] PROGMEM = {" NOISE          "};
const char DCO_Waves_Name7[] PROGMEM = {" NOISE          "};
const char * const DCO_Waves_Name[] PROGMEM  = // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!
{
  DCO_Waves_Name0,
  DCO_Waves_Name1,
  DCO_Waves_Name2,
  DCO_Waves_Name3,
  DCO_Waves_Name4,
  DCO_Waves_Name5,
  DCO_Waves_Name6,
  DCO_Waves_Name7
};

////////// DCO click /////////////////////////////////////////////////////////
const char DCO_Clic_State0[] PROGMEM = {" OFF            "};
const char DCO_Clic_State1[] PROGMEM = {" ON             "};
const char * const DCO_Clic_State[] PROGMEM  =
{
  DCO_Clic_State0,
  DCO_Clic_State1
};

////LFO1 & 2 param value description ///////////////////////////////////////////////////////////////////////// bidouille PROGMEM livepanel
const char LFO_Wave_Names0[] PROGMEM = {"TRIANGLE        "};
const char LFO_Wave_Names1[] PROGMEM = {"UP SAW          "};
const char LFO_Wave_Names2[] PROGMEM = {"DOWN SAW        "};
const char LFO_Wave_Names3[] PROGMEM = {"SQUARE          "};
const char LFO_Wave_Names4[] PROGMEM = {"RANDOM          "};
const char LFO_Wave_Names5[] PROGMEM = {"NOISE           "};
const char LFO_Wave_Names6[] PROGMEM = {"SAMPLED MODULATE"};
const char * const LFO_Wave_Names[] PROGMEM  = // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!
{
  LFO_Wave_Names0,
  LFO_Wave_Names1,
  LFO_Wave_Names2,
  LFO_Wave_Names3,
  LFO_Wave_Names4,
  LFO_Wave_Names5,
  LFO_Wave_Names6
};

////DCO SYNC param value description ///////////////////////////////////////////////////////////////////////// bidouille PROGMEM livepanel
const char SyncModes0[] PROGMEM =  {" OFF            "};
const char SyncModes1[] PROGMEM =  {" SOFT           "};
const char SyncModes2[] PROGMEM =  {" HARD           "};
const char SyncModes3[] PROGMEM =  {" HARDER         "};
const char * const SyncModes[] PROGMEM  =
{
  SyncModes0,
  SyncModes1,
  SyncModes2,
  SyncModes3
};

////LFO Trigger Mode param value description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_oscillators
const char LFO_Trigger_Mode_Names0[] PROGMEM =   {"---"};
const char LFO_Trigger_Mode_Names1[] PROGMEM =   {"SGL"};
const char LFO_Trigger_Mode_Names2[] PROGMEM =   {"MLT"};
const char LFO_Trigger_Mode_Names3[] PROGMEM =   {"EXT"};
const char * const LFO_Trigger_Mode_Names[] PROGMEM  =
{
  LFO_Trigger_Mode_Names0,
  LFO_Trigger_Mode_Names1,
  LFO_Trigger_Mode_Names2,
  LFO_Trigger_Mode_Names3
};

////LFO Modul Sources param value description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_oscillators
const char MMOD_Sources0[] PROGMEM =    {"----"}; // none
const char MMOD_Sources1[] PROGMEM =    {"ENV1"};
const char MMOD_Sources2[] PROGMEM =    {"ENV2"};
const char MMOD_Sources3[] PROGMEM =    {"ENV3"};
const char MMOD_Sources4[] PROGMEM =    {"LFO1"};
const char MMOD_Sources5[] PROGMEM =    {"LFO2"};
const char MMOD_Sources6[] PROGMEM =    {"VIBR"}; // vibrato
const char MMOD_Sources7[] PROGMEM =    {"RMP1"}; // ramps
const char MMOD_Sources8[] PROGMEM =    {"RMP2"};
const char MMOD_Sources9[] PROGMEM =    {"KEYB"}; // keyboard tracking
const char MMOD_Sources10[] PROGMEM =   {"PORT"}; // portamento
const char MMOD_Sources11[] PROGMEM =   {"TRAK"}; //  generator
const char MMOD_Sources12[] PROGMEM =   {"GATE"}; // key gate
const char MMOD_Sources13[] PROGMEM =   {"VELO"}; // velocity
const char MMOD_Sources14[] PROGMEM =   {"RVEL"}; // release velocity
const char MMOD_Sources15[] PROGMEM =   {"PRES"}; // after touch (pressure)
const char MMOD_Sources16[] PROGMEM =   {"PED1"}; // volume?
const char MMOD_Sources17[] PROGMEM =   {"PED2"}; // sustain
const char MMOD_Sources18[] PROGMEM =   {"LEV1"}; // pitch bend
const char MMOD_Sources19[] PROGMEM =   {"LEV2"}; // vibrato
const char MMOD_Sources20[] PROGMEM =   {"LEV3"}; // breath1
const char * const MMOD_Sources[] PROGMEM  =
{
  MMOD_Sources0,
  MMOD_Sources1,
  MMOD_Sources2,
  MMOD_Sources3,
  MMOD_Sources4,
  MMOD_Sources5,
  MMOD_Sources6,
  MMOD_Sources7,
  MMOD_Sources8,
  MMOD_Sources9,
  MMOD_Sources10,
  MMOD_Sources11,
  MMOD_Sources12,
  MMOD_Sources13,
  MMOD_Sources14,
  MMOD_Sources15,
  MMOD_Sources16,
  MMOD_Sources17,
  MMOD_Sources18,
  MMOD_Sources19,
  MMOD_Sources20
};

//// ENV RELEASE MODE description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_envelopes
const char EnvelopeReleaseModes0[] PROGMEM =   {"--------"};
const char EnvelopeReleaseModes1[] PROGMEM =   {"DADR    "};
const char EnvelopeReleaseModes2[] PROGMEM =   {"FREE-RUN"};
const char EnvelopeReleaseModes3[] PROGMEM =   {"BOTH    "}; // =DADR+FRE
const char * const EnvelopeReleaseModes[] PROGMEM =
{
  EnvelopeReleaseModes0,
  EnvelopeReleaseModes1,
  EnvelopeReleaseModes2,
  EnvelopeReleaseModes3
};

//// ENV TRIGGER MODES description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_envelopes
const char EnvelopeTriggerModes0[] PROGMEM =   {"----------"};
const char EnvelopeTriggerModes1[] PROGMEM =   {"RESET     "};
const char EnvelopeTriggerModes2[] PROGMEM =   {"MULTI     "};
const char EnvelopeTriggerModes3[] PROGMEM =   {"RESET+MLTI"};
const char EnvelopeTriggerModes4[] PROGMEM =   {"EXTERN    "};
const char EnvelopeTriggerModes5[] PROGMEM =   {"EXTRN+RSET"};
const char EnvelopeTriggerModes6[] PROGMEM =   {"EXTRN+MLTI"};
const char EnvelopeTriggerModes7[] PROGMEM =   {"EX+MLT+RST"};
const char * const EnvelopeTriggerModes[] PROGMEM =
{
  EnvelopeTriggerModes0,
  EnvelopeTriggerModes1,
  EnvelopeTriggerModes2,
  EnvelopeTriggerModes3,
  EnvelopeTriggerModes4,
  EnvelopeTriggerModes5,
  EnvelopeTriggerModes6,
  EnvelopeTriggerModes7
};

//// ENV LFO TRIGGER MODES description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_envelopes
const char ENV_LFOTriggerModes0[] PROGMEM =   {"NORM"}; // élucider les valeurs envoyées
const char ENV_LFOTriggerModes1[] PROGMEM =   {"GLFO"};
const char ENV_LFOTriggerModes2[] PROGMEM =   {"LFO1"};
const char ENV_LFOTriggerModes3[] PROGMEM =   {"----"};
const char * const ENV_LFOTriggerModes[] PROGMEM =
{
  ENV_LFOTriggerModes0,
  ENV_LFOTriggerModes1,
  ENV_LFOTriggerModes2,
  ENV_LFOTriggerModes3
};

//// PORTAMENTO MODES description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_keyboard
const char PortamentoModes0[] PROGMEM = {"CONS-SPED"};
const char PortamentoModes1[] PROGMEM = {"CONS-TIME"};
const char PortamentoModes2[] PROGMEM = {"EXPONENT1"};
const char PortamentoModes3[] PROGMEM = {"EXPONENT2"};
const char * const PortamentoModes[] PROGMEM =
{
  PortamentoModes0,
  PortamentoModes1,
  PortamentoModes2,
  PortamentoModes3
};

//// KBD MODES description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_keyboard
const char KeyboardModes0[] PROGMEM =   {"REASSIGN"};
const char KeyboardModes1[] PROGMEM =   {"ROTATE  "};
const char KeyboardModes2[] PROGMEM =   {"UNISON  "};
const char KeyboardModes3[] PROGMEM =   {"REAS/ROB"};
const char * const KeyboardModes[] PROGMEM =
{
  KeyboardModes0,
  KeyboardModes1,
  KeyboardModes2,
  KeyboardModes3
};

//// RAMP MODES description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_matrix
const char RampModes0[] PROGMEM = {"SINGLE "};
const char RampModes1[] PROGMEM = {"MULTI  "};
const char RampModes2[] PROGMEM = {"EXTERN "};
const char RampModes3[] PROGMEM = {"EXTGATE"};
const char * const RampModes[] PROGMEM = {
  RampModes0,
  RampModes1,
  RampModes2,
  RampModes3
};

//// MATRIX MODULATIONS DESTINATIONS description ////////////////////////////////////////////////////////// bidouille PROGMEM ui_matrix
const char MMOD_Destinations0[] PROGMEM =  {"--------"};
const char MMOD_Destinations1[] PROGMEM =  {"OS1-FREQ"};
const char MMOD_Destinations2[] PROGMEM =  {"OS1-PLSW"};
const char MMOD_Destinations3[] PROGMEM =  {"OS1-WSHP"};
const char MMOD_Destinations4[] PROGMEM =  {"OS2-FREQ"};
const char MMOD_Destinations5[] PROGMEM =  {"OS2-PLSW"};
const char MMOD_Destinations6[] PROGMEM =  {"OS2-WSHP"};
const char MMOD_Destinations7[] PROGMEM =  {"OSC-MIX "};
const char MMOD_Destinations8[] PROGMEM =  {"FM-AMNT "};
const char MMOD_Destinations9[] PROGMEM =  {"FLTR-FRQ"};
const char MMOD_Destinations10[] PROGMEM =  {"FLTR-RES"};
const char MMOD_Destinations11[] PROGMEM =  {"VCA1-LEV"};
const char MMOD_Destinations12[] PROGMEM =  {"VCA2-LEV"};
const char MMOD_Destinations13[] PROGMEM =  {"ENV1-DLY"};
const char MMOD_Destinations14[] PROGMEM =  {"ENV1-ATK"};
const char MMOD_Destinations15[] PROGMEM =  {"ENV1-DKY"};
const char MMOD_Destinations16[] PROGMEM =  {"ENV1-RLS"};
const char MMOD_Destinations17[] PROGMEM =  {"ENV1-AMP"};
const char MMOD_Destinations18[] PROGMEM =  {"ENV2-DLY"};
const char MMOD_Destinations19[] PROGMEM =  {"ENV2-ATK"};
const char MMOD_Destinations20[] PROGMEM =  {"ENV2-DKY"};
const char MMOD_Destinations21[] PROGMEM =  {"ENV2-RLS"};
const char MMOD_Destinations22[] PROGMEM =  {"ENV2-AMP"};
const char MMOD_Destinations23[] PROGMEM =  {"ENV3-DLY"};
const char MMOD_Destinations24[] PROGMEM =  {"ENV3-ATK"};
const char MMOD_Destinations25[] PROGMEM =  {"ENV3-DKY"};
const char MMOD_Destinations26[] PROGMEM =  {"ENV3-RLS"};
const char MMOD_Destinations27[] PROGMEM =  {"ENV3-AMP"};
const char MMOD_Destinations28[] PROGMEM =  {"LFO1-SPD"};
const char MMOD_Destinations29[] PROGMEM =  {"LFO1-AMP"};
const char MMOD_Destinations30[] PROGMEM =  {"LFO2-SPD"};
const char MMOD_Destinations31[] PROGMEM =  {"LFO2-AMP"};
const char MMOD_Destinations32[] PROGMEM =  {"PORTAMTO"};
const char * const MMOD_Destinations[] PROGMEM =
{
  MMOD_Destinations0,
  MMOD_Destinations1,
  MMOD_Destinations2,
  MMOD_Destinations3,
  MMOD_Destinations4,
  MMOD_Destinations5,
  MMOD_Destinations6,
  MMOD_Destinations7,
  MMOD_Destinations8,
  MMOD_Destinations9,
  MMOD_Destinations10,
  MMOD_Destinations11,
  MMOD_Destinations12,
  MMOD_Destinations13,
  MMOD_Destinations14,
  MMOD_Destinations15,
  MMOD_Destinations16,
  MMOD_Destinations17,
  MMOD_Destinations18,
  MMOD_Destinations19,
  MMOD_Destinations20,
  MMOD_Destinations21,
  MMOD_Destinations22,
  MMOD_Destinations23,
  MMOD_Destinations24,
  MMOD_Destinations25,
  MMOD_Destinations26,
  MMOD_Destinations27,
  MMOD_Destinations28,
  MMOD_Destinations29,
  MMOD_Destinations30,
  MMOD_Destinations31,
  MMOD_Destinations32
};

/////////////////////////////////////////////////////////////////////////////
//  Default initialized patch (stored in FlashRAM ;)
/////////////////////////////////////////////////////////////////////////////
const unsigned char DefaultEditBuffer[134] PROGMEM =
{
  // patch name chars 0-7
  0x4f, // O
  0x42, // B
  0x45, // E
  0x52, // R
  0x48, // H
  0x45, // E
  0x49, // I
  0x4d, // M

  // byte order
  3,  	// EB_KEYBOARD_MODE 		8       reassign with rob
  12, 	// EB_OSC1_FREQ 			9       middle
  0, 	    // EB_OSC1_WAVESHAPE		10      sow
  0,  	// EB_OSC1_PULSEWIDTH		11      narrow
  3,  	// EB_OSC1_LEVERMOD 		12      pb+vibrato
  3,  	// EB_OSC1_WAVEFORM			13      pulse+saw
  12, 	// EB_OSC2_FREQ				14
  0, 	    // EB_OSC2_WAVESHAPE		15      saw
  0,  	// EB_OSC2_PULSEWIDTH		16
  3,  	// EB_OSC2_LEVERMOD			17
  3,  	// EB_OSC2_WAVEFORM			18
  0, 	    // EB_OSC2_DETUNE			19      (6Bsigned)center
  31, 	// EB_MIX					20      center
  1,  	// EB_OSC1_KEYBOARDMOD		21      portamento on
  0,  	// EB_OSC1_CLICK			22      off
  1,  	// EB_OSC2_KEYBOARDMOD		23      portamento on
  0,  	// EB_OSC2_CLICK			24      off
  0,  	// EB_OSC_SYNCMODE			25      off
  127,	// EB_FILTER_FREQ			26      full open
  0,  	// EB_FILTER_RESONANCE		27      off
  0,  	// EB_FILTER_LEVERMOD		28      off
  0,  	// EB_FILTER_KEYBOARDMOD	29      off
  0,  	// EB_FILTER_FM_AMOUNT		30      off
  63, 	// EB_VCA1_AMOUNT			31      full on
  0,  	// EB_PORTAMENTO_RATE		32      off
  0,  	// EB_PORTAMENTO_MODE		33      constant speed
  0,  	// EB_PORTAMENTO_LEGATOENABLE	34  off
  0,  	// EB_LFO1_SPEED			35
  0,  	// EB_LFO1_TRIGGERMODE		36
  0,  	// EB_LFO1_LAGENABLE		37
  0,  	// EB_LFO1_WAVESHAPE		38
  0,  	// EB_LFO1_RETRIGGERPOINT	39
  3,  	// EB_LFO1_SAMPLEDSOURCE	40      env3
  31, 	// EB_LFO1_AMPLITUDE		41      full
  0,  	// EB_LFO2_SPEED			42
  0,  	// EB_LFO2_TRIGGERMODE		43
  0,  	// EB_LFO2_LAGENABLE		44
  0,  	// EB_LFO2_WAVESHAPE		45
  0,  	// EB_LFO2_RETRIGGERPOINT	46
  3,  	// EB_LFO2_SAMPLEDSOURCE	47      env3
  31, 	// EB_LFO2_AMPLITUDE		48      full
  0,  	// EB_ENV1_TRIGGERMODE		49
  0,  	// EB_ENV1_DELAY			50
  0,  	// EB_ENV1_ATTACK			51      fastest
  63, 	// EB_ENV1_DECAY			52      longest
  0,  	// EB_ENV1_SUSTAIN			53      lowest
  10, 	// EB_ENV1_RELEASE			54      fast
  63, 	// EB_ENV1_AMPLITUDE		55
  0,  	// EB_ENV1_LFOTRIGGERMODE	56
  0,  	// EB_ENV1_RELEASEMODE		57
  0,  	// EB_ENV2_TRIGGERMODE		58
  0,  	// EB_ENV2_DELAY			59
  0,  	// EB_ENV2_ATTACK			60
  63, 	// EB_ENV2_DECAY			61
  63, 	// EB_ENV2_SUSTAIN			62
  10, 	// EB_ENV2_RELEASE			63
  63, 	// EB_ENV2_AMPLITUDE		64
  0,  	// EB_ENV2_LFOTRIGGERMODE	65
  0,  	// EB_ENV2_RELEASEMODE		66
  0,  	// EB_ENV3_TRIGGERMODE		67
  0,  	// EB_ENV3_DELAY			68
  0,  	// EB_ENV3_ATTACK			69
  63, 	// EB_ENV3_DECAY			70
  0, 	    // EB_ENV3_SUSTAIN			71
  10, 	// EB_ENV3_RELEASE			72
  63, 	// EB_ENV3_AMPLITUDE		73
  0,  	// EB_ENV3_LFOTRIGGERMODE	74
  0,  	// EB_ENV3_RELEASEMODE		75
  4,  	// EB_TRACK_INPUT			76
  1,  	// EB_TRACK_POINT1			77
  15, 	// EB_TRACK_POINT2			78
  30, 	// EB_TRACK_POINT3			79
  46, 	// EB_TRACK_POINT4			80
  63, 	// EB_TRACK_POINT5			81
  31, 	// EB_RAMP1_RATE			82
  0,  	// EB_RAMP1_MODE			83
  50, 	// EB_RAMP2_RATE			84
  0,  	// EB_RAMP2_MODE			85
  0, 	    // EB_OSC1_FREQBYLFO1AMOUNT 86
  0, 	    // EB_OSC1_PWBYLFO2AMOUNT	87
  0, 	    // EB_OSC2_FREQBYLFO1AMOUNT 88
  0, 	    // EB_OSC2_PWBYLFO2AMOUNT	89
  1, 	    // EB_FILTER_FREQBYENV1AMOUNT 90 // BUG TAUNTEK
  0,  	// EB_FILTER_FREQBYPRESSURE 91
  63,	    // EB_VCA1_AMPBYVELOCITY	92
  63,	    // EB_VCA2_AMPBYENV2		93
  0, 	    // EB_ENV1_AMPBYVELO		94
  0,  	// EB_ENV2_AMPBYVELO		95
  0, 	    // EB_ENV3_AMPBYVELO		96
  0, 	    // EB_LFO1_AMPMODULATION	97 // ramp1
  0, 	    // EB_LFO2_AMPMODULATION	98 // ramp2
  0, 	    // EB_PORTAMENTO_SPEEDBYVELOCITY 	99
  0, 	    // EB_FILTER_FMBYENV3		100
  0, 	    // EB_FILTER_FMBYPRESSURE	101
  0, 	    // EB_LFO1_SPEEDMODULATION	102 // pressure
  0, 	    // EB_LFO2_SPEEDMODULATION	103 // keyboard tracking
  0,  	// EB_MODBUS0_SOURCE		104
  0, 	    // EB_MODBUS0_AMOUNT		105
  0,  	// EB_MODBUS0_DESTINATION 	106
  0,  	// EB_MODBUS1_SOURCE  		107
  0,  	// EB_MODBUS1_AMOUNT		108
  0,  	// EB_MODBUS1_DESTINATION 	109
  0,  	// EB_MODBUS2_SOURCE  		110
  0, 	    // EB_MODBUS2_AMOUNT		111
  0,  	// EB_MODBUS2_DESTINATION 	112
  0,  	// EB_MODBUS3_SOURCE  		113
  0,	    // EB_MODBUS3_AMOUNT		114
  0,  	// EB_MODBUS3_DESTINATION 	115
  0,  	// EB_MODBUS4_SOURCE  		116
  0, 	    // EB_MODBUS4_AMOUNT		117
  0,  	// EB_MODBUS4_DESTINATION 	118
  0,  	// EB_MODBUS5_SOURCE  		119
  0, 	    // EB_MODBUS5_AMOUNT		120
  0,  	// EB_MODBUS5_DESTINATION 	121
  0,  	// EB_MODBUS6_SOURCE  		122
  0, 	    // EB_MODBUS6_AMOUNT		123
  0,  	// EB_MODBUS6_DESTINATION 	124
  0,  	// EB_MODBUS7_SOURCE  		125
  0, 	    // EB_MODBUS7_AMOUNT		126
  0,  	// EB_MODBUS7_DESTINATION 	127
  0,  	// EB_MODBUS8_SOURCE  		128
  0, 	    // EB_MODBUS8_AMOUNT		129
  0,  	// EB_MODBUS8_DESTINATION 	130
  0,  	// EB_MODBUS9_SOURCE  		131
  0, 	    // EB_MODBUS9_AMOUNT		132
  0,  	// EB_MODBUS9_DESTINATION 	133

};
///////////////////////////////////////////////////////////////
// EditBuffer[device] chart used to generate random EditBuffer[device][]
//////////////////////////////////////////////////////////////
const unsigned char singleDataFormatEB[134][5] PROGMEM =
{ // byte, name (same as byte),default, min, max
  {0, EB_PATCHNAME0, 16, 0, 32},
  {1, EB_PATCHNAME1, 17, 0, 32},
  {2, EB_PATCHNAME2, 18, 0, 32},
  {3, EB_PATCHNAME3, 19, 0, 32},
  {4, EB_PATCHNAME4, 20, 0, 32},
  {5, EB_PATCHNAME5, 21, 0, 32},
  {6, EB_PATCHNAME6, 22, 0, 32},
  {7, EB_PATCHNAME7, 23, 0, 32},
  {8, EB_KEYBOARD_MODE, 3, 0, 3},
  {9, EB_OSC1_FREQ, 12, 0, 63},
  {10, EB_OSC1_WAVESHAPE, 0, 0, 63},
  {11, EB_OSC1_PULSEWIDTH, 0, 0, 63},
  {12, EB_OSC1_LEVERMOD, 3, 0, 3},
  {13, EB_OSC1_WAVEFORM, 3, 0, 3},
  {14, EB_OSC2_FREQ, 12, 0, 63},
  {15, EB_OSC2_WAVESHAPE, 0, 0, 63},
  {16, EB_OSC2_PULSEWIDTH, 0, 0, 63},
  {17, EB_OSC2_LEVERMOD, 3, 0, 3},
  {18, EB_OSC2_WAVEFORM, 3, 0, 3},
  {19, EB_OSC2_DETUNE, 0, 0, 63},
  {20, EB_MIX, 31, 0, 63},
  {21, EB_OSC1_KEYBOARDMOD, 1, 0, 3},
  {22, EB_OSC1_CLICK, 0, 0, 1},
  {23, EB_OSC2_KEYBOARDMOD, 1, 0, 3},
  {24, EB_OSC2_CLICK, 0, 0, 1},
  {25, EB_OSC_SYNCMODE, 0, 0, 3},
  {26, EB_FILTER_FREQ, 127, 0, 127},
  {27, EB_FILTER_RESONANCE, 0, 0, 63},
  {28, EB_FILTER_LEVERMOD, 0, 0, 3},
  {29, EB_FILTER_KEYBOARDMOD, 0, 0, 3},
  {30, EB_FILTER_FM_AMOUNT, 0, 0, 63},
  {31, EB_VCA1_AMOUNT, 63, 0, 63},
  {32, EB_PORTAMENTO_RATE, 0, 0, 63},
  {33, EB_PORTAMENTO_MODE, 0, 0, 3},
  {34, EB_PORTAMENTO_LEGATOENABLE, 0, 0, 1},
  {35, EB_LFO1_SPEED, 0, 0, 63},
  {36, EB_LFO1_TRIGGERMODE, 0, 0, 3},
  {37, EB_LFO1_LAGENABLE, 0, 0, 1},
  {38, EB_LFO1_WAVESHAPE, 0, 0, 6},
  {39, EB_LFO1_RETRIGGERPOINT, 0, 0, 31},
  {40, EB_LFO1_SAMPLEDSOURCE, 3, 0, 20},
  {41, EB_LFO1_AMPLITUDE, 31, 0, 63},
  {42, EB_LFO2_SPEED, 0, 0, 63},
  {43, EB_LFO2_TRIGGERMODE, 0, 0, 3},
  {44, EB_LFO2_LAGENABLE, 0, 0, 1},
  {45, EB_LFO2_WAVESHAPE, 0, 0, 6},
  {46, EB_LFO2_RETRIGGERPOINT, 0, 0, 32},
  {47, EB_LFO2_SAMPLEDSOURCE, 3, 0, 20},
  {48, EB_LFO2_AMPLITUDE, 31, 0, 63},
  {49, EB_ENV1_TRIGGERMODE, 0, 0, 3},
  {50, EB_ENV1_DELAY, 0, 0, 63},
  {51, EB_ENV1_ATTACK, 0, 0, 63},
  {52, EB_ENV1_DECAY, 63, 0, 63},
  {53, EB_ENV1_SUSTAIN, 0, 0, 63},
  {54, EB_ENV1_RELEASE, 10, 0, 63},
  {55, EB_ENV1_AMPLITUDE, 63, 0, 63},
  {56, EB_ENV1_LFOTRIGGERMODE, 0, 0, 1},
  {57, EB_ENV1_RELEASEMODE, 0, 0, 1},
  {58, EB_ENV2_TRIGGERMODE, 0, 0, 3},
  {59, EB_ENV2_DELAY, 0, 0, 63},
  {60, EB_ENV2_ATTACK, 0, 0, 63},
  {61, EB_ENV2_DECAY, 63, 0, 63},
  {62, EB_ENV2_SUSTAIN, 63, 0, 63},
  {63, EB_ENV2_RELEASE, 10, 0, 63},
  {64, EB_ENV2_AMPLITUDE, 63, 0, 63},
  {65, EB_ENV2_LFOTRIGGERMODE, 0, 0, 3},
  {66, EB_ENV2_RELEASEMODE, 0, 0, 3},
  {67, EB_ENV3_TRIGGERMODE, 0, 0, 3},
  {68, EB_ENV3_DELAY, 0, 0, 63},
  {69, EB_ENV3_ATTACK, 0, 0, 63},
  {70, EB_ENV3_DECAY, 63, 0, 63},
  {71, EB_ENV3_SUSTAIN, 0, 0, 63},
  {72, EB_ENV3_RELEASE, 10, 0, 63},
  {73, EB_ENV3_AMPLITUDE, 63, 0, 63},
  {74, EB_ENV3_LFOTRIGGERMODE, 0, 0, 3},
  {75, EB_ENV3_RELEASEMODE, 0, 0, 3},
  {76, EB_TRACK_INPUT, 4, 0, 20},
  {77, EB_TRACK_POINT1, 1, 0, 63},
  {78, EB_TRACK_POINT2, 15, 0, 63},
  {79, EB_TRACK_POINT3, 30, 0, 63},
  {80, EB_TRACK_POINT4, 46, 0, 63},
  {81, EB_TRACK_POINT5, 63, 0, 63},
  {82, EB_RAMP1_RATE, 31, 0, 63},
  {83, EB_RAMP1_MODE, 0, 0, 3},
  {84, EB_RAMP2_RATE, 50, 0, 63},
  {85, EB_RAMP2_MODE, 0, 0, 3},
  {86, EB_OSC1_FREQBYLFO1AMOUNT, 0, 0, 127},
  {87, EB_OSC1_PWBYLFO2AMOUNT, 0, 0, 127},
  {88, EB_OSC2_FREQBYLFO1AMOUNT, 0, 0, 127},
  {89, EB_OSC2_PWBYLFO2AMOUNT, 0, 0, 127},
  {90, EB_FILTER_FREQBYENV1AMOUNT, 1, 0, 127},
  {91, EB_FILTER_FREQBYPRESSURE, 0, 0, 127},
  {92, EB_VCA1_AMPBYVELOCITY, 63, 0, 127},
  {93, EB_VCA2_AMPBYENV2, 63, 0, 127},
  {94, EB_ENV1_AMPBYVELO, 0, 0, 127},
  {95, EB_ENV2_AMPBYVELO, 0, 0, 127},
  {96, EB_ENV3_AMPBYVELO, 0, 0, 127},
  {97, EB_LFO1_AMPMODULATION, 0, 0, 127},
  {98, EB_LFO2_AMPMODULATION, 0, 0, 127},
  {99, EB_PORTAMENTO_SPEEDBYVELOCITY, 0, 0, 127},
  {100, EB_FILTER_FMBYENV3, 0, 0, 127},
  {101, EB_FILTER_FMBYPRESSURE, 0, 0, 127},
  {102, EB_LFO1_SPEEDMODULATION, 0, 0, 127},
  {103, EB_LFO2_SPEEDMODULATION, 0, 0, 127},
  {104, EB_MODBUS0_SOURCE, 0, 0, 20},
  {105, EB_MODBUS0_AMOUNT, 0, 0, 127},
  {106, EB_MODBUS0_DESTINATION, 0, 0, 32},
  {107, EB_MODBUS1_SOURCE, 0, 0, 20},
  {108, EB_MODBUS1_AMOUNT, 0, 0, 127},
  {109, EB_MODBUS1_DESTINATION, 0, 0, 32},
  {110, EB_MODBUS2_SOURCE, 0, 0, 20},
  {111, EB_MODBUS2_AMOUNT, 0, 0, 127},
  {112, EB_MODBUS2_DESTINATION, 0, 0, 32},
  {113, EB_MODBUS3_SOURCE, 0, 0, 20},
  {114, EB_MODBUS3_AMOUNT, 0, 0, 127},
  {115, EB_MODBUS3_DESTINATION, 0, 0, 32},
  {116, EB_MODBUS4_SOURCE, 0, 0, 20},
  {117, EB_MODBUS4_AMOUNT, 0, 0, 127},
  {118, EB_MODBUS4_DESTINATION, 0, 0, 32},
  {119, EB_MODBUS5_SOURCE, 0, 0, 20},
  {120, EB_MODBUS5_AMOUNT, 0, 0, 127},
  {121, EB_MODBUS5_DESTINATION, 0, 0, 32},
  {122, EB_MODBUS6_SOURCE, 0, 0, 20},
  {123, EB_MODBUS6_AMOUNT, 0, 0, 127},
  {124, EB_MODBUS6_DESTINATION, 0, 0, 32},
  {125, EB_MODBUS7_SOURCE, 0, 0, 20},
  {126, EB_MODBUS7_AMOUNT, 0, 0, 127},
  {127, EB_MODBUS7_DESTINATION, 0, 0, 32},
  {128, EB_MODBUS8_SOURCE, 0, 0, 20},
  {129, EB_MODBUS8_AMOUNT, 0, 0, 127},
  {130, EB_MODBUS8_DESTINATION, 0, 0, 32},
  {131, EB_MODBUS9_SOURCE, 0, 0, 20},
  {132, EB_MODBUS9_AMOUNT, 0, 0, 127},
  {133, EB_MODBUS9_DESTINATION, 0, 0, 32}
};

/////////////////////////////////////////////////////////////////////////////
//  default MasterParameters (stored in FLASHRAM)
/////////////////////////////////////////////////////////////////////////////
// sysex corresponding (to verify) default factory :
//    0000  F0 10 06 03 03 00 00 08  02 00 00 00 00 00 00 00  |                |
//    0010  00 01 00 0F 03 00 00 00  00 0F 03 00 00 01 00 01  |                |
//    0020  00 01 00 01 00 00 00 04  00 00 04 01 00 02 00 00  |                |
//    0030  00 00 00 0B 01 00 00 00  00 00 00 01 00 05 00 00  |                |
//    0040  00 00 00 01 00 00 00 00  00 00 00 00 00 0F 0F 0F  |                |
//    0050  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0060  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0070  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0080  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0090  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    00A0  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    00B0  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    00C0  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    00D0  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    00E0  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    00F0  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0100  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0110  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0120  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0130  0F 0F 0F 0F 0F 0F 0F 0F  0F 0F 0F 0F 0F 0F 0F 0F  |                |
//    0140  0F 0F 0F 0F 0F 0F 0F 0F  0F 00 00 00 00 02 00 00  |                |
//    0150  00 01 00 00 00 00 00 00  00 00 00 00 00 19 F7     |               |
//
// factory DefaultMasterParameters [ ] captured from Matrix 1000 via serial debug :
// 0,40,0,0,0,0,1,63,0,0,63,0,1,1,1,1,0,4,64,1,2,0,0,27,0,0,0,1,5,0,0,1,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,2,0,1,0,0,0,0,0
/////////////////////////////////////////////////////////////////////////////
const unsigned char DefaultGlobalParameters[172] PROGMEM = // PROGMEM
{
  0, // not used [byte 0]
  48, // vibrato speed : 0 - 63
  0, // vibrato speed mod source code :  0ff, Lev2, Ped1
  0, // vibrato speed modulation amount : 0-63
  0, // vibrato waveform : tri, upsaw, dnsaw, squ, randm, noise
  0, // vibrato amplitude : 0 - 63
  1, // vibrato amp mode source code :  0ff, Lev2, Ped1
  63, // vibrato amp modulation amount : 0-63
  0, // Master tune (signed6) : -32 +32
  0, // not used
  0, // not used
  0, // MIDI basic channel : 0 - 15
  1, // MIDI OMNI mode : enabled = 1
  1, // MIDI controllers enabled : 0/1
  0, // MIDI patch change enabled : 0/1
  0, // not used
  0, // not used
  4, // MIDI Pedal1 controller (4 Foot) : 0-127
  64, // MIDI Pedal2 controller (64 sustain) : 0-127
  1, // MIDI Lever2 controller (1 Modwheel) : 0-127
  2, // MIDI Lever3 controller (2 Breath) : 0-127
  0, // not used [bytes 21 à 31]
  0, // not used
  0, // not used
  0, // not used
  0, // not used
  0, // not used
  0, // not used
  0, // not used
  0, // not used
  0, // not used
  0, // not used (byte31)
  0, // MIDI Echo Enabled : 0/1 (false/true)
  0, // not used (byte 33)
  0, // Master Transpose (signed7) : -63 +63 [byte 34]
  0, // MIDI Mono Mode enable : 0/1
  255, // Group enables [byte 36]
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255,
  255, // Group Enables end [byte 161]
  0, // not used [byte 162]
  0, // not used [byte 163]
  12, // Bend range in semi tones (1bit ?? user manual is false) : 0 - 24
  0, // Bank lock enable (in MSB Only)
  1, // Number of units (group mode) (1bit)
  0, // Current unit number (group mode) (1bit)
  0, // Group mode enable (in MSB only) (1bit)
  0, // Unison enable (1bit)
  0, // Volume invert enable (1bit)
  0, // Memory protect enable (1bit)
};

/////////////////////////////////////////////////////////////////////////////
//  Copies the default init patch to the edit buffer
/////////////////////////////////////////////////////////////////////////////
void InitEditBuffer()
{
  unsigned char i;

  for (i = 0; i < 134; i++) {
    EditBuffer[device][i] = EditBufferOrig[i] = pgm_read_byte_near(&DefaultEditBuffer[i]);
    //    EditBufferOrig[i] = pgm_read_byte_near(&DefaultEditBuffer[i]); // commenting allow an undo
  }
  UnisonDetune[device] = UnisonDetuneOrig = 0;
  UpdateDinStates(); // mise à jour des Leds
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//  Send the default init patch with zeroed settings
//
//  http://machines.hyperreal.org/manufacturers/Oberheim/Matrix-6-1000/info/Matrix-6.sysex.edit.txt
//
//  0DH - Single Patch Data to Edit Buffer
//
//  F0H 10H 06H 0DH 0 <patch data> <checksum> F7H
//
//  <data>     = patch data unpacked to two nibbles per byte
//               see patch format listing
//  <checksum> = sum of packed (not transmitted) <data>
//
//  NOTE: On receipt, this data will be stored into the edit buffer.
//
//  NOTE: Wait at least ten msec after sending a patch to the Matrix-1000.
//
//  xx yy   data, sent nybblewise:
//  a data byte dd is sent as xx=dd&0x0F; yy=(dd>>4)&0x0F
//  cc      checksum:
//          the sum of the original data bytes dd, masked with 0x7F.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void SendPatchInit(unsigned char  interface)
{
  unsigned char i;
  unsigned char checksum = 0;

  byte sysex[275]; // = { 0xf0, 0x10, 0x06, 0x0d, 0, 0, 0xf7 };

  // for matrix 6
  if (matrix_modele == matrix_6)
  {
    // enter remote edit mode matrix 6
    // TO DO -> NO not necessary

    sysex[0] = 0xf0; // begin sysex header
    sysex[1] = 0x10; // mfg
    sysex[2] = 0x06; // device
    sysex[3] = 0x01; // voice data to edit buffer
    sysex[4] = 0x00; // zero
  }
  else
  {
    // for matrix 1000
    sysex[0] = 0xf0; // begin sysex header
    sysex[1] = 0x10; // mfg
    sysex[2] = 0x06; // device
    sysex[3] = 0x0d; // voice data to edit buffer
    sysex[4] = 0x00; // zero
  }
  // other checksum version
  // 134 patch bytes
  for (i = 0; i < 134; i++)
  {
    // transmit the parameter value
    sysex[5 + 2 * i] = pgm_read_byte_near(&DefaultEditBuffer[i]) & 0x0f;
    sysex[5 + 2 * i + 1] = (pgm_read_byte_near(&DefaultEditBuffer[i]) >> 4) & 0x0f;

    checksum += pgm_read_byte_near(&DefaultEditBuffer[i]);
  }

  sysex[273] = checksum & 0x7f;
  sysex[274] = 0xf7;

  //  // print temp message on display
  //  lcd.setCursor(0, 1);
  //  lcd.print(F("Patch initialised   "));
  //  elapsedTime = 0;  //reset tmpMessage-

  if (matrix_modele == matrix_6)
    //send pgrm change number 0
    MIDI_SendPatchProgram(interface, 0);

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_serialout
  // print debug sysex +
  Serial.print(F("SendPatchInit() ARD SERIAL_PORT = ")); Serial.print(interface, DEC); Serial.print(F(" SendPatchInit*SysEx[]= "));
  for (int i = 0; i < 275; ++i)
  {
    Serial.print(sysex[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("end"));
#endif

}


/////////////////////////////////////////////////////////////////////////////
//  Send the edit buffer
/////////////////////////////////////////////////////////////////////////////
void SendEditBuffer(unsigned char device, unsigned char  interface)
{
  unsigned char i;
  unsigned char checksum = 0;

  byte sysex[275]; // = { 0xf0, 0x10, 0x06, 0x01, 0, 0, 0xf7 };

  if (matrix_modele == matrix_6)
  {
    // for Matrix 6
    sysex[0] = 0xf0; // begin sysex header
    sysex[1] = 0x10; // mfg
    sysex[2] = 0x06; // device
    sysex[3] = 0x01; // voice data to edit buffer
    sysex[4] = 0x00; // zero
  }
  else
  {
    // for matrix 1000
    sysex[0] = 0xf0; // begin sysex header
    sysex[1] = 0x10; // mfg
    sysex[2] = 0x06; // device
    sysex[3] = 0x0d; // voice data to edit buffer
    sysex[4] = 0x00; // zero
  }
  // 134 patch bytes
  for (i = 0; i < 134; i++) {
    // transmit the parameter value
    sysex[5 + 2 * i] = EditBuffer[device][i] & 0x0f;
    sysex[5 + 2 * i + 1] = (EditBuffer[device][i] >> 4) & 0x0f;

    //checksum
    checksum += EditBuffer[device][i] ;
  }

  sysex[273] = checksum & 0x7f;
  sysex[274] = 0xf7;

  if ((matrix_modele == matrix_6) && (interface != INTERFACE_SERIAL3))
  {
    //send pgrm change number 0
    MIDI_SendPatchProgram(interface, 0);
  }

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

  // turn on Matrix 6 in Patch Edit Quick mode:
  if (matrix_modele == matrix_6)
  {
    MIDI_EnterRemoteEditMode(interface);
  }

#if DEBUG_serialout
  // print debug sysex +
  Serial.print(F("SendEditBuffer() ARD SERIAL_PORT = ")); Serial.print(interface, DEC); Serial.print(F(" SysEx[]= "));
  for (int i = 0; i < 275; ++i)
  {
    Serial.print(sysex[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("end"));
#endif

}

///////////////////////////////////////////////////////////////////////////////////////
//  Send the unaltered edit buffer from the last program change
////////////////////////////////////////////////////////////////////////////////////////
void SendEditBufferOrig(unsigned char  interface)
{
  unsigned char i;
  unsigned char checksum = 0;

  // = { 0xf0, 0x10, 0x06, 0x0d, 0x00, [patch data 134bytes + cs], 0xf7 };
  byte sysex[275];

  if (matrix_modele == matrix_6)
  {
    // for Matrix 6
    sysex[0] = 0xf0; // begin sysex header
    sysex[1] = 0x10; // mfg
    sysex[2] = 0x06; // device
    sysex[3] = 0x01; // voice data to edit buffer specific to Matrix6
    sysex[4] = 0x00; // zero, we work in slot 00 of Matrix6
  }
  else
  {
    // for matrix 1000
    sysex[0] = 0xf0; // begin sysex header
    sysex[1] = 0x10; // mfg
    sysex[2] = 0x06; // device
    sysex[3] = 0x0d; // voice data to edit buffer
    sysex[4] = 0x00; // zero
  }

  // 134 patch bytes
  for (i = 0; i < 134; i++) {
    // transmit the parameter value
    sysex[5 + 2 * i] = EditBufferOrig[i] & 0x0f;
    sysex[5 + 2 * i + 1] = (EditBufferOrig[i] >> 4) & 0x0f;

    // checksum version
    checksum += EditBufferOrig[i] ;
  }

  sysex[273] = checksum & 0x7f;
  sysex[274] = 0xf7;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_serialout
  // print debug sysex +
  Serial.print(F("SendEditBufferOrig() ARD SERIAL_PORT = ")); Serial.print(interface, DEC); Serial.print(F(" SendEditBufferOrig SysEx[]= "));
  for (int i = 0; i < 275; ++i)
  {
    Serial.print(sysex[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("end"));
#endif

}


/////////////////////////////////////////////////////////////////////////////
//  Send Single Patch Data : --------------------> OUI ! to save extEEPROM
/////////////////////////////////////////////////////////////////////////////
void SendSinglePatchData(unsigned char interface, unsigned char patch_num)
{
  unsigned char i;
  unsigned char checksum = 0;

  byte sysex[275]; // = { 0xf0, 0x10, 0x06, 0x01, patch num, [patch data 134bytes + cs], 0xf7 };

  // for matrix 1000/6
  sysex[0] = 0xf0; // begin sysex header
  sysex[1] = 0x10; // mfg
  sysex[2] = 0x06; // device
  sysex[3] = 0x01; // single patch data
  sysex[4] = patch_num; // destination patch number

  // 134 patch bytes
  for (i = 0; i < 134; i++) {
    // transmit the parameter value
    sysex[(2 * i) + 5] = EditBufferOrig[i] & 0x0f;
    sysex[(2 * i) + 6] = (EditBufferOrig[i] >> 4) & 0x0f;

    // checksum version
    checksum += EditBufferOrig[i] ;
  }

  sysex[273] = checksum & 0x7f;
  sysex[274] = 0xf7;

  // is that useful ??
  //  if (matrix_modele == matrix_6)
  //  {
  //    //send pgrm change number 0
  //    MIDI_SendPatchProgram(interface, 0);  // send program change 0x00
  //  }

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_serialout
  // print debug sysex
  Serial.print(F("SendSinglePatchData() interface = "));
  Serial.print(interface, DEC);
  Serial.print(F(" patchnumber "));
  Serial.println(patch_num, DEC);
  Serial.println(F(" SendSinglePatchData() SysEx[]= "));
  for (int i = 0; i < 275; ++i)
  {
    Serial.print(sysex[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("End"));
  Serial.println();
#endif

}
/////////////////////////////////////////////////////////////////////////////
//  Send Single UnisonDetune via sysex ---> OUI ! to save extEEPROM
/////////////////////////////////////////////////////////////////////////////
void SendSingleUnison(unsigned char interface, unsigned char number)
{
  unsigned char checksum = 0;

  byte sysex[9]; // = { 0xf0, 0x10, 0x06, 0x08, patch num, [2 byte + cs], 0xf7 };

  // for Matrix Ctrlr
  sysex[0] = 0xf0; // begin sysex header
  sysex[1] = 0x10; // mfg
  sysex[2] = 0x06; // device
  sysex[3] = 0x08; // single unison
  sysex[4] = number; // destination patch number

  // transmit the UnisonDetune value
  sysex[5] = UnisonDetuneOrig & 0x0f;
  sysex[6] = (UnisonDetuneOrig >> 4) & 0x0f;

  // checksum version
  checksum += UnisonDetuneOrig;

  sysex[7] = checksum & 0x7f;
  sysex[8] = 0xf7;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_serialout
  // print debug sysex
  Serial.print(F("SendSingleUnison() interface = "));
  Serial.print(interface, DEC);
  Serial.print(F(" nbr "));
  Serial.println(number, DEC);
  Serial.println(F(" SendSingleUnison() SysEx[]= "));
  for (int i = 0; i < 9; ++i)
  {
    Serial.print(sysex[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("End"));
  Serial.println();
#endif

}

/////////////////////////////////////////////////////////////////////////////
//  Send Single Arp Data : ArpParameters[20] ---> OUI ! to save extEEPROM
/////////////////////////////////////////////////////////////////////////////
void SendSingleArpData(unsigned char interface, unsigned char number)
{
  unsigned char checksum = 0;
  byte sysex[143]; // = { 0xf0, 0x10, 0x06, 0x09, patch num, [arp data 20*2 bytes + seq 2x32*2 bytes + cs], 0xf7 };

  // for Matrix Ctrlr
  sysex[0] = 0xf0; // begin sysex header
  sysex[1] = 0x10; // mfg
  sysex[2] = 0x06; // device
  sysex[3] = 0x09; // single arp data
  sysex[4] = number; // destination patch number

  // 20 arp bytes
  for (unsigned char i = 0; i < 20; i++) {
    // transmit the arp value
    sysex[(2 * i) + 5] = ArpParametersOrig[i] & 0x0f;
    sysex[(2 * i) + 6] = (ArpParametersOrig[i] >> 4) & 0x0f;

    // checksum version
    checksum += ArpParametersOrig[i] ;
  }

  for (unsigned char j = 0; j < 32; j++) {
    sysex[45 + (2 * j)] = sequenceOrig[j][0] & 0x0f;
    sysex[46 + (2 * j)] = (sequenceOrig[j][0] >> 4) & 0x0f;

    // checksum version
    checksum += sequenceOrig[j][0];
  }

  for (unsigned char k = 0; k < 32; k++) {
    sysex[77 + (2 * k)] = sequenceOrig[k][1] & 0x0f;
    sysex[78 + (2 * k)] = (sequenceOrig[k][1] >> 4) & 0x0f;

    // checksum version
    checksum += sequenceOrig[k][1];
  }

  sysex[141] = checksum & 0x7f;
  sysex[142] = 0xf7;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_serialout
  // print debug sysex
  Serial.print(F("SendSingleARPData() interface = "));
  Serial.print(interface, DEC);
  Serial.print(F(" nbr "));
  Serial.println(number, DEC);
  Serial.println(F(" SendSingleArpData() SysEx[]= "));
  for (int i = 0; i < 101; ++i)
  {
    Serial.print(sysex[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("End"));
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send default Master Parameters
/////////////////////////////////////////////////////////////////////////////
void SendGlobalParametersInit(unsigned char interface)
{
  // f0 10 06 03 version <data> checksum f7
  // version = 0x03 for Matrix 1000
  // pgm_read_byte_near( &DefaultGlobalParameters[i])

  // http://www.youngmonkey.ca/nose/audio_tech/synth/Oberheim-Matrix6R.html (at the end of page)
  // Global Parameters Data Format for Matrix 6 : 477 total transmitted bytes/Global parameters
  // MasterParameters_6[236]
  // version = 0x02 for Matrix 6

  unsigned char i;
  unsigned char checksum = 0;
  byte sysex[351];

  sysex[0] = 0xf0;
  sysex[1] = 0x10;
  sysex[2] = 0x06;
  sysex[3] = 0x03;
  sysex[4] = 0x03; // version = 0x03 for Matrix 1000

  // 172 global bytes
  for (i = 0; i < 172; i++) {
    // transmit the parameter value
    sysex[2 * i + 5] = pgm_read_byte_near( &DefaultGlobalParameters[i]) & 0x0f;
    sysex[2 * i + 6] = ((pgm_read_byte_near(&DefaultGlobalParameters[i])) >> 4) & 0x0f;

    // checksum version
    checksum += pgm_read_byte_near(&DefaultGlobalParameters[i]);
  }

  sysex[349] = checksum & 0x7f;
  sysex[350] = 0xf7;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  Send Global Parameters
/////////////////////////////////////////////////////////////////////////////
void SendGlobalParameters(unsigned char interface)
{
  // f0 10 06 03 version <data> checksum f7
  // version = 0x03 for Matrix 1000
  // pgm_read_byte_near( &DefaultMasterParameters[i])

  // http://www.youngmonkey.ca/nose/audio_tech/synth/Oberheim-Matrix6R.html (at the end of page)
  // Global Parameters Data Format for Matrix 6 : 477 total transmitted bytes/Global parameters
  // MasterParameters_6[236]
  // version = 0x02 for Matrix 6

  unsigned char i;
  unsigned char checksum = 0;
  byte sysex[351];

  // print temp message on display
  lcd.setCursor(0, 1);
  lcd.print(F("Dumping Master      "));
  elapsedTime = 0;  //reset tmpMessage-

  for (i = 0; i < 172; i++)
  {
    GlobalParameters[i] = EEPROM.read(EEPROM_GLOBALPARAMETERS + i);
  }

#if DEBUG_master
  Serial.println(F("SendGlobalParameters()"));
  Serial.print(F("reading MasterParameters[ ] in EEPROM sent at interface = ")), Serial.println(interface);
  for (i = 0; i < 172; i++)
  {
    Serial.print(GlobalParameters[i]);
    Serial.print(F(","));
  }
  Serial.println();
#endif


  sysex[0] = 0xf0;
  sysex[1] = 0x10;
  sysex[2] = 0x06;
  sysex[3] = 0x03;
  sysex[4] = 0x03; // version = 0x03 for Matrix 1000

  // 172 patch bytes
  for (i = 0; i < 172; i++) {
    // transmit the parameter value
    sysex[2 * i + 5] = GlobalParameters[i] & 0x0f;
    sysex[2 * i + 6] = (GlobalParameters[i] >> 4) & 0x0f;

    // checksum version
    checksum += GlobalParameters[i];
  }

  sysex[349] = checksum & 0x7f;
  sysex[350] = 0xf7;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char tmp;
      tmp = ui_external_clk;
      ui_external_clk = true; // desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = tmp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

}

//////////////////////////////////////////////////////////
// Dump Ctrlr Bank to selected interface & to Core OUT
//////////////////////////////////////////////////////////
void DumpCtrlrBank(unsigned char interface, unsigned char bank)
{
  unsigned char tmp = ui_external_clk;
  ui_external_clk = MIDCLK; // desactive 0xF8 clock message on serial port


  // send patch 0 to patch 99 :
  for (unsigned char k = 0; k < 100; k++)
  {
    DOUT_PinSet(DOUT_FILTER, 1); // temoin ON
    Read_Patch_From_BS(0, bank, k); // read the patch
    DOUT_PinSet(DOUT_FILTER, 0); // temoin OFF

    // print temp message on display
    lcd.setCursor(0, 1);
    lcd.print(F("Dumping :"));
    LCD_PrintBCD3(bank * 100 + k);
    lcd.print(F(" to"));
    switch (interface)
    {
      case INTERFACE_SERIAL1 :
        lcd.print(F(" A   "));
        break;
      case INTERFACE_SERIAL2 :
        lcd.print(F("  B  "));
        break;
      case INTERFACE_SERIAL3 :
        lcd.print(F(" CORE"));
        break;
      case INTERFACE_SERIAL4 :
        lcd.print(F(" C   "));
        break;
      case INTERFACE_SERIAL5 :
        lcd.print(F(" D   "));
        break;
    }

    SendSingleArpData(interface, k);
    SendSingleUnison(interface, k);
    SendSinglePatchData(interface, k); // algo + send via interface

  }
  lcd.setCursor(0, 1);
  lcd.print(F("Dump Bank finished  "));
  elapsedTime = 0;  //reset tmpMessage

  ui_external_clk = tmp; // retrieve state
}

/////////////////////////////////////////////////////////////////////////////
// save the patch in the matrix 1000 at bank and number specified
/////////////////////////////////////////////////////////////////////////////
void StoreEditBuffer(unsigned char interface, unsigned char bank, unsigned char num)
{
  // f0 10 06 0e 03 num. bank. ID. f7 - STORE EDIT BUFFER

  byte sysex[9]; // Matrix 1000 owner's manual page 45

  sysex[0] = 0xf0;
  sysex[1] = 0x10;
  sysex[2] = 0x06;
  sysex[3] = 0x0e;
  sysex[4] = 0x03;
  sysex[5] = num;
  sysex[6] = bank;
  sysex[7] = 0x7f;
  sysex[8] = 0xf7;

  lcd.setCursor(0, 1);
  lcd.print(F("Patch stored in Matrx"));
  elapsedTime = 0;  //reset tmpMessage

  switch (interface)
  {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

      //    case INTERFACE_SERIAL3: // ne sert à rien !
      //      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      //      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }


}

/////////////////////////////////////////////////////////////////////////////
// set Orig arp, unison et seq to defaults
/////////////////////////////////////////////////////////////////////////////
void Set_OrigArpUniSeq_toDefault(void)
{

  // default unison detune
  UnisonDetuneOrig = 0;

  // arp parameters default
  for (unsigned char j = 0; j < 20; j++)
    ArpParametersOrig[j] = pgm_read_byte_near (&Default_ArpParameters[j][0]);

  //sequence default (Carpenter)
  for (unsigned char j = 0; j < 32; j++)
  {
    //    sequenceOrig[2 * j + 0] = pgm_read_byte_near (&Default_Sequence[3][32][0]);
    //    sequenceOrig[2 * j + 1] = pgm_read_byte_near (&Default_Sequence[3][32][1]);
    sequenceOrig[j][0] = pgm_read_byte_near (&Default_Sequence[3][32][0]);
    sequenceOrig[j][1] = pgm_read_byte_near (&Default_Sequence[3][32][1]);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Send Matrix Ctrlr's specific parameters saved in the arduino board EEPROM
// through a 52 bytes SysEx message
/////////////////////////////////////////////////////////////////////////////
void SendCtrlrSystemCfg(unsigned char interface)
{
  // f0 10 06 0f .. .. .. f7 - STORE EDIT BUFFER
  //// adresses in internal EEPROM for midi channel, filtermode, etc :

  // get bytes
  byte sysex[52] =
  {
    0xf0, 0x10, 0x06, 0x0f,
    EEPROM.read(EEPROM_MIDI_CHANNEL),
    EEPROM.read(EEPROM_FILTERSUSTAIN_MODE),
    EEPROM.read(EEPROM_LASTBANKA),
    EEPROM.read(EEPROM_LASTPATCHA),
    EEPROM.read(EEPROM_LASTBANKB),
    EEPROM.read(EEPROM_LASTPATCHB),
    EEPROM.read(EEPROM_LASTBANKC),
    EEPROM.read(EEPROM_LASTPATCHC),
    EEPROM.read(EEPROM_LASTBANKD),
    EEPROM.read(EEPROM_LASTPATCHD),
    EEPROM.read(EEPROM_DEVICE),
    EEPROM.read(EEPROM_MATRIX_MODELE_A),
    EEPROM.read(EEPROM_MATRIX_MODELE_B),
    EEPROM.read(EEPROM_MATRIX_MODELE_C),
    EEPROM.read(EEPROM_MATRIX_MODELE_D),
    EEPROM.read(EEPROM_ENCODER_INVERTED),
    EEPROM.read(EEPROM_MTHRU_XCC), // mThru_XCc
    0x00, // EEPROM.read(EEPROM_EXTCLOCK) -> not used
    EEPROM.read(EEPROM_ZONE + 0), // 0xc4
    EEPROM.read(EEPROM_ZONE + 1),
    EEPROM.read(EEPROM_ZONE + 2),
    EEPROM.read(EEPROM_ZONE + 3),
    EEPROM.read(EEPROM_ZONE + 4),
    EEPROM.read(EEPROM_ZONE + 5),
    EEPROM.read(EEPROM_ZONE + 6), //0xca
    EEPROM.read(EEPROM_ZONE + 7),
    EEPROM.read(EEPROM_ZONE + 8),
    EEPROM.read(EEPROM_ZONE + 9),
    EEPROM.read(EEPROM_ZONE + 10),
    EEPROM.read(EEPROM_ZONE + 11),
    EEPROM.read(EEPROM_ZONE + 12), //0xd0
    EEPROM.read(EEPROM_ZONE + 13),
    EEPROM.read(EEPROM_ZONE + 14),
    EEPROM.read(EEPROM_ZONE + 15),
    EEPROM.read(EEPROM_ZONE + 16),
    EEPROM.read(EEPROM_ZONE + 17),
    EEPROM.read(EEPROM_ZONE + 18),
    EEPROM.read(EEPROM_ZONE + 19),
    EEPROM.read(EEPROM_Z_ACTIVE ), // 0xd8
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF7
  };

  // print temp message on display
  lcd.setCursor(0, 1);
  lcd.print(F("Dumping SystemConfig"));
  elapsedTime = 0;  //reset tmpMessage-

  // send sysex
  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = ui_external_clk;
      ui_external_clk = true; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      ui_external_clk = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.println(F("MIDI_SendCtrlrSystemCfg() "));
  Serial.print (F("interface = ")); Serial.println(interface, HEX);
  Serial.println();
#endif
}


/////////////////////////////////////////////////////////////////////////////
// diagnostics (proper to Matrix Ctrlr
/////////////////////////////////////////////////////////////////////////////
void MIDI_Rcv_Diagnostics(unsigned char interface, byte order1, byte order2)
{
  byte sysex[8] = { 0xf0, 0x10, 0x06, 0x0f, 0x01, order1, order2, 0xf7};

  // 0x0f : system
  // 0x01 : diag
  // order1 : echo

  // simple test message (echo) "hello buddy"
  if (order1 == 0x00 && order2 == 0x00)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Hello Buddy ! :)     "));
    lcd.setCursor(0, 1);
    lcd.print(F("Echo msg on CORE Out "));

    elapsedTime = 0;

    sysex[6] = 0x08;

    // send a midi response
    switch (interface) {
      case  INTERFACE_SERIAL1:
        MIDI1.sendSysEx (sizeof(sysex), sysex, true);
        break;

      case INTERFACE_SERIAL2:
        MIDI2.sendSysEx (sizeof(sysex), sysex, true);
        break;

      case INTERFACE_SERIAL3:
        unsigned char stamp;
        stamp = ui_external_clk;
        ui_external_clk = true; // doing this desactive F8 on serial port
        MIDI3.sendSysEx (sizeof(sysex), sysex, true);
        ui_external_clk = stamp; // retrieve state
        break;

#if SOFTSERIAL_ENABLED
      case INTERFACE_SERIAL4:
        MIDI4.sendSysEx (sizeof(sysex), sysex, true);
        break;

      case INTERFACE_SERIAL5:
        MIDI5.sendSysEx (sizeof(sysex), sysex, true);
        break;
#endif
      default: break;

    }
  }
  // other messages below
  // etc ..
  else if (order1 == 0x00 && order2 == 0x01)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Echo msg on Midi Out:"));
    lcd.setCursor(0, 1);
    lcd.print(F("CORE  A   B   C   D  "));
    elapsedTime = 0;  //reset tmpMessage

    // send a midi response on all midi out
    sysex[6] = 0x0a; MIDI1.sendSysEx (sizeof(sysex), sysex, true);

    sysex[6] = 0x0b; MIDI2.sendSysEx (sizeof(sysex), sysex, true);

    sysex[6] = 0x09; MIDI3.sendSysEx (sizeof(sysex), sysex, true);

#if SOFTSERIAL_ENABLED
    sysex[6] = 0x0c; MIDI4.sendSysEx (sizeof(sysex), sysex, true);
    sysex[6] = 0x0d; MIDI5.sendSysEx (sizeof(sysex), sysex, true);
#endif

  }
  else if (order1 == 0x01 && order2 == 0x03) // format INIT
  {
    FORMAT_Memory(3);
    lcd.setCursor(0, 1);
    lcd.print(F(" Patch 000-999 ready "));
    elapsedTime = 0;
  }
  else if (order1 == 0x01 && order2 == 0x04)  // format SYS
  {
    FORMAT_Memory(4);
  }
  else if (order1 == 0x01 && order2 == 0x01)  // format EXT with 0xff
  {
    FORMAT_Memory(1);
    lcd.setCursor(0, 1);
    lcd.print(F("FORMAT ext.EEPROM w/0"));
    elapsedTime = 0;

  }
  else if (order1 == 0x01 && order2 == 0x02)  // format INT with 0xff
  {
    FORMAT_Memory(2);
  }
  else
    return;
}

/////////////////////////////////////////////////////////////////////////////
// Create a random patch
/////////////////////////////////////////////////////////////////////////////
void WizardEditBuffer(unsigned char device, unsigned char cmd)
{
  // "WIZZARD!" patch name
  EditBuffer[device][0] = 0x57;
  EditBuffer[device][1] = 0x49;
  EditBuffer[device][2] = 0x5a;
  EditBuffer[device][3] = 0x5a;
  EditBuffer[device][4] = 0x41;
  EditBuffer[device][5] = 0x52;
  EditBuffer[device][6] = 0x44;
  EditBuffer[device][7] = 0x21;

  /*  for (unsigned char i = 0; i < 126; ++i)
    {
      // random(min,max) min inclusive, max exclusive, so +1 to include, return a long
      EditBuffer[device][i + 8] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));
    }
  */
  switch (cmd)
  {
    case 0: // PatchName
      for (unsigned char i = 0; i < 8; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 1: // DCO
      for (unsigned char i = 9; i < 26; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 5: // LFO
      for (unsigned char i = 35; i < 48; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 2: // FLT, VCA
      for (unsigned char i = 26; i < 31; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));
      EditBuffer[device][31] = 60; //VCA
      break;

    case 3:  // ENV
      for (unsigned char i = 49; i < 76; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 4: // KBD, PRTA
      EditBuffer[device][8] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[8][3]), pgm_read_byte_near(&singleDataFormatEB[8][4] + 1));
      for (unsigned char i = 32; i < 35; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 8: // MTRX
      for (unsigned char i = 104; i < 134; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 6: // RMP
      for (unsigned char i = 76; i < 86; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    case 7: // Modulations
      for (unsigned char i = 86; i < 104; ++i)
        EditBuffer[device][i] = (byte)random(pgm_read_byte_near(&singleDataFormatEB[i][3]), pgm_read_byte_near(&singleDataFormatEB[i][4] + 1));

      break;

    default:
      break;
  }

  // send the EditBuffer to device ?
  SendEditBuffer(device, INTERFACE_SERIAL);
  SendEditBuffer(device, INTERFACE_SERIAL3);

#if DEBUG_wizz
  Serial.print(F("WizardEditBuffer(")); Serial.print(cmd, DEC); Serial.println(F(" ) / "));
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Converts a pots 7 bit value  to a signed 7 bit with zero at center position
/////////////////////////////////////////////////////////////////////////////
unsigned char Convert_7bit_Signed(unsigned char value) {
  return  ((value + 0x40) & 0x7f); // truc d'origine
  //  return  (value - 64); // NE MARCHE PAS
}

/////////////////////////////////////////////////////////////////////////////
//  Converts a pots 7 bit value to a signed 6 bit with zero at center position
/////////////////////////////////////////////////////////////////////////////
unsigned char Convert_6bit_Signed(unsigned char value) {
  return (value + 0x60) & 0x7f; // truc d'origine
  //  return ((value >> 1 ) - 32); // NE MARCHE PAS
}

/////////////////////////////////////////////////////////////////////////////
//  Converts a pots 7 bit value to a unsigned 6 bit (0 --> 63)
/////////////////////////////////////////////////////////////////////////////
unsigned char Convert_6bit_Unsigned(unsigned char value) {
  return value >> 1;
}
