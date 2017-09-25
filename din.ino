//#include <cmios.h>
//#include "main.h"
#include "pots.h"
//#include "lcd.h"
#include "din.h"
#include "chaosmatrix.h"
//#include "midi.h"
//#include "device.h"

const char DinDescription0 [] PROGMEM = " DCO2 WAVEFORM "; // din_pin 0
const char DinDescription1 [] PROGMEM = " _F1_          "; // 1
const char DinDescription2 [] PROGMEM = " DCO2 WAVEFORM ";
const char DinDescription3 [] PROGMEM = " DCO2 CLIC     ";
const char DinDescription4 [] PROGMEM = " _PATCH_       ";
const char DinDescription5 [] PROGMEM = " _EDIT_        ";
const char DinDescription6 [] PROGMEM = " _ARP_         ";
const char DinDescription7 [] PROGMEM = " _CFG_         ";
const char DinDescription8 [] PROGMEM = " _KEY2_        "; // 8
const char DinDescription9 [] PROGMEM = " _KEY1_        ";
const char DinDescription10 [] PROGMEM = " _INC_         ";
const char DinDescription11 [] PROGMEM = " _DEC_         ";
const char DinDescription12 [] PROGMEM = " _KEY3_        ";
const char DinDescription13 [] PROGMEM = " _KEY4_        ";
const char DinDescription14 [] PROGMEM = " _KEY5_        ";
const char DinDescription15 [] PROGMEM = " _PAGE_        ";
const char DinDescription16 [] PROGMEM = " UNUSED DIN16  "; // 16
const char DinDescription17 [] PROGMEM = " _ALT_         ";
const char DinDescription18 [] PROGMEM = " _SHIFT_       ";
const char DinDescription19 [] PROGMEM = " _F2_          ";
const char DinDescription20 [] PROGMEM = " LFO1 WAVESHAPE ";
const char DinDescription21 [] PROGMEM = " LFO2 WAVESHAPE ";
const char DinDescription22 [] PROGMEM = " UNUSED DIN22  "; 
const char DinDescription23 [] PROGMEM = " DCO2 WAVEFORM "; // new switch for DIN pin0
const char DinDescription24 [] PROGMEM = " DCO1 WAVEFORM "; 
const char DinDescription25 [] PROGMEM = " DCO SYNC MODE "; 
const char DinDescription26 [] PROGMEM = " DCO1 WAVEFORM "; 
const char DinDescription27 [] PROGMEM = " DCO1 CLIC     "; 
const char DinDescription28 [] PROGMEM = " _DEVICE_A_    "; 
const char DinDescription29 [] PROGMEM = " _DEVICE_B_    ";
const char DinDescription30 [] PROGMEM = " _DEVICE_C_    ";
const char DinDescription31 [] PROGMEM = " _DEVICE_D_    ";

const char * const DinDescription [] PROGMEM = 
{
 DinDescription0,
 DinDescription1,
 DinDescription2,
 DinDescription3,
 DinDescription4,
 DinDescription5,
 DinDescription6,
 DinDescription7,

 DinDescription8,
 DinDescription9,
 DinDescription10,
 DinDescription11,
 DinDescription12,
 DinDescription13,
 DinDescription14,
 DinDescription15,

 DinDescription16,
 DinDescription17,
 DinDescription18,
 DinDescription19,
 DinDescription20,
 DinDescription21,
 DinDescription22,
 DinDescription23,

 DinDescription24,
 DinDescription25,
 DinDescription26,
 DinDescription27,
 DinDescription28,
 DinDescription29,
 DinDescription30,
 DinDescription31
 
};

const buttons_t DIN_ConfigMap[32] = {     // arduino SRIO pin (to place in order) : mettre des define pour la 3 eme colonne, + facile à paraméterer
 //mon proto à moi sous arduino LED :
 // BUTGRP_LIVE BUTGRP_IGNORE BUTGRP_SOFT  BUTGRP_DEVICE  BUTGRP_SHIFT
 //   { group,  dout }// Din_pin / nom / led
 /* 4 */ { BUTGRP_LIVE, DOUT_DCO2_PULSE, }, // 3 DCO2 PULSE 
 /* 7 */ { BUTGRP_SOFT, DOUT_F1, }, // 1 F1
 /* 5 */ { BUTGRP_LIVE, DOUT_DCO2_WAVE, }, // 2 DCO2 WAVE
 /* 6 */ { BUTGRP_LIVE, DOUT_DCO2_CLICK, }, // 0 DCO2 CLICK  
 /* 3 */ { BUTGRP_SOFT, DOUT_PATCH_SELECT, }, // 4 PATCH
 /* 2 */ { BUTGRP_SOFT, DOUT_EDIT, }, // 5 EDIT
 /* 0 */ { BUTGRP_SOFT, DOUT_ARP, }, // 6 ARP
 /* 1 */ { BUTGRP_SOFT, DOUT_CFG , }, // 7 _SHIFT_
 /* 15*/ { BUTGRP_SOFT, DOUT_FILTER, }, // 8 FILTER
 /* 14*/ { BUTGRP_SOFT, DOUT_DCO, }, // 9 OSCILLATORS
 /* 13*/ { BUTGRP_SOFT, DOUT_LED31, }, // 10 _INC
 /* 12*/ { BUTGRP_SOFT, DOUT_LED31, }, //11 _DEC
 /* 11*/ { BUTGRP_SOFT, DOUT_ENV, }, //12 ENVELOPES
 /* 10*/ { BUTGRP_SOFT, DOUT_KBD, }, //13 KEYBOARD
 /* 9 */ { BUTGRP_SOFT, DOUT_MATRIXMOD, }, //14 MATRIX modulation
 /* 8 */ { BUTGRP_SOFT, DOUT_PAGE, }, // 15 PAGE
 /* 23*/ { BUTGRP_TRIG, DOUT_LED31, }, // 16 UNUSED DIN16 
 /* 22*/ { BUTGRP_SHIFT, DOUT_ALT, }, // 17 ALT
 /* 21*/ { BUTGRP_SHIFT, DOUT_SHIFT, }, // 18 CFG
 /* 20*/ { BUTGRP_SOFT, DOUT_F2, }, // 19 F2
 /* 19*/ { BUTGRP_LIVE, DOUT_LFO1_WAVESELECT, }, // 20 LFO1 WAVESHAPE 
 /* 18*/ { BUTGRP_LIVE, DOUT_LFO2_WAVESELECT, }, // 21 LFO2 WAVESHAPE
 /* 17*/ { BUTGRP_LIVE, DOUT_LED27, }, // 22 UNUSED DIN22
 /* 16*/ { BUTGRP_LIVE, DOUT_DCO2_PULSE, }, // 23 new tact switch select DIN23
 /* 30*/ { BUTGRP_LIVE, DOUT_DCO1_PULSE, }, // 24 DCO1 CLICK  
 /* 31*/ { BUTGRP_LIVE, DOUT_OSC_SYNC, }, // 25 DCO SYNC MODE 
 /* 29*/ { BUTGRP_LIVE, DOUT_DCO1_WAVE, }, // 26 DCO1 WAVE
 /* 28*/ { BUTGRP_LIVE, DOUT_DCO1_CLICK, }, // 27 DCO1 PULSE
 /* 27*/ { BUTGRP_DEVICE, DOUT_MATRIX_A, }, // 28 _MATRIX_A
 /* 26*/ { BUTGRP_DEVICE, DOUT_MATRIX_B, }, // 29 _MATRIX_B
 /* 25*/ { BUTGRP_DEVICE, DOUT_MATRIX_C, }, // 30 _MATRIX_C
 /* 24*/ { BUTGRP_DEVICE, DOUT_MATRIX_D, }, // 31 _MATRIX_D
};


// updates the led status based on edit buffer
void UpdateDinStates(void)
{

 // pulse
 DOUT_PinSet(DOUT_DCO1_PULSE, (EditBuffer[device][EB_OSC1_WAVEFORM] & (1 << BIT0)) != 0);
 DOUT_PinSet(DOUT_DCO2_PULSE, (EditBuffer[device][EB_OSC2_WAVEFORM] & (1 << BIT0)) != 0);

 // wave
 DOUT_PinSet(DOUT_DCO1_WAVE, (EditBuffer[device][EB_OSC1_WAVEFORM] & (1 << BIT1)) != 0);
 DOUT_PinSet(DOUT_DCO2_WAVE, (EditBuffer[device][EB_OSC2_WAVEFORM] & (1 << BIT1)) != 0);

 // click
 DOUT_PinSet(DOUT_DCO1_CLICK, (EditBuffer[device][EB_OSC1_CLICK] & (1 << BIT0)) != 0);
 DOUT_PinSet(DOUT_DCO2_CLICK, (EditBuffer[device][EB_OSC2_CLICK] & (1 << BIT0)) != 0);

 // noise bit?
 if ((EditBuffer[device][EB_OSC2_WAVEFORM] & (1 << BIT2)) != 0)
  Osc2_ClickMode = 2;
 else
  Osc2_ClickMode = 0;

 // sync mode
 if (EditBuffer[device][EB_OSC_SYNCMODE] > 0)
  DOUT_PinSet(DOUT_OSC_SYNC, 1);
 else
  DOUT_PinSet(DOUT_OSC_SYNC, 0);

// // reset pulse blinking
// Osc_Pulse_Modulate[DCO1] = 0;
// Osc_Pulse_Modulate[DCO2] = 0;

#if DEBUG_dout
 Serial.println(F("UpdateDinStates()"));
  Serial.println();
#endif
}
