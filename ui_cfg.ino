//#include <cmios.h>
#include "chaosmatrix.h"
#include "softpanel.h"
#include "din.h"
#include "lcd.h"
#include "memo.h"
#include "midi.h"
#include "ui_matrix.h"
#include "device.h"
#include "define.h"
#include "ui_filter.h"
#include "oner.h"
#include "uclock.h"

////////// Vibrato waveform /////////////////////////////////////////////////////////
const char VIB_Waves_Name0[] PROGMEM = {"TRI"};
const char VIB_Waves_Name1[] PROGMEM = {"USW"};
const char VIB_Waves_Name2[] PROGMEM = {"DSW"};
const char VIB_Waves_Name3[] PROGMEM = {"SQR"};
const char VIB_Waves_Name4[] PROGMEM = {"RAN"};
const char VIB_Waves_Name5[] PROGMEM = {"NOI"};

const char * const VIB_Waves_Name[] PROGMEM  = // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!
{
  VIB_Waves_Name0,
  VIB_Waves_Name1,
  VIB_Waves_Name2,
  VIB_Waves_Name3,
  VIB_Waves_Name4,
  VIB_Waves_Name5
};

////////// Vibrato Speed modulated by /////////////////////////////////////////////////////////
const char VIB_Speed_By0[] PROGMEM = {"OFF"};
const char VIB_Speed_By1[] PROGMEM = {"Lv2"};
const char VIB_Speed_By2[] PROGMEM = {"Pd1"};

const char * const VIB_Speed_By[] PROGMEM  = // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!
{
  VIB_Speed_By0,
  VIB_Speed_By1,
  VIB_Speed_By2
};

////////// Vibrato Amplitude modulated by /////////////////////////////////////////////////////////
const char VIB_Amp_By0[] PROGMEM = {"OFF"};
const char VIB_Amp_By1[] PROGMEM = {"Lv2"};
const char VIB_Amp_By2[] PROGMEM = {"Pd1"};

const char * const VIB_Amp_By[] PROGMEM  = // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions  And, yes, it's really tricky even for experts!
{
  VIB_Amp_By0,
  VIB_Amp_By1,
  VIB_Amp_By2
};

/////////////////////////////////////////////////////////////////////////////
//  Display Cfg pages,
// page 1
// page 2
// page 3
// page 4
/////////////////////////////////////////////////////////////////////////////
void UI_Display_Cfg()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:
      /*
        Cfg PAGE 1
        []  []   [ ]  []  []
        01234567890123456789
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet_Keypanel(1, 1, 1, 1, 1, 0);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      LCD_Clear();
      lcd.setCursor(0, 0); //1st line
      lcd.print(F("CONFIGURE:          "));
      lcd.setCursor(0, 1);
      lcd.print(F("VIB MID MISC GRP DMP"));
      break;

    case SOFT_PAGE11:
      /*
        Cfg PAGE 1
        []  []   [ ]  []  []
        01234567890123456789
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      //      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        lcd.setCursor(0, 0); // 1st line
        lcd.print(F("SPD MOD AMNT WAV GBL"));
      }
      lcd.setCursor(1, 1);
      LCD_PrintBCD2(GlobalParameters[1]);
      lcd.setCursor(4, 1);
      //LCD_PrintBCD2(GlobalParameters[2]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(VIB_Speed_By[GlobalParameters[2]]))));
      lcd.setCursor(9, 1);
      LCD_PrintBCD2(GlobalParameters[3]);
      lcd.setCursor(13, 1);
      //LCD_PrintBCD2(GlobalParameters[4]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(VIB_Waves_Name[GlobalParameters[4]]))));
      lcd.setCursor(17, 1);
      lcd.print(F("Get"));
      break;

    case SOFT_PAGE12:
      /*
        Cfg PAGE 1
        []  []   [ ]  []  []
        01234567890123456789
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      //      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);
      if (SoftPanel.IsNewPage) {
        LCD_Clear();
        lcd.setCursor(0, 0); // 1st line
        lcd.print(F("AMP MOD AMNT BND GBL"));
      }
      lcd.setCursor(1, 1);
      LCD_PrintBCD2(GlobalParameters[5]);
      lcd.setCursor(4, 1);
      // LCD_PrintBCD2(GlobalParameters[6]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(VIB_Amp_By[GlobalParameters[6]]))));
      lcd.setCursor(9, 1);
      LCD_PrintBCD2(GlobalParameters[7]);
      lcd.setCursor(14, 1);
      LCD_PrintBCD2(GlobalParameters[164]); // bend range 12 demi ton
      lcd.setCursor(17, 1);
      lcd.print(F("Snd"));

      break;

    case SOFT_PAGE2:
      /*
        CFG PAGE 2 (MID)
        []  []   [ ]  []  []
        12345678901234567890
        CH    A   B   C   D
        00    0   0   1   0
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("CH  A:  B:   C:  D: "));
      }
      //            lcd.print("CH  MxA MxB  MxC MxD");
      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintBCD2(MIDI_CHANNEL );

      // deviceA = 1000 or 6
      LCD_CursorSet(64 + 4 + LCD_Offset);
      if (matrix_model_A) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }
      // device B = 1000 or 6
      LCD_CursorSet(64 + 8 + LCD_Offset);
      if (matrix_model_B) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }
      // device C = 1000 or 6
      LCD_CursorSet(64 + 13 + LCD_Offset);
      if (matrix_model_C) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }
      // device D = 1000 or 6
      LCD_CursorSet(64 + 17 + LCD_Offset);
      if (matrix_model_D) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }

      break;

    case SOFT_PAGE21:
      /*
        Cfg PAGE 21
        []  []   [ ]  []  []
        01234567890123456789
      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      //      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);
      if (SoftPanel.IsNewPage) {
        LCD_Clear();
        lcd.setCursor(0, 0); // 1st line
        lcd.print(F("Pd1 Pd2  Lv1 Lv2 Ena"));
      }
      lcd.setCursor(1, 1);
      LCD_PrintBCD2(GlobalParameters[17]); // Midi Pedal 1 Controller
      lcd.setCursor(5, 1);
      LCD_PrintBCD2(GlobalParameters[18]); // midi Pedal 2 Controller
      lcd.setCursor(10, 1);
      LCD_PrintBCD2(GlobalParameters[19]); // midi Lever 1 Controller
      lcd.setCursor(14, 1);
      LCD_PrintBCD2(GlobalParameters[20]); // midi Lever 2 Controller
      lcd.setCursor(17, 1);
      if (GlobalParameters[13])
        lcd.print(F(" * "));
      else
        lcd.print(F(" . "));
      break;

    case SOFT_PAGE22:
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      //      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);
      if (SoftPanel.IsNewPage) {
        LCD_Clear();
      }
      // 1st line
      lcd.setCursor(0, 0); // 1st line
      switch (ui_Zone)
      {
        case 0: lcd.print(F("A:Range  rCH tCH Trp")); break;
        case 1: lcd.print(F("B:Range  rCH tCH Trp")); break;
        case 2: lcd.print(F("C:Range  rCH tCH Trp")); break;
        case 3: lcd.print(F("D:Range  rCH tCH Trp")); break;

      }

      //2nd line
      lcd.setCursor(0, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(pitchNotation[ZONE[ui_Zone][1]]))));

      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(pitchNotation[ZONE[ui_Zone][3]]))));

      lcd.setCursor(9, 1);
      LCD_PrintBCD2(ZONE[ui_Zone][0]);

      lcd.setCursor(13, 1);
      LCD_PrintBCD2(ZONE[ui_Zone][4]);

      lcd.setCursor(17, 1);
      LCD_Display7BitSigned(ZONE[ui_Zone][2] - 5);

      break;

    case SOFT_PAGE3:
      /*
        CFG PAGE 3 (MISC)
        []  []   [ ]  []  []
        01234567890123456789

      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      //  DOUT_PinSet0(DIN_ConfigMap[DIN_PAGE].dout_pin);    // off
      //DOUT_PinSet_Keypanel(0, 0, 1, 1, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      //      lcd.print(F("Cfg page 3"));
      if (SoftPanel.IsNewPage) {
        LCD_Clear();
        lcd.setCursor(0, 0);
        lcd.print(F("CLK ZON LOC  JTR XCC"));
      }

      // 2nd line
      lcd.setCursor(0, 1);
      switch (systmClock)
      {
        case INTCLK:
          LCD_PrintCString(F("INT")); //LCD_PrintBCD3(bpm);
          DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

        case MIDCLK:
          lcd.print(F("Mid"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

        case S24CLK:
          lcd.print(F("s24"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

        case TRGCLK:
          lcd.print(F("Trg"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

        case MTRGCLK:
          lcd.print(F("mTg"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

        case SYSCLK:
          lcd.print(F("Sys")); // not possible normally
          DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

        default:
          lcd.print(F("UKN"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
          break;

      }

      lcd.setCursor(4, 1);

      if (zActive)
      {
        lcd.print(F(" * "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
      }
      else
      {
        lcd.print(F(" . "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);
      }


      lcd.setCursor(8, 1);
      if (localControl)
      {
        //        lcd.print(F(" "));
        //        lcd.write((byte)5); // a note icon
        //        lcd.print(F(" "));
        lcd.print(F("on "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }
      else
      {
        lcd.print(F("off"));
        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }

      lcd.setCursor(13, 1);
      lcd.print(filter_ratio, DEC);
      //      if (FilterSustainMode == FILTER_ENV_MODE_BUGGED)
      //      {
      //        lcd.print(F(" . "));
      //        DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);
      //      }
      //      else
      //      {
      //        lcd.print(F(" $ "));
      //        DOUT_PinSet1(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);
      //      }

      lcd.setCursor(17, 1);
      if (mThru_XCc)
      {
        lcd.print(F(" * "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);
      }
      else
      {
        lcd.print(F(" . "));
        DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin);
      }

      break;

    case SOFT_PAGE4:
      /*
        CFG PAGE 3
        []  []   [ ]  []  []
        01234567890123456789

      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      LCD_Clear();
      lcd.print(F("CFG : GRouP (page 4)"));
      lcd.setCursor(0, 1);
      switch (ui_groupMode) {
        case GROUPSOLO:
          lcd.print(F("Solo"));
          break;

        case GROUPDUO:
          lcd.print(F("Duo "));
          break;

        case GROUPTRIO:
          lcd.print(F("Trio"));
          break;

        case GROUPQUARTET:
          lcd.print(F("Quar"));
          break;

        case GROUPDOUBLEDUO:
          lcd.print(F("2Duo"));
          break;

        default:
          lcd.print(F("none"));
          break;

      }
      //      lcd.print(ui_groupMode);
      break;

    case SOFT_PAGE5: // Matrix Ctrlr memory Dump
      /*
        ARP PAGE 3
        []  []   [ ]  []  []
        01234567890123456789

      */
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);   // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_ARP].dout_pin);     // ON
      DOUT_PinSet_Keypanel(1, 0, 1, 1, 1, 1);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      LCD_Clear();
      lcd.print(F("DUMP CTRLR MEMORY : "));
      lcd.setCursor(0, 1);
      lcd.print(F("Bank"));
      LCD_PrintBCD2(uBank[device]);
      lcd.setCursor(9, 1);
      lcd.print(F("Sys Mst All"));

      break;

    case SOFT_PAGE51:
      LCD_Clear();
      lcd.setCursor(0, 0);
      lcd.print(F("FORMAT EEPROM :  ENC"));
      lcd.setCursor(0, 1);
      lcd.print(F("Ext Int Init Sys "));

      lcd.setCursor(17, 1);
      if (encoder_inverted)
      {
        lcd.print(F("INV"));
        DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin);
      }
      else
      {
        lcd.print(F("NOR"));
        DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);
      }
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  Configure Arp pages
/////////////////////////////////////////////////////////////////////////////
void UI_Handle_Cfg()
{
  if (SoftPanel.Page == SOFT_PAGE1) // cfg page 1 ///////////////////////////// main page CFG ////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        break;

      case SOFT_EDIT_1: //
        SoftPanel.Page = SOFT_PAGE11;

        break;

      case SOFT_EDIT_2: //
        SoftPanel.Page = SOFT_PAGE2;

        break;

      case SOFT_EDIT_3: //
        SoftPanel.Page = SOFT_PAGE3;

        break;

      case SOFT_EDIT_4: //
        SoftPanel.Page = SOFT_PAGE4;

        break;

      case SOFT_EDIT_5: //
        SoftPanel.Page = SOFT_PAGE5;

        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE11) { /// cfg page 1.1 ///////////////////////////////////////////////// CFG/VIB page 1 //////////////////
    switch (SoftPanel.Button) {

      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE12;
        break;

      case SOFT_EDIT_1: // vibrato speed
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_2: //
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_3: //
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        break;

      case SOFT_EDIT_4: //
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        break;

      case SOFT_EDIT_5: //
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 1);
        MIDI_RequestMastersParameters(INTERFACE_SERIAL);

#if DEBUG_din
        Serial.print(F("MIDI_RequestMastersParameters ")); Serial.println(INTERFACE_SERIAL, DEC);
#endif

        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;

    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit1) {
        SetEncoderValue(&GlobalParameters[1], 63);
      }
      if (SoftPanel.EditButton == Edit2) {
        SetEncoderValue(&GlobalParameters[2], 2);
      }
      if (SoftPanel.EditButton == Edit3) {
        SetEncoderValue(&GlobalParameters[3], 63);
      }
      if (SoftPanel.EditButton == Edit4) {
        SetEncoderValue(&GlobalParameters[4], 5); // same as for LFO : Tri DnSaw, UpSaw, Sqr, Rnd, noise
      }
      else return;
    }

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE12) { // cfg page 1.2 ////////////////////////////////////////////////// CFG/VIB page 2 //////////////////
    switch (SoftPanel.Button) {

      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1: // vibrato speed
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_2: //
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_3: //
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        break;

      case SOFT_EDIT_4: //
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        break;

      case SOFT_EDIT_5: //
        STORE_GlobalParameters();
        READ_GlobalParameters();
        SendGlobalParameters(INTERFACE_SERIAL); // edited matrix
        SendGlobalParameters(INTERFACE_SERIAL3); // core OUT to DAW
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;
    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit1) {
        SetEncoderValue(&GlobalParameters[5], 63);
      }
      if (SoftPanel.EditButton == Edit2) {
        SetEncoderValue(&GlobalParameters[6], 2);
      }
      if (SoftPanel.EditButton == Edit3) {
        SetEncoderValue(&GlobalParameters[7], 63);
      }
      if (SoftPanel.EditButton == Edit4) {
        SetEncoderValue(&GlobalParameters[164], 24);
      }
      else return;
    }
    // realtime display feature
    app_flags.Display_DIN_Req = 1;

  }
  else if (SoftPanel.Page == SOFT_PAGE2) { // cfg page 2 //////////////////////////////////////////////////// CFG/MID Page 1 //////////////////

    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE21;
        break;

      case SOFT_EDIT_1:
        // click this button to save the midi channel to eeprom
        EEPROM.update(EEPROM_MIDI_CHANNEL, MIDI_CHANNEL);
        break;

      case SOFT_EDIT_2:
        // store in EEPROM device A model
        matrix_model_A = !matrix_model_A;
        EEPROM.update(EEPROM_MATRIX_MODELE_A, matrix_model_A);
        break;

      case SOFT_EDIT_3:
        // store in EEPROM device B model
        matrix_model_B = !matrix_model_B;
        EEPROM.update(EEPROM_MATRIX_MODELE_B, matrix_model_B);
        break;

      case SOFT_EDIT_4:
        // store in EEPROM device C model
        matrix_model_C = !matrix_model_C;
        EEPROM.update(EEPROM_MATRIX_MODELE_C, matrix_model_C);
        break;

      case SOFT_EDIT_5:
        // store in EEPROM device D model
        matrix_model_D = !matrix_model_D;
        EEPROM.update(EEPROM_MATRIX_MODELE_D, matrix_model_D);
        break;

      default:
        break;

    }
#if DEBUG_device
    Serial.print(F("midi channel = ")); Serial.println(MIDI_CHANNEL, DEC);
    Serial.print(F("MATRIX_MODELE_A = ")); Serial.println(matrix_model_A, DEC);
    Serial.print(F("MATRIX_MODELE_B = ")); Serial.println(matrix_model_B, DEC);
    Serial.print(F("MATRIX_MODELE_C = ")); Serial.println(matrix_model_C, DEC);
    Serial.print(F("MATRIX_MODELE_D = ")); Serial.println(matrix_model_D, DEC);
#endif

    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit1) {
        SetEncoderValue(&MIDI_CHANNEL, 16);
      }
    }
  }

  else if (SoftPanel.Page == SOFT_PAGE21)  // cfg page 21 //////////////////////////////////////////////////// CFG/MID Page 2 //////////////////
  {
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        STORE_GlobalParameters(); // save Masters Parameters into internal eeprom before leaving
        READ_GlobalParameters();
        SendGlobalParameters(INTERFACE_SERIAL); // edited matrix
        SendGlobalParameters(INTERFACE_SERIAL3); // core OUT to DAW
        SoftPanel.Page = SOFT_PAGE22;
        break;

      case SOFT_EDIT_1: // vibrato speed
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_2: //
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_3: //
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        break;

      case SOFT_EDIT_4: //
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        break;

      case SOFT_EDIT_5: //
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 1);
        ++GlobalParameters[13];
        if (GlobalParameters[13] > 1)
          GlobalParameters[13] = 0;
        //
        //        SendGlobalParameters(INTERFACE_SERIAL); // edited matrix
        //        SendGlobalParameters(INTERFACE_SERIAL3); // core OUT to DAW
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;
    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit1) {
        SetEncoderValue(&GlobalParameters[17], 120);
      }
      if (SoftPanel.EditButton == Edit2) {
        SetEncoderValue(&GlobalParameters[18], 120);
      }
      if (SoftPanel.EditButton == Edit3) {
        SetEncoderValue(&GlobalParameters[19], 120);
      }
      if (SoftPanel.EditButton == Edit4) {
        SetEncoderValue(&GlobalParameters[20], 120);
      }
      else
        return;
      /*
        // ceci deconne et ne met pas à jour en temps réel les valeurs des Globalparam : normal c'est pas dans les conditions !
           SendGlobalParameters(INTERFACE_SERIAL); // matrix à éditer
           SendGlobalParameters(INTERFACE_SERIAL3); // core OUT to DAW
      */


#if DEBUG
      // do stuff
#endif
    }

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE22)  // cfg page 22 //////////////////////////////////////////////////// CFG/MID Page 22 //////////////////
  {
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        //ZONE_Save();
        //SoftPanel.Page = SOFT_PAGE23;
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1:
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_2: //
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_3: //
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        break;

      case SOFT_EDIT_4: //
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        break;

      case SOFT_EDIT_5: //
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 1);
        break;

      case SOFT_EDIT_INC: //
        ++ui_Zone;
        if (ui_Zone > 3) ui_Zone = 3;
        break;

      case SOFT_EDIT_DEC: //
        --ui_Zone;
        if (ui_Zone == 255) ui_Zone = 0;
        break;

      default: break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;
    if (SoftPanel.EncoderValue != 0) {

      if (SoftPanel.EditButton == Edit3) {
        SetEncoderValue(&ZONE[ui_Zone][0], 16);
        if (ZONE[ui_Zone][0] == 0)
          ZONE[ui_Zone][0] = 1;
      }

      if (SoftPanel.EditButton == Edit1) {
        SetEncoderValue(&ZONE[ui_Zone][1], 127);
        if (ZONE[ui_Zone][1] < 0)
          ZONE[ui_Zone][1] = 0;
      }

      if (SoftPanel.EditButton == Edit5) {
        SetEncoderValue(&ZONE[ui_Zone][2], 10);
      }

      if (SoftPanel.EditButton == Edit2) {
        SetEncoderValue(&ZONE[ui_Zone][3], 127);
        if (ZONE[ui_Zone][3] < 0)
          ZONE[ui_Zone][3] = 0;
      }
      if (SoftPanel.EditButton == Edit4) {
        SetEncoderValue(&ZONE[ui_Zone][4], 16);
        if (ZONE[ui_Zone][4] == 0)
          ZONE[ui_Zone][4] = 1;
      }

      else
        return;

#if DEBUG
      // do stuff
#endif
    }

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE23)  // cfg page 23 //////////////////////////////////////////////////// CFG/MID Page 23 //////////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1: // vibrato speed
        DOUT_PinSet_Keypanel(1, 0, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_2: //
        DOUT_PinSet_Keypanel(0, 1, 0, 0, 0, 1);
        break;

      case SOFT_EDIT_3: //
        DOUT_PinSet_Keypanel(0, 0, 1, 0, 0, 1);
        break;

      case SOFT_EDIT_4: //
        DOUT_PinSet_Keypanel(0, 0, 0, 1, 0, 1);
        break;

      case SOFT_EDIT_5: //
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 1);
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;

    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;
    if (SoftPanel.EncoderValue != 0)
    {
#if DEBUG
      // do stuff
#endif
    }

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE3) { /// cfg page 3 ////////////////// CFG/MISC page //////////////////
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        if (Shift)
          SoftPanel.Page = SOFT_PAGE51; // hidden feature FORMAT()
        else
        {
          EEPROM.update(EEPROM_SYS_CLK, systmClock); 
          //EEPROM.update(EEPROM_SYS_BPM, bpm); // -> feature ?
          SoftPanel.Page = SOFT_PAGE1; // return to beginning of CFG
        }
        break;

      case SOFT_EDIT_1: // set midi clock for system
        ++systmClock;
        if (systmClock > MTRGCLK)
          systmClock = INTCLK; // can't be zero
        //EEPROM.update(EEPROM_SYS_CLK, systmClock); // don't save with this button as it is frequently pushed. use PAGE
        break;

      case SOFT_EDIT_2: // ext clock
        zActive = !zActive;
        EEPROM.update(EEPROM_Z_ACTIVE, zActive);
        break;

      case SOFT_EDIT_3: // midi local control of Ctrlr
        localControl ^=1; //localControl = !localControl;
        EEPROM.update(EEPROM_LOCAL_CONTROL, localControl);
        break;

      case SOFT_EDIT_4: // edit potentiometers jitter "filter_ratio"
        ++filter_ratio;
        if (filter_ratio > 9) filter_ratio = 1;
        EEPROM.update(EEPROM_FILTER_RATIO, filter_ratio);
        // toggle env1 filter mode to handle sustain bug. enable/disable
        //        FilterSustainMode = FilterSustainMode == FILTER_ENV_MODE_BUGGED ? FILTER_ENV_MODE_HACKED : FILTER_ENV_MODE_BUGGED;
        //        EEPROM.update(EEPROM_FILTERSUSTAIN_MODE, FilterSustainMode); // save this
        break;

      case SOFT_EDIT_5: // Set A/B : deviceA is arp, device B is Seq
        mThru_XCc = !mThru_XCc;
        EEPROM.update(EEPROM_MTHRU_XCC, mThru_XCc);
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }
//    // encoder : set bpm
//    if (systmClock == INTCLK)
//      bpm += SoftPanel.EncoderValue;
//    // range bpm :
//    if (bpm < BPM_min)
//      bpm = BPM_min;
//    if (bpm > BPM_Max)
//      bpm = BPM_Max;
//    uClock.setTempo(bpm);
    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE4) { // cfg  page 4 //////////////////////////////////////////////////// CFG/GRouP page 4 //////////////////
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1: //
        ++ui_groupMode;
        if (ui_groupMode > 5)
          ui_groupMode = 0;

        // update system groupMode value :
        groupMode = ui_groupMode;
        break;

      default:
        break;
    }
    // encoder : set groupMode
    ui_groupMode += SoftPanel.EncoderValue;
    if (ui_groupMode > 5)
      ui_groupMode = 0;

    // update system groupMode value :
    groupMode = ui_groupMode;

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE51) { // cfg  page 51 //////////////////////////////////////////////////// HIDDEN FORMAT FEATURE /////////////////
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1:
        FORMAT_Memory(1);
        lcd.setCursor(0, 1);
        lcd.print(F("Ext.EEPROM formatted "));
        elapsedTime = 0;
        break;

      case SOFT_EDIT_2:
        FORMAT_Memory(2);
        lcd.setCursor(0, 1);
        lcd.print(F("Int.EEPROM formatted "));
        elapsedTime = 0;
        break;

      case SOFT_EDIT_3:
        FORMAT_Memory(3);
        lcd.setCursor(0, 1);
        lcd.print(F(" Patch 000-999 ready "));
        elapsedTime = 0;
        break;

      case SOFT_EDIT_4: //
        FORMAT_Memory(4);
        lcd.setCursor(0, 1);
        lcd.print(F(" Sys & Masters ready "));
        elapsedTime = 0;
        break;

      case SOFT_EDIT_5: //
        encoder_inverted = !encoder_inverted;
        EEPROM.update(EEPROM_ENCODER_INVERTED, encoder_inverted);
        break;

      default:
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else
  {
    switch (SoftPanel.Button) { // cfg page5 //////////////////////////////////////////////////// CFG/DuMP page //////////////////

      case DIN_PAGE:
        if (Shift)
          SoftPanel.Page = SOFT_PAGE51;
        else
          SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1: // l
        // dump bank selected in Patch to CORE OUT & selected device
        DumpCtrlrBank(INTERFACE_SERIAL3, uBank[device]);
        DumpCtrlrBank(INTERFACE_SERIAL, uBank[device]);
        break;

      case SOFT_EDIT_2: // bank dump
        break;

      case SOFT_EDIT_3:
        SendCtrlrSystemCfg(INTERFACE_SERIAL3);
        break;

      case SOFT_EDIT_4:
        // dump Global Parameters to CORE OUT & selected device
        SendGlobalParameters(INTERFACE_SERIAL3);
        SendGlobalParameters(INTERFACE_SERIAL);
        break;

      case SOFT_EDIT_5:
        // dump ALL Banks and Global parameters to CORE OUT & selected device
        for (unsigned char i = 0; i <= uBANK_MAX; ++i)
        {
          // specify bank (in sysex) first:
          MIDI_SetBank(INTERFACE_SERIAL3, i);
          // then dump 100 patches on core out
          DumpCtrlrBank(INTERFACE_SERIAL3, i);
        }
        // ends with Globals
        SendGlobalParameters(INTERFACE_SERIAL3);
        SendCtrlrSystemCfg(INTERFACE_SERIAL3);
        break;

      default:
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;

    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit2) {
        SetEncoderValue(&uBank[device], uBANK_MAX);
      }
    }
    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
}
