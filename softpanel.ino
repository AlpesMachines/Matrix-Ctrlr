#include "pots.h"
#include "lcd.h"
#include "din.h"
#include "chaosmatrix.h"
#include "midi.h"
#include "softpanel.h"
#include "memo.h"
#include "EEPROM.h"

SoftPanel_t SoftPanel;
enum PanelMode_t getpanelmode();

void handle_patch(void);
void handle_edit(void);
void handle_arp(void);
void handle_cfg(void);
void display_patch(void);
void display_edit(void);
void display_oscillators(void);
void display_filter(void);
void display_envelopes(void);
void display_keyboard(void);
void display_matrix(void);
void display_arp(void);
void display_cfg(void);
void set_panelmode_led(void);
void scan_edit_buttons(void);

/////////////////////////////////////////////////////////////////////////////
//  Initializes variables for the softpanel
/////////////////////////////////////////////////////////////////////////////
void SoftPanel_Init()
{
  ProgramNumber = 0;
  BankNumber = 0;
  ProgramNumberEdit = ProgramNumber;
  BankNumberEdit = BankNumber;

  // soft panel setup
  SoftPanel.Mode          = Patch;
  SoftPanel.Selected_Ramp = RAMP1;
  SoftPanel.Selected_Osc  = DCO1; // 0 = DCO1, 1 = DCO2
  SoftPanel.Selected_Env	= ENV1; // 0 = ENV1, 1 = ENV2, 2 = ENV3
  SoftPanel.Selected_LFO	= LFO1; // 0 = LFO1, 1 = LFO2

  // light up the led for panel mode
  DOUT_PinSet(DOUT_PATCH_SELECT, 1);

  FilterSustainMode       = EEPROM.read(EEPROM_FILTERSUSTAIN_MODE);
  FilterCutoffMode        = FILTER_FREQ_MODE_SYSEX;

  SoftPanel_DisplayHandler();
}

/////////////////////////////////////////////////////////////////////////////
//  Handles display of the softpanel
/////////////////////////////////////////////////////////////////////////////
void SoftPanel_DisplayHandler()
{
  if (SoftPanel.Mode != SoftPanel.LastMode || SoftPanel.LastPage != SoftPanel.Page || RefreshSoftPanel)
  {
    SoftPanel.IsNewPage = 1;
    RefreshSoftPanel    = 0; // we're currently handled, so reset
  }
  set_panelmode_led();

  switch (SoftPanel.Mode)
  {
    case Patch:
      UI_Display_Patch();
      break;

    case Edit:
      UI_Display_Edit();
      break;

    case Oscillators:
      UI_Display_Oscillators();
      break;

    case Filter:
      UI_Display_Filter();
      break;

    case Envelopes:
      UI_Display_Envelopes();
      break;

    case Keyboard:
      UI_Display_Keyboard();
      break;

    case Matrix:
      UI_Display_Matrix();
      break;

    case Arp:
      UI_Display_Arp();
      break;

    case Cfg:
      UI_Display_Cfg();
      break;

    default:
      break;
  }

  SoftPanel.LastMode = SoftPanel.Mode;
  SoftPanel.LastPage = SoftPanel.Page;
  SoftPanel.IsNewPage = 0;
}

/////////////////////////////////////////////////////////////////////////////
//  Sends MIDI and updates EditBuffer[device]
/////////////////////////////////////////////////////////////////////////////
void SoftPanel_Handler (unsigned char pin, unsigned char encval)
{
  SoftPanel.Button = pin;
  SoftPanel.EncoderValue = encval;

  SoftPanel.Mode = getpanelmode();

#if(DEBUG_softpanel)
  Serial.print(F("SoftPanel_Handler()/ pin ")); Serial.print(SoftPanel.Button, DEC); 
  Serial.print(F(" /encval : "));Serial.print(SoftPanel.EncoderValue, DEC); 
  Serial.print(F("/ SoftPanel.Mode : ")); Serial.println(SoftPanel.Mode, DEC);
#endif

  scan_edit_buttons();

  switch (SoftPanel.Mode)
  {
    case Patch:
      //    Device_Select(pin);
      UI_Handle_Patch();
      break;

    case Edit:
      UI_Handle_Edit();
      break;

    case Envelopes:
      UI_Handle_Envelopes();
      break;

    case Keyboard:
      UI_Handle_Keyboard();
      break;

    case Oscillators:
      UI_Handle_Oscillators();
      break;

    case Filter:
      UI_Handle_Filter();
      break;

    case Matrix:
      UI_Handle_Matrix();
      break;

    case Arp:
      UI_Handle_Arp();
      break;

    case Cfg:
      UI_Handle_Cfg();
      break;

    default:
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  Light up leds when the edit button is pressed.
/////////////////////////////////////////////////////////////////////////////
void set_panelmode_led()
{
  DOUT_PinSet0(DIN_ConfigMap[DIN_PATCH].dout_pin);		// off
  DOUT_PinSet1(DIN_ConfigMap[DIN_EDIT].dout_pin); 		// on
  DOUT_PinSet0(DIN_ConfigMap[DIN_ARP].dout_pin); 		// on
  DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 0);

  DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

  if (SoftPanel.Page > SOFT_PAGE1)
  {
    DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 	// on
  }
  switch (SoftPanel.Mode)
  {
    case Patch: // allumer ou pas les leds ici : question ?
      break;
    case Edit:
      break;
    case Arp:
      break;

    case Cfg :
      DOUT_PinSet1(DIN_ConfigMap[DIN_CFG].dout_pin);
      break;

    case Oscillators:
      DOUT_PinSet1(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin);
      break;

    case Filter:
      DOUT_PinSet1(DIN_ConfigMap[DIN_FILTER].dout_pin);
      break;

    case Envelopes:
      DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin);
      break;

    case Keyboard:
      DOUT_PinSet1(DIN_ConfigMap[DIN_KEYBOARD].dout_pin);
      break;

    case Matrix:
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX].dout_pin);
      break;

    default:
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  These buttons keep track of which displayed paramter will be edited by the encoder
/////////////////////////////////////////////////////////////////////////////
void scan_edit_buttons()
{
  switch (SoftPanel.Button)
  {
    case SOFT_EDIT_1:
      SoftPanel.EditButton = Edit1;
      break;

    case SOFT_EDIT_2:
      SoftPanel.EditButton = Edit2;
      break;

    case SOFT_EDIT_3:
      SoftPanel.EditButton = Edit3;
      break;

    case SOFT_EDIT_4:
      SoftPanel.EditButton = Edit4;
      break;

    case SOFT_EDIT_5:
      SoftPanel.EditButton = Edit5;
      break;

    case SOFT_EDIT_INC:
      SoftPanel.EditButton = EditInc;
      break;

    case SOFT_EDIT_DEC:
      SoftPanel.EditButton = EditDec;
      break;

    case SOFT_EDIT_SHIFT:
      SoftPanel.EditButton = EditShift;
      break;

    case SOFT_EDIT_ALT :
      SoftPanel.EditButton = EditAlt;
      break;

    case SOFT_EDIT_ENC_CLIC:
      SoftPanel.EditButton = EditEncClic;
      break;

    default:
      break;
  }
  
#if(DEBUG_softpanel)
  Serial.print(F("scan_edit_buttons()/ SoftPanel.EditButton : ")); Serial.println(SoftPanel.EditButton, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Determine what mode were in and return it
/////////////////////////////////////////////////////////////////////////////
enum PanelMode_t getpanelmode()
{
  enum PanelMode_t mode = SoftPanel.Mode; // default

  switch (SoftPanel.Button)
  {
    case DIN_PATCH:  // set panel mode to patch
      mode = Patch;
      break;

    case DIN_EDIT:  // set panel mode to edit
      mode = Edit;
      break;

    case DIN_ARP: // // set panel mode to arp
      mode = Arp;
      break;

    case DIN_CFG:
      mode = Cfg;
      break;

    default:
      break;
  }

  switch (mode)
  {
    case Edit:
      if (SoftPanel.Page == SOFT_PAGE1)
      {
        // user is being prompted to select a edit mode below
        switch (SoftPanel.Button)
        {
          case DIN_OSCILLATORS:
            mode = Oscillators;
            break;

          case DIN_FILTER:
            mode = Filter;
            break;

          case DIN_ENVELOPES:
            mode = Envelopes;
            break;

          case DIN_KEYBOARD:
            mode = Keyboard;
            break;

          case DIN_MATRIX:
            mode = Matrix;
            break;

          default:
            break;
        }
      }
      break;

    default:
      break;
  }

  // if the mode changes, reset the page to zero
  if (SoftPanel.Mode != mode)
    SoftPanel.Page = 0;
  return mode;
}


/////////////////////////////////////////////////////////////////////////////
//  Sets an unsigned value with saturation at zero and max.
//  Returns 0 if no change was made
/////////////////////////////////////////////////////////////////////////////
char SetEncoderValue(unsigned char *value, unsigned char max)
{
  if ((*value > 0 && *value < max) || (*value == 0 && SoftPanel.EncoderValue == 1) || (*value == max && SoftPanel.EncoderValue < 0))
  {
    *value += SoftPanel.EncoderValue;

    return 1;
  }
  else
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//  Sets an unsigned value with saturation at zero and max.
//  Returns 0 if no change was made
/////////////////////////////////////////////////////////////////////////////
int SetEncoderValue2(unsigned int *value, unsigned int max)
{
  if ((*value > 0 && *value < max) || (*value == 0 && SoftPanel.EncoderValue == 1) || (*value == max && SoftPanel.EncoderValue < 0))
  {
    *value += SoftPanel.EncoderValue;

    return 1;
  }
  else
    return 0;
}
/////////////////////////////////////////////////////////////////////////////
//  Sets 7bit signed value with saturation
//  Returns 0 if no change was made
/////////////////////////////////////////////////////////////////////////////
char SetEncoder7Signed(unsigned char *value)
{
  if (*value > 0 && *value < 63) // either direction
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else if (*value == 0 && SoftPanel.EncoderValue == 1 )
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else if (*value == 0 && SoftPanel.EncoderValue == -1 )
  {
    *value = 127;
    return 1;
  }
  else if (*value == 63 && SoftPanel.EncoderValue == -1) // if max then allow down
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else if (*value > 65 && *value < 127) // either direction
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else if (*value == 127 && SoftPanel.EncoderValue == 1 )
  {
    *value = 0;
    return 1;
  }
  else if (*value == 127 && SoftPanel.EncoderValue - 1 )
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else if (*value == 65 && SoftPanel.EncoderValue == 1 )
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else if (*value > 127 && *value <= 255) // either direction // MAURO BUG solved
  {
    *value += SoftPanel.EncoderValue;
    return 1;
  }
  else
    return 0;
}
