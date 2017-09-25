#include <SRIO_mxctr.h>
#include "din.h"

// cool functions :)
// https://github.com/paradajz/OpenDeck/blob/master/src/Firmware/core/helpers/BitManipulation.h
//
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define invertByte(value) ((value) ^ 0xFF)

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
void DOUT_PinSet(unsigned char dout_pin ,  unsigned char dout_pin_state)
{
  SR.Led_Pin_Write(dout_pin, dout_pin_state);
}

////////////////////////////////////////////////////////////////////
void DOUT_PinSet0(unsigned char dout_pin)
{
  SR.Led_Pin_Write(dout_pin, 0);
}

////////////////////////////////////////////////////////////////////
void DOUT_PinSet1(unsigned char dout_pin)
{
  SR.Led_Pin_Write(dout_pin, 1);
}

////////////////////////////////////////////////////////////////////
void DOUT_PinSet_Keypanel(unsigned char key1_led, unsigned char key2_led, unsigned char key3_led, unsigned char key4_led, unsigned char key5_led, unsigned char key_page_led)
{
  SR.Led_Pin_Write(DOUT_DCO, key1_led);
  SR.Led_Pin_Write(DOUT_FILTER, key2_led);
  SR.Led_Pin_Write(DOUT_ENV,key3_led);
  SR.Led_Pin_Write(DOUT_KBD,key4_led);
  SR.Led_Pin_Write(DOUT_MATRIXMOD,key5_led);
  SR.Led_Pin_Write(DOUT_PAGE,key_page_led); 
}

////////////////////////////////////////////////////////////////////
unsigned char DOUT_PinGet(unsigned char pin)
{
  // return Button_Pin_Read (pin);
  return SR.Led_Pin_Read(pin);
  // return 0;
}
 
////////////////////////////////////////////////////////////////////
void LCD_CursorSet(unsigned char cell)
{
  // void LCD_CursorSet(unsigned char pos)
  //sets the text cursor on LCD screen
  //0x00-0x3f: first line
  //0x40-0x7f: second line

  int row, col;
  if (cell < 0x40) {
    row = 0;
    col = cell;
  } else {
    row = 1;
    col = cell - 0x40;
  }
  lcd.setCursor(col, row);
}

 
////////////////////////////////////////////////////////////////////
void LCD_Clear(void)
{
  lcd.clear();
}
 
////////////////////////////////////////////////////////////////////
void LCD_PrintBCD3(int value)
{
  if (value < 100) {
    lcd.print(" ");
    if (value < 10)
      lcd.print(" ");
    lcd.print(value, DEC);
  } else {
    lcd.print(value, DEC);
  }
}

 
////////////////////////////////////////////////////////////////////
void LCD_PrintBCD2(int value)
{
  if (value < 10) {
    lcd.print(" ");
    lcd.print(value, DEC);
  } else {
    lcd.print(value, DEC);
  }
}

 
////////////////////////////////////////////////////////////////////
void LCD_PrintBCD1(int value)
{
  lcd.print(value, DEC);
}

 
////////////////////////////////////////////////////////////////////
void LCD_PrintChar(char caractere)
{
  lcd.print(caractere);
}

 
////////////////////////////////////////////////////////////////////
void LCD_PrintCString( String msg)
{
  lcd.print (msg);
}

////////////////////////////////////////////////////////////////////
void LCD_PrintHex2(unsigned char data)
{
  lcd.print (data, HEX);
}


