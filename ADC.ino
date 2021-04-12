#include "lcd.h"
#include "pots.h"

///////////////////////////////////////////////////////////
// Globales variables
// ADC is 10 bits : int type necessary
///////////////////////////////////////////////////////////
unsigned int Analog[NBR_POT] = {0};
unsigned int Previous_Analog[NBR_POT] = {0};
bool Analog_Debug[NBR_POT] = {0}; // permet de supprimer un potard
unsigned int Analog_Mapped[NBR_POT] = {0};
unsigned int Previous_Analog_Mapped[NBR_POT] = {0};
//int Analog[NBR_POT] = {0};
//int Previous_Analog[NBR_POT] = {0};
//bool Analog_Debug[NBR_POT] = {0}; // permet de supprimer un potard
//unsigned char Analog_Mapped[NBR_POT] = {0};
//unsigned char Previous_Analog_Mapped[NBR_POT] = {0};

unsigned int filter_ratio; // = 1 to 9 value, filtering digital noise from PSU by Exponential Moving Average (EMA) technic

///////////////////////////////////////////////////////////
// lecture des potards - scanner
///////////////////////////////////////////////////////////
void ReadAnalog()
{
  //  static byte AnaIndex[NBR_POT] = {
  //    0, 1, 2, 3, 4, 5, 6, 7,
  //    8, 9, 10, 11, 12, 13, 14, 15,
  //    16, 17, 18, 19, 20, 21, 22, 23,
  //    24, 25, 26, 27, 28, 29, 30, 31
  //  };

  static unsigned char i; // i increment on each mainloop (iteration) to fasten mainloop (472 microsec !!!)
  setPin(i);
  // Each 'nop' statement executes in one machine cycle (at 16 MHz) yielding a 62.5 ns (nanosecond) delay (tPHL 150 ns). http://playground.arduino.cc/Main/AVR
  //__asm__("nop\n\t");
  delayMicroseconds (20);

  // we read pots 8 by 4 on each loop ;)
  Analog[i]       = (Analog[i] * (10 - filter_ratio) + analogRead(A0) * filter_ratio) / 10; // nouveauté 1.10c : digital noise filtering !!
  Analog[i + 8]   = (Analog[i + 8] * (10 - filter_ratio) + analogRead(A1) * filter_ratio) / 10; // (c) soundforce.nl -> EMA filtering (exponential mean average)
  Analog[i + 16]  = (Analog[i + 16] * (10 - filter_ratio) + analogRead(A2) * filter_ratio) / 10; // https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
  Analog[i + 24]  = (Analog[i + 24] * (10 - filter_ratio) + analogRead(A3) * filter_ratio) / 10; // https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
  //__asm__("nop\n\t");
  ++i;
  if (i >= 8) { 
    i = 0;
  }
}

///////////////////////////////////////////////////////////
// Notify a change in Analog input
///////////////////////////////////////////////////////////
void AIN_NotifyChange()
{
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
#if DEBUG_analog
            Serial.print(F("ain_pin : ")); Serial.print(i);
            Serial.print(F(" / ain_pin_value : ")); Serial.println(Analog_Mapped[i]);
#endif
            // display
            lcd.setCursor(0, 1);
            lcd.print(F(" "));
            lcd.print(strcpy_P(bufferProgmem, (PGM_P)pgm_read_word(&(PotDescription[i]))));
            LCD_PrintBCD3(Analog_Mapped[i]);
          }
        }

        if (Analog_Mapped[i] != Previous_Analog_Mapped[i]) {
          ain_pin = i;
          ain_pin_value = Analog_Mapped[i];

          if (booting)
            return;
          else
          {
            RefreshSoftPanel = 1;

#if DEBUG_analog
            // print something like : A0-### value
            Serial.print(F("AIN_NotifyChange()")); Serial.print(F("ain_pin = ")); Serial.print(ain_pin); Serial.print(F(", ain_pin_value = ")); Serial.println(ain_pin_value);
#endif

            if (ain_pin == 11)
              LivePanel_HandleAin(ain_pin, 127 - ain_pin_value); // le bouton DCO MIX tourne dans le mauvais sens
            else
              LivePanel_HandleAin(ain_pin, ain_pin_value); 
          }
        }
        Previous_Analog[i] = Analog[i]; //reset current value
        Previous_Analog_Mapped[i] = Analog_Mapped[i];
      }
    }
  }
  // booting = 0; -> géré dans la loop() car utilisé par DIN_Notify
  RefreshSoftPanel = 0;
}

///////////////////////////////////////////////////////////
// Recall du loop, sert au démarrage ANALOG INIT
///////////////////////////////////////////////////////////
void Recall()
{
  if (InitFlag)
  {
    for (int i = 0; i < NBR_POT; i++)
    {
      if (Analog_Debug[i] == LOW)
      {
        Analog_Mapped[i] = (Analog[i] >> 3);
      }
    }
  }
}
