#ifndef lcd_h
#define lcd_h

#define LCDCHAR_CENTERDOT       165
#define LCDCHAR_ARROW_RIGHT     126
#define LCDCHAR_ARROW_LEFT      127
#define LCDCHAR_HYPHEN          176
#define LCDCHAR_BRACKET_RIGHT   93
#define LCDCHAR_BRACKET_LEFT    91
#define LCDCHAR_BLANK           160
#define LCDCHAR_VERTBAR         124

#define LCD_PARAMSEPARATOR LCDCHAR_CENTERDOT

#define LCD_PAGEARROWPOSITION           19
#define LCD_BANKPATCHPOSITION           0 // top row first column
#define LCD_POTVALPOSITION              64 // 0x40 second row, first column
#define LCD_SECONDROWSTARTPOSITION      64
#define LCD_POTVALDESCRIPTIONPOSITION   4 // second row, 5th column
#define LCD_BARGRAPHPOSITION            4
#define LCD_POTDESCRIPTIONPOSITION      4 // top row 4th column


// extern unsigned char RefreshSoftPanel;
extern char bufferProgmem[17];

extern void LCD_Init(void);
extern void LCD_DisplayEditBufferOrig(unsigned char value, unsigned char valtype);

extern void LCD_DisplayParamDescription( char *str); // live panel descr
extern void LCD_DisplayParamValue(unsigned char value, unsigned char valtype); // live panel value
extern void LCD_DisplayParamValueDescription( char *str); // live panel value descr

extern void LCD_DisplayPatchSelect(void);
extern void LCD_DisplayBarGraph (unsigned char bartype, unsigned char value);

extern void LCD_DisplayEditModePrompt(void);
extern void LCD_Display7BitSigned(unsigned char value);

#endif
