#ifndef ui_patch_h
#define ui_patch_h

extern unsigned char uBank[4];
extern unsigned char uPatch[4];

extern unsigned char ProgramNumberReq;
extern unsigned char BankNumberReq;

extern unsigned char BankNumberDump;

void UI_Display_Patch(void);
void UI_Handle_Patch(void);

#endif
