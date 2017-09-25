/*
    memo.h


    Created by Julien VOIRIN on 07/02/11. on midibox plateform
    Copyright 2011 __MyCompanyName__. All rights reserved.

*/
#ifndef MEMO_H
#define MEMO_H

//#include "main.h"
//#include <cmios.h>

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// global definitions
/////////////////////////////////////////////////////////////////////////////
#define BITING 0
#define PAGING 1

/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////


//unsigned char PATCH_ReadByte(unsigned char addr);
//void PATCH_WriteByte(unsigned char addr, unsigned char biite);

unsigned char PATCH_Load(unsigned char bank, unsigned char patch);
unsigned char PATCH_Store(unsigned char bank, unsigned char patch, bool fast);

extern void Read_Patch_From_BS(unsigned char device, unsigned char bank, unsigned char patch);
extern void Write_Patch_To_BS(unsigned char device, unsigned char bank, unsigned char patch);
extern void Write_Bank_To_BS(unsigned char device, unsigned char bank, unsigned char patch);
extern void Store_LastBankPatch_to_EEPROM(unsigned char device, unsigned char bank, unsigned char patch);
extern void Recall_LastBankPatch_from_EEPROM(unsigned char device);

extern void PATCH_Init(unsigned char device);
//extern void Clear_BS(unsigned char bankstick);



#endif
