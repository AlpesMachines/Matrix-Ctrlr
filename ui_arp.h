#ifndef ui_arp_h
#define ui_arp_h

//
//extern unsigned char pattern_number;
//extern bool arp_send_notes;
//
//
extern bool ui_SetAB;
extern unsigned char ui_aSplit;
extern unsigned char ui_TrspA;
extern unsigned char ui_TrspB;
extern bool ui_toggleSeq;


void UI_Display_Arp(void);
void UI_Handle_Arp(void);
void ui_aSpeedGateGrooveLimits(void);

#endif



