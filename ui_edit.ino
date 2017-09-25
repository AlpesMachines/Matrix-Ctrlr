//#include <cmios.h>
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
//#include "patchnames.h"
#include "memo.h"
#include "ui_patch.h"
#include "device.h"
#include <LiquidCrystal.h>

unsigned char banksaved;
unsigned char programsaved;
unsigned char saved;
//static unsigned char cursor;
unsigned char ascii;


// for blinking characters




/*
  EDIT PAGE 1
  []  []   [ ]  []  []
  12345678901234567890
  SELECT EDIT GROUP
  LFO VCA ENV PRTA RMP

  EDIT PAGE 3
  []  []   [ ]  []  []
  12345678901234567890
  BNK SAVE PATCH?
  0   abcdeghj:00 save

   EDIT PAGE 2
   []  []   [ ]  []  []
   0123567890123567890
  	uOOO      ________
    <   >     editname
    o   o   *    *   o
*/

////////////////////////////////////////////////////////
// Display for edit
////////////////////////////////////////////////////////
void UI_Display_Edit (void)
{
  unsigned char i;

  switch (SoftPanel.Page)
  {
#if DEBUG_LCD
    case SOFT_PAGE4:
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print((char)ascii);
      lcd.setCursor(4, 0);
      lcd.print(F("CLEAR EEPROM"));

      lcd.setCursor(0, 1);
      lcd.print(ascii, DEC);
      lcd.setCursor(4, 1);
      lcd.print(F("Ext Int Init "));
      break;
#endif

    case SOFT_PAGE1:
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);			// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_EDIT].dout_pin); 			// on
      DOUT_PinSet_Keypanel(1, 1, 1, 1, 1, 1);
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("DCO FLT ENV KBD MTRX"));
      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintCString(F("LFO VCA ENV PRTA RMP"));
      break;

    case SOFT_PAGE2: // name the patch

      /*
        EDIT PAGE 2
        []  []   [ ]  []  []
        12345678901234567890
          uOOO ________
        <   >  editname save
        o   o   *    *   o
      */

      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);			// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_EDIT].dout_pin); 			// on
      DOUT_PinSet_Keypanel(1, 1, 0, 0, 1, 1);
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      //1st line
      LCD_Clear();
      LCD_CursorSet(1 + LCD_Offset);

      LCD_PrintCString(F(" "));
      LCD_PrintBCD1(uBank[device]);
      LCD_PrintBCD2(uPatch[device]);

      if (uPatch[device] < 10) {
        LCD_CursorSet(3 + LCD_Offset);
        LCD_PrintCString(F("0"));
      }
      LCD_CursorSet(7 + LCD_Offset);
      LCD_PrintCString(F(":"));
      // print patch name :
      LCD_CursorSet(9 + 3 + LCD_Offset);
      for (i = 0; i < 8; i++) {
        LCD_PrintChar(EditBuffer[device][i]);
      }

      // 2nd line
      //      lcd.setCursor(0,1);
      LCD_CursorSet(65 + LCD_Offset);
      LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(69 + LCD_Offset);
      //      LCD_PrintCString(F(">"));
      LCD_PrintChar(CHAR_UP);

      LCD_CursorSet(71 + 5 + LCD_Offset);
      LCD_PrintCString(F("EditName"));

      break;

    case SOFT_PAGE3: // save patch to matrix1000 memory

      if (SoftPanel.IsNewPage) {
        DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);      // off
        DOUT_PinSet1(DIN_ConfigMap[DIN_EDIT].dout_pin);       // off
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
        DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("BNK SAVE PATCH "));
        LCD_PrintBCD1(uBank[device]);
        LCD_PrintBCD2(uPatch[device]);
        if (uPatch[device] < 10)
        {
          LCD_CursorSet(16 + LCD_Offset);
          LCD_PrintCString(F("0")); // place a zero here for values below 10
        }
        saved = 0;
      }

      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintBCD1(banksaved);
      LCD_CursorSet(64 + 4 + LCD_Offset);
      LCD_PrintCString(F("store in"));
      LCD_PrintCString(F(":"));
      LCD_PrintBCD2(programsaved);
      if (programsaved < 10)
      {
        LCD_CursorSet(64 + 13 + LCD_Offset);
        LCD_PrintCString(F("0")); // place a zero here for values below 10
      }

      LCD_CursorSet(64 + 16 + LCD_Offset);
      if (!saved)
        LCD_PrintCString(F(" DO "));
      else
        LCD_PrintCString(F("    "));
      break;

    case SOFT_PAGE4: // wizard
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("RANDOMIZE:          "));

      lcd.setCursor(0, 1);
      lcd.print(F("DCO FLT ENV  KBD LFO"));
      break;

    case SOFT_PAGE5: // wizard
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("RANDOMIZE:          "));

      lcd.setCursor(0, 1);
      lcd.print(F("RMP MOD MTRX --- ---"));
      break;

    default:
      break;
  }
}

////////////////////////////////////////////////////////
// Handle for Edit
////////////////////////////////////////////////////////
void UI_Handle_Edit(void)
{

  static unsigned char cursor;

  if (SoftPanel.Page == SOFT_PAGE1)
  { //////////////////////////////////////////////////////////////////  PAGE 1 /////////
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE2;
        break;
    }
  }
  else if (SoftPanel.Page == SOFT_PAGE2) //////////////////////////////////////////////////////////////////  PAGE 2 /////////
  {

    // // for blinking charaters : TO DO   http://forum.arduino.cc/index.php?topic=224521.0
    //    if (millis() - startedFlash > intervall)
    //    {
    //      if (charToPrint[cursor] == ' ')
    //      {
    //        charToPrint[cursor] = EditBuffer[device][cursor];
    //      }
    //      else
    //      {
    //        charToPrint[cursor] = ' ';
    //      }
    //      startedFlash = millis();
    //      lcd.setCursor(0, 1);
    //      lcd.print(charToPrint[0]);
    //      lcd.print(charToPrint[1]);
    //      lcd.print(charToPrint[2]);
    //    }

    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE3;
        break;

      case SOFT_EDIT_1: // "<"
        cursor--;
        break;

      case SOFT_EDIT_2: // ">"
        cursor++;
        break;

      case SOFT_EDIT_5:
        //		  Write_Patch_To_BS(uBank[device],uPatch[device]); // NON cela se fera dans le menu "PATCH"
        break;

      case SOFT_EDIT_INC:
        EditBuffer[device][cursor]++;
        break;

      case SOFT_EDIT_DEC:
        EditBuffer[device][cursor]--;
        break;

      case DIN_EDIT:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_ENC_CLIC:
        break;

      default:
        break;
    }

    // encoder
    cursor += encoderClic;

    // cursor 0 .. 8 : name characters of the patch
    if (EditBuffer[device][cursor] >= 90) // limit letters to human readable range (cf ASCII table)
      EditBuffer[device][cursor] = 90;
    if (EditBuffer[device][cursor] < 32)
      EditBuffer[device][cursor] = 32;

    EditBuffer[device][cursor] += SoftPanel.EncoderValue;
    if (cursor > 7)
      cursor = 0;
  }

  else if  (SoftPanel.Page == SOFT_PAGE6) ////////////////////////////////////// PAGE6///////////////// provisoire pour test lcd ascii
  {
    ascii += SoftPanel.EncoderValue;

    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      case SOFT_EDIT_2:
        FORMAT_Memory(1);
        lcd.clear();
        lcd.print(F("all 24LC512 cleared  "));
        elapsedTime = 0;
        break;

      case SOFT_EDIT_3:
        FORMAT_Memory(2);
        lcd.clear();
        lcd.print(F("intern EEPROM cleared"));
        elapsedTime = 0;
        break;

      case SOFT_EDIT_4:
        FORMAT_Memory(3);
        lcd.clear();
        lcd.print(F("4x24LC512 initiliased"));
        elapsedTime = 0;
        break;

      //      case SOFT_EDIT_5:
      //        lcd.noBlink();
      //        break;
      //      case SOFT_EDIT_1:
      //        lcd.blink();
      //        break;

      default:
        break;
    }
  }
  else if  (SoftPanel.Page == SOFT_PAGE4) ////////////////////////////////////// PAGE4 /////////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE5;
        break;

      case SOFT_EDIT_1:
        WizardEditBuffer( device, 1);
        break;

      case SOFT_EDIT_2:
        WizardEditBuffer( device, 2);
        break;

      case SOFT_EDIT_3:
        WizardEditBuffer( device, 3);
        break;

      case SOFT_EDIT_4:
        WizardEditBuffer( device, 4);
        break;

      case SOFT_EDIT_5:
        WizardEditBuffer( device, 5);
        break;

      default:
        break;
    }
  }
  else if  (SoftPanel.Page == SOFT_PAGE5) ////////////////////////////////////// PAGE5 /////////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE4; // aller/retour page 4, 5
        break;

      case SOFT_EDIT_1:
        WizardEditBuffer( device, 6);
        break;

      case SOFT_EDIT_2:
        WizardEditBuffer( device, 7);
        break;

       case SOFT_EDIT_3:
        WizardEditBuffer( device, 8);
        break;
        
        default:
        break;
    }
  }
  else
  { //////////////////////////////////////////////////////////////////  PAGE 3 /////////

    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        if (!RefreshSoftPanel)
          SoftPanel.Page = SOFT_PAGE4;
#if DEBUG_LCD
        SoftPanel.Page = SOFT_PAGE6; // retour debut
#endif
        break;

      case SOFT_EDIT_1:
        if (!RefreshSoftPanel) {
          banksaved = banksaved == 0 ? 1 : 0;
          saved = 0;
        }
        break;

      case SOFT_EDIT_5:
        if (!RefreshSoftPanel) {
          StoreEditBuffer(INTERFACE_SERIAL, banksaved, programsaved);
          saved = 1;
        }
        break;

      case DIN_EDIT:
        if (!RefreshSoftPanel)
          SoftPanel.Page = SOFT_PAGE1;
        break;
    }
    if (SoftPanel.EncoderValue != 0)
    {
      // encoder
      programsaved += SoftPanel.EncoderValue;
      if (programsaved > 99)
        programsaved = 0;
      if (programsaved > 99)
        programsaved = 99;
      saved = 0;
    } // end if encoder
  } // end else page 3
}




