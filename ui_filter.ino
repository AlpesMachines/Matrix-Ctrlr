//#include <cmios.h>
//#include "main.h"
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
#include "ui_matrix.h"
#include "device.h"
#include "define.h"

unsigned char send_vcaenvelope;
unsigned char getFreeModBus(void);
void undoFilterMod(void);

/*
    FILTER PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    BND MDW PRTA KBD
    OFF OFF OFF  OFF

    FILTER PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    VCA AMT VEL0 ENV2
        63  +63  +63 snd

    FILTER PAGE 3
    []  []   [ ]  []  []
    12345678901234567890
    FILTER FREQ  SUSTAIN
    SEND SYSEX    OFF
*/
void UI_Display_Filter()
{
  if (SoftPanel.Page == SOFT_PAGE1)
  {
    if (SoftPanel.IsNewPage)
    {
      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("BND VIB PRTA KBD    "));
    }

    LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);

    // set up a default display
    LCD_PrintCString(F("OFF OFF OFF  OFF    "));

    if ((1 << 0) & EditBuffer[device][EB_FILTER_LEVERMOD]) // is pitch bend bit 0 set?
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintCString(F("ON "));
    }

    if ((1 << 1) & EditBuffer[device][EB_FILTER_LEVERMOD]) // is mod wheel bit 1 set?
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
      LCD_PrintCString(F("ON "));
    }

    if ((1 << 0) & EditBuffer[device][EB_FILTER_KEYBOARDMOD]) // is portamento bit 0 set?
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8 + LCD_Offset);
      LCD_PrintCString(F("ON "));
    }

    if ((1 << 1) & EditBuffer[device][EB_FILTER_KEYBOARDMOD]) // is kbd tracking bit 1 set?
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 13 + LCD_Offset);
      LCD_PrintCString(F("ON "));
    }

  }
  else if (SoftPanel.Page == SOFT_PAGE2)// page 2
  {
    if (SoftPanel.IsNewPage)
    {
      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("VCA AMT VEL0 ENV2"));
    }

    // vca1 amount
    LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
    LCD_PrintBCD2(EditBuffer[device][EB_VCA1_AMOUNT]);

    // vca1 velocity
    LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8 + LCD_Offset);
    LCD_Display7BitSigned(EditBuffer[device][EB_VCA1_AMPBYVELOCITY]);

    // vca2-env2
    LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 13 + LCD_Offset);
    LCD_Display7BitSigned(EditBuffer[device][EB_VCA2_AMPBYENV2]);
    if (send_vcaenvelope == 1)
    {
      LCD_PrintChar(LCDCHAR_ARROW_RIGHT);
      LCD_PrintCString(F("snd"));
    }
    else
      LCD_PrintCString(F("    "));
  }
  else // page 3
  {
    if (SoftPanel.IsNewPage)
    {
      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("FILTER FREQ  SUSTAIN"));
    }

    if (FilterCutoffMode == FILTER_FREQ_MODE_CCONTROL) // is filter cutoff mode set to controller (not sysex), sysex is slow, cc is fast. sysex is needed to program a patch, cc for performance tweeking
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintCString(F("SEND CCONTRL"));
    }
    else
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintCString(F("SEND SYSEX  "));
    }
    if (FilterSustainMode == FILTER_ENV_MODE_BUGGED)
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 14 + LCD_Offset);
      LCD_PrintCString(F("OFF"));
    }
    else
    {
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 14 + LCD_Offset);
      LCD_PrintCString(F("ON "));
    }

//    lcd.clear();
//    lcd.print(F("FLT/VCA page3"));

  } // end page 3
}

/*
    FILTER PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    BND MDW PRTA KBD FRQ
    OFF OFF OFF  OFF SYX (LEVER3 OR PEDAL1)

    FILTER PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    VCA AMT VEL0 ENV2
        63  +63  +63 snd

    FILTER PAGE 3
    []  []   [ ]  []  []
    12345678901234567890
    FILTER FREQ  SUSTAIN
    SEND SYSEX    OFF

*/
void UI_Handle_Filter()
{
  unsigned char param;
  unsigned char value;
  static unsigned char filter_cc_modbus;
  param = 0;
  value = 0;

  if (SoftPanel.Page == SOFT_PAGE1)
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE2;
        break;

      case SOFT_EDIT_1:
        EditBuffer[device][EB_FILTER_LEVERMOD]  ^= (1 << 0); // toggle pitch bend bit 0
        param = SX_FILTER_LEVERMOD;
        value = EditBuffer[device][EB_FILTER_LEVERMOD];
        MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);
        break;

      case SOFT_EDIT_2:
        EditBuffer[device][EB_FILTER_LEVERMOD] ^= (1 << 1); // toggle mod wheel bit 1
        param = SX_FILTER_LEVERMOD;
        value = EditBuffer[device][EB_FILTER_LEVERMOD];
        MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);
        break;

      case SOFT_EDIT_3:
        EditBuffer[device][EB_FILTER_KEYBOARDMOD]  ^= (1 << 0); // toggle portamento bit 0
        param = SX_FILTER_KEYBOARDMOD;
        value = EditBuffer[device][EB_FILTER_KEYBOARDMOD];
        MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);
        break;

      case SOFT_EDIT_4:
        EditBuffer[device][EB_FILTER_KEYBOARDMOD]  ^= (1 << 1); // toggle keyboard tracking bit 1
        param = SX_FILTER_KEYBOARDMOD;
        value = EditBuffer[device][EB_FILTER_KEYBOARDMOD];
        MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);
        break;

    } // end switch SoftPanel.Button

  } // end page 1
  else if ( SoftPanel.Page == SOFT_PAGE2) //////////////////////////////////////////// page 2 ///////////////////////////
  {
    if (SoftPanel.Button == DIN_PAGE)
      SoftPanel.Page = SOFT_PAGE3;

    if (SoftPanel.Button == SOFT_EDIT_5 && send_vcaenvelope == 1 && FilterSustainMode != FILTER_ENV_MODE_BUGGED)
    {
      SendEditBuffer(device, INTERFACE_SERIAL);
      send_vcaenvelope = 0;
    }

    if (SoftPanel.EncoderValue != 0)
    {
      switch (SoftPanel.EditButton) // n'envoie pas de sysex ! à corriger pour OS 2.14 et 1.20
      {
        case Edit2:
          // vca amount
          SetEncoderValue(&EditBuffer[device][EB_VCA1_AMOUNT], 63);
          param = SX_VCA1_AMOUNT;
          value = EditBuffer[device][EB_VCA1_AMOUNT];
          MIDI_SendDelayedVoiceParam(param, value);
          break;

        case Edit3:
          // vca velocity
          SetEncoder7Signed(&EditBuffer[device][EB_VCA1_AMPBYVELOCITY]);
          param = SX_VCA1_AMPBYVELOCITY;
          value = EditBuffer[device][EB_VCA1_AMPBYVELOCITY];
          MIDI_SendDelayedVoiceParam(param, value);
          break;

        case Edit4:
          // vca envelope
          SetEncoder7Signed(&EditBuffer[device][EB_VCA2_AMPBYENV2]);
          param = SX_VCA2_AMPBYENV2;
          value = EditBuffer[device][EB_VCA2_AMPBYENV2];
          MIDI_SendDelayedVoiceParam(param, value);
          send_vcaenvelope = 1; // can't send this as a single sysex... bugged. prompt user to send entire buffer instead
          break;

        default: break;
      }
    }
  }
  else //////////////////////////////////////////////// page 3 de FLT/VCA //////////////////////////////
  {
    if (SoftPanel.Button == DIN_PAGE)
      SoftPanel.Page = SOFT_PAGE1;

    switch (SoftPanel.Button)
    {
      /////////////////////////////// A REVOIR //////////////////////////////////////////
      case SOFT_EDIT_1:
      case SOFT_EDIT_2: // toggle filter cutoff tweak mode, sysex, or continuous controller, lever 3 (breath)
        FilterCutoffMode = FilterCutoffMode == FILTER_FREQ_MODE_SYSEX ? FILTER_FREQ_MODE_CCONTROL : FILTER_FREQ_MODE_SYSEX;
        if (FilterCutoffMode == FILTER_FREQ_MODE_CCONTROL)
        {
          filter_cc_modbus = getFreeModBus();
          if (filter_cc_modbus < 10)
          {
            EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Amount ]       = 63; // max
            EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Source ]       = MMOD_SRC_LEVER3;
            EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Destination ]  = MMOD_DEST_FILTER_FREQ;
            // set filter to zero for maximum effect
            EditBuffer[device][ EB_FILTER_FREQ] = 0;
            MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_FILTER_FREQ, 0, true);
            MIDI_SendMatrixModParam(filter_cc_modbus, EditBuffer[device][EB_MmodMap[filter_cc_modbus].Source], EditBuffer[device][EB_MmodMap[filter_cc_modbus].Destination], 63); // max
          }
          else
          {
            // no free bus available
          }
        }
        else // free up the bus
        {
          if (filter_cc_modbus < 10)
          {
            EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Amount ]       = 0; // off
            EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Source ]       = MMOD_SRC_UNUSED;
            EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Destination ]  = MMOD_DEST_UNUSED;
            MIDI_SendMatrixModParam(filter_cc_modbus, EditBuffer[device][EB_MmodMap[filter_cc_modbus].Source], EditBuffer[device][EB_MmodMap[filter_cc_modbus].Destination], EditBuffer[device][ EB_MmodMap[filter_cc_modbus].Amount ]  );
          }
        } // end FilterCutoffMode

        break;

      case SOFT_EDIT_4: // toggle env1 filter mode to handle sustain bug. enable/disable
        FilterSustainMode = FilterSustainMode == FILTER_ENV_MODE_BUGGED ? FILTER_ENV_MODE_HACKED : FILTER_ENV_MODE_BUGGED;
        EEPROM.update(EEPROM_FILTERSUSTAIN_MODE, FilterSustainMode); // save this
        break;

      default:
        break;

    } // end switch button
  } //////////////////////////////////////////////////////// end page 3 ///////////////////////////
}

// loop through the mmod buses and return the first free bus
// return 10 (0x0a) if none found
unsigned char getFreeModBus(void)
{
  unsigned char i;
  unsigned char bus = 10;

  for (i = 0; i < 10; i++)
  {
    if ( EditBuffer[device][ EB_MmodMap[i].Destination ] == 0)
    {
      bus = i;
      break;
    }
  }
  return bus; // none found
}


