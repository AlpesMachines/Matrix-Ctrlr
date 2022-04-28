#include <avr/pgmspace.h>
#include "chaosmatrix.h"
#include "pots.h"
#include "lcd.h"
#include "define.h"

// for delayed pot midi transmit
unsigned int transmit_counter;
unsigned char is_transmit_delayed;

unsigned char lastparam;        // last patch parameter number
unsigned char lastvalue;        // signed converted value
unsigned char lastbyteindex;    // edit buffer index of last patch parameter

unsigned char last_ain_pin;

unsigned char getparam(unsigned char pin);
unsigned char getvalue(unsigned char pin);


/////////////////////////////////////////////////////////////////////////////
//  Transmit MIDI and update the edit buffer on the last button pushed
/////////////////////////////////////////////////////////////////////////////
void LivePanel_DinHandler(unsigned char pin)
{
  unsigned char dout_pin;
  dout_pin = DIN_ConfigMap[pin].dout_pin;

  switch (pin)
  {
    //////////// DCO1 /////////
    case DIN_DCO1_CLICK: // click
      // toggle its on/off state at the bit level
      EditBuffer[device][EB_OSC1_CLICK] ^= (1 << BIT0);
      DOUT_PinSet(dout_pin, EditBuffer[device][EB_OSC1_CLICK]);
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC1_CLICK, EditBuffer[device][EB_OSC1_CLICK], mThru_XCc);
      break;

    case DIN_DCO1_WAVE: // wave
      // toggle its on/off state at the bit level
      EditBuffer[device][EB_OSC1_WAVEFORM] ^= (1 << BIT1);
      //val = EditBuffer[device][EB_OSC1_WAVEFORM];
      // set the LED to the value of the bit change above
      DOUT_PinSet(dout_pin, (EditBuffer[device][EB_OSC1_WAVEFORM] & (1 << BIT1)) != 0);
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC1_WAVEFORM, EditBuffer[device][EB_OSC1_WAVEFORM], mThru_XCc);
      break;

    case DIN_DCO1_PULSE: // pulse
      //Osc_Pulse_Modulate[DCO1] = 0;
      // toggle its on/off state at the bit level
      EditBuffer[device][EB_OSC1_WAVEFORM] ^= (1 << BIT0);
      //val = EditBuffer[device][EB_OSC1_WAVEFORM];
      // set the LED to the value of the bit change above
      DOUT_PinSet(dout_pin, (EditBuffer[device][EB_OSC1_WAVEFORM] & (1 << BIT0)) != 0);
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC1_WAVEFORM, EditBuffer[device][EB_OSC1_WAVEFORM], mThru_XCc);
      break;

    //////////// DCO2 /////////
    case DIN_DCO2_CLICK: // click only
      // toggle its on/off state at the bit level
      EditBuffer[device][EB_OSC2_CLICK] ^= (1 << BIT0);
      DOUT_PinSet(dout_pin, EditBuffer[device][EB_OSC2_CLICK]);
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC2_CLICK, EditBuffer[device][EB_OSC2_CLICK], mThru_XCc);
      break;

    case DIN_DCO2_WAVE: // wave
      if (EditBuffer[device][EB_OSC2_WAVEFORM] > 3) // if we have noisebit set (EB=4 or EB=5 or EB=6) -> quit
      {
        return;
      }
      else
      {
        // toggle its on/off state at the bit level
        EditBuffer[device][EB_OSC2_WAVEFORM] ^= (1 << BIT1);
        // set the LED to the value of the bit change above
        DOUT_PinSet(dout_pin, (EditBuffer[device][EB_OSC2_WAVEFORM] & (1 << BIT1)) != 0);
        MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC2_WAVEFORM, EditBuffer[device][EB_OSC2_WAVEFORM], mThru_XCc);
      }
      break;

    case DIN_SW9:
    case DIN_DCO2_PULSE: // pulse & noise here
      // 3 states : 0 = wav off, 1 = wav on, 2 = noise on
      // https://www.arduino.cc/en/Reference/BitSet
      // https://www.arduino.cc/en/Reference/BitClear
      switch (EditBuffer[device][EB_OSC2_WAVEFORM])
      {
        // if it is off (000) then pulse = 1 (bit0)
        case 0: bitSet(EditBuffer[device][EB_OSC2_WAVEFORM], BIT0); break;

        // if it is pulse (001) then it becomes noise (bit2=1)
        case 1: bitSet(EditBuffer[device][EB_OSC2_WAVEFORM], BIT2); break;

        // if it is wave (010) then it becomes wave+pulse (bit0=1)
        case 2: bitSet(EditBuffer[device][EB_OSC2_WAVEFORM], BIT0); break;

        // if it wave+pulse (011) then it becomes noise (bit2=1)
        case 3: bitSet(EditBuffer[device][EB_OSC2_WAVEFORM], BIT2); break;

        // if it is noise (100 or 110 or 111) then set bit noise to zero (bit2 = 0) and pulse to zero(bit0=0-
        case 4: bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT2); bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT0); break;
        case 5: bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT2); bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT0); break;
        case 6: bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT2); bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT0); break;
        case 7: bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT2); bitClear(EditBuffer[device][EB_OSC2_WAVEFORM], BIT0); break;
        default: break;
      }
      //      // toggle its on/off state at the bit level
      //      EditBuffer[device][EB_OSC2_WAVEFORM] ^= (1 << BIT0);
      //      // set the LED to the value of the bit change above
      DOUT_PinSet(dout_pin, (EditBuffer[device][EB_OSC2_WAVEFORM] & (1 << BIT0)) != 0);
      DOUT_PinSet(DOUT_DCO2_WAVE, (EditBuffer[device][EB_OSC2_WAVEFORM] & (1 << BIT1)) != 0);
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC2_WAVEFORM, EditBuffer[device][EB_OSC2_WAVEFORM], mThru_XCc);
      break;



    case DIN_DCO_SYNC: //DCO1 Sync Toggle
      EditBuffer[device][EB_OSC_SYNCMODE]++;
      if (EditBuffer[device][EB_OSC_SYNCMODE] > 3) {
        EditBuffer[device][EB_OSC_SYNCMODE] = 0;
      }
      // set the LED to the value if we are > 0
      DOUT_PinSet(dout_pin, (EditBuffer[device][EB_OSC_SYNCMODE] > 0) ? DIN_STATE_ON : DIN_STATE_OFF);
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_OSC_SYNCMODE, EditBuffer[device][EB_OSC_SYNCMODE], mThru_XCc);
      break;


    case DIN_LFO1_WAVESELECT: // LFO Wave Select
      EditBuffer[device][EB_LFO1_WAVESHAPE]++;
      if (EditBuffer[device][EB_LFO1_WAVESHAPE] > 6)
      {
        EditBuffer[device][EB_LFO1_WAVESHAPE] = 0;
      }
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_LFO1_WAVESHAPE, EditBuffer[device][EB_LFO1_WAVESHAPE], mThru_XCc);
      break;

    case DIN_LFO2_WAVESELECT: // LFO Wave Select
      EditBuffer[device][EB_LFO2_WAVESHAPE]++;
      if (EditBuffer[device][EB_LFO2_WAVESHAPE] > 6)
      {
        EditBuffer[device][EB_LFO2_WAVESHAPE] = 0;
      }
      MIDI_SendVoiceParam(INTERFACE_SERIAL, SX_LFO2_WAVESHAPE, EditBuffer[device][EB_LFO2_WAVESHAPE], mThru_XCc);
      break;


    case DIN_PATCH:
      SendEditBuffer(device, INTERFACE_SERIAL);
      break;

    default:
      break;
  }
  elapsedTime = 0; // for the RestTmpMsg function

}

/////////////////////////////////////////////////////////////////////////////
//  Display info on the last button pushed
/////////////////////////////////////////////////////////////////////////////
void LivePanel_DisplayDin (unsigned char pin) // , unsigned char value
{
  LCD_Clear(); // risque de foutre la merde dans les menus EDIT : pas du tout :)
  RefreshSoftPanel = 1; // soft panel display may have been overwritten, make sure it completely refreshes on a button click

  switch (pin) {

    case DIN_LFO1_WAVESELECT: // LFO1 Wave Select Pin
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_LFO1_WAVESHAPE] + 1, UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_LFO1_WAVESHAPE] + 1, UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(LFO_Wave_Names[EditBuffer[device][EB_LFO1_WAVESHAPE]]))));
      break;

    case DIN_LFO2_WAVESELECT: // LFO2 Wave Select Pin
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_LFO2_WAVESHAPE] + 1, UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_LFO2_WAVESHAPE] + 1, UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(LFO_Wave_Names[EditBuffer[device][EB_LFO2_WAVESHAPE]]))));
      break;

    case DIN_DCO_SYNC: // DCO Sync Mode
      //      LCD_DisplayParamDescription(DIN_ConfigMap[pin].description);
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_OSC_SYNCMODE], UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_OSC_SYNCMODE], UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(SyncModes[EditBuffer[device][EB_OSC_SYNCMODE]]))));
      break;


    case DIN_DCO1_WAVE: // DCO1
    case DIN_DCO1_PULSE:
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_OSC1_WAVEFORM], UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_OSC1_WAVEFORM], UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DCO_Waves_Name[EditBuffer[device][EB_OSC1_WAVEFORM]]))));
      break;

    case DIN_SW9:
    case DIN_DCO2_WAVE: // DCO2
    case DIN_DCO2_PULSE:
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_OSC2_WAVEFORM], UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_OSC2_WAVEFORM], UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DCO_Waves_Name[EditBuffer[device][EB_OSC2_WAVEFORM]]))));
      break;

    case DIN_DCO1_CLICK: // DCO1 click
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_OSC1_CLICK], UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_OSC1_CLICK], UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DCO_Clic_State[EditBuffer[device][EB_OSC1_CLICK]]))));
      break;

    case DIN_DCO2_CLICK: // DCO2 Click
      //      LCD_DisplayParamDescription(DIN_ConfigMap[pin].description);
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      LCD_DisplayEditBufferOrig(EditBufferOrig[EB_OSC2_CLICK], UNSIGNED7);
      LCD_DisplayParamValue(EditBuffer[device][EB_OSC2_CLICK], UNSIGNED7);
      lcd.setCursor(4, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DCO_Clic_State[EditBuffer[device][EB_OSC2_CLICK]]))));
      break;

    default:
      lcd.setCursor(4, 0);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription [pin]))));
      break;

  }
  //  UpdateDinStates(); // NE PAS METTRE PLANTE LE TRUC
}

/////////////////////////////////////////////////////////////////////////////
//  Blink the LFO LEDs to a close aproximation of their current LFO parameter speed
/////////////////////////////////////////////////////////////////////////////
void LivePanel_BlinkLFOs(void)
{
  static unsigned char lfo1_blinkcounter;
  static unsigned char lfo2_blinkcounter;

  if (inTest && booting) return;

  // lfo 1
  if (lfo1_blinkcounter < 62 - EditBuffer[device][EB_LFO1_SPEED])
  {
    lfo1_blinkcounter++;
  }
  else
  {
    DOUT_PinSet(DOUT_LFO1_WAVESELECT, (DOUT_PinGet(DOUT_LFO1_WAVESELECT) == DIN_STATE_ON) ? DIN_STATE_OFF : DIN_STATE_ON);
    lfo1_blinkcounter = 0;
  }

  // lfo 2
  if (lfo2_blinkcounter < 62 - EditBuffer[device][EB_LFO2_SPEED])
  {
    lfo2_blinkcounter++;
  }
  else
  {
    DOUT_PinSet(DOUT_LFO2_WAVESELECT, (DOUT_PinGet(DOUT_LFO2_WAVESELECT) == DIN_STATE_ON) ? DIN_STATE_OFF : DIN_STATE_ON);
    lfo2_blinkcounter = 0;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  Blink LEDs. Called periodically from main::Timer()
/////////////////////////////////////////////////////////////////////////////
void LivePanel_BlinkLEDs(void)
{
  static unsigned char led_blinkcounter;
  static unsigned char sync_blinkcounter;
  static unsigned char F1F2_blinkcounter;
  static bool blink;

  if (inTest && booting) return;

  // Is there a MIDI message incoming ?
  // MIDI ACTIVITY LED On/Off : OK :)
  if (MIDI_Incoming)
  {
    DOUT_PinSet(DOUT_ACTIVITY, DIN_STATE_ON);
    MIDI_Incoming = 0;
  }
  else if (DOUT_PinGet(DOUT_ACTIVITY) == DIN_STATE_ON)
  {
    DOUT_PinSet(DOUT_ACTIVITY, DIN_STATE_OFF);
  }
  else
  {
    // nope
  }

//  --> managed in the interrupt now (fw 1.12)
// // external trigger TR707 
//  if (trigger && (systmClock == TRGCLK))
//  {
//    DOUT_PinSet(DOUT_ACTIVITY2, DIN_STATE_ON);
//    //DOUT_PinSet(DOUT_ARP, DIN_STATE_ON); // nuit à la compréhension
//
//    trigger = 0;
//  }
//  else if ((DOUT_PinGet(DOUT_ACTIVITY2) == DIN_STATE_ON) && (systmClock == TRGCLK))
//  {
//    DOUT_PinSet(DOUT_ACTIVITY2, DIN_STATE_OFF);
//    //DOUT_PinSet(DOUT_ARP, DIN_STATE_OFF); // nuit à compréhension
//
//  }
//  else
//  {
//    // nope
//  }

  // SHIFT BUTTON LED (On if Pressed, Off when released)
  if (Shift)
  {
    DOUT_PinSet(DOUT_SHIFT, DIN_STATE_ON);
  }
  else if (DOUT_PinGet(DOUT_SHIFT) == DIN_STATE_ON)
  {
    DOUT_PinSet(DOUT_SHIFT, DIN_STATE_OFF);
  }

  if (Alt)
  {
    DOUT_PinSet(DOUT_ALT, DIN_STATE_ON);
  }
  else if (DOUT_PinGet(DOUT_ALT) == DIN_STATE_ON)
  {
    DOUT_PinSet(DOUT_ALT, DIN_STATE_OFF);
  }

  // blink the LEDs below at the same speed
  if (led_blinkcounter > LED_BLINK_SPEED)
  {
    if (EditBuffer[device][EB_OSC2_WAVEFORM] > 3)
    {
      DOUT_PinSet(DOUT_DCO2_PULSE, (DOUT_PinGet(DOUT_DCO2_PULSE) == DIN_STATE_ON) ? DIN_STATE_OFF : DIN_STATE_ON);
      DOUT_PinSet(DOUT_DCO2_WAVE, (DOUT_PinGet(DOUT_DCO2_WAVE) == DIN_STATE_ON) ? DIN_STATE_OFF : DIN_STATE_ON);
    }

    // blink ARP led if sequenciate or arpegiate
    if (ui_seqPlay || router_arp_tag)
    {
      DOUT_PinSet(DOUT_ARP, (DOUT_PinGet(DOUT_ARP) == DIN_STATE_ON) ? DIN_STATE_OFF : DIN_STATE_ON);
    }

    led_blinkcounter = 0;
  }
  led_blinkcounter++;


  if (sync_blinkcounter > (LED_BLINK_SPEED >> (EditBuffer[device][EB_OSC_SYNCMODE] - 2)))
  {
    // blink SYNC Led is Hard or Harder :
    if (EditBuffer[device][EB_OSC_SYNCMODE] > 1)
    {
      DOUT_PinSet(DOUT_OSC_SYNC, (DOUT_PinGet(DOUT_OSC_SYNC) == DIN_STATE_ON) ? DIN_STATE_OFF : DIN_STATE_ON);
    }

    sync_blinkcounter = 0;
  }
  ++sync_blinkcounter;


  if (F1F2_blinkcounter > (arp_div_index)) // arp_div_index instead of F1F2_BLINK_SPEED
  {
    blink = !blink;

    // blink F1 led if arp play & Hold
    if (router_arp_tag && !ui_aHold)
      DOUT_PinSet(DOUT_F1, DIN_STATE_ON);
    else if (router_arp_tag && ui_aHold)
      DOUT_PinSet(DOUT_F1, blink);
    else
      DOUT_PinSet(DOUT_F1, DIN_STATE_OFF);

    // blink F2 led if seqplay & toggle
    if (ui_seqPlay && !ui_toggleSeq)
      DOUT_PinSet(DOUT_F2, DIN_STATE_ON);
    else if (ui_seqPlay && ui_toggleSeq)
      DOUT_PinSet(DOUT_F2, blink);

    else
      DOUT_PinSet(DOUT_F2, DIN_STATE_OFF);

    F1F2_blinkcounter = 0;
  }
  ++F1F2_blinkcounter;
}


/////////////////////////////////////////////////////////////////////////////
//  Transmit MIDI on a pot change. Called directly from main::AIN_NotifyChange()
/////////////////////////////////////////////////////////////////////////////
void LivePanel_HandleAin(unsigned char pin , unsigned char pin_value)
{
  unsigned char param = 0;
  unsigned char value = 0;

  param = getparam(pin);
  value = getvalue(pin, pin_value);

  update_EditBuffer(device, param, value); //mis a jour dans la fct suivante

  if (param == SX_PORTAMENTO_SPEEDBYVELOCITY) // Alt Pot PORTA/VELO is sloooooow
    MIDI_SendDelayedVoiceParam(param, value);
  else
    MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, mThru_XCc);

  if(SoftPanel.Mode != Cfg && MIDI_Incoming == false) {
  //if(SoftPanel.Mode != Cfg && Serial3.peek() < 0) {
    LivePanel_DisplayAin(pin, value); // don't show jittering pot value when setting JITTER in CFG/MISC
    //app_flags.Display_Pot_Req = 1;
  }

  // filter sustain is bugged, handle it here, and return
  if ((pin == POT_FILTER_SUSTAIN) && (Alt == 0)) // create bug on ENV3 AMP
  {
    if (FilterSustainMode == FILTER_ENV_MODE_HACKED)
    {
      MIDI_SendSustainBuffer(value); // send the whole editbuffer with new param updated (HACK)
    }
    else
    {
      lastvalue       = value; // signed converted value
      lastparam       = param;
      last_ain_pin    = pin;
      elapsedTime = 0; // for the RestTmpMsg function
      return;
    }
  }

  // should filter frequency send continuous controller messages?
  if (pin == POT_FILTER_FREQUENCY && FilterCutoffMode == FILTER_FREQ_MODE_CCONTROL)
  {
    MIDI_Send_BreathController(INTERFACE_SERIAL, value);
  }
  else
  {
    if (!is_transmit_delayed)
    {
      MIDI_SendVoiceParam(INTERFACE_SERIAL, param, value, mThru_XCc);
      is_transmit_delayed = 1; // set the next request to delay
    }
  }
  lastvalue       = value; // signed converted value
  lastparam       = param;
  last_ain_pin    = pin;

  elapsedTime = 0; // for the RestTmpMsg function
}

/////////////////////////////////////////////////////////////////////////////
//  Get a matrix sysex parameter number associated with a pot
/////////////////////////////////////////////////////////////////////////////
unsigned char getparam(unsigned char pin)
{
  unsigned char param = 0;
  param = PotConfigMap[pin + Alt * 32].param; // attention à ne pas mettre dans MIDI_Sendvoiceparam() !!! // ici mettre + ALT*32
  return param;
}

////////////////////////////////////////////////////////////////////////////
//  Convert a pot to a matrix parameter value based on the PotConfigMap
/////////////////////////////////////////////////////////////////////////////
unsigned char getvalue(unsigned char pin, unsigned char valu)
{
  unsigned char value = 0;

  // convert values for signed parameters
  switch (PotConfigMap[pin + Alt * 32].valtype)
  {
    case SIGNED7:
      //value = Convert_7bit_Signed(value);
      value = Convert_7bit_Signed(valu);
      break;

    case SIGNED6:
      //value = Convert_6bit_Signed(value);
      value = Convert_6bit_Signed(valu >> 1);
      break;

    case UNSIGNED7:
      value = valu;
      break;

    case UNSIGNED6:
    case MIXBALANCE:
      value = valu >> 1;
      break;

    default :
      break;
  }
  Serial.println(valu, DEC);
  return value;
}

/////////////////////////////////////////////////////////////////////////////
//  Update the edit buffer and keep track of what was changed
/////////////////////////////////////////////////////////////////////////////
void update_EditBuffer(unsigned char device, unsigned char param, unsigned char value)
{
#if DEBUG_uEB
  Serial.print(F("update_EditBuffer(")); Serial.print(device, DEC); Serial.print(F(", "));
  Serial.print(param, DEC); Serial.print(F(", "));
  Serial.print(value, DEC); Serial.println(F(")"));
#endif

  switch (param)
  {
    // made using Excel ;)

    case SX_KEYBOARD_MODE         : EditBuffer[device][EB_KEYBOARD_MODE         ] = value; lastbyteindex = EB_KEYBOARD_MODE         ; break;
    case SX_OSC1_FREQ             : EditBuffer[device][EB_OSC1_FREQ             ] = value; lastbyteindex = EB_OSC1_FREQ             ; break;
    case SX_OSC1_WAVESHAPE        : EditBuffer[device][EB_OSC1_WAVESHAPE        ] = value; lastbyteindex = EB_OSC1_WAVESHAPE        ; break;
    case SX_OSC1_PULSEWIDTH       : EditBuffer[device][EB_OSC1_PULSEWIDTH       ] = value; lastbyteindex = EB_OSC1_PULSEWIDTH       ; break;
    case SX_OSC1_LEVERMOD         : EditBuffer[device][EB_OSC1_LEVERMOD         ] = value; lastbyteindex = EB_OSC1_LEVERMOD         ; break;
    case SX_OSC1_WAVEFORM         : EditBuffer[device][EB_OSC1_WAVEFORM         ] = value; lastbyteindex = EB_OSC1_WAVEFORM         ; break;
    case SX_OSC2_FREQ             : EditBuffer[device][EB_OSC2_FREQ             ] = value; lastbyteindex = EB_OSC2_FREQ             ; break;
    case SX_OSC2_WAVESHAPE        : EditBuffer[device][EB_OSC2_WAVESHAPE        ] = value; lastbyteindex = EB_OSC2_WAVESHAPE        ; break;
    case SX_OSC2_PULSEWIDTH       : EditBuffer[device][EB_OSC2_PULSEWIDTH       ] = value; lastbyteindex = EB_OSC2_PULSEWIDTH       ; break;
    case SX_OSC2_LEVERMOD         : EditBuffer[device][EB_OSC2_LEVERMOD         ] = value; lastbyteindex = EB_OSC2_LEVERMOD         ; break;
    case SX_OSC2_WAVEFORM         : EditBuffer[device][EB_OSC2_WAVEFORM         ] = value; lastbyteindex = EB_OSC2_WAVEFORM         ; break;
    case SX_OSC2_DETUNE           : EditBuffer[device][EB_OSC2_DETUNE           ] = value; lastbyteindex = EB_OSC2_DETUNE           ; break;
    case SX_MIX                   : EditBuffer[device][EB_MIX                   ] = value; lastbyteindex = EB_MIX                   ; break;
    case SX_OSC1_KEYBOARDMOD      : EditBuffer[device][EB_OSC1_KEYBOARDMOD      ] = value; lastbyteindex = EB_OSC1_KEYBOARDMOD      ; break;
    case SX_OSC1_CLICK            : EditBuffer[device][EB_OSC1_CLICK            ] = value; lastbyteindex = EB_OSC1_CLICK            ; break;
    case SX_OSC2_KEYBOARDMOD      : EditBuffer[device][EB_OSC2_KEYBOARDMOD      ] = value; lastbyteindex = EB_OSC2_KEYBOARDMOD      ; break;
    case SX_OSC2_CLICK            : EditBuffer[device][EB_OSC2_CLICK            ] = value; lastbyteindex = EB_OSC2_CLICK            ; break;
    case SX_OSC_SYNCMODE          : EditBuffer[device][EB_OSC_SYNCMODE          ] = value; lastbyteindex = EB_OSC_SYNCMODE          ; break;
    case SX_FILTER_FREQ           : EditBuffer[device][EB_FILTER_FREQ           ] = value; lastbyteindex = EB_FILTER_FREQ           ; break;
    case SX_FILTER_RESONANCE      : EditBuffer[device][EB_FILTER_RESONANCE      ] = value; lastbyteindex = EB_FILTER_RESONANCE      ; break;
    case SX_FILTER_LEVERMOD       : EditBuffer[device][EB_FILTER_LEVERMOD       ] = value; lastbyteindex = EB_FILTER_LEVERMOD       ; break;
    case SX_FILTER_KEYBOARDMOD    : EditBuffer[device][EB_FILTER_KEYBOARDMOD    ] = value; lastbyteindex = EB_FILTER_KEYBOARDMOD    ; break;
    case SX_FILTER_FM_AMOUNT      : EditBuffer[device][EB_FILTER_FM_AMOUNT      ] = value; lastbyteindex = EB_FILTER_FM_AMOUNT      ; break;
    case SX_VCA1_AMOUNT           : EditBuffer[device][EB_VCA1_AMOUNT           ] = value; lastbyteindex = EB_VCA1_AMOUNT           ; break;
    case SX_PORTAMENTO_RATE       : EditBuffer[device][EB_PORTAMENTO_RATE       ] = value; lastbyteindex = EB_PORTAMENTO_RATE       ; break;
    case SX_PORTAMENTO_MODE       : EditBuffer[device][EB_PORTAMENTO_MODE       ] = value; lastbyteindex = EB_PORTAMENTO_MODE       ; break;
    case SX_PORTAMENTO_LEGATOENABLE: EditBuffer[device][EB_PORTAMENTO_LEGATOENABLE] = value; lastbyteindex = EB_PORTAMENTO_LEGATOENABLE; break;
    case SX_LFO1_SPEED            : EditBuffer[device][EB_LFO1_SPEED            ] = value; lastbyteindex = EB_LFO1_SPEED            ; break;
    case SX_LFO1_TRIGGERMODE      : EditBuffer[device][EB_LFO1_TRIGGERMODE      ] = value; lastbyteindex = EB_LFO1_TRIGGERMODE      ; break;
    case SX_LFO1_LAGENABLE        : EditBuffer[device][EB_LFO1_LAGENABLE        ] = value; lastbyteindex = EB_LFO1_LAGENABLE        ; break;
    case SX_LFO1_WAVESHAPE        : EditBuffer[device][EB_LFO1_WAVESHAPE        ] = value; lastbyteindex = EB_LFO1_WAVESHAPE        ; break;
    case SX_LFO1_RETRIGGERPOINT   : EditBuffer[device][EB_LFO1_RETRIGGERPOINT   ] = value; lastbyteindex = EB_LFO1_RETRIGGERPOINT   ; break;
    case SX_LFO1_SAMPLEDSOURCE    : EditBuffer[device][EB_LFO1_SAMPLEDSOURCE    ] = value; lastbyteindex = EB_LFO1_SAMPLEDSOURCE    ; break;
    case SX_LFO1_AMPLITUDE        : EditBuffer[device][EB_LFO1_AMPLITUDE        ] = value; lastbyteindex = EB_LFO1_AMPLITUDE        ; break;
    case SX_LFO2_SPEED            : EditBuffer[device][EB_LFO2_SPEED            ] = value; lastbyteindex = EB_LFO2_SPEED            ; break;
    case SX_LFO2_TRIGGERMODE      : EditBuffer[device][EB_LFO2_TRIGGERMODE      ] = value; lastbyteindex = EB_LFO2_TRIGGERMODE      ; break;
    case SX_LFO2_LAGENABLE        : EditBuffer[device][EB_LFO2_LAGENABLE        ] = value; lastbyteindex = EB_LFO2_LAGENABLE        ; break;
    case SX_LFO2_WAVESHAPE        : EditBuffer[device][EB_LFO2_WAVESHAPE        ] = value; lastbyteindex = EB_LFO2_WAVESHAPE        ; break;
    case SX_LFO2_RETRIGGERPOINT   : EditBuffer[device][EB_LFO2_RETRIGGERPOINT   ] = value; lastbyteindex = EB_LFO2_RETRIGGERPOINT   ; break;
    case SX_LFO2_SAMPLEDSOURCE    : EditBuffer[device][EB_LFO2_SAMPLEDSOURCE    ] = value; lastbyteindex = EB_LFO2_SAMPLEDSOURCE    ; break;
    case SX_LFO2_AMPLITUDE        : EditBuffer[device][EB_LFO2_AMPLITUDE        ] = value; lastbyteindex = EB_LFO2_AMPLITUDE        ; break;
    case SX_ENV1_TRIGGERMODE      : EditBuffer[device][EB_ENV1_TRIGGERMODE      ] = value; lastbyteindex = EB_ENV1_TRIGGERMODE      ; break;
    case SX_ENV1_DELAY            : EditBuffer[device][EB_ENV1_DELAY            ] = value; lastbyteindex = EB_ENV1_DELAY            ; break;
    case SX_ENV1_ATTACK           : EditBuffer[device][EB_ENV1_ATTACK           ] = value; lastbyteindex = EB_ENV1_ATTACK           ; break;
    case SX_ENV1_DECAY            : EditBuffer[device][EB_ENV1_DECAY            ] = value; lastbyteindex = EB_ENV1_DECAY            ; break;
    case SX_ENV1_SUSTAIN          : EditBuffer[device][EB_ENV1_SUSTAIN          ] = value; lastbyteindex = EB_ENV1_SUSTAIN          ; break;
    case SX_ENV1_RELEASE          : EditBuffer[device][EB_ENV1_RELEASE          ] = value; lastbyteindex = EB_ENV1_RELEASE          ; break;
    case SX_ENV1_AMPLITUDE        : EditBuffer[device][EB_ENV1_AMPLITUDE        ] = value; lastbyteindex = EB_ENV1_AMPLITUDE        ; break;
    case SX_ENV1_LFOTRIGGERMODE   : EditBuffer[device][EB_ENV1_LFOTRIGGERMODE   ] = value; lastbyteindex = EB_ENV1_LFOTRIGGERMODE   ; break;
    case SX_ENV1_RELEASEMODE      : EditBuffer[device][EB_ENV1_RELEASEMODE      ] = value; lastbyteindex = EB_ENV1_RELEASEMODE      ; break;
    case SX_ENV2_TRIGGERMODE      : EditBuffer[device][EB_ENV2_TRIGGERMODE      ] = value; lastbyteindex = EB_ENV2_TRIGGERMODE      ; break;
    case SX_ENV2_DELAY            : EditBuffer[device][EB_ENV2_DELAY            ] = value; lastbyteindex = EB_ENV2_DELAY            ; break;
    case SX_ENV2_ATTACK           : EditBuffer[device][EB_ENV2_ATTACK           ] = value; lastbyteindex = EB_ENV2_ATTACK           ; break;
    case SX_ENV2_DECAY            : EditBuffer[device][EB_ENV2_DECAY            ] = value; lastbyteindex = EB_ENV2_DECAY            ; break;
    case SX_ENV2_SUSTAIN          : EditBuffer[device][EB_ENV2_SUSTAIN          ] = value; lastbyteindex = EB_ENV2_SUSTAIN          ; break;
    case SX_ENV2_RELEASE          : EditBuffer[device][EB_ENV2_RELEASE          ] = value; lastbyteindex = EB_ENV2_RELEASE          ; break;
    case SX_ENV2_AMPLITUDE        : EditBuffer[device][EB_ENV2_AMPLITUDE        ] = value; lastbyteindex = EB_ENV2_AMPLITUDE        ; break;
    case SX_ENV2_LFOTRIGGERMODE   : EditBuffer[device][EB_ENV2_LFOTRIGGERMODE   ] = value; lastbyteindex = EB_ENV2_LFOTRIGGERMODE   ; break;
    case SX_ENV2_RELEASEMODE      : EditBuffer[device][EB_ENV2_RELEASEMODE      ] = value; lastbyteindex = EB_ENV2_RELEASEMODE      ; break;
    case SX_ENV3_TRIGGERMODE      : EditBuffer[device][EB_ENV3_TRIGGERMODE      ] = value; lastbyteindex = EB_ENV3_TRIGGERMODE      ; break;
    case SX_ENV3_DELAY            : EditBuffer[device][EB_ENV3_DELAY            ] = value; lastbyteindex = EB_ENV3_DELAY            ; break;
    case SX_ENV3_ATTACK           : EditBuffer[device][EB_ENV3_ATTACK           ] = value; lastbyteindex = EB_ENV3_ATTACK           ; break;
    case SX_ENV3_DECAY            : EditBuffer[device][EB_ENV3_DECAY            ] = value; lastbyteindex = EB_ENV3_DECAY            ; break;
    case SX_ENV3_SUSTAIN          : EditBuffer[device][EB_ENV3_SUSTAIN          ] = value; lastbyteindex = EB_ENV3_SUSTAIN          ; break;
    case SX_ENV3_RELEASE          : EditBuffer[device][EB_ENV3_RELEASE          ] = value; lastbyteindex = EB_ENV3_RELEASE          ; break;
    case SX_ENV3_AMPLITUDE        : EditBuffer[device][EB_ENV3_AMPLITUDE        ] = value; lastbyteindex = EB_ENV3_AMPLITUDE        ; break;
    case SX_ENV3_LFOTRIGGERMODE   : EditBuffer[device][EB_ENV3_LFOTRIGGERMODE   ] = value; lastbyteindex = EB_ENV3_LFOTRIGGERMODE   ; break;
    case SX_ENV3_RELEASEMODE      : EditBuffer[device][EB_ENV3_RELEASEMODE      ] = value; lastbyteindex = EB_ENV3_RELEASEMODE      ; break;
    case SX_TRACK_INPUT           : EditBuffer[device][EB_TRACK_INPUT           ] = value; lastbyteindex = EB_TRACK_INPUT           ; break;
    case SX_TRACK_POINT1          : EditBuffer[device][EB_TRACK_POINT1          ] = value; lastbyteindex = EB_TRACK_POINT1          ; break;
    case SX_TRACK_POINT2          : EditBuffer[device][EB_TRACK_POINT2          ] = value; lastbyteindex = EB_TRACK_POINT2          ; break;
    case SX_TRACK_POINT3          : EditBuffer[device][EB_TRACK_POINT3          ] = value; lastbyteindex = EB_TRACK_POINT3          ; break;
    case SX_TRACK_POINT4          : EditBuffer[device][EB_TRACK_POINT4          ] = value; lastbyteindex = EB_TRACK_POINT4          ; break;
    case SX_TRACK_POINT5          : EditBuffer[device][EB_TRACK_POINT5          ] = value; lastbyteindex = EB_TRACK_POINT5          ; break;
    case SX_RAMP1_RATE            : EditBuffer[device][EB_RAMP1_RATE            ] = value; lastbyteindex = EB_RAMP1_RATE            ; break;
    case SX_RAMP1_MODE            : EditBuffer[device][EB_RAMP1_MODE            ] = value; lastbyteindex = EB_RAMP1_MODE            ; break;
    case SX_RAMP2_RATE            : EditBuffer[device][EB_RAMP2_RATE            ] = value; lastbyteindex = EB_RAMP2_RATE            ; break;
    case SX_RAMP2_MODE            : EditBuffer[device][EB_RAMP2_MODE            ] = value; lastbyteindex = EB_RAMP2_MODE            ; break;
    case SX_OSC1_FREQBYLFO1AMOUNT : EditBuffer[device][EB_OSC1_FREQBYLFO1AMOUNT ] = value; lastbyteindex = EB_OSC1_FREQBYLFO1AMOUNT ; break;
    case SX_OSC1_PWBYLFO2AMOUNT   : EditBuffer[device][EB_OSC1_PWBYLFO2AMOUNT   ] = value; lastbyteindex = EB_OSC1_PWBYLFO2AMOUNT   ; break;
    case SX_OSC2_FREQBYLFO1AMOUNT : EditBuffer[device][EB_OSC2_FREQBYLFO1AMOUNT ] = value; lastbyteindex = EB_OSC2_FREQBYLFO1AMOUNT ; break;
    case SX_OSC2_PWBYLFO2AMOUNT   : EditBuffer[device][EB_OSC2_PWBYLFO2AMOUNT   ] = value; lastbyteindex = EB_OSC2_PWBYLFO2AMOUNT   ; break;
    case SX_FILTER_FREQBYENV1AMOUNT: EditBuffer[device][EB_FILTER_FREQBYENV1AMOUNT] = value; lastbyteindex = EB_FILTER_FREQBYENV1AMOUNT; break;
    case SX_FILTER_FREQBYPRESSURE : EditBuffer[device][EB_FILTER_FREQBYPRESSURE ] = value; lastbyteindex = EB_FILTER_FREQBYPRESSURE ; break;
    case SX_VCA1_AMPBYVELOCITY    : EditBuffer[device][EB_VCA1_AMPBYVELOCITY    ] = value; lastbyteindex = EB_VCA1_AMPBYVELOCITY    ; break;
    case SX_VCA2_AMPBYENV2        : EditBuffer[device][EB_VCA2_AMPBYENV2        ] = value; lastbyteindex = EB_VCA2_AMPBYENV2        ; break;
    case SX_ENV1_AMPBYVELO        : EditBuffer[device][EB_ENV1_AMPBYVELO        ] = value; lastbyteindex = EB_ENV1_AMPBYVELO        ; break;
    case SX_ENV2_AMPBYVELO        : EditBuffer[device][EB_ENV2_AMPBYVELO        ] = value; lastbyteindex = EB_ENV2_AMPBYVELO        ; break;
    case SX_ENV3_AMPBYVELO        : EditBuffer[device][EB_ENV3_AMPBYVELO        ] = value; lastbyteindex = EB_ENV3_AMPBYVELO        ; break;
    case SX_LFO1_AMPMODULATION    : EditBuffer[device][EB_LFO1_AMPMODULATION    ] = value; lastbyteindex = EB_LFO1_AMPMODULATION    ; break;
    case SX_LFO2_AMPMODULATION    : EditBuffer[device][EB_LFO2_AMPMODULATION    ] = value; lastbyteindex = EB_LFO2_AMPMODULATION    ; break;
    case SX_PORTAMENTO_SPEEDBYVELOCITY: EditBuffer[device][EB_PORTAMENTO_SPEEDBYVELOCITY] = value; lastbyteindex = EB_PORTAMENTO_SPEEDBYVELOCITY; break;
    case SX_FILTER_FMBYENV3       : EditBuffer[device][EB_FILTER_FMBYENV3       ] = value; lastbyteindex = EB_FILTER_FMBYENV3       ; break;
    case SX_FILTER_FMBYPRESSURE   : EditBuffer[device][EB_FILTER_FMBYPRESSURE   ] = value; lastbyteindex = EB_FILTER_FMBYPRESSURE   ; break;
    case SX_LFO1_SPEEDMODULATION  : EditBuffer[device][EB_LFO1_SPEEDMODULATION  ] = value; lastbyteindex = EB_LFO1_SPEEDMODULATION  ; break;
    case SX_LFO2_SPEEDMODULATION  : EditBuffer[device][EB_LFO2_SPEEDMODULATION  ] = value; lastbyteindex = EB_LFO2_SPEEDMODULATION  ; break;
    case SX_UNISON_DETUNE         : UnisonDetune[device]                          = value; lastbyteindex = EB_UNISON_DETUNE         ; break;

      // Matrix Modulation is missing because is doesn't have SX_ parameters despite it has EB_
  }
}

/////////////////////////////////////////////////////////////////////////////
//  Display info on the most recent pot changes
/////////////////////////////////////////////////////////////////////////////
void LivePanel_DisplayAin( char param, byte value)
{
  
  unsigned char ain_val;
  unsigned char valtype = PotConfigMap[last_ain_pin + Alt * 32].valtype;

  RefreshSoftPanel = 1; // soft panel display may have been overwritten, make sure it completely refreshes on a button click

  lcd.setCursor(4, 0);
  lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(PotDescription [last_ain_pin + Alt * 32]))));

#if DEBUG_LCDparamvaluedescription
  Serial.print (F("LCD_DisplayParamDescription ")); Serial.println(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(PotDescription [last_ain_pin + Alt * 32]))));
#endif

  ain_val = value;
  if (PotConfigMap[last_ain_pin + Alt * 32].param == SX_UNISON_DETUNE)
  {
    LCD_DisplayEditBufferOrig(UnisonDetuneOrig, UNSIGNED6); // OK c'est bon :) 20/8/17, y a meme le msg temporaire
  }
  else
  {
    LCD_DisplayEditBufferOrig(EditBufferOrig[lastbyteindex], valtype);
  }
  LCD_DisplayParamValue(ain_val, valtype);

  LCD_DisplayBarGraph(valtype, ain_val); // bouffe 0,8 K de RAM !!! https://zestedesavoir.com/tutoriels/374/gestion-de-la-memoire-sur-arduino/  : déjà la Macro F() dans les Serial.print

#if DEBUG_bargraph
  Serial.print(F("valtype = ")); Serial.print(valtype, DEC); Serial.print(F(" ain_val = ")); Serial.println(ain_val, DEC);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Pots with a high update_interval will transmit their MIDI on the 1st value, and the last value detected
//  values in between are only sent once the interval time has been met,
//  all other values are skipped. This is called from main::Tick()
/////////////////////////////////////////////////////////////////////////////
void LivePanel_HandleTransmitDelay()
{
  unsigned int pot_interval;

  if (!is_transmit_delayed)
    return;

  pot_interval = (unsigned int)PotConfigMap[last_ain_pin + Alt * 32].update_interval;

  if (transmit_counter >= (pot_interval << 9)) // 9 = 512, 10 = 1024
  {
    MIDI_SendVoiceParam(INTERFACE_SERIAL, lastparam , lastvalue, mThru_XCc);
    is_transmit_delayed = 0;
    transmit_counter = 0;
  }
  transmit_counter++;
}

/////////////////////////////////////////////////////////////////////////////
//  Initialize the live panel
/////////////////////////////////////////////////////////////////////////////
void LivePanel_Init()
{
  transmit_counter    = 0;
  is_transmit_delayed = 0;

#if DEBUG_adc
  Serial.println(F("LivePanel_Init()")); Serial.println();
#endif
}
