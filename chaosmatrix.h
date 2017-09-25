#ifndef chaosmatrix_h
#define chaosmatrix_h


/*
    used for blinking LEDs like LFO speed on the
    LFO waveform buttons
*/
#define TIMER_PRESCALER 3
#define TIMER_TICKS 32000
#define LED_BLINK_SPEED 16
#define SYNC_BLINK_SPEED 7
#define F1F2_BLINK_SPEED 8

#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3

#define FILTER_ENV_MODE_BUGGED      0	// ignores env1 sustain pot
#define FILTER_ENV_MODE_HACKED      1	// transmits filter sustain by sending entire edit buffer
#define FILTER_FREQ_MODE_SYSEX      0   // values for FilterCutoffMode, pot tweaks send out Sysex
#define FILTER_FREQ_MODE_CCONTROL   1   // pot tweaks send out continuous controller

#define KBDMODE_REASSIGN 0
#define KBDMODE_ROTATE 1
#define KBDMODE_UNISON 2
#define KBDMODE_REAROB 3
#define DEFAULT_KBD_MODE KBDMODE_REASSIGN


/*
    Oberheim Matrix 1000 sysex parameter numbers
    in byte order
*/
#define SX_KEYBOARD_MODE 		                 48
#define SX_OSC1_FREQ 			                   0
#define SX_OSC1_WAVESHAPE		                 5
#define SX_OSC1_PULSEWIDTH		               3
#define SX_OSC1_LEVERMOD 		                 7
#define SX_OSC1_WAVEFORM		                 6
#define SX_OSC2_FREQ			                   10
#define SX_OSC2_WAVESHAPE		                 15
#define SX_OSC2_PULSEWIDTH		               13
#define SX_OSC2_LEVERMOD		                 17
#define SX_OSC2_WAVEFORM		                 16
#define SX_OSC2_DETUNE			                 12
#define SX_MIX					                     20
#define SX_OSC1_KEYBOARDMOD		               8
#define SX_OSC1_CLICK			                   9
#define SX_OSC2_KEYBOARDMOD		               18
#define SX_OSC2_CLICK			                   19
#define SX_OSC_SYNCMODE			                 2
#define SX_FILTER_FREQ			                 21
#define SX_FILTER_RESONANCE		               24
#define SX_FILTER_LEVERMOD		               25   // pbend, vibrato
#define SX_FILTER_KEYBOARDMOD	               26
#define SX_FILTER_FM_AMOUNT		               30
#define SX_VCA1_AMOUNT			                 27
#define SX_PORTAMENTO_RATE		               44
#define SX_PORTAMENTO_MODE		               46
#define SX_PORTAMENTO_LEGATOENABLE	         47
#define SX_LFO1_SPEED			                   80
#define SX_LFO1_TRIGGERMODE		               86
#define SX_LFO1_LAGENABLE		                 87
#define SX_LFO1_WAVESHAPE		                 82
#define SX_LFO1_RETRIGGERPOINT	             83
#define SX_LFO1_SAMPLEDSOURCE	               88
#define SX_LFO1_AMPLITUDE		                 84
#define SX_LFO2_SPEED			                   90
#define SX_LFO2_TRIGGERMODE		               96
#define SX_LFO2_LAGENABLE		                 97
#define SX_LFO2_WAVESHAPE		                 92
#define SX_LFO2_RETRIGGERPOINT	             93
#define SX_LFO2_SAMPLEDSOURCE	               98
#define SX_LFO2_AMPLITUDE		                 94
#define SX_ENV1_TRIGGERMODE		               57
#define SX_ENV1_DELAY			                   50
#define SX_ENV1_ATTACK			                 51
#define SX_ENV1_DECAY			                   52
#define SX_ENV1_SUSTAIN			                 53
#define SX_ENV1_RELEASE			                 54
#define SX_ENV1_AMPLITUDE		                 55
#define SX_ENV1_LFOTRIGGERMODE	             59
#define SX_ENV1_RELEASEMODE		               58
#define SX_ENV2_TRIGGERMODE		               67
#define SX_ENV2_DELAY			                   60
#define SX_ENV2_ATTACK			                 61
#define SX_ENV2_DECAY			                   62
#define SX_ENV2_SUSTAIN			                 63
#define SX_ENV2_RELEASE			                 64
#define SX_ENV2_AMPLITUDE		                 65
#define SX_ENV2_LFOTRIGGERMODE	             69
#define SX_ENV2_RELEASEMODE		               68
#define SX_ENV3_TRIGGERMODE		               77
#define SX_ENV3_DELAY			                   70
#define SX_ENV3_ATTACK			                 71
#define SX_ENV3_DECAY			                   72
#define SX_ENV3_SUSTAIN			                 73
#define SX_ENV3_RELEASE			                 74
#define SX_ENV3_AMPLITUDE		                 75
#define SX_ENV3_LFOTRIGGERMODE	             79
#define SX_ENV3_RELEASEMODE		               78
#define SX_TRACK_INPUT			                 33
#define SX_TRACK_POINT1			                 34
#define SX_TRACK_POINT2			                 35
#define SX_TRACK_POINT3			                 36
#define SX_TRACK_POINT4			                 37
#define SX_TRACK_POINT5			                 38
#define SX_RAMP1_RATE			                   40
#define SX_RAMP1_MODE			                   41
#define SX_RAMP2_RATE			                   42
#define SX_RAMP2_MODE			                   43
#define SX_OSC1_FREQBYLFO1AMOUNT             1
#define SX_OSC1_PWBYLFO2AMOUNT	             4
#define SX_OSC2_FREQBYLFO1AMOUNT             11
#define SX_OSC2_PWBYLFO2AMOUNT	             14
#define SX_FILTER_FREQBYENV1AMOUNT           22
#define SX_FILTER_FREQBYPRESSURE             23
#define SX_VCA1_AMPBYVELOCITY	               28
#define SX_VCA2_AMPBYENV2		                 29
#define SX_ENV1_AMPBYVELO		                 56
#define SX_ENV2_AMPBYVELO		                 66
#define SX_ENV3_AMPBYVELO		                 76
#define SX_LFO1_AMPMODULATION	               85 // ramp1
#define SX_LFO2_AMPMODULATION	               95 // ramp2
#define SX_PORTAMENTO_SPEEDBYVELOCITY 	     45
#define SX_FILTER_FMBYENV3		               31
#define SX_FILTER_FMBYPRESSURE	             32
#define SX_LFO1_SPEEDMODULATION	             81 // pressure
#define SX_LFO2_SPEEDMODULATION	             91 // keyboard tracking
// Gligli OS116 additional parameter :
#define SX_UNISON_DETUNE                     99

// edit buffer byte order
// 0-7 patch name characters
#define EB_PATCHNAME0 0
#define EB_PATCHNAME1 1 
#define EB_PATCHNAME2 2 
#define EB_PATCHNAME3 3 
#define EB_PATCHNAME4 4 
#define EB_PATCHNAME5 5
#define EB_PATCHNAME6 6
#define EB_PATCHNAME7 7
#define EB_KEYBOARD_MODE 		8
#define EB_OSC1_FREQ 			9
#define EB_OSC1_WAVESHAPE		10
#define EB_OSC1_PULSEWIDTH		11
#define EB_OSC1_LEVERMOD 		12
#define EB_OSC1_WAVEFORM		13
#define EB_OSC2_FREQ			14
#define EB_OSC2_WAVESHAPE		15
#define EB_OSC2_PULSEWIDTH		16
#define EB_OSC2_LEVERMOD		17
#define EB_OSC2_WAVEFORM		18
#define EB_OSC2_DETUNE			19
#define EB_MIX				20
#define EB_OSC1_KEYBOARDMOD		21
#define EB_OSC1_CLICK			22
#define EB_OSC2_KEYBOARDMOD		23
#define EB_OSC2_CLICK			24
#define EB_OSC_SYNCMODE			25
#define EB_FILTER_FREQ			26
#define EB_FILTER_RESONANCE		27
#define EB_FILTER_LEVERMOD		28
#define EB_FILTER_KEYBOARDMOD	        29
#define EB_FILTER_FM_AMOUNT		30
#define EB_VCA1_AMOUNT			31
#define EB_PORTAMENTO_RATE		32
#define EB_PORTAMENTO_MODE		33
#define EB_PORTAMENTO_LEGATOENABLE	34
#define EB_LFO1_SPEED			35
#define EB_LFO1_TRIGGERMODE		36
#define EB_LFO1_LAGENABLE		37
#define EB_LFO1_WAVESHAPE		38
#define EB_LFO1_RETRIGGERPOINT	        39
#define EB_LFO1_SAMPLEDSOURCE	        40
#define EB_LFO1_AMPLITUDE		41
#define EB_LFO2_SPEED			42
#define EB_LFO2_TRIGGERMODE		43
#define EB_LFO2_LAGENABLE		44
#define EB_LFO2_WAVESHAPE		45
#define EB_LFO2_RETRIGGERPOINT	        46
#define EB_LFO2_SAMPLEDSOURCE	        47
#define EB_LFO2_AMPLITUDE		48
#define EB_ENV1_TRIGGERMODE		49
#define EB_ENV1_DELAY			50
#define EB_ENV1_ATTACK			51
#define EB_ENV1_DECAY			52
#define EB_ENV1_SUSTAIN			53
#define EB_ENV1_RELEASE			54
#define EB_ENV1_AMPLITUDE		55
#define EB_ENV1_LFOTRIGGERMODE	        56
#define EB_ENV1_RELEASEMODE		57
#define EB_ENV2_TRIGGERMODE		58
#define EB_ENV2_DELAY			59
#define EB_ENV2_ATTACK			60
#define EB_ENV2_DECAY			61
#define EB_ENV2_SUSTAIN			62
#define EB_ENV2_RELEASE			63
#define EB_ENV2_AMPLITUDE		64
#define EB_ENV2_LFOTRIGGERMODE	        65
#define EB_ENV2_RELEASEMODE		66
#define EB_ENV3_TRIGGERMODE		67
#define EB_ENV3_DELAY			68
#define EB_ENV3_ATTACK			69
#define EB_ENV3_DECAY			70
#define EB_ENV3_SUSTAIN			71
#define EB_ENV3_RELEASE			72
#define EB_ENV3_AMPLITUDE		73
#define EB_ENV3_LFOTRIGGERMODE	        74
#define EB_ENV3_RELEASEMODE		75
#define EB_TRACK_INPUT			76
#define EB_TRACK_POINT1			77
#define EB_TRACK_POINT2			78
#define EB_TRACK_POINT3			79
#define EB_TRACK_POINT4			80
#define EB_TRACK_POINT5			81
#define EB_RAMP1_RATE			82
#define EB_RAMP1_MODE			83
#define EB_RAMP2_RATE			84
#define EB_RAMP2_MODE			85
#define EB_OSC1_FREQBYLFO1AMOUNT         86
#define EB_OSC1_PWBYLFO2AMOUNT	        87
#define EB_OSC2_FREQBYLFO1AMOUNT         88
#define EB_OSC2_PWBYLFO2AMOUNT	        89
#define EB_FILTER_FREQBYENV1AMOUNT       90
#define EB_FILTER_FREQBYPRESSURE         91
#define EB_VCA1_AMPBYVELOCITY	        92
#define EB_VCA2_AMPBYENV2		93
#define EB_ENV1_AMPBYVELO		94
#define EB_ENV2_AMPBYVELO		95
#define EB_ENV3_AMPBYVELO		96
#define EB_LFO1_AMPMODULATION	        97 // ramp1
#define EB_LFO2_AMPMODULATION	        98 // ramp2
#define EB_PORTAMENTO_SPEEDBYVELOCITY 	99
#define EB_FILTER_FMBYENV3		100
#define EB_FILTER_FMBYPRESSURE	        101
#define EB_LFO1_SPEEDMODULATION	        102 // pressure
#define EB_LFO2_SPEEDMODULATION	        103 // keyboard tracking
#define EB_MODBUS0_SOURCE		104
#define EB_MODBUS0_AMOUNT		105
#define EB_MODBUS0_DESTINATION 	        106
#define EB_MODBUS1_SOURCE  		107
#define EB_MODBUS1_AMOUNT		108
#define EB_MODBUS1_DESTINATION 	        109
#define EB_MODBUS2_SOURCE  		110
#define EB_MODBUS2_AMOUNT		111
#define EB_MODBUS2_DESTINATION 	        112
#define EB_MODBUS3_SOURCE  		113
#define EB_MODBUS3_AMOUNT		114
#define EB_MODBUS3_DESTINATION 	        115
#define EB_MODBUS4_SOURCE  		116
#define EB_MODBUS4_AMOUNT		117
#define EB_MODBUS4_DESTINATION 	        118
#define EB_MODBUS5_SOURCE  		119
#define EB_MODBUS5_AMOUNT		120
#define EB_MODBUS5_DESTINATION 	        121
#define EB_MODBUS6_SOURCE  		122
#define EB_MODBUS6_AMOUNT		123
#define EB_MODBUS6_DESTINATION 	        124
#define EB_MODBUS7_SOURCE  		125
#define EB_MODBUS7_AMOUNT		126
#define EB_MODBUS7_DESTINATION 	        127
#define EB_MODBUS8_SOURCE  		128
#define EB_MODBUS8_AMOUNT		129
#define EB_MODBUS8_DESTINATION 	        130
#define EB_MODBUS9_SOURCE  		131
#define EB_MODBUS9_AMOUNT		132
#define EB_MODBUS9_DESTINATION 	        133

// Gligli OS116 additional parameter :
#define EB_UNISON_DETUNE                     134

#define MMOD_BUS1 0
#define MMOD_BUS2 1
#define MMOD_BUS3 2
#define MMOD_BUS4 3
#define MMOD_BUS5 4
#define MMOD_BUS6 5
#define MMOD_BUS7 6
#define MMOD_BUS8 7
#define MMOD_BUS9 8
#define MMOD_BUS10 9

#define MMOD_SRC_UNUSED 0
#define MMOD_SRC_ENV1   1
#define MMOD_SRC_ENV2   2
#define MMOD_SRC_ENV3   3
#define MMOD_SRC_LFO1   4
#define MMOD_SRC_LFO2   5
#define MMOD_SRC_VEL    13
#define MMOD_SRC_LEVER3 20    // breath control

#define MMOD_DEST_UNUSED        0
#define MMOD_DEST_DCO1_Freq     1
#define MMOD_DEST_DCO2_Freq     4
#define MMOD_DEST_DCO1_PW       2
#define MMOD_DEST_FILTER_FREQ   9

#define NUM_OSCILLATORS     2
#define NUM_LFOS            2
#define NUM_ENVS            3
#define NUM_RAMPS           2

// parameters stored in arrays
#define DCO1 0
#define DCO2 1
#define LFO1 0
#define LFO2 1
#define ENV1 0
#define ENV2 1
#define ENV3 2
#define RAMP1 0
#define RAMP2 1

#define OFF 0
#define ON  1

#define UNSIGNED6   0
#define SIGNED6     1
#define UNSIGNED7   2
#define SIGNED7     3
#define MIXBALANCE  4

//extern const unsigned char DefaultEditBuffer[134];

extern unsigned char EditBuffer[4][134]; // was 134 bytes originally, now 4x134
extern unsigned char EditBufferOrig[134]; // for the compare function
extern unsigned char UnisonDetune[4];
extern unsigned char UnisonDetuneOrig;
extern const unsigned char singlePatchDataFormatCC[100][9];
extern const unsigned char singlePatchDataFormatX[128][9];
extern const unsigned char singlePatchDataFormatEB[97][9];

extern unsigned char ProgramNumber;     // patches
extern unsigned char ProgramNumberEdit; // patches
extern unsigned char BankNumber;
extern unsigned char BankNumberEdit;
extern unsigned char Osc_Pulse_Modulate[NUM_OSCILLATORS];
extern unsigned char FilterCutoffMode;  // 0=sysex, 1=lever 3 (continuous controller)
extern unsigned char FilterSustainMode;     // use FILTER_ENV_MODE_BUGGED or FILTER_ENV_MODE_HACKED
extern unsigned char Osc2_ClickMode;    // DCO2 click button toggles click and noise waveform. 0=both off, 1=click, 2=noise

//extern void PrintDescription(code char *pretext, code char *description);
void PrintDescription( char *pretext,  char *description);
extern void SetPatchBank(void);
extern void SetPatchChange(void);

extern void InitEditBuffer(void);
extern void SendPatchInit(unsigned char  interface);
extern void SendEditBuffer(unsigned char device, unsigned char  interface);
extern void SendEditBufferOrig(unsigned char  interface);
extern void SendSinglePatchData(unsigned char interface, unsigned char patch);

extern unsigned char Convert_7bit_Signed(unsigned char value);
extern unsigned char Convert_6bit_Signed(unsigned char value);

#endif

