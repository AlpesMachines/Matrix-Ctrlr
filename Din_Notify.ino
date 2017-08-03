#include "din.h"
#include "device.h"
#include "define.h"


static bool button_state[NBR_BUTTON]; // button state
static bool old_button_state[NBR_BUTTON];
static unsigned char butt;

byte din_pin;
byte din_pin_value;

///////////////////////////////////////////////////////////
// lecture des tact switch (digital)
///////////////////////////////////////////////////////////
void ReadDigital(void) {
  //Loop as many as connected buttons
  button_state[butt] = SR.Button_Pin_Read(butt); // read button state hold value in the variable
  //__asm__("nop\n\t");
  ++butt;
  if (butt == NBR_BUTTON)
  {
    // return to zero
    butt = 0;
  }
}

////////////////////////////////////////////////////////////
// notify state change in Din modules
////////////////////////////////////////////////////////////
void DIN_NotifyChange(void)
{
  if (timer_digital > DigitalScanPeriod)
  {
    //Loop as many as connected buttons
    for (unsigned char i = 0; i < NBR_BUTTON; i++)
    {
      if (button_state[i] != old_button_state[i]) { // has Button state changed ?
        din_pin = i;
        din_pin_value = button_state[i];

#if DEBUG_din
        // print something like : Din-0-### value
        Serial.print(F("DIN_NotifyChange()"));
        Serial.print(F("Din-"));
        Serial.print(din_pin);
        Serial.print(F(":"));
        Serial.println(din_pin_value);
#endif

        if (din_pin_value && (DIN_ConfigMap[din_pin].group != BUTGRP_IGNORE))
        { // if button pressed,  == 1 && qu'on est pas IGNORE
          switch (DIN_ConfigMap[din_pin].group)
          {
            case BUTGRP_LIVE:
              LivePanel_DinHandler(din_pin);
              LivePanel_DisplayDin(din_pin);
              break;

            case BUTGRP_DEVICE:
              Device_Select(din_pin);
              break;

            case BUTGRP_SHIFT:
              if (din_pin == DIN_SHIFT)
                Shift = SHIFT_VALUE;
              if (din_pin == DIN_ALT)
              {
                ++Alt;
                if (Alt > ALT_VALUE) Alt = 0;
              }
#if DEBUG_din
              Serial.print("Shift="); Serial.println(Shift);
              Serial.print("Alt="); Serial.println(Alt);
              Serial.println();
#endif
              break;

            case BUTGRP_TRIG:
              ++trigger;
              Serial.print("trigger = "); Serial.println(trigger);
              break;

            default:
              SoftPanel_Handler(din_pin, 0); // no encoder inc/dec
              SoftPanel_DisplayHandler(); // à mettre dans display tick avec un req
              //      appflag_updateDisplay = 1;
              break;
          }
        }
        else if ((din_pin_value == 0) && (DIN_ConfigMap[din_pin].group == BUTGRP_SHIFT))
        {
          Shift = 0;
          //  Alt = 0;
          if (DEBUG_din)
          {
            Serial.print("Shift="); Serial.println(Shift);
            Serial.print("Alt="); Serial.println(Alt);
            Serial.println();
          }
        }
        else
        {
          // nop
        }
        // Don't forget to hold the state of the button:
        old_button_state[i] = button_state[i];
        timer_digital = 0;
      }
    }
  }

}


