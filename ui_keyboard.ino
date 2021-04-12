//#include <cmios.h>
//#include "main.h"
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
#include "pots.h"
#include "device.h"

/*
    KEYBOARD PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    PORTAMODE VELO  RATE
    CONS-TIME OFF     63

    KEYBOARD PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    KEYB-MODE      ULGTO
    REASSIGN/ROB     OFF
*/
void UI_Display_Keyboard()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1: // 1st page
      if (SoftPanel.IsNewPage) {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("PORTAMODE  VELO RATE"));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      //   LCD_PrintCString(PortamentoModes[EditBuffer[device][EB_PORTAMENTO_MODE]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(PortamentoModes[EditBuffer[device][EB_PORTAMENTO_MODE]]))));

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 12 + LCD_Offset);
      LCD_Display7BitSigned(EditBuffer[device][EB_PORTAMENTO_SPEEDBYVELOCITY]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 18 + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_PORTAMENTO_RATE]);
      break;

    case SOFT_PAGE2: // 2nd page
      if (SoftPanel.IsNewPage) {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("KEYB-MODE UNI.LEGATO"));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 0 + LCD_Offset);
      // LCD_PrintCString(KeyboardModes[EditBuffer[device][EB_KEYBOARD_MODE]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(KeyboardModes[EditBuffer[device][EB_KEYBOARD_MODE]]))));

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 14 + LCD_Offset);
      if (EditBuffer[device][EB_PORTAMENTO_LEGATOENABLE] == 0)
        LCD_PrintCString(F("OFF"));
      else
        LCD_PrintCString(F("ON "));

      break;

    default: break;
  }
}

/// TODO: Need 3rd keyboard page

/*

    KEYBOARD PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    PORTAMODE VELO  RATE
    CONS-TIME OFF     63

    KEYBOARD PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    KEYB-MODE      ULGTO
    REASSIGN/ROB     OFF

*/
void UI_Handle_Keyboard()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:
      if (SoftPanel.Button == DIN_PAGE)
        SoftPanel.Page = SOFT_PAGE2;
      // portamento mode down
      if (SoftPanel.Button == SOFT_EDIT_1)
      {
        if (EditBuffer[device][EB_PORTAMENTO_MODE] > 0)
          EditBuffer[device][EB_PORTAMENTO_MODE]--;
        MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_PORTAMENTO_MODE, EditBuffer[device][EB_PORTAMENTO_MODE], true);
        return;
      }

      // portamento mode up
      if (SoftPanel.Button == SOFT_EDIT_2)
      {
        EditBuffer[device][EB_PORTAMENTO_MODE]++;
        if (EditBuffer[device][EB_PORTAMENTO_MODE] > 3)
          EditBuffer[device][EB_PORTAMENTO_MODE] = 3;
        MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_PORTAMENTO_MODE, EditBuffer[device][EB_PORTAMENTO_MODE], true);
        return;
      }

      // handle page 1 encoder changes
      if (SoftPanel.EncoderValue != 0)
      {
        switch (SoftPanel.EditButton)
        {
          case Edit4: // portamento rate by velocity
            SetEncoder7Signed(&EditBuffer[device][EB_PORTAMENTO_SPEEDBYVELOCITY]);
           // MIDI_SendDelayedVoiceParam(SX_PORTAMENTO_SPEEDBYVELOCITY, Convert_7bit_Signed(EditBuffer[device][EB_PORTAMENTO_SPEEDBYVELOCITY]));
            MIDI_SendDelayedVoiceParam(SX_PORTAMENTO_SPEEDBYVELOCITY, (EditBuffer[device][EB_PORTAMENTO_SPEEDBYVELOCITY]));
            break;

          case Edit5: // portamento rate
            SetEncoderValue(&EditBuffer[device][EB_PORTAMENTO_RATE], 63);
            MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_PORTAMENTO_RATE, EditBuffer[device][EB_PORTAMENTO_RATE], true);
            break;

          default: break;
        }
      }

      break; // end page 1

    case SOFT_PAGE2:

      switch (SoftPanel.Button)
      {
        case SOFT_EDIT_1: // keyboard modes down
          if (EditBuffer[device][EB_KEYBOARD_MODE] > 0)
            EditBuffer[device][EB_KEYBOARD_MODE]--;
          MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_KEYBOARD_MODE, EditBuffer[device][EB_KEYBOARD_MODE], true);
          break;

        case SOFT_EDIT_2: // keyboard modes up
          EditBuffer[device][EB_KEYBOARD_MODE]++;
          if (EditBuffer[device][EB_KEYBOARD_MODE] > 3)
            EditBuffer[device][EB_KEYBOARD_MODE] = 3;
          MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_KEYBOARD_MODE, EditBuffer[device][EB_KEYBOARD_MODE], true);
          break;

        case SOFT_EDIT_4: // unison legato enable
          EditBuffer[device][EB_PORTAMENTO_LEGATOENABLE] = EditBuffer[device][EB_PORTAMENTO_LEGATOENABLE] == 0 ? 1 : 0;
          MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_PORTAMENTO_LEGATOENABLE , EditBuffer[device][EB_PORTAMENTO_LEGATOENABLE], true);
          break;

        case DIN_PAGE:
          SoftPanel.Page = SOFT_PAGE1;
          break;

        default: break;
      }
      break; // page 2

    default: break;
  }

}
