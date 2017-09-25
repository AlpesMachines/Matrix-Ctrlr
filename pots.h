#ifndef pots_h
#define pots_h

typedef struct {
//  const  char description[17];
  const unsigned char param;
  const unsigned char Min;
  const unsigned char Max;
  const unsigned char valtype;
  const unsigned char update_interval;
} pots;

extern const char * const PotDescription[] PROGMEM;

extern const pots PotConfigMap[NBR_POT + NBR_POT];

#endif
