
#include "chaosmatrix.h"
#include "softpanel.h"
#include "ui_arp.h"
#include "din.h"
#include "lcd.h"
#include "arp.h"
#include "memo.h"
#include "seq.h"

#define BPM_Max 230 // less than 255
#define BPM_min 20 // more than 0
#define ui_aGate_Max 1024
#define aMltp_MAX 4

// array to correspond ui_aSpeed and musical beat denominations
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

////////// explain note in a musical way instead of unsigned char :
const char pitchNotation0[] PROGMEM = {"C-1 "};
const char pitchNotation1[] PROGMEM = {"C#-1"};
const char pitchNotation2[] PROGMEM = {"D-1 "};
const char pitchNotation3[] PROGMEM = {"D#-1"};
const char pitchNotation4[] PROGMEM = {"E-1 "};
const char pitchNotation5[] PROGMEM = {"F-1 "};
const char pitchNotation6[] PROGMEM = {"F#-1"};
const char pitchNotation7[] PROGMEM = {"G-1 "};
const char pitchNotation8[] PROGMEM = {"G#-1"};
const char pitchNotation9[] PROGMEM = {"A-1 "};
const char pitchNotation10[] PROGMEM = {"A#-1"};
const char pitchNotation11[] PROGMEM = {"B-1 "};
const char pitchNotation12[] PROGMEM = {"C0  "};
const char pitchNotation13[] PROGMEM = {"C#0 "};
const char pitchNotation14[] PROGMEM = {"D0  "};
const char pitchNotation15[] PROGMEM = {"D#0 "};
const char pitchNotation16[] PROGMEM = {"E0  "};
const char pitchNotation17[] PROGMEM = {"F0  "};
const char pitchNotation18[] PROGMEM = {"F#0 "};
const char pitchNotation19[] PROGMEM = {"G0  "};
const char pitchNotation20[] PROGMEM = {"G#0 "};
const char pitchNotation21[] PROGMEM = {"A0  "};
const char pitchNotation22[] PROGMEM = {"A#0 "};
const char pitchNotation23[] PROGMEM = {"B0  "};
const char pitchNotation24[] PROGMEM = {"C1  "};
const char pitchNotation25[] PROGMEM = {"C#1 "};
const char pitchNotation26[] PROGMEM = {"D1  "};
const char pitchNotation27[] PROGMEM = {"D#1 "};
const char pitchNotation28[] PROGMEM = {"E1  "};
const char pitchNotation29[] PROGMEM = {"F1  "};
const char pitchNotation30[] PROGMEM = {"F#1 "};
const char pitchNotation31[] PROGMEM = {"G1  "};
const char pitchNotation32[] PROGMEM = {"G#1 "};
const char pitchNotation33[] PROGMEM = {"A1  "};
const char pitchNotation34[] PROGMEM = {"A#1 "};
const char pitchNotation35[] PROGMEM = {"B1  "};
const char pitchNotation36[] PROGMEM = {"C2  "};
const char pitchNotation37[] PROGMEM = {"C#2 "};
const char pitchNotation38[] PROGMEM = {"D2  "};
const char pitchNotation39[] PROGMEM = {"D#2 "};
const char pitchNotation40[] PROGMEM = {"E2  "};
const char pitchNotation41[] PROGMEM = {"F2  "};
const char pitchNotation42[] PROGMEM = {"F#2 "};
const char pitchNotation43[] PROGMEM = {"G2  "};
const char pitchNotation44[] PROGMEM = {"G#2 "};
const char pitchNotation45[] PROGMEM = {"A2  "};
const char pitchNotation46[] PROGMEM = {"A#2 "};
const char pitchNotation47[] PROGMEM = {"B2  "};
const char pitchNotation48[] PROGMEM = {"C3  "};
const char pitchNotation49[] PROGMEM = {"C#3 "};
const char pitchNotation50[] PROGMEM = {"D3  "};
const char pitchNotation51[] PROGMEM = {"D#3 "};
const char pitchNotation52[] PROGMEM = {"E3  "};
const char pitchNotation53[] PROGMEM = {"F3  "};
const char pitchNotation54[] PROGMEM = {"F#3 "};
const char pitchNotation55[] PROGMEM = {"G3  "};
const char pitchNotation56[] PROGMEM = {"G#3 "};
const char pitchNotation57[] PROGMEM = {"A3  "};
const char pitchNotation58[] PROGMEM = {"A#3 "};
const char pitchNotation59[] PROGMEM = {"B3  "};
const char pitchNotation60[] PROGMEM = {"C4  "};
const char pitchNotation61[] PROGMEM = {"C#4 "};
const char pitchNotation62[] PROGMEM = {"D4  "};
const char pitchNotation63[] PROGMEM = {"D#4 "};
const char pitchNotation64[] PROGMEM = {"E4  "};
const char pitchNotation65[] PROGMEM = {"F4  "};
const char pitchNotation66[] PROGMEM = {"F#4 "};
const char pitchNotation67[] PROGMEM = {"G4  "};
const char pitchNotation68[] PROGMEM = {"G#4 "};
const char pitchNotation69[] PROGMEM = {"A4  "};
const char pitchNotation70[] PROGMEM = {"A#4 "};
const char pitchNotation71[] PROGMEM = {"B4  "};
const char pitchNotation72[] PROGMEM = {"C5  "};
const char pitchNotation73[] PROGMEM = {"C#5 "};
const char pitchNotation74[] PROGMEM = {"D5  "};
const char pitchNotation75[] PROGMEM = {"D#5 "};
const char pitchNotation76[] PROGMEM = {"E5  "};
const char pitchNotation77[] PROGMEM = {"F5  "};
const char pitchNotation78[] PROGMEM = {"F#5 "};
const char pitchNotation79[] PROGMEM = {"G5  "};
const char pitchNotation80[] PROGMEM = {"G#5 "};
const char pitchNotation81[] PROGMEM = {"A5  "};
const char pitchNotation82[] PROGMEM = {"A#5 "};
const char pitchNotation83[] PROGMEM = {"B5  "};
const char pitchNotation84[] PROGMEM = {"C6  "};
const char pitchNotation85[] PROGMEM = {"C#6 "};
const char pitchNotation86[] PROGMEM = {"D6  "};
const char pitchNotation87[] PROGMEM = {"D#6 "};
const char pitchNotation88[] PROGMEM = {"E6  "};
const char pitchNotation89[] PROGMEM = {"F6  "};
const char pitchNotation90[] PROGMEM = {"F#6 "};
const char pitchNotation91[] PROGMEM = {"G6  "};
const char pitchNotation92[] PROGMEM = {"G#6 "};
const char pitchNotation93[] PROGMEM = {"A6  "};
const char pitchNotation94[] PROGMEM = {"A#6 "};
const char pitchNotation95[] PROGMEM = {"B6  "};
const char pitchNotation96[] PROGMEM = {"C7  "};
const char pitchNotation97[] PROGMEM = {"C#7 "};
const char pitchNotation98[] PROGMEM = {"D7  "};
const char pitchNotation99[] PROGMEM = {"D#7 "};
const char pitchNotation100[] PROGMEM = {"E7  "};
const char pitchNotation101[] PROGMEM = {"F7  "};
const char pitchNotation102[] PROGMEM = {"F#7 "};
const char pitchNotation103[] PROGMEM = {"G7  "};
const char pitchNotation104[] PROGMEM = {"G#7 "};
const char pitchNotation105[] PROGMEM = {"A7  "};
const char pitchNotation106[] PROGMEM = {"A#7 "};
const char pitchNotation107[] PROGMEM = {"B7  "};
const char pitchNotation108[] PROGMEM = {"C8  "};
const char pitchNotation109[] PROGMEM = {"C#8 "};
const char pitchNotation110[] PROGMEM = {"D8  "};
const char pitchNotation111[] PROGMEM = {"D#8 "};
const char pitchNotation112[] PROGMEM = {"E8  "};
const char pitchNotation113[] PROGMEM = {"F8  "};
const char pitchNotation114[] PROGMEM = {"F#8 "};
const char pitchNotation115[] PROGMEM = {"G8  "};
const char pitchNotation116[] PROGMEM = {"G#8 "};
const char pitchNotation117[] PROGMEM = {"A8  "};
const char pitchNotation118[] PROGMEM = {"A#8 "};
const char pitchNotation119[] PROGMEM = {"B8  "};
const char pitchNotation120[] PROGMEM = {"C9  "};
const char pitchNotation121[] PROGMEM = {"C#9 "};
const char pitchNotation122[] PROGMEM = {"D9  "};
const char pitchNotation123[] PROGMEM = {"D#9 "};
const char pitchNotation124[] PROGMEM = {"E9  "};
const char pitchNotation125[] PROGMEM = {"F9  "};
const char pitchNotation126[] PROGMEM = {"F#9 "};
const char pitchNotation127[] PROGMEM = {"G9  "};

// https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!

const char * const pitchNotation[] PROGMEM  = {
  pitchNotation0,
  pitchNotation1,
  pitchNotation2,
  pitchNotation3,
  pitchNotation4,
  pitchNotation5,
  pitchNotation6,
  pitchNotation7,
  pitchNotation8,
  pitchNotation9,
  pitchNotation10,
  pitchNotation11,
  pitchNotation12,
  pitchNotation13,
  pitchNotation14,
  pitchNotation15,
  pitchNotation16,
  pitchNotation17,
  pitchNotation18,
  pitchNotation19,
  pitchNotation20,
  pitchNotation21,
  pitchNotation22,
  pitchNotation23,
  pitchNotation24,
  pitchNotation25,
  pitchNotation26,
  pitchNotation27,
  pitchNotation28,
  pitchNotation29,
  pitchNotation30,
  pitchNotation31,
  pitchNotation32,
  pitchNotation33,
  pitchNotation34,
  pitchNotation35,
  pitchNotation36,
  pitchNotation37,
  pitchNotation38,
  pitchNotation39,
  pitchNotation40,
  pitchNotation41,
  pitchNotation42,
  pitchNotation43,
  pitchNotation44,
  pitchNotation45,
  pitchNotation46,
  pitchNotation47,
  pitchNotation48,
  pitchNotation49,
  pitchNotation50,
  pitchNotation51,
  pitchNotation52,
  pitchNotation53,
  pitchNotation54,
  pitchNotation55,
  pitchNotation56,
  pitchNotation57,
  pitchNotation58,
  pitchNotation59,
  pitchNotation60,
  pitchNotation61,
  pitchNotation62,
  pitchNotation63,
  pitchNotation64,
  pitchNotation65,
  pitchNotation66,
  pitchNotation67,
  pitchNotation68,
  pitchNotation69,
  pitchNotation70,
  pitchNotation71,
  pitchNotation72,
  pitchNotation73,
  pitchNotation74,
  pitchNotation75,
  pitchNotation76,
  pitchNotation77,
  pitchNotation78,
  pitchNotation79,
  pitchNotation80,
  pitchNotation81,
  pitchNotation82,
  pitchNotation83,
  pitchNotation84,
  pitchNotation85,
  pitchNotation86,
  pitchNotation87,
  pitchNotation88,
  pitchNotation89,
  pitchNotation90,
  pitchNotation91,
  pitchNotation92,
  pitchNotation93,
  pitchNotation94,
  pitchNotation95,
  pitchNotation96,
  pitchNotation97,
  pitchNotation98,
  pitchNotation99,
  pitchNotation100,
  pitchNotation101,
  pitchNotation102,
  pitchNotation103,
  pitchNotation104,
  pitchNotation105,
  pitchNotation106,
  pitchNotation107,
  pitchNotation108,
  pitchNotation109,
  pitchNotation110,
  pitchNotation111,
  pitchNotation112,
  pitchNotation113,
  pitchNotation114,
  pitchNotation115,
  pitchNotation116,
  pitchNotation117,
  pitchNotation118,
  pitchNotation119,
  pitchNotation120,
  pitchNotation121,
  pitchNotation122,
  pitchNotation123,
  pitchNotation124,
  pitchNotation125,
  pitchNotation126,
  pitchNotation127
};
// what to use : lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(pitchNotation127[ num]))));

unsigned char arp_div_index;
unsigned char ui_patchbay_limits_index;

unsigned char enc_value; // general purpose value for test
unsigned char arp_on; // launch clockbox (PLAY/STOP)

bool ui_SetAB;
bool ui_aSplitLearning;
bool ui_toggleSeq;
unsigned char ui_aSplit;
unsigned char ui_TrspA;
unsigned char ui_TrspB;



/////////////////////////////////////////////////////////////////////////////
//  Display Arp pages,// page 1// page 2// page 3// page 4
/////////////////////////////////////////////////////////////////////////////
void UI_Display_Arp()
{
  if (SoftPanel.Mode != Arp) return; // avoid constantly switch to arp display when editing a patch !

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
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin);    // ON

      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        // 1st line
        lcd.setCursor(0, 0);
        lcd.print(F("TYP DIV MLTP GAT 8va"));
      }
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
      lcd.setCursor(4, 1);
      switch (arp_div_index) // 24PPQN (24 pulsations à la noire)
      {
        // case a : lcd.print("b/c") // (a+1) x c = 24*b
        case 0:
          //lcd.print(F("1/24")); // 0+1 x24 = 24*1
          lcd.print(F("trg "));
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
      if (ui_aMltp > 1)
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);   // on
      else
        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);   // off

      // groove
      lcd.setCursor(13, 1);
      LCD_PrintBCD2(ui_aGate);

      // arp oct (0-3):
      lcd.setCursor(18, 1);
      LCD_PrintBCD1(ui_aOctave);
      if (ui_aOctave)
        DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);    // on
      else
        DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin);    // off
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
      // 1st line
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear(); // 
        lcd.setCursor(0, 0);
        lcd.print(F("SPD REC PLAY R/T StP"));
      }

      // 2nd line
      lcd.setCursor(0, 1);
      switch (seqSpeed)
      {
        case 0:
          lcd.print(F("trg "));
          break;

        case 1:
          lcd.print(F("1/6 "));
          break;

        case 3:
          lcd.print(F("1/4 "));
          break;

        case 4:
          lcd.print(F("1/3 "));
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
          lcd.print(F("4/1 "));
          break;

        case 96:
          lcd.print(F("8/1 "));
          break;

        case 192:
          lcd.print(F("16/1"));
          break;

        default:
          LCD_PrintBCD3(seqSpeed);
          lcd.print(F(" "));
          break;
      }
      if (seqSpeed)
        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);    // led off
      else
        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);    // led off


      lcd.setCursor(4, 1);
      if (ui_seqRec)
      {
        lcd.print(F(" o  "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
      }
      else
      {
        lcd.print(F(" .  "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);
      }



      lcd.setCursor(8, 1);
      if (ui_seqPlay && ui_toggleSeq)
      {
        lcd.print(F("togl"));
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }
      else if (ui_seqPlay)
      {
        lcd.write(254); lcd.write((byte)0); lcd.write(254); lcd.write(254);  //lcd.print(F(" >  "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }
      else
      {
        lcd.print(F(" .  "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }

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
          // print seq pitch e.g C#3,rest,tie under R/T
          lcd.setCursor(13, 1);
          if (sequence[seqRecStep - 1][1] == 0) // rest
            lcd.print(F("Rest "));
          else if (sequence[seqRecStep - 1][1] == 128) // tie
            lcd.print(F("Tie  "));
          else // pitch
            lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(pitchNotation[sequence[seqRecStep - 1][0]]))));
          lcd.print(F(" "));

          // print step while recording
          lcd.setCursor(18, 1);
          LCD_PrintBCD2(seqRecStep);
        }
      }

      if (ui_seqPlay) {
        // print seq pitch while playing it e.g C#3,rest,tie under R/T
        lcd.setCursor(13, 1);
        if (sequence[seqPlayStep][1] == 0) // rest
          lcd.print(F("    "));
        else if (sequence[seqPlayStep][1] == 128) // tie
        {
          // do nothing
        }
        else // pitch
          lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(pitchNotation[sequence[seqPlayStep][0]]))));
        // print step while playing
        lcd.setCursor(18, 1);
        LCD_PrintBCD2(seqPlayStep);
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

      if (SoftPanel.IsNewPage)
      {
        //1st line
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        lcd.print(F("BPM ACT THRU GRV HLD"));
      }

      // bpm
      lcd.setCursor(0, 1);
      switch (systmClock)
      {
        case INTCLK:
          LCD_PrintBCD3(bpm);
          break;

        case MIDCLK:
          lcd.print(F("Mid "));
          break;

        case S24CLK:
          lcd.print(F("s24 "));
          break;

        case TRGCLK:
          lcd.print(F("Trg "));
          break;

        case MTRGCLK:
          lcd.print(F("mTg "));
          break;

//        case SYSCLK:
//          if (systmClock == systmClock && systmClock == INTCLK)
//            LCD_PrintBCD3(bpm);
//          else
//            lcd.print(F("Sys "));
//          break;

        default:
          lcd.print(F("--- "));
          break;
      }
//      if (systmClock == INTCLK)
//        DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
//      else
//        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);


      lcd.setCursor(4, 1); // activate the arp router
      if (router_arp_tag)
      {
        lcd.print(F(" *  "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);    // on
      }
      else
      {
        lcd.print(F(" .  "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);    // off
      }

      lcd.setCursor(9, 1); // THRU
      if (arp_send_notes)
      {
        lcd.print(F("Yes "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }
      else
      {
        lcd.print(F("No  "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }

      lcd.setCursor(14, 1); // Groove
      lcd.print(ui_aGroove, DEC);
      lcd.print(F(" "));

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
        SET SPLT TrA TrS SEQ
        A/S  B3  + 1 - 2  5
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 0);
      // 1st line
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        lcd.setCursor(0, 0);
        lcd.print(F("SET SPLT TrA TrS SEQ"));
      }
      // 2nd line
      lcd.setCursor(0, 1);
      if (ui_SetAB)
      {
        lcd.print(F("S/A "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
      }
      else
      {
        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
        lcd.print(F("A/S "));
      }

      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(pitchNotation[ui_aSplit]))));
      lcd.print(F(" "));

      lcd.setCursor(9, 1);
      LCD_Display7BitSigned(ui_TrspA - 3);
      lcd.print(F(" "));

      lcd.setCursor(13, 1);
      LCD_Display7BitSigned(ui_TrspB - 3);
      lcd.print(F(" "));

      lcd.setCursor(17, 1);
      LCD_PrintBCD2(seqNbr);
      lcd.print(F(" "));

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
      if (Shift)
      {
        if (ui_aHold)
          Release_aChordLatch(ui_aHold);
        ui_aHold = !ui_aHold;
      }
      else
      {
        router_arp_tag = !router_arp_tag;
        if (router_arp_tag == 0)
          active_arp = false;
      }
      break;

    case SOFT_EDIT_F2:
      if (Shift)
      {
        ui_toggleSeq = !ui_toggleSeq;
      }
      else
      {
        ui_seqPlay = !ui_seqPlay;
        seqTick = 0;
      }
      break;

    case DIN_ARP:
      if (Shift)
        SendSingleArpData(INTERFACE_SERIAL3, uPatch[device]);
      break;
  }

  if (SoftPanel.Page == SOFT_PAGE3) ////////////////// arp page 3 : arp setup //////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE: // next page
        SoftPanel.Page = SOFT_PAGE4;
        break;

      case SOFT_EDIT_2: // AB cfg
        router_arp_tag = !router_arp_tag;
        if (router_arp_tag == 0)
          active_arp = false;
        break;

      case SOFT_EDIT_3:  // THRU
        arp_send_notes = !arp_send_notes;
        break;

      case SOFT_EDIT_4: // swing time
        // SEE BELOW
        break;

      case SOFT_EDIT_5:
        if (ui_aHold)
          Release_aChordLatch(ui_aHold);
        ui_aHold = !ui_aHold;
        break;

      default:
        break;
    }

    switch (SoftPanel.EditButton)
    {
      case Edit4:
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 0);
        SetEncoderValueARP(&ui_aGroove, 127);
        // rules:
        if (ui_aGroove > (ui_aGate - 1))
          ui_aGroove = ui_aGate - 1;
        if (ui_aGroove == 255)
        {
          ui_aGroove = 0;
          ui_aGate = 1;
        }
        break;

      case Edit1:
          SetEncoderValueARP(&bpm, BPM_Max);
          if (bpm < BPM_min)
            bpm = BPM_min;
            uClock.setTempo(bpm);

      default:
        break;
    }
  }
  else if (SoftPanel.Page == SOFT_PAGE1) /////////////// arp page1 : ARP ///////////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE2; // go to seq page2
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
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 0);
        SetEncoderValueARP(&ui_aSplit, 127);

        // here we could implement a midi learn of the pitch for split (only by NoteOn msg)
        ui_aSplitLearning = true;
        break;

      case Edit3:
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 0);
        SetEncoderValueARP(&ui_TrspA, 6);
        break;

      case Edit4:
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 0);
        SetEncoderValueARP(&ui_TrspB, 6);
        break;

      case Edit5:
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 0);
        SetEncoderValueARP(&seqNbr, 10);
        DefaultSeq_Load(seqNbr);
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
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 0);
        SetEncoderValueARP(&seqSpeed, 96);
        if (seqSpeed == 0)
          seqSpeed = 1;
        UI_Display_Arp();
        break;

      default:
        break;
    }

    switch (SoftPanel.Button)
    {
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
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 0);
        Init_Seq();
        ui_seqRec = !ui_seqRec;

#if DEBUG_SEQ
        Serial.print(F("systmClock = ")); Serial.println(systmClock, DEC);
        Serial.print(F("ui_seqRec = ")); Serial.println(ui_seqRec, DEC);
        Serial.print(F("ui_seqPlay = ")); Serial.println(ui_seqPlay, DEC);
#endif
        break;

      case SOFT_EDIT_3: // stop record sequence & play/stop sequence
        ui_seqRec = false;
        ui_seqPlay = !ui_seqPlay; // + ui_toggleSeq
        seqTick = 0;

#if DEBUG_SEQ
        Serial.print(F("systmClock = ")); Serial.println(systmClock, DEC);
        Serial.print(F("ui_seqRec = ")); Serial.println(ui_seqRec, DEC);
        Serial.print(F("ui_seqPlay = ")); Serial.println(ui_seqPlay, DEC);
#endif
        break;

      case SOFT_EDIT_4:
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 0);
        // silence if shift = 0 , Tie if Shift = 128
        Insert_SeqRest(Shift << 7);
        break;

      case SOFT_EDIT_5:
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 0);
        DefaultSeq_Load(seqNbr);
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
        Serial.print(F("systmClock = ")); Serial.println(systmClock, DEC);
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

#if DEBUG_ARP
  Serial.println(F("ui_aSpeedGateGrooveLimits() happened"));
#endif
}

///////////////////////////////////////////////////////////////////////////////////
// reset arp ui_ parameters when loading a patch from eeprom external
///////////////////////////////////////////////////////////////////////////////////
void Reset_UI_ARP(void)
{
  // only work when device = A
  if (device != Matrix_Device_A)
    return; // quit

  if (ui_aHold == true)
    Release_aChordLatch(true);

  ui_aHold = false;

  Active_Arp(false);

  router_arp_tag = false;
  active_arp = false;

  ui_toggleSeq = false;
  seqTick = 0;
  ui_seqPlay = ui_seqRec = false;

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
    //    if (Shift)
    //      *value = *value + 9;

    ui_aSpeed = arp_div[arp_div_index];

    if (arp_div_index < 1) arp_div_index = 1;
    if (arp_div_index > 254) arp_div_index = 254;

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
