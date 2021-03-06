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

      lcd.clear();
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

      lcd.clear();
      lcd.setCursor(0, 0); // 1st line
      lcd.print(F("SPD MOD AMNT WAV GBL"));
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

      lcd.clear();
      lcd.setCursor(0, 0); // 1st line
      lcd.print(F("AMP MOD AMNT BND GBL"));
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

      //      LCD_Clear();
      //      lcd.print(F("Cfg page 2"));

      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("CH  A:  B:   C:  D: "));
      //            lcd.print("CH  MxA MxB  MxC MxD");
      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintBCD2(MIDI_CHANNEL );

      // deviceA = 1000 or 6
      LCD_CursorSet(64 + 4 + LCD_Offset);
      if (matrix_model.A == 1) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }
      // device B = 1000 or 6
      LCD_CursorSet(64 + 8 + LCD_Offset);
      if (matrix_model.B == 1) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }
      // device C = 1000 or 6
      LCD_CursorSet(64 + 13 + LCD_Offset);
      if (matrix_model.C == 1) {
        LCD_PrintCString(F("M6 "));
      } else {
        LCD_PrintCString(F("M1K"));
      }
      // device D = 1000 or 6
      LCD_CursorSet(64 + 17 + LCD_Offset);
      if (matrix_model.D == 1) {
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

      lcd.clear();
      lcd.setCursor(0, 0); // 1st line
      lcd.print(F("Pd1 Pd2  Lv1 Lv2 Ena"));

      //      lcd.setCursor(0, 1); // 2nd line
      lcd.setCursor(1, 1);
      LCD_PrintBCD2(GlobalParameters[17]); // Midi Pedal 1 Controller
      lcd.setCursor(5, 1);
      LCD_PrintBCD2(GlobalParameters[18]); // midi Pedal 2 Controller
      lcd.setCursor(10, 1);
      LCD_PrintBCD2(GlobalParameters[19]); // midi Lever 1 Controller
      lcd.setCursor(14, 1);
      LCD_PrintBCD2(GlobalParameters[20]); // midi Lever 2 Controller
      lcd.setCursor(17, 1);
      //LCD_PrintBCD2(GlobalParameters[13]); // midi Controllers enable
      if (GlobalParameters[13])
        lcd.print(F(" * "));
      else
        lcd.print(F(" . "));

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

      lcd.clear();
      lcd.setCursor(0, 0);
      LCD_PrintCString(F("BPM SRC ENC  STN XCC"));

      // 2nd line
      lcd.setCursor(0, 1);
      switch (ui_external_clk)
      {
        case INTCLK:
          LCD_PrintBCD3(bpm); //LCD_PrintCString(F("INT"));
          DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);
          break;

        case MIDCLK:
          LCD_PrintCString(F("Mid"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
          break;

        case S24CLK:
          LCD_PrintCString(F("s24"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
          break;

        case TRGCLK:
          LCD_PrintCString(F("Trg"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
          break;

        case MTRGCLK:
          LCD_PrintCString(F("mTg"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
          break;

        default:
          LCD_PrintCString(F("Ukn"));
          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
          break;

      }

      //      if (ui_external_clk == false) // INTCLK // code prior 0.99x
      //      {
      //        LCD_PrintBCD3(bpm);
      //        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      //      }
      //      else
      //      {
      //        lcd.print(F("EXT"));
      //        DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      //      }
      //      //
      //      //      lcd.setCursor(4, 1);
      //      //      if (ui_external_clk) {
      //      //        LCD_PrintCString(F("EXT"));
      //      //        DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
      //      //      } else {
      //      //        LCD_PrintCString(F("INT"));
      //      //        DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);
      //      //      }

      lcd.setCursor(4, 1);
      LCD_PrintCString(F("---"));
//      switch (clk_src)
//      {
//        case INTCLK:
//          LCD_PrintCString(F("INT"));
//          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
//          break;
//
//        case MIDCLK:
//          LCD_PrintCString(F("Mid"));
//          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
//          break;
//
//        case S24CLK:
//          LCD_PrintCString(F("s24"));
//          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
//          break;
//
//        case TRGCLK:
//          LCD_PrintCString(F("Trg"));
//          DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
//          break;
//
//        default:
//          LCD_PrintCString(F("Ukn"));
//          DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin);
//          break;
//
//      }

      lcd.setCursor(8, 1);
      if (encoder_inverted) {
        LCD_PrintCString(F("INV"));
        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);

      } else {
        LCD_PrintCString(F("NOR"));
        DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      }

      lcd.setCursor(13, 1);
      if (FilterSustainMode == FILTER_ENV_MODE_BUGGED) {
        LCD_PrintCString(F("OFF"));
        DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);
      } else {
        LCD_PrintCString(F("ON "));
        DOUT_PinSet1(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);
      }

      lcd.setCursor(17, 1);
      if (mThru_XCc) {
        LCD_PrintCString(F("Yes"));
        DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);
      } else {
        LCD_PrintCString(F("No "));
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
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 0);
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);

      LCD_Clear();
      lcd.print(F("CFG : GRouP (page 4)"));
      lcd.setCursor(0, 1);
      lcd.print(F(" - TO DO - "));
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
      lcd.print(F("Dump Ctrlr Memo :   "));
      lcd.setCursor(0, 1);
      lcd.print(F("Bank"));
      LCD_PrintBCD2(uBank);
      lcd.setCursor(9, 1);
      lcd.print(F("Sys Gbl All"));

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
    switch (SoftPanel.Button) {

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
        MIDI_RequestGP(INTERFACE_SERIAL);

#if DEBUG_din
        Serial.print(F("MIDI_RequestGP ")); Serial.println(INTERFACE_SERIAL, DEC);
#endif

        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
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
        SetEncoderValue(&GlobalParameters[4], 5); // same as fo LFO : Tri DnSaw, UpSaw, Sqr, Rnd, noise
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
        SendGlobalParameters(INTERFACE_SERIAL); // edited matrix
        SendGlobalParameters(INTERFACE_SERIAL3); // core OUT to DAW
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
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
        EEPROM.write(EEPROM_MIDI_CHANNEL, MIDI_CHANNEL);
        break;

      case SOFT_EDIT_2:
        // store in EEPROM device A model
        matrix_model.A++;
        EEPROM.write(EEPROM_MATRIX_MODELE_A, matrix_model.A);
        break;

      case SOFT_EDIT_3:
        // store in EEPROM device B model
        matrix_model.B++;
        EEPROM.write(EEPROM_MATRIX_MODELE_B, matrix_model.B);
        break;

      case SOFT_EDIT_4:
        // store in EEPROM device C model
        matrix_model.C++;
        EEPROM.write(EEPROM_MATRIX_MODELE_C, matrix_model.C);
        break;

      case SOFT_EDIT_5:
        // store in EEPROM device D model
        matrix_model.D++;
        EEPROM.write(EEPROM_MATRIX_MODELE_D, matrix_model.D);
        break;

    }
#if DEBUG_device
    Serial.print(F("midi channel = ")); Serial.println(MIDI_CHANNEL, DEC);
    Serial.print(F("MATRIX_MODELE_A = ")); Serial.println(matrix_model.A, DEC);
    Serial.print(F("MATRIX_MODELE_B = ")); Serial.println(matrix_model.B, DEC);
    Serial.print(F("MATRIX_MODELE_C = ")); Serial.println(matrix_model.C, DEC);
    Serial.print(F("MATRIX_MODELE_D = ")); Serial.println(matrix_model.D, DEC);
#endif

    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit1) {
        SetEncoderValue(&MIDI_CHANNEL, 16);
      }
    }

  }

  else if (SoftPanel.Page == SOFT_PAGE21) { // cfg page 21 //////////////////////////////////////////////////// CFG/MID Page 2 //////////////////

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
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 1, 1);
        ++GlobalParameters[13];
        if (GlobalParameters[13] > 1)
          GlobalParameters[13] = 0;
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
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
      //      if (SoftPanel.EditButton == Edit5) {
      //        SetEncoderValue(&GlobalParameters[13], 1);
      //      }

      else 
      return;
      // ceci deconne et ne met pas à jour en temps réel les valeurs des Globalparam :
      //                SendGlobalParameters(INTERFACE_SERIAL); // matrix à éditer
      //                SendGlobalParameters(INTERFACE_SERIAL3); // core OUT to DAW
#if DEBUG
      // do stuff
#endif
    }

    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }

  else if (SoftPanel.Page == SOFT_PAGE3) { /// cfg page 3 ////////////////// CFG/MISC page //////////////////
    switch (SoftPanel.Button) {

      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1; // return to begin CFG
        break;

      case SOFT_EDIT_1: // bpm
        //ui_external_clk = !ui_external_clk; // prior 0.99x
        ++ui_external_clk;
        if (ui_external_clk > MTRGCLK)
          ui_external_clk = INTCLK;
        break;

      case SOFT_EDIT_2: // ext clock
        //        ++clk_src;
        //        if (clk_src > CLKSRCTRIG)
        //          clk_src = 0;
        break;

      case SOFT_EDIT_3: // set encoder polarity
        encoder_inverted = !encoder_inverted;
        EEPROM.write(EEPROM_ENCODER_INVERTED, encoder_inverted);
        break;

      case SOFT_EDIT_4: // toggle env1 filter mode to handle sustain bug. enable/disable
        FilterSustainMode = FilterSustainMode == FILTER_ENV_MODE_BUGGED ? FILTER_ENV_MODE_HACKED : FILTER_ENV_MODE_BUGGED;
        EEPROM.write(EEPROM_FILTERSUSTAIN_MODE, FilterSustainMode); // save this
        break;

      case SOFT_EDIT_5: // Set A/B : deviceA is arp, device B is Seq
        mThru_XCc = !mThru_XCc;
        EEPROM.write(EEPROM_MTHRU_XCC, mThru_XCc);
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;

      default:
        break;
    }
    // encoder : set bpm
    if (ui_external_clk == INTCLK)
      bpm += SoftPanel.EncoderValue;
    // range bpm :
    if (bpm < BPM_min)
      bpm = BPM_min;
    if (bpm > BPM_Max)
      bpm = BPM_Max;
    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }
  else if (SoftPanel.Page == SOFT_PAGE4) { // cfg  page 4 //////////////////////////////////////////////////// CFG/GRouP page //////////////////
    switch (SoftPanel.Button) {

      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1: //
        // do  stuff
        break;

      case SOFT_EDIT_2: //
        break;

      case SOFT_EDIT_3:
        break;

      case SOFT_EDIT_4: //
        break;

      case SOFT_EDIT_5: //
        break;

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
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
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_1: // l
        // dump bank selected in Patch to CORE OUT & selected device
        DumpCtrlrBank(INTERFACE_SERIAL3, uBank);
        DumpCtrlrBank(INTERFACE_SERIAL, uBank);
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
        for (unsigned char i = 0; i <= uBank_max; ++i)
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

      case SOFT_EDIT_INC: //
        break;

      case SOFT_EDIT_DEC: //
        break;
    }
    // encoder : set toto
    // toto += SoftPanel.EncoderValue;

    if (SoftPanel.EncoderValue != 0) {
      if (SoftPanel.EditButton == Edit2) {
        SetEncoderValue(&uBank, uBank_max);
      }
    }
    // realtime display feature
    app_flags.Display_DIN_Req = 1;
  }

}



