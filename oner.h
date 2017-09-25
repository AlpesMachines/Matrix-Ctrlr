#ifndef oner_h
#define oner_h

#define Z_ACTIVE 0
#define Z_TRANSPOSE_MIDDLE 5 // 5 octav x 12 semitons = 60

// 4 zones available
#define ZONEA 0
#define ZONEB 1
#define ZONEC 2
#define ZONED 3

// of 5 parameters : receipt channel, lower limit, transpose, higher limit, transmit channel
#define Z_RCH 0
#define Z_LOW 1
#define Z_TRANSPOSE 2
#define Z_HIGH 3
#define Z_TCH 4



extern const unsigned char defaultZONE[4][5] ;

// var
extern unsigned char ZONE[4][5];
//extern const unsigned char defaultZONE[4][5];
extern bool zActive;
extern unsigned char ui_Zone;


// fcts
extern void ZONE_Init(void);
extern void ZONE_Save(void);
extern void ZONE_Load(void);

extern void ZoneNoteOff(byte channel, byte pitch, byte velocity);
extern void ZoneNoteOn(byte channel, byte pitch, byte velocity);



#endif
