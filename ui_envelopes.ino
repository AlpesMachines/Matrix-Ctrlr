//#include <cmios.h>
//#include "main.h"
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "midi.h"
#include "din.h"
#include "device.h"



typedef struct {
  unsigned char TriggerMode;
  unsigned char Delay;
  unsigned char Attack;
  unsigned char Decay;
  unsigned char Sustain;
  unsigned char Release;
  unsigned char Amplitude;
  unsigned char LfoMode;
  unsigned char ReleaseMode;
  unsigned char AmpVelocity;
} envelopemap_t;

const envelopemap_t EB_EnvelopeMap[NUM_ENVS] =
{
  {EB_ENV1_TRIGGERMODE, EB_ENV1_DELAY, EB_ENV1_ATTACK, EB_ENV1_DECAY, EB_ENV1_SUSTAIN, EB_ENV1_RELEASE, EB_ENV1_AMPLITUDE, EB_ENV1_LFOTRIGGERMODE, EB_ENV1_RELEASEMODE, EB_ENV1_AMPBYVELO },
  {EB_ENV2_TRIGGERMODE, EB_ENV2_DELAY, EB_ENV2_ATTACK, EB_ENV2_DECAY, EB_ENV2_SUSTAIN, EB_ENV2_RELEASE, EB_ENV2_AMPLITUDE, EB_ENV2_LFOTRIGGERMODE, EB_ENV2_RELEASEMODE, EB_ENV2_AMPBYVELO },
  {EB_ENV3_TRIGGERMODE, EB_ENV3_DELAY, EB_ENV3_ATTACK, EB_ENV3_DECAY, EB_ENV3_SUSTAIN, EB_ENV3_RELEASE, EB_ENV3_AMPLITUDE, EB_ENV3_LFOTRIGGERMODE, EB_ENV3_RELEASEMODE, EB_ENV3_AMPBYVELO },
};

/*
    ENVELOPES PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    EV3 ATK DKY  SUS REL
        63  63   63   63

    ENVELOPES PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    ENV DLY AMP REL-MODE
     1  63  63  DADR+FRE

    ENVELOPES PAGE 3
    []  []   [ ]  []  []
    12345678901234567890
    ENV TRIGGERMODE  LFO
     1  RESET+MLTI  GATE

*/
void UI_Display_Envelopes()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1: ///// ENV 3 ///////////
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("EV3 ATK DEC  SUS REL"));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4);
      LCD_PrintBCD2(EditBuffer[device][EB_ENV3_ATTACK]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8);
      LCD_PrintBCD2(EditBuffer[device][EB_ENV3_DECAY]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 13);
      LCD_PrintBCD2(EditBuffer[device][EB_ENV3_SUSTAIN]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 18);
      LCD_PrintBCD2(EditBuffer[device][EB_ENV3_RELEASE]);
      break;

    case SOFT_PAGE2:
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("ENV DLY AMP REL-MODE"));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 1);
      LCD_PrintBCD1(SoftPanel.Selected_Env + 1);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4);
      LCD_PrintBCD2(EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].Delay]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 8);
      LCD_PrintBCD2(EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].Amplitude]);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 12);
      //LCD_PrintCString(EnvelopeReleaseModes[EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].ReleaseMode]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(EnvelopeReleaseModes[EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].ReleaseMode]]))));
      break;

    // ENV TRIGGERMODE  LFO
    //  1  RESET+MLTI  GATE
    case SOFT_PAGE3:
      if (SoftPanel.IsNewPage)
      {
        LCD_Clear();
        LCD_CursorSet(0 + LCD_Offset);
        LCD_PrintCString(F("ENV TRIGGERMODE  LFO"));
      }
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 1);
      LCD_PrintBCD1(SoftPanel.Selected_Env + 1);
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 4);
      //	LCD_PrintCString(EnvelopeTriggerModes[EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].TriggerMode]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(EnvelopeTriggerModes[EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].TriggerMode]]))));
      LCD_CursorSet(LCD_SECONDROWSTARTPOSITION + 16);
      //	LCD_PrintCString(ENV_LFOTriggerModes[EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].LfoMode]]);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(ENV_LFOTriggerModes[EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].LfoMode]]))));
      break;
  }
}

/*
    ENVELOPES PAGE 1
    []  []   [ ]  []  []
    12345678901234567890
    EV3 ATK DKY  SUS REL
        63  63   63   63

    ENVELOPES PAGE 2
    []  []   [ ]  []  []
    12345678901234567890
    ENV DLY AMP REL-MODE
     1  63  63  DADR+FRE

    ENVELOPES PAGE 3
    []  []   [ ]  []  []
    12345678901234567890
    ENV TRIGGERMODE  LFO
     1  RESET+MLTI  GATE

*/
void UI_Handle_Envelopes()
{
  switch (SoftPanel.Page)
  {
    case SOFT_PAGE1:  // envelope 3 adsr
      if (SoftPanel.Button == DIN_PAGE)
        SoftPanel.Page = SOFT_PAGE2;

      // handle page 1 encoder changes
      if (SoftPanel.EncoderValue != 0)
      {
        switch (SoftPanel.EditButton)
        {
          case Edit2: // env3 attack
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[ENV3].Attack], 63);
            MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_ATTACK, EditBuffer[device][EB_EnvelopeMap[ENV3].Attack], true);
            break;

          case Edit3: // env3 decay
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[ENV3].Decay], 63);
            MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_DECAY, EditBuffer[device][EB_EnvelopeMap[ENV3].Decay], true);
            break;

          case Edit4: // env3 sustain
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[ENV3].Sustain], 63);
            MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_SUSTAIN, EditBuffer[device][EB_EnvelopeMap[ENV3].Sustain], true);
            break;

          case Edit5: // env3 release
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[ENV3].Release], 63);
            MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_RELEASE, EditBuffer[device][EB_EnvelopeMap[ENV3].Release], true);
            break;

          default:
            break;
        } // end page 1
      }

      break; // end page 1

    case SOFT_PAGE2:
      if (SoftPanel.Button == DIN_PAGE)
        SoftPanel.Page = SOFT_PAGE3;
      else if (SoftPanel.Button == SOFT_EDIT_1)
      {
        SoftPanel.Selected_Env++;
        if (SoftPanel.Selected_Env > ENV3)
          SoftPanel.Selected_Env = ENV1;
      }

      // handle page 1 encoder changes
      if (SoftPanel.EncoderValue != 0)
      {
        switch (SoftPanel.EditButton)
        {
          case Edit2:  // envelope delay
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].Delay], 63);
            if (SoftPanel.Selected_Env == ENV1)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV1_DELAY, EditBuffer[device][EB_EnvelopeMap[ENV1].Delay], true);
            else if (SoftPanel.Selected_Env == ENV2)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV2_DELAY, EditBuffer[device][EB_EnvelopeMap[ENV2].Delay], true);
            else MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_DELAY, EditBuffer[device][EB_EnvelopeMap[ENV3].Delay], true);
            break;

          case Edit3:  // envelope amplitude
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].Amplitude], 63);
            if (SoftPanel.Selected_Env == ENV1)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV1_AMPLITUDE, EditBuffer[device][EB_EnvelopeMap[ENV1].Amplitude], true);
            else if (SoftPanel.Selected_Env == ENV2)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV2_AMPLITUDE, EditBuffer[device][EB_EnvelopeMap[ENV2].Amplitude], true);
            else MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_AMPLITUDE, EditBuffer[device][EB_EnvelopeMap[ENV3].Amplitude], true);
            break;

          case Edit4:  // envelope release mode
          case Edit5:  // envelope release mode
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].ReleaseMode], 3);
            if (SoftPanel.Selected_Env == ENV1)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV1_RELEASEMODE, EditBuffer[device][EB_EnvelopeMap[ENV1].ReleaseMode], true);
            else if (SoftPanel.Selected_Env == ENV2)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV2_RELEASEMODE, EditBuffer[device][EB_EnvelopeMap[ENV2].ReleaseMode], true);
            else MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_RELEASEMODE, EditBuffer[device][EB_EnvelopeMap[ENV3].ReleaseMode], true);
            break;

          default: break;

        } // end switch button
      } // endif encodervalue

      break; // end page 2

    case SOFT_PAGE3:
      if (SoftPanel.Button == DIN_PAGE)
        SoftPanel.Page = SOFT_PAGE1;
      else if (SoftPanel.Button == SOFT_EDIT_1)
      {
        SoftPanel.Selected_Env++;
        if (SoftPanel.Selected_Env > ENV3)
          SoftPanel.Selected_Env = ENV1;
      }

      // handle page 1 encoder changes
      if (SoftPanel.EncoderValue != 0)
      {
        switch (SoftPanel.EditButton)
        {
          case Edit2:  // envelope trigger mode
          case Edit3:
          case Edit4:
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].TriggerMode], 7);
            if (SoftPanel.Selected_Env == ENV1)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV1_TRIGGERMODE, EditBuffer[device][EB_EnvelopeMap[ENV1].TriggerMode], true);
            else if (SoftPanel.Selected_Env == ENV2)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV2_TRIGGERMODE, EditBuffer[device][EB_EnvelopeMap[ENV2].TriggerMode], true);
            else MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_TRIGGERMODE, EditBuffer[device][EB_EnvelopeMap[ENV3].TriggerMode], true);
            break;

          case Edit5:  // envelope lfo trigger mode
            SetEncoderValue(&EditBuffer[device][EB_EnvelopeMap[SoftPanel.Selected_Env].LfoMode], 2);
            if (SoftPanel.Selected_Env == ENV1)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV1_LFOTRIGGERMODE, EditBuffer[device][EB_EnvelopeMap[ENV1].LfoMode], true);
            else if (SoftPanel.Selected_Env == ENV2)
              MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV2_LFOTRIGGERMODE, EditBuffer[device][EB_EnvelopeMap[ENV2].LfoMode], true);
            else MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_ENV3_LFOTRIGGERMODE, EditBuffer[device][EB_EnvelopeMap[ENV3].LfoMode], true);
            break;

          default:
            break;
        } // end switch edit button
      } // end encodervalue
      break; // end page 2
  } // end switch page
}


