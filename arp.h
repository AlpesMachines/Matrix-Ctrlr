#ifndef arp_h 
#define arp_h

// STANDALONE ARPEGGIATOR
#define EXT_MIDI_TRIG 0x0f // = channel 16, whatever is the event
#define EXT_CV_TRIG 0x01 // trig for the arp and seq on jack J1 (+5V DC 1 ms pulse)

extern byte mClock; // clock tick
extern unsigned int ui_aSpeed;
extern unsigned char ui_aGate;
extern unsigned char arp_div_index; 
extern unsigned char ui_aGroove;

extern const unsigned int arp_div[14];
extern const unsigned char ui_ui_aSpeed[14][5];

extern unsigned char ui_aMotif;
extern unsigned char ui_aMltp;
extern bool ui_aHold;

extern bool active_arp;
extern bool router_arp_tag;
extern bool router_seq_tag;
extern bool arp_send_notes;
extern unsigned char pattern_number;

extern unsigned char seqTick;

extern void ArpParameters_Load(unsigned char device);
extern void ArpParameters_Store(unsigned char device);
extern void ArpParameters_Init(unsigned char device);
extern void ARP_GLOBAL_INIT(unsigned char device);

extern void ARP2(void);

extern void Active_Arp(unsigned char state);
extern void ArpegiateOption(unsigned char device_arp, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2, unsigned char option);


#endif
