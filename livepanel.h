#ifndef livepanel_h
#define livepanel_h

extern void LivePanel_DinHandler(unsigned char pin);
extern void LivePanel_DisplayDin(unsigned char pin);
extern void LivePanel_BlinkLFOs(void);
extern void LivePanel_BlinkLEDs(void);

extern void LivePanel_Init(void);
extern void LivePanel_HandleAin(unsigned char pin , unsigned char pin_value);
extern void LivePanel_HandleTransmitDelay(void);
extern void LivePanel_DisplayAin();

#endif
