//#include <cmios.h>
//
//#include "main.h"
#include "chaosmatrix.h"
#include "pots.h"
#include "din.h"
#include "lcd.h"

// https://zestedesavoir.com/tutoriels/374/gestion-de-la-memoire-sur-arduino/
// http://www.nongnu.org/avr-libc/user-manual/pgmspace.html PROGMEM + pgm_read_byte_near(& )
char bufferProgmem[17];

// char 255 = solid block, char 160 = blank, 176 = hyphen = - , 4 = [ , 5 = ]

const unsigned char barmap_signed[16][16] PROGMEM = { // solid bar 8-15 // 0-7
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176}, // 0
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 255, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 255, 255, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 255, 255, 255, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 255, 255, 255, 255, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 255, 255, 255, 255, 255, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 255, 255, 255, 255, 255, 255, 255}, // 15
  {255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176}, // 8
  {176, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176} // 14
};

// char 255 = solid block, char 160 = blank, char 176 = -
const unsigned char barmap_mix[16][16] PROGMEM =
{
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 255},
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 255, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 176, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {176, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176} // DJ crossfader style


};

// char 255 = solid block, char 160 = blank
const unsigned char barmap_linear[16][16] PROGMEM =
{
  {176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176}, // 0
  {255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 176, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 176, 176},
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 176} // 15
};

// https://www.arduino.cc/en/Reference/LiquidCrystalCreateChar
const unsigned char customChar0[] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
}; // matrice ensemble vide

const unsigned char customChar1[] PROGMEM = {
  0b10000,
  0b10000,
  0b10000,
  0b11111,
  0b10000,
  0b10000,
  0b10000,
  0b10000
};

const unsigned char customChar2[] PROGMEM = {
  0b11000,
  0b11000,
  0b11000,
  0b11111,
  0b11000,
  0b11000,
  0b11000,
  0b11000
};

const unsigned char customChar3[] PROGMEM = {
  0b11100,
  0b11100,
  0b11100,
  0b11111,
  0b11100,
  0b11100,
  0b11100,
  0b11100
};

const unsigned char customChar4[] PROGMEM = {
  0b11110,
  0b11110,
  0b11110,
  0b11111,
  0b11110,
  0b11110,
  0b11110,
  0b11110
};

const unsigned char customChar5[] PROGMEM = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

/////////////////////////////////////////////////////////////////////////////
//  Initialized LCD variables
/////////////////////////////////////////////////////////////////////////////
void LCD_Init()
{
  // https://www.arduino.cc/en/Reference/LiquidCrystalCreateChar
  // https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions
  // https://github.com/hjgode/sunrise_alarm_clock/blob/master/sunrise_alarm_clock.ino
  // https://github.com/hjgode/sunrise_alarm_clock/blob/master/SpechialChars.h
  // complicated ! finally without any bug on display 5/mars/2017 v0.99s :

  byte customChar[8];

  // init custom characters:
  for (int k = 0; k < 8; k++)
    customChar[k] = pgm_read_byte_near(customChar0 + k);
  lcd.createChar(0, customChar);

  for (int k = 0; k < 8; k++)
    customChar[k] = pgm_read_byte_near(customChar1 + k);
  lcd.createChar(1, customChar);

  for (int k = 0; k < 8; k++)
    customChar[k] = pgm_read_byte_near(customChar2 + k);
  lcd.createChar(2, customChar);

  for (int k = 0; k < 8; k++)
    customChar[k] = pgm_read_byte_near(customChar3 + k);
  lcd.createChar(3, customChar);

  for (int k = 0; k < 8; k++)
    customChar[k] = pgm_read_byte_near(customChar4 + k);
  lcd.createChar(4, customChar);

  for (int k = 0; k < 8; k++)
    customChar[k] = pgm_read_byte_near(customChar5 + k);
  lcd.createChar(5, customChar);

  // add more below if needed and create the necessary const unsigned char arrays

#if DEBUG_bargraph
  Serial.println(F("LCD_Init() "));
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  The live panel displays original value on the upper left corner
/////////////////////////////////////////////////////////////////////////////
void LCD_DisplayEditBufferOrig(unsigned char value, unsigned char valtype)
{
  LCD_CursorSet(0);

  switch (valtype)
  {
    case MIXBALANCE:
    case UNSIGNED6:
    case UNSIGNED7:
      LCD_PrintBCD3(value);
      break;

    case SIGNED6:
      display6BitSigned(value);
      break;

    case SIGNED7:
      LCD_Display7BitSigned(value);
      break;

  }

  LCD_PrintChar(LCD_PARAMSEPARATOR);
}

////////////////////////////////////////////////////////////////////////////
//  The live panel displays its current value on the lower left corner
/////////////////////////////////////////////////////////////////////////////
void LCD_DisplayParamValue(unsigned char value, unsigned char valtype)
{
  lcd.setCursor(0, 1);

  switch (valtype)
  {
    case MIXBALANCE:
    case UNSIGNED6:
    case UNSIGNED7:
      LCD_PrintBCD3(value);
      break;

    case SIGNED6:
      display6BitSigned(value);
      break;

    case SIGNED7:
      LCD_Display7BitSigned(value);
      break;
  }
  lcd.setCursor(3, 1); 
  lcd.print(F(" ")); // blank space
}

/////////////////////////////////////////////////////////////////////////////
//  Display description of button values, eg. LFO1 wave 'Up Sawtooth'
/////////////////////////////////////////////////////////////////////////////
void LCD_DisplayParamValueDescription(const char *data)
{
  lcd.setCursor(4, 1);
  lcd.print(data);

#if DEBUG_LCDparamvaluedescription
  Serial.print (F("LCD_DisplayParamValueDescription ")); Serial.println(data);
#endif
}

////////////////////////////////////////////////////////////////////////////
//  Describes the parameter that was changed on the live panel (only DIN)
/////////////////////////////////////////////////////////////////////////////
void LCD_DisplayParamDescription(const char *data)
{
  lcd.setCursor(4, 0);
  lcd.print(data);

#if DEBUG_LCDparamvaluedescription
  Serial.print (F("LCD_DisplayParamDescription ")); Serial.println(data);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Displays a bargraph on the lower row
/////////////////////////////////////////////////////////////////////////////
void LCD_DisplayBarGraph (unsigned char bartype, unsigned char value)
{
  // N.B NE PLUS Y TOUCHER CA MARCHE PARFAITEMENT !!! v6.3

  unsigned char solid_bars = 0;
  unsigned char tiny_bars = 0;
  unsigned char i;

  lcd.setCursor(LCD_BARGRAPHPOSITION + LCD_Offset, 1);

  switch (bartype) {

    case UNSIGNED6: // 0/63
      solid_bars = (value >> 2); // get max 16 bars from value of 0-63
      tiny_bars =  ((value << 1) & 7 ) >> 1; // formule Julien arduino
      for ( i = 0; i < 16; i++)
        lcd.write (byte(pgm_read_byte_near( &barmap_linear[solid_bars][i])));
      lcd.setCursor(LCD_BARGRAPHPOSITION + solid_bars + LCD_Offset, 1);
      //          LCD_PrintChar(barmap_fractional[tiny_bars]);
      lcd.write(byte(tiny_bars));
      break;

    case SIGNED7: // -63/+63
      solid_bars = value >> 3;
      for ( i = 0; i < 16; i++)
        lcd.write(byte( pgm_read_byte_near( &barmap_signed[solid_bars][i])));
      break;

    case UNSIGNED7: // 0/127
      solid_bars = value >> 3;
      tiny_bars =  (((value) & 7 ) + 1 ) >> 1; // formule Julien arduino qui marche :)
      for ( i = 0; i < 16; i++)
        lcd.write(byte(pgm_read_byte_near( &barmap_linear[solid_bars][i])));
      lcd.setCursor(LCD_BARGRAPHPOSITION + solid_bars + LCD_Offset , 1);
      lcd.write(byte(tiny_bars)); // customChar = tiny_bars
      break;

    case SIGNED6: // there's only one of these, the detune -32/+31
      solid_bars = value >> 3; // on peut pas faire mieux :(
      for ( i = 0; i < 16; i++)
        lcd.write(byte(pgm_read_byte_near( &barmap_signed[solid_bars][i])));
      break;

    case MIXBALANCE: // balance DCO1 / DCO2
      solid_bars = ((value * 2) >> 3);
      for ( i = 0; i < 16; i++)
        lcd.write(byte(pgm_read_byte_near( &barmap_mix[solid_bars][i])));
      break;
  }

  // cette fonction est à améliorer car ça bouffe 800 octets de RAM !!! PROGMEM -> c'est fait 4.4.2016
  // https://www.arduino.cc/en/Reference/PROGMEM

#if DEBUG_bargraph
  Serial.print(F("solid bars = ")); Serial.print(solid_bars, DEC); Serial.print(F(" tiny_bars = ")); Serial.println(tiny_bars, DEC);
#endif
}


/////////////////////////////////////////////////////////////////////////////
//  requires 3 spaces, set cursor before using
//  displays a 7bit value using - and + signs
/////////////////////////////////////////////////////////////////////////////
void LCD_Display7BitSigned(unsigned char value)
{
  if (value == 0)
  {
    lcd.print("  0");
  }
  else if ( value < 64 )
  {
    lcd.print('+');
    LCD_PrintBCD2(value & 0x3f); //
  }
  else
  {
    lcd.print('-');
    LCD_PrintBCD2( 64 - (value & 0x3f)); // 0x3f
  }
}

/////////////////////////////////////////////////////////////////////////////
//  requires 3 spaces, set cursor before using
//  displays a 6bit value using - and + signs
/////////////////////////////////////////////////////////////////////////////
void display6BitSigned(unsigned char value)
{
  if (value == 0)
  {
    lcd.print("  0");
  }
  else if ( value < 32 )
  {
    lcd.print('+');
    LCD_PrintBCD2(value & 0x1f); // 0x1f
  }
  else
  {
    lcd.print('-');
    LCD_PrintBCD2(32 - (value & 0x1f)); // 0x1f
  }
}

