//#include "chaosmatrix.h"
#ifndef din_h
#define din_h

#define DIN_STATE_OFF			0
#define DIN_STATE_ON			1
#define DOUT_STATE_OFF			0
#define DOUT_STATE_ON			1

//#define DCO_PULSE_PIN 13 // zero based
//#define DCO_WAVE_PIN 14 // zero based
#define MMOD_SRC_ENV1	  1
#define MMOD_SRC_ENV2	  2
#define MMOD_SRC_ENV3	  3//8
#define MMOD_DEST_DCO1_Freq 1
#define DOUT_NULLPIN	  63

//mon proto à moi sous arduino :
// DIN CONFIG LIVE PANEL
// DCO
#define DIN_DCO1_PULSE   24 // was 27
#define DIN_DCO1_WAVE    26
#define DIN_DCO1_CLICK   27 // was 24

#define DIN_DCO2_PULSE   0 // was 3
#define DIN_DCO2_WAVE    2
#define DIN_DCO2_CLICK   3 // was 0
#define DIN_DCO_SYNC 25

// LFO
#define DIN_LFO1_WAVESELECT 20
#define DIN_LFO2_WAVESELECT 21

// VCF

// DIN Defines : 
#define DIN_OSCILLATORS 9
#define SOFT_EDIT_1     DIN_OSCILLATORS
#define DIN_FILTER      8
#define SOFT_EDIT_2     DIN_FILTER
#define DIN_ENVELOPES   12
#define SOFT_EDIT_3     DIN_ENVELOPES
#define DIN_KEYBOARD    13
#define SOFT_EDIT_4     DIN_KEYBOARD
#define DIN_MATRIX      14
#define SOFT_EDIT_5     DIN_MATRIX
#define DIN_PAGE        15

#define DIN_PATCH       4
#define DIN_EDIT        5
#define DIN_ARP         6 
#define DIN_SHIFT       18 // was 7
#define SOFT_EDIT_ARP   DIN_ARP
#define SOFT_EDIT_SHIFT DIN_SHIFT
#define SOFT_EDIT_ENC_CLIC 32

#define DIN_F1        1
#define DIN_F2        19
#define DIN_CFG       7 // was 18
#define DIN_ALT       17

#define SOFT_EDIT_F1    DIN_F1        
#define SOFT_EDIT_F2    DIN_F2       
#define SOFT_EDIT_CFG   DIN_CFG       
#define SOFT_EDIT_ALT   DIN_ALT       

#define DIN_SW9     23
#define DIN_SW10    22
#define DIN_SW16    16

#define DIN_MATRIX_A   28
#define DIN_MATRIX_B   29
#define DIN_MATRIX_C   30
#define DIN_MATRIX_D   31
#define DIN_INC        10
#define DIN_DEC        11

#define SOFT_EDIT_INC  DIN_INC
#define SOFT_EDIT_DEC  DIN_DEC

// DOUT DEFINES : 
#define DOUT_LFO1_WAVESELECT 26 // dout pin
#define DOUT_LFO2_WAVESELECT 27 // dout pin
#define DOUT_DCO1_PULSE   2 // led D2 0
#define DOUT_DCO1_WAVE    1
#define DOUT_DCO1_CLICK   0 // 2

#define DOUT_DCO2_PULSE   7 //5
#define DOUT_DCO2_WAVE    6
#define DOUT_DCO2_CLICK   5 //7
#define DOUT_DCO2_NOISE   DOUT_DCO2_CLICK // 5

#define DOUT_ACTIVITY     13 // led midi1
#define DOUT_ACTIVITY2     12 // led midi2 D14
#define DOUT_OSC_SYNC     4 // led D6

#define DOUT_MATRIX_A   8 //led D10
#define DOUT_MATRIX_B   11
#define DOUT_MATRIX_C   9
#define DOUT_MATRIX_D   10

#define DOUT_PATCH_SELECT 20 // led D23
#define DOUT_EDIT       21 // led D24
#define DOUT_ARP        22 // led D25
#define DOUT_SHIFT      30 //ledD26 23

#define DOUT_F1   3 // led D5
#define DOUT_F2   29 // led D32
#define DOUT_CFG  23 // led D33 30
#define DOUT_ALT  31 // led D34

#define DOUT_DCO  14 // led D16 KEY1
#define DOUT_FILTER 15 // KEY2
#define DOUT_ENV 16 // KEY3
#define DOUT_KBD 17 // KEY4
#define DOUT_MATRIXMOD 18 // KEY5
#define DOUT_PAGE 19 //

#define DOUT_LED27  24
#define DOUT_LED28  25
#define DOUT_LED31  28

#define BUTGRP_IGNORE  0
#define BUTGRP_LIVE	  1
#define BUTGRP_SOFT	  2
#define BUTGRP_DEVICE 3
#define BUTGRP_SHIFT  4
#define BUTGRP_TRIG 5

void UpdateDinStates(void);

typedef struct {
//  const  char description[18];
  const unsigned char group;
  const unsigned char dout_pin; 	//  corresponding dout led to show its on/off toggled state
} buttons_t;
extern const buttons_t DIN_ConfigMap[32];
extern const char * const DinDescription [] PROGMEM;

#endif
