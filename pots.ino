#include "pots.h"

const char PotDescription0[]PROGMEM = "VCF F /ENV1     "; // ain_pin 0
const char PotDescription1[]PROGMEM = "VCF RESO        ";
const char PotDescription2[]PROGMEM = "DCO1 F /LFO1    "; // code à implémenter en Control change
const char PotDescription3[]PROGMEM = "VCF FM          ";
const char PotDescription4[]PROGMEM = "DCO1 FREQ       ";
const char PotDescription5[]PROGMEM = "VCF F /PRS      ";
const char PotDescription6[]PROGMEM = "VCF FREQ CUT    ";
const char PotDescription7[]PROGMEM = "RAMP 2 RATE     "; // A DEFINIR

const char PotDescription8[]PROGMEM = "DCO1 WAVE SHAPE "; // ain_pin 8
const char PotDescription9[]PROGMEM = "RAMP 1 RATE     "; //  A DEFINIR
const char PotDescription10[]PROGMEM = "DCO1 PW /LFO2   ";
const char PotDescription11[]PROGMEM = "DCO BALANCE     ";
const char PotDescription12[]PROGMEM = "DCO1 PW         ";
const char PotDescription13[]PROGMEM = "DCO2 FREQ       ";
const char PotDescription14[]PROGMEM = "UNISON DETUNE   ";
const char PotDescription15[]PROGMEM = "DCO2 F /LFO1    ";

const char PotDescription16[]PROGMEM = "ENV1 ATTACK     "; // ain_pin 16
const char PotDescription17[]PROGMEM = "DCO2 WAVE SHAPE ";
const char PotDescription18[]PROGMEM = "DCO2 DETUNE     ";
const char PotDescription19[]PROGMEM = "DCO2 PW         ";
const char PotDescription20[]PROGMEM = "DCO2 PW /LFO2   ";
const char PotDescription21[]PROGMEM = "ENV1 SUSTAIN    "; // ain_pin 21
const char PotDescription22[]PROGMEM = "ENV1 DECAY      ";
const char PotDescription23[]PROGMEM = "ENV1 RELEASE    ";

const char PotDescription24[]PROGMEM = "ENV2 SUSTAIN    "; // ain_pin 24
const char PotDescription25[]PROGMEM = "LFO1 SPEED      ";
const char PotDescription26[]PROGMEM = "ENV2 ATTACK     ";
const char PotDescription27[]PROGMEM = "LFO1 AMPLITUDE  ";
const char PotDescription28[]PROGMEM = "ENV2 DECAY      ";
const char PotDescription29[]PROGMEM = "LFO2 AMPLITUDE  ";
const char PotDescription30[]PROGMEM = "ENV2 RELEASE    ";
const char PotDescription31[]PROGMEM = "LFO2 SPEED      ";

// here you can add pots : const char PotDescription32[]PROGMEM = "  ";
// etc ...

const char PotDescription32[]PROGMEM = "VCF FM /ENV3    "; // ain_pin 0
const char PotDescription33[]PROGMEM = "VCA /VELO       ";
const char PotDescription34[]PROGMEM = "DCO1 F /LFO1    "; // 
const char PotDescription35[]PROGMEM = "VCA /ENV2       ";
const char PotDescription36[]PROGMEM = "DCO1 FREQ       ";
const char PotDescription37[]PROGMEM = "VCF FM/PRS      ";
const char PotDescription38[]PROGMEM = "VCF FREQ CUT    ";
const char PotDescription39[]PROGMEM = "PORTA /VELO     "; // A DEFINIR

const char PotDescription40[]PROGMEM = "DCO1 WAVE SHAPE "; // ain_pin 8
const char PotDescription41[]PROGMEM = "PORTA RATE      "; //  A DEFINIR
const char PotDescription42[]PROGMEM = "DCO1 PW /LFO2   ";
const char PotDescription43[]PROGMEM = "DCO BALANCE     ";
const char PotDescription44[]PROGMEM = "DCO1 PW         ";
const char PotDescription45[]PROGMEM = "DCO2 FREQ       ";
const char PotDescription46[]PROGMEM = "UNISON DETUNE   ";
const char PotDescription47[]PROGMEM = "DCO2 F/LFO1     ";

const char PotDescription48[]PROGMEM = "ENV1 AMPLITUDE  "; // ain_pin 16
const char PotDescription49[]PROGMEM = "DCO2 WAVE SHAPE ";
const char PotDescription50[]PROGMEM = "DCO2 DETUNE     ";
const char PotDescription51[]PROGMEM = "DCO2 PW         ";
const char PotDescription52[]PROGMEM = "DCO2 PW /LFO2   ";
const char PotDescription53[]PROGMEM = "ENV3 AMPLITUDE  ";
const char PotDescription54[]PROGMEM = "ENV2 AMPLITUDE  ";
const char PotDescription55[]PROGMEM = "ENV1 AMP /VELO  ";

const char PotDescription56[]PROGMEM = "ENV3 SUSTAIN    "; // ain_pin 24
const char PotDescription57[]PROGMEM = "LFO1 SPEED /PRS ";
const char PotDescription58[]PROGMEM = "ENV3 ATTACK     ";
const char PotDescription59[]PROGMEM = "LFO1 AMP /RAMP1 ";
const char PotDescription60[]PROGMEM = "ENV3 DECAY      ";
const char PotDescription61[]PROGMEM = "LFO2 AMP /RAMP2 ";
const char PotDescription62[]PROGMEM = "ENV3 RELEASE    ";
const char PotDescription63[]PROGMEM = "LFO2 SPEED /KBD ";

const char * const PotDescription[] PROGMEM =
{
  PotDescription0,
  PotDescription1,
  PotDescription2,
  PotDescription3,
  PotDescription4,
  PotDescription5,
  PotDescription6,
  PotDescription7,

  PotDescription8,
  PotDescription9,
  PotDescription10,
  PotDescription11,
  PotDescription12,
  PotDescription13,
  PotDescription14,
  PotDescription15,

  PotDescription16,
  PotDescription17,
  PotDescription18,
  PotDescription19,
  PotDescription20,
  PotDescription21,
  PotDescription22,
  PotDescription23,

  PotDescription24,
  PotDescription25,
  PotDescription26,
  PotDescription27,
  PotDescription28,
  PotDescription29,
  PotDescription30,
  PotDescription31,

  // here add pots 
  // etc ...
  PotDescription32,
  PotDescription33,
  PotDescription34,
  PotDescription35,
  PotDescription36,
  PotDescription37,
  PotDescription38,
  PotDescription39,

  PotDescription40,
  PotDescription41,
  PotDescription42,
  PotDescription43,
  PotDescription44,
  PotDescription45,
  PotDescription46,
  PotDescription47,

  PotDescription48,
  PotDescription49,
  PotDescription50,
  PotDescription51,
  PotDescription52,
  PotDescription53,
  PotDescription54,
  PotDescription55,

  PotDescription56,
  PotDescription57,
  PotDescription58,
  PotDescription59,
  PotDescription60,
  PotDescription61,
  PotDescription62,
  PotDescription63
 // Hint : watch out coma ;)
};

const pots PotConfigMap[] =
{ // in order : from ain_pin = 0 to ain_pin = 31

  // { param, min,  max ,  valtype , update_interval},
  {   SX_FILTER_FREQBYENV1AMOUNT, 0, 127, SIGNED7,   2}, // env1=param 22, // ain_pin 0 update_interval was 5
  {   SX_FILTER_RESONANCE,     0,   63, UNSIGNED6, 1}, // ain_pin 1
  {   SX_OSC1_FREQBYLFO1AMOUNT,  0,  127, SIGNED7,   2}, // ain_pin 2 update_interval was 5  
  {   SX_FILTER_FM_AMOUNT,     0,   63, UNSIGNED6, 1}, // ain_pin = 3 
  {   SX_OSC1_FREQ,            0,  63, UNSIGNED6, 1}, // ain_pin 4 
  {   SX_FILTER_FREQBYPRESSURE,  0,  127, SIGNED7,   2}, // ain_pin 5 update_interval was 5    SX_FILTER_FREQBYPRESSURE
  {   SX_FILTER_FREQ,          0,  127, UNSIGNED7, 1}, // ain_pin 6, update_interval was 2
  {   SX_RAMP2_RATE,             0,  63, UNSIGNED6, 1}, // ain_pin 7 A DEFINIR  

  {   SX_OSC1_WAVESHAPE,         0,   63, UNSIGNED6, 2}, // ain_pin 8   
  {   SX_RAMP1_RATE,            0,  63, UNSIGNED6, 1}, // ain_pin 9 A DEFINIR     
  {   SX_OSC1_PWBYLFO2AMOUNT,     0,  127, SIGNED7, 1}, // ain_pin 10
  {   SX_MIX,                     0,   63, MIXBALANCE, 1}, // ain_pin 11
  {   SX_OSC1_PULSEWIDTH,        0,   63, UNSIGNED6, 2}, // ain_pin 12
  {   SX_OSC2_FREQ,              0,  63, UNSIGNED6, 1}, // ain_pin 13
  {   SX_UNISON_DETUNE,        0,  63, UNSIGNED6, 1}, // ain_pin 14 Gligli unison detune
  {   SX_OSC2_FREQBYLFO1AMOUNT, 0,  127, SIGNED7,   3}, // making it positive only // ain_pin 15

  {   SX_ENV1_ATTACK,           0,   63, UNSIGNED6, 1}, // attack du dessus ain_pin 16
  {   SX_OSC2_WAVESHAPE,        0,   63, UNSIGNED6, 2}, // ain_pin 17
  {   SX_OSC2_DETUNE,            0,   63, SIGNED6,   2}, // ain_pin 18
  {   SX_OSC2_PULSEWIDTH,       0,   63, UNSIGNED6, 2}, // ain_pin 19
  {   SX_OSC2_PWBYLFO2AMOUNT,    0,  127, SIGNED7, 1}, // ain_pin 20
  {   SX_ENV1_SUSTAIN,           0,   63, UNSIGNED6, 3}, //ain_pin 21
  {   SX_ENV1_DECAY,            0,   63, UNSIGNED6, 1}, // du ain_pin 22
  {   SX_ENV1_RELEASE,         0,   63, UNSIGNED6, 1}, // du ain_pin 23

  {   SX_ENV2_SUSTAIN,           0,   63, UNSIGNED6, 1}, // env2// du ain_pin 24
  {   SX_LFO1_SPEED,             0,   63, UNSIGNED6, 1},// ain_pin 25
  {   SX_ENV2_ATTACK,           0,   63, UNSIGNED6, 1}, // ain_pin 26
  {   SX_LFO1_AMPLITUDE,         0,   63, UNSIGNED6, 1}, // ain_pin 27
  {   SX_ENV2_DECAY,            0,   63, UNSIGNED6, 1}, // ain_pin 28
  {   SX_LFO2_AMPLITUDE,       0,   63, UNSIGNED6, 1}, // ain_pin 29
  {   SX_ENV2_RELEASE,          0,   63, UNSIGNED6, 1}, // ain_pin 30
  {   SX_LFO2_SPEED,           0,   63, UNSIGNED6, 1}, // ain_pin 31
  
  // here complete characteristics corresponding to PotDescription32-63 (using ALT function): 
         
  {   SX_FILTER_FMBYENV3, 0,  127, SIGNED7, 1}, // ain_pin 0
  {   SX_VCA1_AMPBYVELOCITY, 0,  127, SIGNED7, 1}, // ain_pin 1, 
  {   SX_OSC1_FREQBYLFO1AMOUNT, 0,  127, SIGNED7,   2}, // ain_pin 2 update_interval was 5  
  {   SX_VCA2_AMPBYENV2,        0,  127, SIGNED7, 1}, // ain_pin = 3 
  {   SX_OSC1_FREQ,             0,  63, UNSIGNED6, 1}, // ain_pin 4 
  {   SX_FILTER_FMBYPRESSURE,   0,  127, SIGNED7,   3}, // ain_pin 5 update_interval was 5    
  {   SX_FILTER_FREQ,           0,  127, UNSIGNED7, 1}, // ain_pin 6, update_interval was 2
  {   SX_PORTAMENTO_SPEEDBYVELOCITY, 0,  127, SIGNED7, 2}, // ain_pin 7 A DEFINIR  

  {   SX_OSC1_WAVESHAPE,        0,   63, UNSIGNED6, 2}, // ain_pin 8   
  {   SX_PORTAMENTO_RATE,            0,  63, UNSIGNED6, 2}, // ain_pin 9 A DEFINIR     
  {   SX_OSC1_PWBYLFO2AMOUNT,   0,  127, SIGNED7, 1}, // ain_pin 10
  {   SX_MIX,                   0,   63, MIXBALANCE, 1}, // ain_pin 11
  {   SX_OSC1_PULSEWIDTH,       0,   63, UNSIGNED6, 2}, // ain_pin 12
  {   SX_OSC2_FREQ,             0,  63, UNSIGNED6, 1}, // ain_pin 13
  {   SX_UNISON_DETUNE,         0,  63, UNSIGNED6, 1}, // ain_pin 14 Global detune
  {   SX_OSC2_FREQBYLFO1AMOUNT, 0,  127, SIGNED7,   3}, // making it positive only // ain_pin 15

  {   SX_ENV1_AMPLITUDE,        0,   63, UNSIGNED6, 1}, // attack du dessus ain_pin 16
  {   SX_OSC2_WAVESHAPE,        0,   63, UNSIGNED6, 2}, // ain_pin 17
  {   SX_OSC2_DETUNE,           0,   63, SIGNED6,   2}, // ain_pin 18
  {   SX_OSC2_PULSEWIDTH,       0,   63, UNSIGNED6, 2}, // ain_pin 19
  {   SX_OSC2_PWBYLFO2AMOUNT,   0,  127, SIGNED7, 1}, // ain_pin 20
  {   SX_ENV3_AMPLITUDE,        0,   63, UNSIGNED6, 1}, //ain_pin 21
  {   SX_ENV2_AMPLITUDE,        0,   63, UNSIGNED6, 1}, // du ain_pin 22
  {   SX_ENV1_AMPBYVELO,        0,  127, SIGNED7, 5}, // du ain_pin 23

  {   SX_ENV3_SUSTAIN,          0,   63, UNSIGNED6, 1}, // env2// du ain_pin 24
  {   SX_LFO1_SPEEDMODULATION,  0,  127, SIGNED7, 1},// ain_pin 25
  {   SX_ENV3_ATTACK,           0,   63, UNSIGNED6, 1}, // ain_pin 26
  {   SX_LFO1_AMPMODULATION,    0,  127, SIGNED7, 1}, // ain_pin 27
  {   SX_ENV3_DECAY,            0,   63, UNSIGNED6, 1}, // ain_pin 28
  {   SX_LFO2_AMPMODULATION,    0,  127, SIGNED7, 1}, // ain_pin 29
  {   SX_ENV3_RELEASE,          0,   63, UNSIGNED6, 1}, // ain_pin 30
  {   SX_LFO2_SPEEDMODULATION,  0,  127, SIGNED7, 1} // ain_pin 31
};



