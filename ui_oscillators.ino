//#include <cmios.h>
//#include "main.h"
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
#include "device.h"


typedef struct {
  unsigned char Freq;         // EB_OSCx_FREQ
  unsigned char WaveShape;    // EB_OSCx_WAVESHAPE
  unsigned char PulseWidth;   // EB_OSCx_PULSEWIDTH
  unsigned char LeverMod;     // EB_OSCx_LEVERMOD
  unsigned char Waveform;     // EB_OSCx_WAVEFORM
  unsigned char KeyboardMod;  // EB_OSCx_KEYBOARDMOD
  unsigned char Click;        // EB_OSCx_CLICK

} oscillatormap_t;

const oscillatormap_t EB_OscillatorMap[NUM_OSCILLATORS] =
{
  {EB_OSC1_FREQ, EB_OSC1_WAVESHAPE, EB_OSC1_PULSEWIDTH, EB_OSC1_LEVERMOD, EB_OSC1_WAVEFORM, EB_OSC1_KEYBOARDMOD, EB_OSC1_CLICK },
  {EB_OSC2_FREQ, EB_OSC2_WAVESHAPE, EB_OSC2_PULSEWIDTH, EB_OSC2_LEVERMOD, EB_OSC2_WAVEFORM, EB_OSC2_KEYBOARDMOD, EB_OSC2_CLICK },
};

typedef struct {
  unsigned char TriggerMode;      // EB_LFOx_TRIGGERMODE
  unsigned char LagEnable;        // EB_LFOx_LAGENABLE
  unsigned char RetriggerPoint;   // EB_LFOx_RETRIGGERPOINT
  unsigned char SampledSource;    // EB_LFOx_SAMPLEDSOURCE
  unsigned char AmpModulation;    // EB_LFOx_AMPMODULATION
  unsigned char SpeedModulation;  // EB_LFOx_SPEEDMODULATION

} lfomap_t;

const lfomap_t EB_LfoMap[NUM_LFOS] =
{
  {EB_LFO1_TRIGGERMODE, EB_LFO1_LAGENABLE, EB_LFO1_RETRIGGERPOINT, EB_LFO1_SAMPLEDSOURCE, EB_LFO1_AMPMODULATION, EB_LFO1_SPEEDMODULATION },
  {EB_LFO2_TRIGGERMODE, EB_LFO2_LAGENABLE, EB_LFO2_RETRIGGERPOINT, EB_LFO2_SAMPLEDSOURCE, EB_LFO2_AMPMODULATION, EB_LFO2_SPEEDMODULATION },
};

/*
    OSCILLATORS PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    OSC BEND MODW PORT KBD
    1   OFF  OFF  OFF


    OSCILLATORS PAGE 2 (LFO)
    []  []   [ ]  []  []
    12345678901234567890
    LFO RMP1>AMP PRS>SPD    (LFO RMP2>AMP KBD>SPD)
    1  +63       +63


    OSCILLATORS PAGE 3 (LFO)
    []  []   [ ]  []  []
    12345678901234567890
    LFO MDE SRCE LAG RTP
    1   OFF ENV1 OFF  63
*/

void UI_Display_Oscillators()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1: // first page
//      if (SoftPanel.IsNewPage)
//      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        if (SoftPanel.Selected_Osc)
          LCD_PrintCString(F("DCO BND VIB PORT KBD")); // DCO2
        else
          LCD_PrintCString(F("DCO BND VIB PORT    ")); // DCO1
//      }

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);

      // set up a default display
      if (SoftPanel.Selected_Osc)
        LCD_PrintCString(F("1   OFF OFF OFF  OFF")); // DCO2
      else
        LCD_PrintCString(F("1   OFF OFF OFF     ")); //DCO1

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintBCD2(SoftPanel.Selected_Osc + 1);

      if ((1 << 0) & EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].LeverMod]) // is pitch bend bit 0 set?
      {
        LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
        LCD_PrintCString(F("ON "));
      }

      if ((1 << 1) & EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].LeverMod]) // is vibrato bit 1 set?
      {
        LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8 + LCD_Offset);
        LCD_PrintCString(F("ON "));
      }

      if ((1 << 0) & EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].KeyboardMod]) // is portamento bit 0 set?
      {
        LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 12 + LCD_Offset);
        LCD_PrintCString(F("ON "));
      }

      if ((1 << 1) & EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].KeyboardMod]) // is portamento bit 1 set?
      {
        LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 17 + LCD_Offset);
        LCD_PrintCString(F("ON "));
      }
      break;

    case SOFT_PAGE2:
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        //LFO RMP1>AMP PRS>SPD
        if (SoftPanel.Selected_LFO == LFO1)
          LCD_PrintCString(F("LFO RMP1"));
        else
          LCD_PrintCString(F("LFO RMP2"));

        LCD_PrintChar(LCDCHAR_ARROW_RIGHT);

        if (SoftPanel.Selected_LFO == LFO1)
          LCD_PrintCString(F("AMP PRS"));
        else
          LCD_PrintCString(F("AMP KBD"));

        LCD_PrintChar(LCDCHAR_ARROW_RIGHT);
        LCD_PrintCString(F("SPD"));
      }


      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);

      // set up a default display
      LCD_PrintBCD1(SoftPanel.Selected_LFO + 1);

      // lfo amp modulation
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
      LCD_Display7BitSigned(EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].AmpModulation]);

      // lfo speed modulation
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 13 + LCD_Offset);
      LCD_Display7BitSigned(EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SpeedModulation]);

      break;

    case SOFT_PAGE3:

      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("LFO MDE SRCE LAG RTG"));
      }

      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + LCD_Offset);
      LCD_PrintBCD1(SoftPanel.Selected_LFO + 1);

      // lfo trigger mode
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4 + LCD_Offset);
      //      LCD_PrintCString(LFO_Trigger_Mode_Names[EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].TriggerMode]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(LFO_Trigger_Mode_Names[EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].TriggerMode]]))));

      // lfo sample source
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8 + LCD_Offset);
      //      LCD_PrintCString(MMOD_Sources[EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SampledSource]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(MMOD_Sources[EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SampledSource]]))));

      // lfo lag enable
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 13);
      if (EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].LagEnable] == 0)
        LCD_PrintCString(F("OFF"));
      else
        LCD_PrintCString(F(" ON"));

      // lfo retrigger point
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 18 + LCD_Offset);
      LCD_PrintBCD2(EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].RetriggerPoint]);

      break;
  }
}

/*
    OSCILLATORS PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    OSC BEND MODW PORTA
    1   OFF  OFF  OFF
*/
void UI_Handle_Oscillators()
{
  unsigned char param;
  unsigned char value;
  param = 0;
  value = 0;

  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:

      switch (SoftPanel.Button)
      {
        case SOFT_EDIT_1:
          SoftPanel.Selected_Osc = SoftPanel.Selected_Osc == 0 ? 1 : 0;
          break;

        case SOFT_EDIT_2: // pitch bend, toggle on/off, levermod
          EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].LeverMod] ^= (1 << 0); // toggle bit 0
          param = SoftPanel.Selected_Osc == 0 ? SX_OSC1_LEVERMOD : SX_OSC2_LEVERMOD;
          value = EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].LeverMod];
          break;

        case SOFT_EDIT_3: // mod wheel, toggle on/off, levermod
          EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].LeverMod] ^= (1 << 1); // toggle bit 1
          param = SoftPanel.Selected_Osc == 0 ? SX_OSC1_LEVERMOD : SX_OSC2_LEVERMOD;
          value = EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].LeverMod];
          break;

        case SOFT_EDIT_4: // portamento, toggle on/off, keyboardmod
          EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].KeyboardMod] ^= (1 << 0); // toggle bit 0
          param = SoftPanel.Selected_Osc == 0 ? SX_OSC1_KEYBOARDMOD : SX_OSC2_KEYBOARDMOD;
          value = EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].KeyboardMod];
          break;

        case SOFT_EDIT_5: // keybd tracking, toggle on/off, keyboardmod
          if (SoftPanel.Selected_Osc) // only DCO2
          {

            EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].KeyboardMod] ^= (1 << 1); // toggle bit 1
            param = SoftPanel.Selected_Osc == 0 ? SX_OSC1_KEYBOARDMOD : SX_OSC2_KEYBOARDMOD;
            value = EditBuffer[device][EB_OscillatorMap[SoftPanel.Selected_Osc].KeyboardMod];
          }
          break;

        case DIN_PAGE:
          SoftPanel.Page = SOFT_PAGE2;
          break;

      }

      if (param > 0)
        MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);

      break; // end soft page 1

    case SOFT_PAGE2:

      switch (SoftPanel.Button) {
        case SOFT_EDIT_1:
          SoftPanel.Selected_LFO = SoftPanel.Selected_LFO == 0 ? 1 : 0;
          RefreshSoftPanel = 1;
          break;

        case DIN_PAGE:
          SoftPanel.Page = SOFT_PAGE3;
          break;
      }

      if (SoftPanel.EncoderValue != 0)
      {
        switch (SoftPanel.EditButton)
        {
          case Edit2:
            // lfo amp modulation
            SetEncoder7Signed(&EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].AmpModulation]);
            param = (SoftPanel.Selected_LFO == LFO1) ? SX_LFO1_AMPMODULATION : SX_LFO2_AMPMODULATION;

            MIDI_SendDelayedVoiceParam(param, EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].AmpModulation]);

            break;

          case Edit4:
            // lfo speed modulation
            SetEncoder7Signed(&EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SpeedModulation]);
            param = (SoftPanel.Selected_LFO == LFO1) ? SX_LFO1_SPEEDMODULATION : SX_LFO2_SPEEDMODULATION;
            MIDI_SendDelayedVoiceParam(param, EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SpeedModulation]);
            break;

          default:
            break;


        } // end switch

      } // end if encoderval

      break; // end page 2


    /*
            OSCILLATORS PAGE 3 (LFO)
            []  []   [ ]  []  []
            12345678901234567890
            LFO MDE SRCE LAG RTP
            1   OFF ENV1 OFF  63
    */
    case SOFT_PAGE3:

      switch (SoftPanel.Button)
      {
        case SOFT_EDIT_1:  // selected lfo

          SoftPanel.Selected_LFO = SoftPanel.Selected_LFO == 0 ? 1 : 0;

          break;

        case SOFT_EDIT_2: // lfo trigger mode

          // increment the value
          value = (SoftPanel.Selected_LFO == LFO1) ? EditBuffer[device][EB_LFO1_TRIGGERMODE] : EditBuffer[device][EB_LFO2_TRIGGERMODE];
          value++;
          if (value > 3)
            value = 0;

          // update the edit buffer
          if (SoftPanel.Selected_LFO == LFO1)
            EditBuffer[device][EB_LFO1_TRIGGERMODE] = value;
          else
            EditBuffer[device][EB_LFO2_TRIGGERMODE] = value;

          // send midi
          param = (SoftPanel.Selected_LFO == LFO1) ? SX_LFO1_TRIGGERMODE : SX_LFO2_TRIGGERMODE;
          MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);

          break;

        case SOFT_EDIT_4: // lfo lag enable

          if (SoftPanel.Selected_LFO == LFO1)
          {
            param = SX_LFO1_LAGENABLE;
            EditBuffer[device][EB_LFO1_LAGENABLE] = (EditBuffer[device][EB_LFO1_LAGENABLE] == ON) ? OFF : ON;
            value = EditBuffer[device][EB_LFO1_LAGENABLE];
          }
          else
          {
            param = SX_LFO2_LAGENABLE;
            EditBuffer[device][EB_LFO2_LAGENABLE] = (EditBuffer[device][EB_LFO2_LAGENABLE] == ON) ? OFF : ON;
            value = EditBuffer[device][EB_LFO2_LAGENABLE];
          }

          MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, true);

          break;


        case DIN_PAGE:
          SoftPanel.Page = SOFT_PAGE1;
          break;

      } // end switch button

      if (SoftPanel.EncoderValue != 0)
      {
        switch (SoftPanel.EditButton)
        {
          case Edit3: // lfo sample source

            SetEncoderValue(&EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SampledSource], 20);
            param = (SoftPanel.Selected_LFO == LFO1) ? SX_LFO1_SAMPLEDSOURCE : SX_LFO2_SAMPLEDSOURCE;

            MIDI_SendVoiceParam(INTERFACE_SERIAL, param, EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].SampledSource], true);

            break;

          case Edit5: // lfo retrigger point

            SetEncoderValue(&EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].RetriggerPoint], 63);
            param = (SoftPanel.Selected_LFO == LFO1) ? SX_LFO1_RETRIGGERPOINT : SX_LFO2_RETRIGGERPOINT;

            MIDI_SendVoiceParam(INTERFACE_SERIAL, param, EditBuffer[device][EB_LfoMap[SoftPanel.Selected_LFO].RetriggerPoint], true);

            break;
            
          default:
            break;
        }
      }
      break;
  }
}


