//#include <cmios.h>
//#include "main.h"
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
#include "pots.h"
#include "ui_matrix.h"
#include "device.h"

void handle_matrix_page1(void);
void handle_matrix_page2(void);
void handle_matrix_page3(void);

const mmodmap_t EB_MmodMap[10] =
{
  {EB_MODBUS0_SOURCE, EB_MODBUS0_DESTINATION, EB_MODBUS0_AMOUNT },
  {EB_MODBUS1_SOURCE, EB_MODBUS1_DESTINATION, EB_MODBUS1_AMOUNT },
  {EB_MODBUS2_SOURCE, EB_MODBUS2_DESTINATION, EB_MODBUS2_AMOUNT },
  {EB_MODBUS3_SOURCE, EB_MODBUS3_DESTINATION, EB_MODBUS3_AMOUNT },
  {EB_MODBUS4_SOURCE, EB_MODBUS4_DESTINATION, EB_MODBUS4_AMOUNT },
  {EB_MODBUS5_SOURCE, EB_MODBUS5_DESTINATION, EB_MODBUS5_AMOUNT },
  {EB_MODBUS6_SOURCE, EB_MODBUS6_DESTINATION, EB_MODBUS6_AMOUNT },
  {EB_MODBUS7_SOURCE, EB_MODBUS7_DESTINATION, EB_MODBUS7_AMOUNT },
  {EB_MODBUS8_SOURCE, EB_MODBUS8_DESTINATION, EB_MODBUS8_AMOUNT },
  {EB_MODBUS9_SOURCE, EB_MODBUS9_DESTINATION, EB_MODBUS9_AMOUNT },
};


/*

    #define EB_MODBUS0_SOURCE		104
    #define EB_MODBUS0_AMOUNT		105
    #define EB_MODBUS0_DESTINATION 	106
*/

/*

    MATRIX MOD PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    BS SRC  DST     AMNT
     0 ABCD ABCDEFGH +99

    MATRIX MOD PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    RMP SPD TRGMODE TGIN
    1   63  SINGLE  ENV1

    MATRIX MOD PAGE 3
    []  []   [ ]  []  []
    12345678901234567890
    TRACKGEN-INPUT: ENV1
    63  63   63   63  63


  //// code used to PROGMEM :
  //// lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(
*/

void UI_Display_Matrix()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("BS SRC  DEST    AMNT"));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintBCD2(SoftPanel.Selected_MatrixBus + 0); // 0 is BS1 like on Matrix6 display
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 3 + LCD_Offset);
      //LCD_PrintCString(MMOD_Sources[EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Source]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(MMOD_Sources[EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Source]]))));
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8 + LCD_Offset);
      //LCD_PrintCString(MMOD_Destinations[EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Destination]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(MMOD_Destinations[EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Destination]]))));
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 17 + LCD_Offset);
      LCD_Display7BitSigned(EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Amount]);
      //            LCD_PrintBCD3(EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Amount]);
      break;

    //    RMP SPD TRGMODE TGIN
    //    1   63  SINGLE  ENV1

    case SOFT_PAGE2:
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("RMP SPD TRGMODE|TGIN"));
      }

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintBCD2(SoftPanel.Selected_Ramp + 1);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
      if (SoftPanel.Selected_Ramp == RAMP1)
        LCD_PrintBCD2(EditBuffer[device][EB_RAMP1_RATE]);
      else
        LCD_PrintBCD2(EditBuffer[device][EB_RAMP2_RATE]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8 + LCD_Offset);

      if (SoftPanel.Selected_Ramp == RAMP1)
        //LCD_PrintCString(RampModes[EditBuffer[device][EB_RAMP1_MODE]]);
        lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(RampModes[EditBuffer[device][EB_RAMP1_MODE]]))));
      else
        //LCD_PrintCString(RampModes[EditBuffer[device][EB_RAMP2_MODE]]);
        lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(RampModes[EditBuffer[device][EB_RAMP2_MODE]]))));

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 15 + LCD_Offset);
      lcd.print(F("|")); // mise en page
      //LCD_PrintCString(MMOD_Sources[EditBuffer[device][EB_TRACK_INPUT]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(MMOD_Sources[EditBuffer[device][EB_TRACK_INPUT]]))));
      break;

    // 63  63   63   63  63

    case SOFT_PAGE3:
      if (SoftPanel.IsNewPage)	{
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("TRACKGEN-INPUT: "));
        // LCD_PrintCString(MMOD_Sources[EditBuffer[device][EB_TRACK_INPUT]]);
        lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(MMOD_Sources[EditBuffer[device][EB_TRACK_INPUT]]))));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_TRACK_POINT1]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_TRACK_POINT2]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 9 + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_TRACK_POINT3]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 14 + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_TRACK_POINT4]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 18 + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_TRACK_POINT5]);

      break;
  }
}

/*

    MATRIX MOD PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    BS SRC  DST     AMNT
     0 ABCD ABCDEFGH +99

    MATRIX MOD PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    RMP SPD TRGMODE TGIN
    1   63  SINGLE  ENV1

    MATRIX MOD PAGE 3
    []  []   [ ]  []  []
    12345678901234567890
    TRAKGEN-IN: ENV1
    63  63   63   63  63



*/
void UI_Handle_Matrix()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:
      handle_matrix_page1();
      break;

    case SOFT_PAGE2:
      handle_matrix_page2();
      break;

    case SOFT_PAGE3:
      handle_matrix_page3();
      break;
  }
}

void handle_matrix_page1()
{
  unsigned char bus       = 0;
  unsigned char source    = 0;
  unsigned char dest      = 0;
  unsigned char amount    = 0;

  //	ChooseEditBuffer[device]ABCD(); // define which EditBuffer[device] to use

  if (SoftPanel.Button == DIN_PAGE)
    SoftPanel.Page = SOFT_PAGE2;
  if (SoftPanel.Button == SOFT_EDIT_1) {
    ++SoftPanel.Selected_MatrixBus;

    if (SoftPanel.Selected_MatrixBus > MMOD_BUS10)
    {
      SoftPanel.Selected_MatrixBus = MMOD_BUS1;
    }
  }

  if (SoftPanel.Button == SOFT_EDIT_5) // send midi when this button is pushed
  {
    bus = SoftPanel.Selected_MatrixBus;
    source = EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Source];
    dest = EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Destination];
    amount = EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Amount];
    MIDI_SendMatrixModParam(bus, source, dest, amount);
  }

  // handle page 1 encoder changes
  if (SoftPanel.EncoderValue != 0)
  {
    switch (SoftPanel.EditButton)
    {
      case Edit1: // matrix mod bus
        SetEncoderValue(&SoftPanel.Selected_MatrixBus, 9);
        break;

      case Edit2: // matrix mod source
        SetEncoderValue(&EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Source], 20);
        break;

      case Edit3: // matrix mod dest
        SetEncoderValue(&EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Destination], 32);

        break;

      case Edit5: // matrix mod amount, dont send midi while we turn the encoder, matrix takes to long to respond if we do.
        SetEncoder7Signed(&EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Amount]);
        //                SetEncoderValue(&EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Amount], 255);

        break;

      default : break;

    } //end switch edit_button

    bus = SoftPanel.Selected_MatrixBus;
    source = EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Source];
    dest = EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Destination];
    amount = EditBuffer[device][EB_MmodMap[SoftPanel.Selected_MatrixBus].Amount];
    MIDI_SendMatrixModParam(bus, source, dest, amount);
  } // end if encoder val
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////
/*
    MATRIX MOD PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    RMP SPD TRGMODE TGIN
    1   63  SINGLE  ENV1
*/

void handle_matrix_page2()
{
  unsigned char param = 0;
  unsigned char value = 0;

  //	ChooseEditBuffer[device]ABCD(); // define which EditBuffer[device] to use

  switch (SoftPanel.Button)
  {
    case SOFT_EDIT_1: // selected ramp
      SoftPanel.Selected_Ramp = SoftPanel.Selected_Ramp == RAMP1 ? RAMP2 : RAMP1;
      break;
    case DIN_PAGE:
      SoftPanel.Page = SOFT_PAGE3;
      break;
  }

  // handle page 1 encoder changes
  if (SoftPanel.EncoderValue != 0)
  {
    switch (SoftPanel.EditButton)
    {
      case Edit2: // ramp rate
        if (SoftPanel.Selected_Ramp == RAMP1)
        {
          param = SX_RAMP1_RATE;
          SetEncoderValue(&EditBuffer[device][EB_RAMP1_RATE], 63);
          value = EditBuffer[device][EB_RAMP1_RATE];
        }
        else
        {
          param = SX_RAMP2_RATE;
          SetEncoderValue(&EditBuffer[device][EB_RAMP2_RATE], 63);
          value = EditBuffer[device][EB_RAMP2_RATE];
        }

        break;

      case Edit3: // ramp trigger modes
        if (SoftPanel.Selected_Ramp == RAMP1)
        {
          param = SX_RAMP1_MODE;
          SetEncoderValue(&EditBuffer[device][EB_RAMP1_MODE], 3);
          value = EditBuffer[device][EB_RAMP1_MODE];
        }
        else
        {
          param = SX_RAMP2_MODE;
          SetEncoderValue(&EditBuffer[device][EB_RAMP2_MODE], 3);
          value = EditBuffer[device][EB_RAMP2_MODE];
        }
        break;

      case Edit5: // tracking generator input
        SetEncoderValue(&EditBuffer[device][EB_TRACK_INPUT], 20);
//        ++EditBuffer[device][EB_TRACK_INPUT];
//        if (EditBuffer[device][EB_TRACK_INPUT] > 20)
//          EditBuffer[device][EB_TRACK_INPUT] = 0;
        param = SX_TRACK_INPUT;
        value = EditBuffer[device][EB_TRACK_INPUT];

        break;

      default: break;

    } //end switch edit_button

    if (param > 0)
      MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);

  } // end if encoder val

}
///////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
void handle_matrix_page3()
{
  unsigned char param = 0;
  unsigned char value = 0;

  //	ChooseEditBuffer[device]ABCD(); // define which EditBuffer[device] to use
  if (SoftPanel.Button == DIN_PAGE)
    SoftPanel.Page = SOFT_PAGE1;

  if (SoftPanel.EncoderValue != 0)
  {
    switch (SoftPanel.EditButton)
    {
      case Edit1://point 1
        SetEncoderValue(&EditBuffer[device][EB_TRACK_POINT1], 63);
        param = SX_TRACK_POINT1;
        value = EditBuffer[device][EB_TRACK_POINT1];
        break;

      case Edit2: //point 2
        SetEncoderValue(&EditBuffer[device][EB_TRACK_POINT2], 63);
        param = SX_TRACK_POINT2;
        value = EditBuffer[device][EB_TRACK_POINT2];
        break;

      case Edit3: //point 3
        SetEncoderValue(&EditBuffer[device][EB_TRACK_POINT3], 63);
        param = SX_TRACK_POINT3;
        value = EditBuffer[device][EB_TRACK_POINT3];
        break;

      case Edit4: //point 4
        SetEncoderValue(&EditBuffer[device][EB_TRACK_POINT4], 63);
        param = SX_TRACK_POINT4;
        value = EditBuffer[device][EB_TRACK_POINT4];
        break;

      case Edit5: //point 5
        SetEncoderValue(&EditBuffer[device][EB_TRACK_POINT5], 63);
        param = SX_TRACK_POINT5;
        value = EditBuffer[device][EB_TRACK_POINT5];
        break;

//      case EditInc:
//        ++EditBuffer[device][EB_TRACK_INPUT];
//        if (EditBuffer[device][EB_TRACK_INPUT] > 20)
//          EditBuffer[device][EB_TRACK_INPUT] = 0;
//        param = EB_TRACK_INPUT;
//        value = EditBuffer[device][EB_TRACK_INPUT];
//        break;
//
//      case EditDec:
//        --EditBuffer[device][EB_TRACK_INPUT];
//        if (EditBuffer[device][EB_TRACK_INPUT] == 255)
//          EditBuffer[device][EB_TRACK_INPUT] = 0;
//        param = EB_TRACK_INPUT;
//        value = EditBuffer[device][EB_TRACK_INPUT];
//        break;

      default:
        break;

    }
    if (param > 0)
      MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);
  }
}


