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

  switch (SoftPanel.Page) {

    case SOFT_PAGE1:
      DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);			// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_EDIT].dout_pin); 			// on
      DOUT_PinSet_Keypanel(1, 1, 1, 1, 1, 1);

      //      DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); 	// on
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin); 		    // on
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	    // on
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 		// on
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		    // on
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_PAGE].dout_pin); 		    // off

      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("DCO FLT ENV KBD MTRX"));
      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintCString(F("LFO VCA ENV PRTA RMP"));
      break;

    case SOFT_PAGE3: // save patch to matrix1000 memory

      if (SoftPanel.IsNewPage) {
        //        LCD_Clear();
        //        LCD_PrintCString(F("OSC BEND MODW PORTA")); // ????????? d'où ça sort ?

        DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);			// off
        DOUT_PinSet1(DIN_ConfigMap[DIN_EDIT].dout_pin); 			// off

        //        DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); 	// off
        //        DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin); 		    // off
        //        DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	    // off
        //        DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 		// off
        //        DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		    // off
        //        DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 		    // on
        DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);

        DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("BNK SAVE PATCH "));
        LCD_PrintBCD1(uBank);
        LCD_PrintBCD2(uPatch);
        if (uPatch < 10)
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

      //      DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); 	// on
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin); 		    // on
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	    // off
      //      DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 		// off
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		    // on
      //      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 		    // on
      DOUT_PinSet_Keypanel(1, 1, 0, 0, 1, 1);

      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      //1st line
      LCD_Clear();
      LCD_CursorSet(1 + LCD_Offset);

      LCD_PrintCString(F(" "));
      LCD_PrintBCD1(uBank);
      LCD_PrintBCD2(uPatch);
//      // place a zero here for values below 10 (bank & patch) :
//      if (uBank < 10) {
//        LCD_CursorSet(2 + LCD_Offset);
//        LCD_PrintCString(F("0"));
//      }

      if (uPatch < 10) {
        LCD_CursorSet(3 + LCD_Offset);
        LCD_PrintCString(F("0"));
      }
      LCD_CursorSet(7 + LCD_Offset);
      LCD_PrintCString(F(":"));
      // print patch name :
      LCD_CursorSet(9 + 3 + LCD_Offset);
      for (i = 0; i < 8; i++) {
        LCD_PrintChar(EditBuffer[i]);
      }

      //cursor
      //  LCD_CursorSet(cursor - 9 + LCD_Offset);
//              lcd.setCursor(11+cursor, 0); // MARCHE PAS
//              lcd.cursor(); lcd.blink();
      // lcd.noCursor();
      //MIOS_LCD_Cmd(0x80 + cursor + 9 + LCD_Offset);
      //MIOS_LCD_Cmd(0x0e);

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
      //}
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
  else if (SoftPanel.Page == SOFT_PAGE2)
  { //////////////////////////////////////////////////////////////////  PAGE 2 /////////
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE3;
        break;

      case SOFT_EDIT_1: // "<"
        cursor--;
        //        lcd.setCursor(12+cursor,0);
        //        lcd.cursor();
        //MIOS_LCD_Cmd(0x10);
        break;

      case SOFT_EDIT_2: // ">"
        cursor++;
        //        lcd.setCursor(12+cursor, 0);
        //        lcd.cursor();
        //MIOS_LCD_Cmd(0x14);
        break;

      case SOFT_EDIT_5:
        //		  Write_Patch_To_BS(uBank,uPatch); // NON cela se fera dans le menu "PATCH"
        break;

      case SOFT_EDIT_INC:
        EditBuffer[cursor]++;
        break;

      case SOFT_EDIT_DEC:
        EditBuffer[cursor]--;
        break;

      case DIN_EDIT:
        SoftPanel.Page = SOFT_PAGE1;
        break;
    }

    // encoder
    // cursor 0 .. 8 : name of the patch
    if (EditBuffer[cursor] >= 90) // limit letters to human readable range (cf ASCII table)
      EditBuffer[cursor] = 90;
    if (EditBuffer[cursor] < 32)
      EditBuffer[cursor] = 32;

    EditBuffer[cursor] += SoftPanel.EncoderValue;
    if (cursor > 7)
      cursor = 0;
  }
  else
  { //////////////////////////////////////////////////////////////////  PAGE 3 /////////

    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        if (!RefreshSoftPanel)
          SoftPanel.Page = SOFT_PAGE1;
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




