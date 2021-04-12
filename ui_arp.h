#ifndef ui_arp_h
#define ui_arp_h

extern bool ui_SetAB;
extern bool ui_aSplitLearning;
extern unsigned char ui_aSplit;
extern unsigned char ui_TrspA;
extern unsigned char ui_TrspB;
extern bool ui_toggleSeq;

void UI_Display_Arp(void);
void UI_Handle_Arp(void);
void ui_aSpeedGateGrooveLimits(void);

#endif
