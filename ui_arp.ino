//#include <cmios.h>
#include "chaosmatrix.h"
#include "softpanel.h"
#include "ui_arp.h"
#include "din.h"
//#include "main.h"
#include "lcd.h"
//#include "mclock.h"
#include "arp.h"
#include "memo.h"
#include "seq.h"

#define BPM_Max 230 // less than 255
#define BPM_min 20 // more than 0
#define ui_aGate_Max 1024
#define aMltp_MAX 4
/*
  // array to coorespon ui_aSpeed and musical beat denominations
  const unsigned char arp_div[14] = { // for 24ppqn
  0,  // 64:4 = 16:1 = quadruple croche (16 plays per 1black)
  2,  // 12:1  = quadruple croche pointée
  3,  // 32:4 = 8:1 = triple croche
  4,  // 6:1 = triple croche pointée
  6,  // 16:4 = 4:1 = double croche
  8,  // 3:1 = 3 play per 1 black
  12, // 8:4 = 2:1 = croche
  16, // 3:2 = croche pointée
  24, // 1:1 = noire (24 ppqn)
  36, // 2:3 = noire pointée
  48, // 1:2 = blanche
  72, // 3:4 blanche pointée (dure 3 temps dans une mesure 4/4)
  96, // 1:4 = ronde (dure 4 temps dans une mesure 4/4)
  144,
  };

*/


//// array to coorespon ui_aSpeed and musical beat denominations
//const unsigned int arp_div[14] = { // for 96ppqn
//  0,  // 64:4 = 16:1 = quadruple croche (16 plays per 1black)
//  8,  // 12:1  = quadruple croche pointée
//  12, // 32:4 = 8:1 = triple croche
//  16, // 6:1 = triple croche pointée
//  24, // 16:4 = 4:1 = double croche
//  32, // 3:1 = 3 play per 1 black
//  48, // 8:4 = 2:1 = croche
//  64, // 3:2 = croche pointée
//  96, // 1:1 = noire (24 ppqn)
//  144, // 2:3 = noire pointée
//  196, // 1:2 = blanche
//  288, // 3:4 blanche pointée (dure 3 temps dans une mesure 4/4)
//  384, // 1:4 = ronde (dure 4 temps dans une mesure 4/4)
//  556,// ronde pointée
//};

// array to coorespon ui_aSpeed and musical beat denominations
const unsigned int arp_div[14] = { // for 24ppqn
  0,  // 64:4 = 16:1 = quadruple croche (16 plays per 1black)
  2,  // 12:1  = quadruple croche pointée
  3, // 32:4 = 8:1 = triple croche
  4, // 6:1 = triple croche pointée
  6, // 16:4 = 4:1 = double croche
  8, // 3:1 = 3 play per 1 black
  12, // 8:4 = 2:1 = croche
  16, // 3:2 = croche pointée
  24, // 1:1 = noire (24 ppqn)
  36, // 2:3 = noire pointée
  48, // 1:2 = blanche
  72, // 3:4 blanche pointée (dure 3 temps dans une mesure 4/4)
  96, // 1:4 = ronde (dure 4 temps dans une mesure 4/4)
  144,// ronde pointée
};
//const unsigned char ui_ui_aSpeed[14][5] = {
//  {"16:1"}, // quadruple croche
//  {"12:1"},
//  {" 8:1"}, // triple croche
//  {" 6:1"},
//  {" 4:1"}, // double croche
//  {" 3:1"},
//  {" 2:1"}, // croche
//  {" 3:2"},
//  {" 1:1"}, // noire
//  {" 2:3"},
//  {" 1:2"}, // blanche
//  {" 3:4"},
//  {" 1:4"}, // ronde
//  {" o. "}, // ronde pointée
//};
//
//const unsigned char ui_patchbay_limits[9][4] = {
//  {"LD1"},
//  {"LU1"},
//  {"LD2"},
//  {"LU2"},
//  {"LD3"},
//  {"LU3"},
//  {"LD4"},
//  {"LU4"},
//  {"end"},
//};


////////// Split note in a musical way instead of unsigned char :
const char SPLIT_Note0[] PROGMEM = {"C-1 "};
const char SPLIT_Note1[] PROGMEM = {"C#-1"};
const char SPLIT_Note2[] PROGMEM = {"D-1 "};
const char SPLIT_Note3[] PROGMEM = {"D#-1"};
const char SPLIT_Note4[] PROGMEM = {"E-1 "};
const char SPLIT_Note5[] PROGMEM = {"F-1 "};
const char SPLIT_Note6[] PROGMEM = {"F#-1"};
const char SPLIT_Note7[] PROGMEM = {"G-1 "};
const char SPLIT_Note8[] PROGMEM = {"G#-1"};
const char SPLIT_Note9[] PROGMEM = {"A-1 "};
const char SPLIT_Note10[] PROGMEM = {"A#-1"};
const char SPLIT_Note11[] PROGMEM = {"B-1 "};
const char SPLIT_Note12[] PROGMEM = {"C0 "};
const char SPLIT_Note13[] PROGMEM = {"C#0"};
const char SPLIT_Note14[] PROGMEM = {"D0 "};
const char SPLIT_Note15[] PROGMEM = {"D#0"};
const char SPLIT_Note16[] PROGMEM = {"E0 "};
const char SPLIT_Note17[] PROGMEM = {"F0 "};
const char SPLIT_Note18[] PROGMEM = {"F#0"};
const char SPLIT_Note19[] PROGMEM = {"G0 "};
const char SPLIT_Note20[] PROGMEM = {"G#0"};
const char SPLIT_Note21[] PROGMEM = {"A0 "};
const char SPLIT_Note22[] PROGMEM = {"A#0"};
const char SPLIT_Note23[] PROGMEM = {"B0 "};
const char SPLIT_Note24[] PROGMEM = {"C1 "};
const char SPLIT_Note25[] PROGMEM = {"C#1"};
const char SPLIT_Note26[] PROGMEM = {"D1 "};
const char SPLIT_Note27[] PROGMEM = {"D#1"};
const char SPLIT_Note28[] PROGMEM = {"E1 "};
const char SPLIT_Note29[] PROGMEM = {"F1 "};
const char SPLIT_Note30[] PROGMEM = {"F#1"};
const char SPLIT_Note31[] PROGMEM = {"G1 "};
const char SPLIT_Note32[] PROGMEM = {"G#1"};
const char SPLIT_Note33[] PROGMEM = {"A1 "};
const char SPLIT_Note34[] PROGMEM = {"A#1"};
const char SPLIT_Note35[] PROGMEM = {"B1 "};
const char SPLIT_Note36[] PROGMEM = {"C2 "};
const char SPLIT_Note37[] PROGMEM = {"C#2"};
const char SPLIT_Note38[] PROGMEM = {"D2 "};
const char SPLIT_Note39[] PROGMEM = {"D#2"};
const char SPLIT_Note40[] PROGMEM = {"E2 "};
const char SPLIT_Note41[] PROGMEM = {"F2 "};
const char SPLIT_Note42[] PROGMEM = {"F#2"};
const char SPLIT_Note43[] PROGMEM = {"G2 "};
const char SPLIT_Note44[] PROGMEM = {"G#2"};
const char SPLIT_Note45[] PROGMEM = {"A2 "};
const char SPLIT_Note46[] PROGMEM = {"A#2"};
const char SPLIT_Note47[] PROGMEM = {"B2 "};
const char SPLIT_Note48[] PROGMEM = {"C3 "};
const char SPLIT_Note49[] PROGMEM = {"C#3"};
const char SPLIT_Note50[] PROGMEM = {"D3 "};
const char SPLIT_Note51[] PROGMEM = {"D#3"};
const char SPLIT_Note52[] PROGMEM = {"E3 "};
const char SPLIT_Note53[] PROGMEM = {"F3 "};
const char SPLIT_Note54[] PROGMEM = {"F#3"};
const char SPLIT_Note55[] PROGMEM = {"G3 "};
const char SPLIT_Note56[] PROGMEM = {"G#3"};
const char SPLIT_Note57[] PROGMEM = {"A3 "};
const char SPLIT_Note58[] PROGMEM = {"A#3"};
const char SPLIT_Note59[] PROGMEM = {"B3 "};
const char SPLIT_Note60[] PROGMEM = {"C4 "};
const char SPLIT_Note61[] PROGMEM = {"C#4"};
const char SPLIT_Note62[] PROGMEM = {"D4 "};
const char SPLIT_Note63[] PROGMEM = {"D#4"};
const char SPLIT_Note64[] PROGMEM = {"E4 "};
const char SPLIT_Note65[] PROGMEM = {"F4 "};
const char SPLIT_Note66[] PROGMEM = {"F#4"};
const char SPLIT_Note67[] PROGMEM = {"G4 "};
const char SPLIT_Note68[] PROGMEM = {"G#4"};
const char SPLIT_Note69[] PROGMEM = {"A4 "};
const char SPLIT_Note70[] PROGMEM = {"A#4"};
const char SPLIT_Note71[] PROGMEM = {"B4 "};
const char SPLIT_Note72[] PROGMEM = {"C5 "};
const char SPLIT_Note73[] PROGMEM = {"C#5"};
const char SPLIT_Note74[] PROGMEM = {"D5 "};
const char SPLIT_Note75[] PROGMEM = {"D#5"};
const char SPLIT_Note76[] PROGMEM = {"E5 "};
const char SPLIT_Note77[] PROGMEM = {"F5 "};
const char SPLIT_Note78[] PROGMEM = {"F#5"};
const char SPLIT_Note79[] PROGMEM = {"G5 "};
const char SPLIT_Note80[] PROGMEM = {"G#5"};
const char SPLIT_Note81[] PROGMEM = {"A5 "};
const char SPLIT_Note82[] PROGMEM = {"A#5"};
const char SPLIT_Note83[] PROGMEM = {"B5 "};
const char SPLIT_Note84[] PROGMEM = {"C6 "};
const char SPLIT_Note85[] PROGMEM = {"C#6"};
const char SPLIT_Note86[] PROGMEM = {"D6 "};
const char SPLIT_Note87[] PROGMEM = {"D#6"};
const char SPLIT_Note88[] PROGMEM = {"E6 "};
const char SPLIT_Note89[] PROGMEM = {"F6 "};
const char SPLIT_Note90[] PROGMEM = {"F#6"};
const char SPLIT_Note91[] PROGMEM = {"G6 "};
const char SPLIT_Note92[] PROGMEM = {"G#6"};
const char SPLIT_Note93[] PROGMEM = {"A6 "};
const char SPLIT_Note94[] PROGMEM = {"A#6"};
const char SPLIT_Note95[] PROGMEM = {"B6 "};
const char SPLIT_Note96[] PROGMEM = {"C7 "};
const char SPLIT_Note97[] PROGMEM = {"C#7"};
const char SPLIT_Note98[] PROGMEM = {"D7 "};
const char SPLIT_Note99[] PROGMEM = {"D#7"};
const char SPLIT_Note100[] PROGMEM = {"E7 "};
const char SPLIT_Note101[] PROGMEM = {"F7 "};
const char SPLIT_Note102[] PROGMEM = {"F#7"};
const char SPLIT_Note103[] PROGMEM = {"G7 "};
const char SPLIT_Note104[] PROGMEM = {"G#7"};
const char SPLIT_Note105[] PROGMEM = {"A7 "};
const char SPLIT_Note106[] PROGMEM = {"A#7"};
const char SPLIT_Note107[] PROGMEM = {"B7 "};
const char SPLIT_Note108[] PROGMEM = {"C8 "};
const char SPLIT_Note109[] PROGMEM = {"C#8"};
const char SPLIT_Note110[] PROGMEM = {"D8 "};
const char SPLIT_Note111[] PROGMEM = {"D#8"};
const char SPLIT_Note112[] PROGMEM = {"E8 "};
const char SPLIT_Note113[] PROGMEM = {"F8 "};
const char SPLIT_Note114[] PROGMEM = {"F#8"};
const char SPLIT_Note115[] PROGMEM = {"G8 "};
const char SPLIT_Note116[] PROGMEM = {"G#8"};
const char SPLIT_Note117[] PROGMEM = {"A8 "};
const char SPLIT_Note118[] PROGMEM = {"A#8"};
const char SPLIT_Note119[] PROGMEM = {"B8 "};
const char SPLIT_Note120[] PROGMEM = {"C9 "};
const char SPLIT_Note121[] PROGMEM = {"C#9"};
const char SPLIT_Note122[] PROGMEM = {"D9 "};
const char SPLIT_Note123[] PROGMEM = {"D#9"};
const char SPLIT_Note124[] PROGMEM = {"E9 "};
const char SPLIT_Note125[] PROGMEM = {"F9 "};
const char SPLIT_Note126[] PROGMEM = {"F#9"};
const char SPLIT_Note127[] PROGMEM = {"G9 "};

const char * const SPLIT_Note[] PROGMEM  = // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!
{
  SPLIT_Note0,
  SPLIT_Note1,
  SPLIT_Note2,
  SPLIT_Note3,
  SPLIT_Note4,
  SPLIT_Note5,
  SPLIT_Note6,
  SPLIT_Note7,
  SPLIT_Note8,
  SPLIT_Note9,
  SPLIT_Note10,
  SPLIT_Note11,
  SPLIT_Note12,
  SPLIT_Note13,
  SPLIT_Note14,
  SPLIT_Note15,
  SPLIT_Note16,
  SPLIT_Note17,
  SPLIT_Note18,
  SPLIT_Note19,
  SPLIT_Note20,
  SPLIT_Note21,
  SPLIT_Note22,
  SPLIT_Note23,
  SPLIT_Note24,
  SPLIT_Note25,
  SPLIT_Note26,
  SPLIT_Note27,
  SPLIT_Note28,
  SPLIT_Note29,
  SPLIT_Note30,
  SPLIT_Note31,
  SPLIT_Note32,
  SPLIT_Note33,
  SPLIT_Note34,
  SPLIT_Note35,
  SPLIT_Note36,
  SPLIT_Note37,
  SPLIT_Note38,
  SPLIT_Note39,
  SPLIT_Note40,
  SPLIT_Note41,
  SPLIT_Note42,
  SPLIT_Note43,
  SPLIT_Note44,
  SPLIT_Note45,
  SPLIT_Note46,
  SPLIT_Note47,
  SPLIT_Note48,
  SPLIT_Note49,
  SPLIT_Note50,
  SPLIT_Note51,
  SPLIT_Note52,
  SPLIT_Note53,
  SPLIT_Note54,
  SPLIT_Note55,
  SPLIT_Note56,
  SPLIT_Note57,
  SPLIT_Note58,
  SPLIT_Note59,
  SPLIT_Note60,
  SPLIT_Note61,
  SPLIT_Note62,
  SPLIT_Note63,
  SPLIT_Note64,
  SPLIT_Note65,
  SPLIT_Note66,
  SPLIT_Note67,
  SPLIT_Note68,
  SPLIT_Note69,
  SPLIT_Note70,
  SPLIT_Note71,
  SPLIT_Note72,
  SPLIT_Note73,
  SPLIT_Note74,
  SPLIT_Note75,
  SPLIT_Note76,
  SPLIT_Note77,
  SPLIT_Note78,
  SPLIT_Note79,
  SPLIT_Note80,
  SPLIT_Note81,
  SPLIT_Note82,
  SPLIT_Note83,
  SPLIT_Note84,
  SPLIT_Note85,
  SPLIT_Note86,
  SPLIT_Note87,
  SPLIT_Note88,
  SPLIT_Note89,
  SPLIT_Note90,
  SPLIT_Note91,
  SPLIT_Note92,
  SPLIT_Note93,
  SPLIT_Note94,
  SPLIT_Note95,
  SPLIT_Note96,
  SPLIT_Note97,
  SPLIT_Note98,
  SPLIT_Note99,
  SPLIT_Note100,
  SPLIT_Note101,
  SPLIT_Note102,
  SPLIT_Note103,
  SPLIT_Note104,
  SPLIT_Note105,
  SPLIT_Note106,
  SPLIT_Note107,
  SPLIT_Note108,
  SPLIT_Note109,
  SPLIT_Note110,
  SPLIT_Note111,
  SPLIT_Note112,
  SPLIT_Note113,
  SPLIT_Note114,
  SPLIT_Note115,
  SPLIT_Note116,
  SPLIT_Note117,
  SPLIT_Note118,
  SPLIT_Note119,
  SPLIT_Note120,
  SPLIT_Note121,
  SPLIT_Note122,
  SPLIT_Note123,
  SPLIT_Note124,
  SPLIT_Note125,
  SPLIT_Note126,
  SPLIT_Note127
};
// what to use : lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(SPLIT_Note127[ num]))));




unsigned char arp_div_index;
unsigned char ui_patchbay_limits_index;

unsigned char enc_value; // general purpose value for test
unsigned char arp_on; // launch clockbox (PLAY/STOP)

bool ui_SetAB;
unsigned char ui_aSplit;
unsigned char ui_TrspA;
unsigned char ui_TrspB;



/////////////////////////////////////////////////////////////////////////////
//  Display Arp pages,
// page 1
// page 2
// page 3
// page 4
/////////////////////////////////////////////////////////////////////////////
void UI_Display_Arp()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:
      /*
        ARP PAGE
        []  []   [ ]  []  []
        01234567890123456789
        TYP DIV MLTP GAT 8va
         up 1/32 x2   0   3
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      //DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);    // off
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);   // off
      //DOUT_PinSet1(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);  // ons
      //DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);    // on
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin);    // ON

      LCD_Clear();
      // 1st line
      lcd.setCursor(0, 0);
      lcd.print(F("TYP DIV MLTP GAT 8va"));

      // 2nd line
      // motif (arp pattern)
      lcd.setCursor(0, 1);
      switch (ui_aMotif)
      {
        case 0:
          lcd.print(F("Ordr"));
          break;

        case 1:
          lcd.print(F(" Up "));
          break;

        case 2:
          lcd.print(F("Down"));
          break;

        case 3:
          lcd.print(F("Bnce"));
          break;

        case 4:
          lcd.print(F("UpDn"));
          break;

        case 5:
          lcd.print(F("Rand"));
          break;

        case 6:
          lcd.print(F("1>>3"));
          break;

        case 7:
          lcd.print(F("BncD"));
          break;

        case 8:
          lcd.print(F("DnUp"));
          break;

        case 9:
          lcd.print(F("1<<3"));
          break;

        default:
          LCD_PrintBCD2(ui_aMotif);
          break;
      }

      // ui_aSpeed :
      //      lcd.setCursor(4, 1);
      //      LCD_PrintBCD3(arp_div_index);
      lcd.setCursor(4, 1);
      switch (arp_div_index) // 24PPQN (24 pulsations à la noire)
      {
        // case a : lcd.print("b/c") // (a+1) x c = 24*b
        case 0:
          //lcd.print(F("1/24")); // 0+1 x24 = 24*1
          lcd.print(F("trg"));
          break;

        case 1:
          lcd.print(F("1/12")); // (1+1) x12 = 24*1
          break;

        case 2:
          lcd.print(F("1/8 ")); // (2+1) x8 = 24*1
          break;

        case 3:
          lcd.print(F("1/6 ")); // (3+1) x6 = 24*1
          break;

        case 5:
          lcd.print(F("1/4 ")); // (5+1) x4 = 24*1
          break;

        case 7:
          lcd.print(F("1/3 ")); // (7+1) x3 = 24*1
          break;

        case 11:
          lcd.print(F("1/2 ")); // 11+1 x2 = 24*1
          break;

        case 23:
          lcd.print(F("1/1 ")); // 23+1 x1 = 24*1 // noire (quarter note)
          break;

        case 35:
          lcd.print(F("3/2 ")); // 35+1 dotted quarter note
          break;

        case 47:
          lcd.print(F("2/1 ")); // 47+1 = 24*2
          break;

        case 71:
          lcd.print(F("3/1 ")); // 71+1 = 24*3
          break;

        case 95:
          lcd.print(F("1Mes")); // 95 + 1 = 24*4
          break;

        case 191:
          lcd.print(F("2Mes")); // 191+1 = 24*8
          break;

        default:
          LCD_PrintBCD3(arp_div_index);
          break;
      }

      // multiplicate
      lcd.setCursor(9, 1);
      lcd.print(F("x"));
      LCD_PrintBCD1(ui_aMltp);
      //      if (ui_aMltp > 1)
      //        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);   // on
      //      else
      //        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);   // off

      // groove
      lcd.setCursor(13, 1);
      LCD_PrintBCD2(ui_aGate);

      // arp oct (0-3):
      lcd.setCursor(18, 1);
      LCD_PrintBCD1(ui_aOctave);

      break;

    case SOFT_PAGE2:
      /*
        ARP PAGE 3
        []  []   [ ]  []  []
        01234567890123456789
        SPD REC Opt PLAY StP

      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 0);

      lcd.clear();
      lcd.print(F("SPD REC PLAY R/T StP"));
      lcd.setCursor(0, 1);

      switch (seqSpeed)
      {
                
        case 0:
          lcd.print(F("trg"));
          break;
        
        case 1:
          lcd.print(F("1/6"));
          break;

        case 3:
          lcd.print(F("1/4"));
          break;

        case 4:
          lcd.print(F("1/3"));
          break;

        case 6:
          lcd.print(F("1/2 "));
          break;

        case 8:
          lcd.print(F("3/2 "));
          break;

        case 12: // gate+gate = 12+12 = 24 ppqn
          lcd.print(F("1/1 "));
          break;

        case 24:
          lcd.print(F("2/1 "));
          break;

        case 48:
          lcd.print(F("4/1"));
          break;

        case 96:
          lcd.print(F("8/1"));
          break;

        case 192:
          lcd.print(F("16/1"));
          break;

        default:
          LCD_PrintBCD3(seqSpeed);
          break;
      }
      //      LCD_PrintBCD3(seqSpeed); //seqSpeed
      DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);    // led off

      lcd.setCursor(4, 1);
      if (ui_seqRec) lcd.print(F(" o "));
      else lcd.print(F(" . "));

      lcd.setCursor(8, 1);
      if (ui_seqPlay) lcd.print(F(" > "));
      else lcd.print(F(" . "));

      if (ui_seqRec)
      {
        if (seqRecStep > 31)
        {
          lcd.setCursor(13, 1);
          lcd.print(F("   "));
          lcd.setCursor(17, 1);
          lcd.print(F("End"));
        }
        else
        {
          lcd.setCursor(13, 1); // print seq note name e.g C#3 under R/T
          //lcd.print(sequence[seqRecStep - 1][0]);
          if (sequence[seqRecStep - 1][1] == 0) // rest
            lcd.print(F("Rest"));
          else if (sequence[seqRecStep - 1][1] == 128) // tie
            lcd.print(F("Tie "));
          else // note
            lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(SPLIT_Note[sequence[seqRecStep - 1][0]]))));

          lcd.setCursor(18, 1);
          LCD_PrintBCD2(seqRecStep); // print step while recording
        }
      }

      if ( ui_seqPlay)
      {
        lcd.setCursor(13, 1);
        if (sequence[seqPlayStep][1] == 0) // rest
          lcd.print(F("    "));
        else if (sequence[seqPlayStep][1] == 128) // tie
        {
          // do nothing
        }
        else // note
          lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(SPLIT_Note[sequence[seqPlayStep][0]]))));

        lcd.setCursor(18, 1);
        LCD_PrintBCD2(seqPlayStep); // print step while playing
      }
      break;

    case SOFT_PAGE3:
      /*
        ARP PAGE
        []  []   [ ]  []  []
        01234567890123456789
        BPM ACT THRU GRV HLD
        120  *   Yes  0   *
      */

      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      //DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);    // off
      //DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);   // off
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);  // off
      //DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin);    // off
      //DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin);    // on

      LCD_Clear();
      //1st line
      LCD_CursorSet(0 + LCD_Offset);
      //     lcd.print(F("arp page 1"));
      lcd.print(F("BPM ACT THRU GRV HLD"));

      // bpm
      lcd.setCursor(0, 1);
      //      if (ui_external_clk == MIDCLK) {
      //        lcd.print(F("EXT")); // print EXT for BPM if external clock
      //        DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);    // Led on
      //      }
      //      else
      //      {
      //        LCD_PrintBCD3(bpm);
      //        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);    // led off
      //      }

      switch (ui_external_clk)
      {
        case INTCLK:
          LCD_PrintBCD3(bpm);
          DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);    // led off
          break;

        case MIDCLK:
          lcd.print(F("Mid"));
          break;

        case S24CLK:
          lcd.print(F("s24"));
          break;

        case TRGCLK:
          lcd.print(F("Trg"));
          break;

        default:
          lcd.print(F("---"));
          break;
      }


      lcd.setCursor(4, 1); // activate the arp router
      if (router_arp_tag)
      {
        lcd.print(F(" * "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);    // on
      }
      else
      {
        lcd.print(F(" . "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);    // off
      }

      lcd.setCursor(9, 1); // THRU
      if (arp_send_notes)
      {
        lcd.print(F("Yes"));
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }
      else
      {
        lcd.print(F("No "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }

      lcd.setCursor(14, 1); // Groove
      lcd.print(ui_aGroove, DEC);
      if (ui_aGroove)
        DOUT_PinSet1(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);
      else
        DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);

      lcd.setCursor(17, 1); //  HOLD indicator
      if (ui_aHold)
      {
        lcd.print(F(" * "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);   // on
      }
      else
      {
        lcd.print(F(" . "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin);   // off
      }

      break;

    case SOFT_PAGE4: // SAVE and LOAD Arp Pattern + send notes to core out
      /*
        ARP PAGE
        []  []   [ ]  []  []
        01234567890123456789
        pattern # 1    notes
        load save       keep
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 0);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("SET SPLT TrA TrS SEQ"));
      lcd.setCursor(0, 1);
      if (ui_SetAB)
      {
        lcd.print(F("S/A"));
        DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
      }
      else
      {
        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
        lcd.print(F("A/S"));
      }


      lcd.setCursor(5, 1);
      //LCD_PrintBCD2(ui_aSplit);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(SPLIT_Note[ ui_aSplit]))));


      lcd.setCursor(9, 1);
      // LCD_PrintBCD2(ui_TrspA);
      LCD_Display7BitSigned(ui_TrspA - 3);

      lcd.setCursor(13, 1);
      // LCD_PrintBCD2(ui_TrspB);
      LCD_Display7BitSigned(ui_TrspB - 3);

      lcd.setCursor(17, 1);
      LCD_PrintBCD2(seqNbr);
      break;

  }
}

/////////////////////////////////////////////////////////////////////////////
//  Configure Arp pages
/////////////////////////////////////////////////////////////////////////////
void UI_Handle_Arp()
{
  // functions available to each page :
  switch (SoftPanel.Button)
  {
    case SOFT_EDIT_F1:
      router_arp_tag = !router_arp_tag;
      if (router_arp_tag == 0) active_arp = false;
      break;

    case SOFT_EDIT_F2:
      ui_seqPlay = !ui_seqPlay;
      seqTick = 0;
      break;

    case DIN_ARP:
      SendSingleArpData(INTERFACE_SERIAL3, uPatch);
      SendSingleArpData(INTERFACE_SERIAL, uPatch);
      break;
  }

  if (SoftPanel.Page == SOFT_PAGE3) ////////////////// arp page : arp setup //////////////
  {
    // encoder : set BPM :: WORK :)
    if (ui_external_clk==INTCLK) bpm += SoftPanel.EncoderValue;
    // range bpm :
    if (bpm < BPM_min)
      bpm = BPM_min;
    if (bpm > BPM_Max)
      bpm = BPM_Max;

    switch (SoftPanel.Button)
    {
      case DIN_PAGE: // next page
        SoftPanel.Page = SOFT_PAGE4;
        break;

      case SOFT_EDIT_1:  // int/mid/s24/trig midiclock state
        ++ui_external_clk;
        if (ui_external_clk > TRGCLK)
          ui_external_clk = INTCLK;
        break;

      case SOFT_EDIT_2: // AB cfg
        router_arp_tag = !router_arp_tag;
        if (router_arp_tag == 0) active_arp = false;
        break;

      case SOFT_EDIT_3:  // THRU
        arp_send_notes = !arp_send_notes;
        break;

      case SOFT_EDIT_4: // swing time
        // SEE BELOW
        break;

      case SOFT_EDIT_5:
        if (ui_aHold) Release_aChordLatch(ui_aHold);
        ui_aHold = !ui_aHold;
        //ui_aHold = !ui_aHold;
        break;

      case SOFT_EDIT_INC:
        break;

      case SOFT_EDIT_DEC:
        break;

      case SOFT_EDIT_F1:
        //router_arp_tag = !router_arp_tag;
        break;

      case SOFT_EDIT_F2:
        break;
    }

    switch (SoftPanel.EditButton)
    {
      case Edit4:
        // DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 0);
        SetEncoderValueARP(&ui_aGroove, 127);
        // rules:
        if (ui_aGroove < 0)
          ui_aGroove = 0;
        if (ui_aGroove > (ui_aGate - 1))
          ui_aGroove = ui_aGate - 1;
        break;

      default:
        break;
    }
  }
  else if (SoftPanel.Page == SOFT_PAGE1) /////////////// arp page : ARP ///////////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE2; // go to seq page2
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }

    switch (SoftPanel.EditButton)
    {
      case Edit1:
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 1);
        SetEncoderValueARP(&ui_aMotif, 5);
        break;

      case Edit2:
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        SetEncoderValueARP(&arp_div_index, 127);
        break;

      case Edit3:
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        SetEncoderValueARP(&ui_aMltp, 4);
        if (ui_aMltp < 1)
          ui_aMltp = 1;
        break;

      case Edit4:
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        SetEncoderValueARP(&ui_aGate, 127);
        break;

      case Edit5:
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 1);
        SetEncoderValueARP(&ui_aOctave, 3);
        break;

      default:
        break;

        ui_aSpeedGateGrooveLimits();
        if (ui_aOctave > 1)
          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);    // on
        else
          DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);    // off
        if (ui_aOctave > 5)
          ui_aOctave = 1;
    }
    // rules :
    if (arp_div_index == 255) arp_div_index = 0;
    if (ui_aGate < 0) ui_aGate = 0;
    if (ui_aGate > arp_div_index) ui_aGate = arp_div_index;
    if (ui_aGroove < 0)
      ui_aGroove = 0;
    if (ui_aGroove > (ui_aGate - 1))
      ui_aGroove = ui_aGate - 1;

  }
  else if (SoftPanel.Page == SOFT_PAGE4) ////////////////////// arp page4 : transpose, load ////////////////////
  { // pattern saved in ext eeprom

    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1; // go to arp page
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      case SOFT_EDIT_1:
        ui_SetAB = !ui_SetAB;
        break;

      default:
        break;
    }

    switch (SoftPanel.EditButton)
    {

      case Edit2:
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        SetEncoderValueARP(&ui_aSplit, 127);
        break;

      case Edit3:
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        SetEncoderValueARP(&ui_TrspA, 6);

        break;

      case Edit4:
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        SetEncoderValueARP(&ui_TrspB, 6);
        break;

      case Edit5:
        SetEncoderValueARP(&seqNbr, 10);
        LoadSeq(seqNbr);
        UI_Display_Arp();
        break;

      default:
        break;
    }
  }
  else if (SoftPanel.Page == SOFT_PAGE2) //////////////////////////////////// arp page : SEQ ////////////////////////
  {
    switch (SoftPanel.EditButton)
    {

      case Edit1:
        SetEncoderValueARP(&seqSpeed, 96);
        UI_Display_Arp();
        break;

      default:
        break;
    }

    // encoder :
    //    seqSpeed += SoftPanel.EncoderValue;
    switch (SoftPanel.Button) {

      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE3;
        break;

      case SOFT_EDIT_1: //
        // rules (range)
        if (seqSpeed == 0)
          seqSpeed = 1;
        if (seqSpeed > 96)
          seqSpeed = 96;
        break;

      case SOFT_EDIT_2: //
        Init_Seq();
        ui_seqRec = !ui_seqRec;

#if DEBUG_SEQ
        Serial.print(F("ui_external_clk = ")); Serial.println(ui_external_clk, DEC);
        Serial.print(F("ui_seqRec = ")); Serial.println(ui_seqRec, DEC);
        Serial.print(F("ui_seqPlay = ")); Serial.println(ui_seqPlay, DEC);
#endif
        break;

      case SOFT_EDIT_4: //
        //seqOption = Shift<<7; // silence if shift = 0 , Tie if Shift = 128
        // then, insert silent
        //Insert_SeqRest(seqOption);
        Insert_SeqRest(Shift << 7);
        break;

      case SOFT_EDIT_3: // stop record sequence & play/stop sequence
        ui_seqRec = false;
        ui_seqPlay = !ui_seqPlay;
        seqTick = 0;

#if DEBUG_SEQ
        Serial.print(F("ui_external_clk = ")); Serial.println(ui_external_clk, DEC);
        Serial.print(F("ui_seqRec = ")); Serial.println(ui_seqRec, DEC);
        Serial.print(F("ui_seqPlay = ")); Serial.println(ui_seqPlay, DEC);
#endif
        break;

      case SOFT_EDIT_5: //
        LoadSeq(seqNbr);
        break;

      case SOFT_EDIT_INC: // increment
        if (ui_seqRec)
        {
          ++seqRecStep;
          if (seqRecStep > 32)
            seqRecStep = 0;
          UI_Display_Arp();
        }
        if (ui_seqPlay)
        {
          skipSeqStep = +1;
          if (seqPlayStep > seqLength)
            seqPlayStep = 0;
          UI_Display_Arp();
        }
        break;

      case SOFT_EDIT_DEC: // decrement
        if (ui_seqRec)
        {
          --seqRecStep;
          if (seqRecStep == 255)
            seqRecStep = 0;
          UI_Display_Arp();
        }
        if (ui_seqPlay)
        {
          skipSeqStep = -1;
          if (seqPlayStep == 255)
            seqPlayStep = seqLength;
          UI_Display_Arp();
        }
        break;

#if DEBUG_SEQ
        Serial.print(F("ui_external_clk = ")); Serial.println(ui_external_clk, DEC);
        Serial.print(F("ui_seqRec = ")); Serial.println(ui_seqRec, DEC);
        Serial.print(F("ui_seqPlay = ")); Serial.println(ui_seqPlay, DEC);
#endif

    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else
  {
    // nope
  }
  // rules between speed gate and groove
  ui_aSpeedGateGrooveLimits();
}

///////////////////////////////////////////////////////////////////////////////////
// rules between speed gate and groove
///////////////////////////////////////////////////////////////////////////////////
void ui_aSpeedGateGrooveLimits()
{
  ui_aSpeed = arp_div[arp_div_index];

  if (arp_div_index > 254) arp_div_index = 254;
  if (arp_div_index == 255) arp_div_index = 0;

  //  if (ui_aGate > ui_aSpeed) ui_aGate = ui_aSpeed; //= ui_aSpeed - 1
  //  if (ui_aGate == 0) ui_aGate = 1;
  //  if (ui_aGate > 144) ui_aGate = 144;
  //
  //  if (ui_aGroove >= (ui_aGate - 1)) ui_aGroove = ui_aGate - 1;
  //  if (ui_aGroove == 255) ui_aGroove = 0;

#if DEBUG_ARP
  Serial.println(F("ui_aSpeedGateGrooveLimits() happened"));
#endif
}
/////////////////////////////////////////////////////////////////////////////
//  Sets an unsigned value with saturation at zero and max.
//  Returns 0 if no change was made
/////////////////////////////////////////////////////////////////////////////
char SetEncoderValueARP(unsigned char *value, unsigned char max)
{
  if ((*value > 0 && *value < max) || (*value == 0 && SoftPanel.EncoderValue == 1) || (*value == max && SoftPanel.EncoderValue < 0))
  {
    *value += SoftPanel.EncoderValue;

    ui_aSpeed = arp_div[arp_div_index];

    if (arp_div_index < 1) arp_div_index = 1;
    if (arp_div_index > 254) arp_div_index = 254;

    //    if (ui_aGate > ui_aSpeed) ui_aGate = ui_aSpeed - 1;
    //    if (ui_aGate == 0) ui_aGate = 1;
    //    if (ui_aGate > 144) ui_aGate = 144;
    //
    //    if (ui_aGroove >= (ui_aGate - 1)) ui_aGroove = ui_aGate - 1;
    //    if (ui_aGroove == 255) ui_aGroove = 0;
    return 1;
  }
  else
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//  Sets an unsigned value with saturation at zero and max.
//  Returns 0 if no change was made
/////////////////////////////////////////////////////////////////////////////
int SetEncoderValue2ARP(unsigned int *value, unsigned int max)
{
  if ((*value > 0 && *value < max) || (*value == 0 && SoftPanel.EncoderValue == 1) || (*value == max && SoftPanel.EncoderValue < 0))
  {
    *value += SoftPanel.EncoderValue;
    ui_aSpeed = arp_div[arp_div_index];

    if (arp_div_index > 13) arp_div_index = 0;
    if (arp_div_index == 255) arp_div_index = 13;

    //    if (ui_aGate > ui_aSpeed) ui_aGate = ui_aSpeed - 1;
    //    if (ui_aGate == 0) ui_aGate = 1;
    //    if (ui_aGate > 144) ui_aGate = 144;
    //
    //    if (ui_aGroove >= (ui_aGate - 1)) ui_aGroove = ui_aGate - 1;
    //    if (ui_aGroove == 255) ui_aGroove = 0;
    return 1;
  }
  else
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
char SetEncoderValue3ARP(char *value)
{
  if (SoftPanel.EncoderValue < 0)
  {
    *value += SoftPanel.EncoderValue;
    return -1;
  }

  if (SoftPanel.EncoderValue > 0)
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else return 0;
}

