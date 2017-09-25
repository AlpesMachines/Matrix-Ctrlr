#ifndef seq_h
#define seq_h


extern unsigned char sequence[32][2]; //
extern unsigned char sequenceOrig[32][2]; //
extern const unsigned char Default_Sequence[10][32][2] ;

extern unsigned char seqOption; // false, rest or tie

extern bool ui_seqRec; // true or false, used in router.c and in ui_arp.ino
extern bool ui_seqPlay; // true or false, used in router and ui_arp
extern bool seqTrig; // sequence trigger using note On/Off
extern unsigned char seqNbr;
extern unsigned char seqLength;
extern unsigned char seqTick;
extern unsigned char playSeqTrigger[4]; // pitch, velo, channel, option
extern bool seqMonoMode;
extern unsigned char seqSpeed; // 0 - 255
extern signed char skipSeqStep;

extern void Insert_SeqRest(unsigned char param);

#endif
