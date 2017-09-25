#include "din.h"
#include "lcd.h"
#include "device.h"
#include "define.h"
#include "chaosmatrix.h"
#include "pots.h"

static bool button_state[NBR_BUTTON]; // button state
static bool old_button_state[NBR_BUTTON];
static unsigned char butt;

static byte din_pin;
static byte din_pin_value;

///////////////////////////////////////////////////////////
// lecture des tact switch (digital)
///////////////////////////////////////////////////////////
void ReadDigital(void)
{
  //  static unsigned char butt;
  unsigned char a, b;

  //Loop as many as connected buttons :
  //button_state[butt] = SR.Button_Pin_Read(butt);

  // read (twice) button state, hold value in the variable
  a = SR.Button_Pin_Read(butt);
  b = SR.Button_Pin_Read(butt);

  if (a == b) // if double reading is same result ()
  {
    button_state[butt] = a;

    // enter test mode if DCO2 Pulse & F1 pressed
    if (booting && button_state[19] == true && button_state[1] == true)
    {
      TestMode();
    }

#if DEBUG_SRIO
    if (a == 1) // "pressed"
    {
      Serial.print(F("butt ")); Serial.print(butt, DEC); Serial.print(F("  "));  Serial.print(button_state[butt]);
    }

    if (SR.Button_Pin_Read(butt)) // see bug
    {
      Serial.print(F("/ buggy butt ")); Serial.print(butt, DEC); Serial.print(F("  "));
      Serial.println( SR.Button_Pin_Read(butt));
    }
#endif
  }

  __asm__("nop\n\t"); // wait ...

  DIN_NotifyChange();

  // increment for next loop
  butt++;

  if (butt == NBR_BUTTON ) // reset
  {
    butt = 0;
  }
}

////////////////////////////////////////////////////////////
// notify state change in Din modules
////////////////////////////////////////////////////////////
void DIN_NotifyChange(void)
{
  if (button_state[butt] != old_button_state[butt]) { // has Button state changed ?
    // Don't forget to hold the state of the button for next time :
    old_button_state[butt] = button_state[butt];

    din_pin = butt;
    din_pin_value = button_state[butt];

#if DEBUG_din
    // print something like : Din-0-### value
    Serial.print(F("DIN_NotifyChange()"));
    Serial.print(F("Din-"));
    Serial.print(din_pin);
    Serial.print(F(":"));
    Serial.println(din_pin_value);
#endif

    if (booting)
    {
      // test mode
      Serial.print(F("din : ")); Serial.print(din_pin); Serial.print(F(" "));
      if (din_pin_value)
        Serial.print(F("*"));
      else
        Serial.print(F("."));
      Serial.println();

      // display
      lcd.setCursor(0, 1);
      lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(DinDescription[din_pin]))));
      //      lcd.print(F("din:"));
      //      LCD_PrintBCD2(din_pin);
      lcd.print(F(" "));
      if (din_pin_value)
        lcd.print(F("*"));
      else
        lcd.print(F("."));
      lcd.print(F("   "));

      // led
      switch (din_pin)
      {
        case DIN_DCO1_CLICK:
          DOUT_PinSet(DOUT_DCO1_CLICK, din_pin_value);
          break;

        case DIN_DCO1_WAVE:
          DOUT_PinSet(DOUT_DCO1_WAVE, din_pin_value);
          break;

        case DIN_DCO1_PULSE:
          DOUT_PinSet(DOUT_DCO1_PULSE, din_pin_value);
          break;

        case DIN_DCO2_CLICK:
          DOUT_PinSet(DOUT_DCO2_CLICK, din_pin_value);
          break;

        case DIN_DCO2_WAVE:
          DOUT_PinSet(DOUT_DCO2_WAVE, din_pin_value);
          break;

        case DIN_DCO2_PULSE:
          DOUT_PinSet(DOUT_DCO2_PULSE, din_pin_value);
          break;

        case DIN_DCO_SYNC:
          DOUT_PinSet(DOUT_OSC_SYNC, din_pin_value);
          break;

        case DIN_LFO1_WAVESELECT:
          DOUT_PinSet(DOUT_LFO1_WAVESELECT, din_pin_value);
          break;

        case DIN_LFO2_WAVESELECT:
          DOUT_PinSet(DOUT_LFO2_WAVESELECT, din_pin_value);
          break;

        case DIN_OSCILLATORS:
          DOUT_PinSet(DOUT_DCO, din_pin_value);
          break;

        case DIN_FILTER:
          DOUT_PinSet(DOUT_FILTER, din_pin_value);
          break;

        case DIN_ENVELOPES:
          DOUT_PinSet(DOUT_ENV, din_pin_value);
          break;

        case DIN_KEYBOARD:
          DOUT_PinSet(DOUT_KBD, din_pin_value);
          break;

        case DIN_MATRIX:
          DOUT_PinSet(DOUT_MATRIXMOD, din_pin_value);
          break;

        case DIN_PAGE:
          DOUT_PinSet(DOUT_PAGE, din_pin_value);
          break;

        case DIN_PATCH:
          DOUT_PinSet(DOUT_PATCH_SELECT, din_pin_value);
          break;

        case DIN_EDIT:
          DOUT_PinSet(DOUT_EDIT, din_pin_value);
          break;

        case DIN_ARP:
          DOUT_PinSet(DOUT_ARP, din_pin_value);
          break;

        case DIN_CFG:
          DOUT_PinSet(DOUT_CFG, din_pin_value);
          break;

        case DIN_ALT:
          DOUT_PinSet(DOUT_ALT, din_pin_value);
          break;

        case DIN_SHIFT:
          DOUT_PinSet(DOUT_SHIFT, din_pin_value);
          break;

        case DIN_F1:
          DOUT_PinSet(DOUT_F1, din_pin_value);
          break;

        case DIN_F2:
          DOUT_PinSet(DOUT_F2, din_pin_value);
          break;

        case DIN_MATRIX_A:
          DOUT_PinSet(DOUT_MATRIX_A, din_pin_value);
          break;

        case DIN_MATRIX_B:
          DOUT_PinSet(DOUT_MATRIX_B, din_pin_value);
          break;

        case DIN_MATRIX_C:
          DOUT_PinSet(DOUT_MATRIX_C, din_pin_value);
          break;

        case DIN_MATRIX_D:
          DOUT_PinSet(DOUT_MATRIX_D, din_pin_value);
          break;

        case DIN_INC:
          DOUT_PinSet(DOUT_ACTIVITY2, din_pin_value);
          break;

        case DIN_DEC:
          DOUT_PinSet(DOUT_ACTIVITY, din_pin_value);
          break;

        default:
          break;
      }

    }
    else
    {
      if ((din_pin_value == true) && (DIN_ConfigMap[din_pin].group != BUTGRP_IGNORE))
      { // if button pressed,  == 1 && qu'on est pas IGNORE
        switch (DIN_ConfigMap[din_pin].group)
        {
          case BUTGRP_LIVE:
            LivePanel_DinHandler(din_pin);
            LivePanel_DisplayDin(din_pin);
#if DEBUG_din
            Serial.println("btngrp live");
#endif
            break;

          case BUTGRP_SOFT:
            SoftPanel_Handler(din_pin, 0); // no encoder inc/dec
            SoftPanel_DisplayHandler(); // à mettre dans display tick avec un req
            //      appflag_updateDisplay = 1;
#if DEBUG_din
            Serial.println("btngrp soft");
#endif
            break;

          case BUTGRP_DEVICE:
            Device_Select(din_pin);
#if DEBUG_din
            Serial.println("btngrp device");
#endif
            break;

          case BUTGRP_SHIFT:
            if (din_pin == DIN_SHIFT)
              Shift = SHIFT_VALUE; // mise à 1 de Shift
            if (din_pin == DIN_ALT)
            {
              ++Alt;
              if (Alt > ALT_VALUE) Alt = 0; // toggle ALT
            }
#if DEBUG_din
            Serial.print("Shift="); Serial.println(Shift);
            Serial.print("Alt="); Serial.println(Alt);
            Serial.println();
#endif
            break;

          case BUTGRP_TRIG:
            ++trigger;
#if DEBUG_TRIG
            Serial.print("trigger = "); Serial.println(trigger);
#endif
            break;

          default:
            break;
        }
      }
      else if ((din_pin_value == 0) && (DIN_ConfigMap[din_pin].group == BUTGRP_SHIFT))
      {
        Shift = 0; // remise à zero de Shift
        //  Alt = 0;
#if DEBUG_din

        Serial.print("Shift="); Serial.println(Shift);
        Serial.print("Alt="); Serial.println(Alt);
        Serial.println();
#endif
      }
      else
      {
        // nop
      }
      //        // Don't forget to hold the state of the button:
      //        old_button_state[i] = button_state[i];
      //timer_digital = 0;
    }
  }
}

////////////////////////////////////////////////////////
// Test Mode on boot to test the hardware
////////////////////////////////////////////////////////
void TestMode(void)
{
  inTest = true;
  
  for (unsigned char i = 0; i < NBR_LED; ++i)
  {
    SR.Led_Pin_Write(i, 0); // éteint toutes les LEDS
  }

  Serial.println(F("TestMode()"));

  lcd.clear();
  lcd.print(F(" --- TEST MODE ---  "));

  delay(255);

  for (;;) // endless loop to mimic loop()
  {
    // read buttons
    ReadDigital();

    // read potentiometers
    ReadAnalog();
    AIN_NotifyChange();

    // quit test mode by pressing KEY2 & KEY3 & KEY4 while booting
    if (booting && button_state[8] == true && button_state[12] == true && button_state[13] == true)
    {
      inTest = false;
      software_Reboot();
    }
  }
}


