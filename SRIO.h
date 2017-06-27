#ifndef SRIO_h
#define SRIO_h
#include "Arduino.h"


#define _latchPinOut 12
#define _dataPinOut 13
#define _latchPinIn 10
#define _dataPinIn  11
#define _clockPin 9

class SRIO
{
  public:
    void Initialize();
    uint8_t shiftIn_SRIO();
    void shiftOut_SRIO(uint8_t val);
    void Led_SR_Write (byte SR_num, byte data);
    void Led_Pin_Write (byte Pin_num, byte flag);
    void Led_All_On();
    void Led_All_Off();
    byte Led_SR_Read (byte SR_num);
    byte Led_Pin_Read (byte Pin_num);
    byte Button_SR_Read (byte SR_num);
    byte Button_Pin_Read (byte Pin_num);
  private:
    byte _SR[8];
    byte _data[8];
};

extern SRIO SR;

#endif // Fin si

