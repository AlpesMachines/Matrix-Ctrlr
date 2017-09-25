#include "lcd.h"
#include "pots.h"


///////////////////////////////////////////////////////////
// Globales variables
// 10 bits : int type necessary
///////////////////////////////////////////////////////////
int Analog[NBR_POT] = { // 0 1024
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

int Previous_Analog[NBR_POT] = { // 0 1024
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

bool Analog_Debug[NBR_POT] = { // 0 or 1
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char Analog_Mapped[NBR_POT] = { // 0 127
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char Previous_Analog_Mapped[NBR_POT] = { // 0 127
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

///////////////////////////////////////////////////////////
// lecture des potards scanner
///////////////////////////////////////////////////////////
void ReadAnalog() {

  static byte AnaIndex[NBR_POT] = { // was int type
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31
  };

  static unsigned char i; // i increment on each mainloop (iteration) to fasten mainloop (472 microsec !!!)

  //// NE PAS EFFACER CETTE SECTION !
  ////    for(int j = 0; j < 3; j ++){
  ////      digitalWrite(SPINS[j], muxChannel[i][j]);
  ////    }
  setPin(i);
  // Each 'nop' statement executes in one machine cycle (at 16 MHz) yielding a 62.5 ns (nanosecond) delay (tPHL 150 ns). http://playground.arduino.cc/Main/AVR
  __asm__("nop\n\t");

  // we read pots 4 by 4 on each loop ;)
  Analog[AnaIndex[i]]       = analogRead(VOUT_ANA1);
  Analog[AnaIndex[i + 8]]   = analogRead(VOUT_ANA2);
  Analog[AnaIndex[i + 16]]  = analogRead(VOUT_ANA3);
  Analog[AnaIndex[i + 24]]  = analogRead(VOUT_ANA4);
  //  Analog[AnaIndex[i + 32]]  = analogRead(VOUT_ANA5);
  //  Analog[AnaIndex[i + 40]]  = analogRead(VOUT_ANA6);
  __asm__("nop\n\t");

  ++i;
  if (i == (NBR_POT >> 2)) { // '>>2' same as divide by 8
    i = 0;
  }
}


///////////////////////////////////////////////////////////
// Notify a change in Analog input
///////////////////////////////////////////////////////////
void AIN_NotifyChange() {
  //  if (timer_analog > AnalogSamplingPeriod) { // inutile
  for (int i = 0; i < NBR_POT; i++) {
    if (Analog_Debug[i] == LOW) {
      if (abs(Previous_Analog[i] - Analog[i]) > AnalogThreshold) { //ensure value changed more than our threshold

        Analog_Mapped[i] = (Analog[i] >> 3); //  ==> Analog_Mapped[i] = map(Analog[i], 0, 1022, 0, 127);
        if (booting)
        {
          Previous_Analog_Mapped[i] = Analog_Mapped[i];
          if (inTest)
          {
            // test mode
            Serial.print(F("ain_pin : ")); Serial.print(i);
            Serial.print(F(" / ain_pin_value : ")); Serial.println(Analog_Mapped[i]);

            // display
            lcd.setCursor(0, 1);
            lcd.print(F(" "));
            lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(PotDescription[i]))));
            //            lcd.setCursor(9, 1);
            //            lcd.print(F(" ain:"));
            //            LCD_PrintBCD2(i);
            //lcd.print(F(" "));
            LCD_PrintBCD3(Analog_Mapped[i]);
          }
        }
        if (Analog_Mapped[i] != Previous_Analog_Mapped[i]) {
          ain_pin = i;
          ain_pin_value = Analog_Mapped[i];

          if (booting)
          {
            return;
          }
          else
          {
            RefreshSoftPanel = 1;

#if DEBUG_analog
            // print something like : A0-### value
            Serial.print(F("AIN_NotifyChange()")); Serial.print(F("ain_pin ")); Serial.print(ain_pin); Serial.print(F(" ain_pin_value ")); Serial.println(ain_pin_value);
#endif

            if (ain_pin == 11)
            {
              LivePanel_HandleAin(ain_pin, 127 - ain_pin_value); // le bouton DCO MIX tourne dans le mauvais sens
            }
            else
              LivePanel_HandleAin(ain_pin, ain_pin_value); // consomme 1,8 KB de RAM !!! à optimiser
            //      appflag_updateDisplay = 1;
          }
        }
        Previous_Analog[i] = Analog[i]; //reset current value
        Previous_Analog_Mapped[i] = Analog_Mapped[i];
      }
    }
  }
  //    timer_analog = 0;
  RefreshSoftPanel = 0;
  //  }
}

///////////////////////////////////////////////////////////
// Recall du loop, sert au démarrage
///////////////////////////////////////////////////////////
void Recall()
{
  if (InitFlag)
  {
    //ANALOG INIT
    for (int i = 0; i < NBR_POT; i++)
    {
      if (Analog_Debug[i] == LOW)
      {
        Analog_Mapped[i] = (Analog[i] >> 3);
      }
    }
  }
}




