/*
   memo.ino
   Manage read/write into external/internal EEPROM 24LC512 Microchip

   Created by Julien VOIRIN on 07/02/11.
   Copyright 2017 Alpes Machines. All rights reserved.

*/

// TWI and Wire http://playground.arduino.cc/Main/WireLibraryDetailedReference


/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////
//#include <cmios.h>
#include <EEPROM.h>
#include <EEPROM24LC256_512.h>
#include <Wire.h>
#include "chaosmatrix.h"
#include "din.h"
#include "memo.h"
#include "ui_patch.h"
#include "seq.h"
#include "arp.h"


/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////
// the patch structure (could also be located somewhere else, depending on
// where and how you are storing values in RAM)
//unsigned char patch_structure[134];
unsigned char patch_structure[219]; //134+20+1+64=219
unsigned char data128A[128];

/////////////////////////////////////////////////////////////////////////////
// This function returns a byte from patch structure in RAM
/////////////////////////////////////////////////////////////////////////////
unsigned char PATCH_ReadByte(unsigned char addr)
{
  return patch_structure[addr];
}

/////////////////////////////////////////////////////////////////////////////
// This function writes a byte into patch structure in RAM
/////////////////////////////////////////////////////////////////////////////
void PATCH_WriteByte(unsigned char addr, unsigned char biite)
{
  patch_structure[addr] = biite;
}


/////////////////////////////////////////////////////////
// This function read a patch stored into BS
// This function loads the patch structure from EEPROM/BankStick
// Returns != 0 if Load failed (e.g. BankStick not connected)
////////////////////////////////////////////////////////
unsigned char PATCH_Load(unsigned char bank, unsigned char patch)
{
  unsigned char i = 0;
  unsigned char mem;

  if (PATCH_USE_BANKSTICK)
  {

    if (ext_EEPROM_PAGING) // paging eeprom
    { // offset page = patch + 100 x bank car y a 100 patch par banque
      if (bank < 5)
      {
        mem_1.readPage(patch + (bank * 100), data128A); //delay(5);  // page addr = 0 ... 511
        for (i = 0; i < 128; i++) {
          patch_structure[i] = data128A[i];
        }

        mem_2.readPage(patch + (bank * 100), data128A); //delay(5);
        for (i = 0; i < 91; i++) {
          patch_structure[128 + i] = data128A[i];
        }
        mem = 12;
      }
      else
      {
        mem_3.readPage(patch + ((bank - 5) * 100), data128A); //delay(5);  // page addr = 0 ... 511
        for (i = 0; i < 128; i++) {
          patch_structure[i] = data128A[i];
        }

        mem_4.readPage(patch + ((bank - 5) * 100), data128A); //delay(5);
        for (i = 0; i < 91; i++) {
          patch_structure[128 + i] = data128A[i];
        }
        mem = 34;
      }
#if DEBUG_eeprom
      Serial.print(F("Patch Load extEEPROM paging offset = ")); Serial.println(patch + (bank * 100), DEC);
      Serial.print(F("Patch Load extEEPROM mem_")); Serial.println(mem, DEC);
      Serial.println();
#endif
    }
    else // not paging eeprom // obsolete code prior june 2016
    {
      for ( i = 0; i < 134; i++) {
        if ((bank > 4 ) && ( patch > 50 ))
          patch_structure[i] = mem_2.readByte((bank + 1) * (patch + 1) * 134 - 134 + i); // ça fonctionne :) offset
        else
          patch_structure[i] = mem_1.readByte((bank + 1) * (patch + 1) * 134 - 134 + i);
        //delay(5);
      }
#if DEBUG_eeprom
      Serial.print(F("Patch Load extEEPROM offset = ")); Serial.println((bank + 1) * (patch + 1) * 134 - 134, DEC); Serial.println();
#endif
    }

  }
  else
  {
    // ignore patch and bank, internal eeprom writing OK 24.03.2016
    for (i = 0; i < 134; ++i) {
      patch_structure[i] = EEPROM.read(10 + patch * 134 + i); // 10 is the first cell containing editbuffer values
      delay(5);
    }

#if DEBUG_inteeprom
    Serial.print(F("Patch Load intEEPROM addr = ")); Serial.println(10 + patch * 134, DEC); Serial.println();
#endif

  }
  return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// This function stores the patch structure into EEPROM/BankStick
// Returns != 0 if Store failed (e.g. BankStick not connected)
///////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char PATCH_Store(unsigned char bank, unsigned char patch)
{
  unsigned char i;
  unsigned char mem;

#if PATCH_USE_BANKSTICK

  if (ext_EEPROM_PAGING) // paging eeprom
  { // offset page = patch + 100xbank
    if (bank < 5)
    {
      for (i = 0; i < 128; i++) {
        data128A[i] = patch_structure[i]; // data of EditBuffer
      }
      mem_1.writePage(patch + (bank * 100), data128A); //delay(5);

      for (i = 0; i < 91; i++) {
        data128A[i] = patch_structure[128 + i]; // patchname of EditBuffer
      }
      mem_2.writePage(patch + (bank * 100), data128A); //delay(5);
      mem = 12;
    }
    else
    {
      for (i = 0; i < 128; i++) {
        data128A[i] = patch_structure[i];
      }
      mem_3.writePage(patch + ((bank - 5) * 100), data128A); //delay(5);

      for (i = 0; i < 91; i++) {
        data128A[i] = patch_structure[128 + i];
      }
      mem_4.writePage(patch + ((bank - 5) * 100), data128A); //delay(5);
      mem = 34;
    }
#if DEBUG_eeprom
    Serial.print(F("Patch Store extEEPROM paging offset = ")); Serial.println(patch + (bank * 100), DEC);
    Serial.print(F("Patch Load extEEPROM mem_")); Serial.println(mem, DEC);
    Serial.println();
#endif
  }
  else // byte by byte eeprom : onsolete limited to 512
  {
    for (i = 0; i < 134; ++i) {
      if ((bank > 4 ) && ( patch > 50 ))
        mem_2.writeByte((bank + 1) * (patch + 1) * 134 - 134 + i, patch_structure[i] ); // ça marche :) offset
      else
        mem_1.writeByte((bank + 1) * (patch + 1) * 134 - 134 + i, patch_structure[i] );
      //          delay(5);
    }
#if DEBUG_eeprom
    Serial.print(F("Patch Store extEEPROM offset = ")); Serial.println((bank + 1) * (patch + 1) * 134 - 134, DEC); Serial.println();
#endif
  }

#else
  // use internal EEPROM
  for ( i = 0; i < 134; ++i) {
    EEPROM.write(10 + patch * 134 + i, patch_structure[i] );
    delay(5);
  }

#if DEBUG_inteepro
  Serial.print(F("Patch Store intEEPROM, addr = ")); Serial.println(10 + patch * 134, DEC ); Serial.println();
#endif

#endif
  return 0;
}


/////////////////////////////////////////////////////////////////////////////
/// read a patch from a BS
/////////////////////////////////////////////////////////////////////////////
void Read_Patch_From_BS(unsigned char bank, unsigned char patch)
{
  unsigned char j;

  PATCH_Load(bank, patch);
  // matrix patch
  for ( j = 0; j < 134; j++) {
    EditBuffer[j] = patch_structure[j];
    EditBufferOrig[j] = patch_structure[j];
  }

  // unison detune
  if (GliGliON)
  {
    UnisonDetune = patch_structure[134];
    if (UnisonDetune > 63) //limit
      UnisonDetune = 63;
  }
  else
    UnisonDetune = 0;


  // arp parameters
  for ( j = 0; j < 20; j++) {
    ArpParameters[j] = patch_structure[j + 135];
  }
  // shift the array into variables :
  ArpLoad();

  //sequence associated to patch
  for ( j = 0; j < 32; j++) { // end:220
    sequence[j][0] = patch_structure[155 + (2 * j + 0)];
    sequence[j][1] = patch_structure[155 + (2 * j + 1)];
  }



  //  if (ext_EEPROM_PAGING) { // paging read eeprom
  //    PATCH_Load(bank, patch);
  //    for (j = 0; j < 134; ++j) {
  //      //    EditBufferOrig[j] = EditBuffer[j];
  //      EditBuffer[j] = patch_structure[j];
  //      EditBufferOrig[j] = patch_structure[j];
  //    }
  //  }
  //  else // not paging
  //  {
  //    PATCH_Load(bank, patch);
  //    for ( j = 0; j < 134; j++) {
  //      EditBuffer[j] = patch_structure[j];
  //      EditBufferOrig[j] = patch_structure[j];
  //    }
  //  }

  //	Recall_LastBankPatch_from_EEPROM(); // <-- surtout ne pas mettre ça crée une boucle sur uBS uPatch
  UpdateDinStates();

#if DEBUG_eeprom
  Serial.print(F("Read patch from extEEPROM")); Serial.print(F(" bank = ")); Serial.print(bank, DEC); Serial.print(F(" patch = " )); Serial.println(patch, DEC);
  Serial.print(F("editbuffer = patch_structure[] : "));
  for (j = 0; j < 134; j++) {
    Serial.print(patch_structure[j], HEX); Serial.print(F(" "));
  }
  Serial.println(F("end"));
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// write a single patch to a BS
/////////////////////////////////////////////////////////////////////////////
void Write_Patch_To_BS(unsigned char bank, unsigned char patch)
{
  unsigned char j;

  if (ext_EEPROM_PAGING) // paging write eeprom
  {
    for ( j = 0; j < 134; j++) {
      patch_structure[j] = EditBuffer[j];
    }

    // unison detune
    if (GliGliON)
      patch_structure[134] = UnisonDetune;
    else
      patch_structure[134] = 0;

    // arp parameters
    ArpStore(); // place the indiv var into the array, then cpy arrays between them
    for ( j = 0; j < 20; j++) {
      patch_structure[j + 135] = ArpParameters[j];
    }

    //sequence associated to patch
    for ( j = 0; j < 32; j++) { // end:220
      patch_structure[155 + (2 * j + 0)] = sequence[j][0];
      patch_structure[155 + (2 * j + 1)] = sequence[j][1];
    }

    PATCH_Store(bank, patch);
  }
  else // not paging
  {
    for ( j = 0; j < 134; j++) {
      patch_structure[j] = EditBuffer[j];
    }

    patch_structure[134] = UnisonDetune;

    // arp parameters
    for ( j = 135; j < 155; j++) {
      patch_structure[j] = ArpParameters[j - 135];
    }

    //sequence associated to patch
    for ( j = 0; j < 32; j++) { // end:220
      patch_structure[155 + (2 * j + 0)] = sequence[j][0];
      patch_structure[155 + (2 * j + 1)] = sequence[j][1];
    }
    PATCH_Store(bank, patch);
  }
  Store_LastBankPatch_to_EEPROM(bank, patch);


#if DEBUG_eeprom
  Serial.print(F("Write patch to extEEPROM")); Serial.print(F(" bank = ")); Serial.print(bank, DEC); Serial.print(F(" patch = " )); Serial.println(patch, DEC);
  Serial.print(F("patch_structure = editbuffer[] : "));
  for (j = 0; j < 134; j++) {
    Serial.print(patch_structure[j], HEX); Serial.print(F(" "));
  }
  Serial.println(F(" end"));
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// write a bank dump (several patches) to a BS
/////////////////////////////////////////////////////////////////////////////
void Write_Bank_To_BS(unsigned char bank, unsigned char patch)
{
  unsigned char j;

  if (ext_EEPROM_PAGING)
  {
    for ( j = 0; j < 134; j++) {
      patch_structure[j] = EditBuffer[j];
    }
    PATCH_Store(bank, patch);
  }
  else
  {
    for (j = 0; j < 134; j++) {
      patch_structure[j] = EditBuffer[j]; // ?? works ?? yep :)
    }
    PATCH_Store(bank, patch);
  }

#if DEBUG_eeprom
  Serial.print(F("Write bank extEEPROM : ")); Serial.println(bank, DEC); Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// store in arduino mega EEPROM last bank and patch number used
////////////////////////////////////////////////////////////////////////////
void Store_LastBankPatch_to_EEPROM(unsigned char bank, unsigned char patch)
{
  EEPROM.write(EEPROM_LASTBANK, bank);
  EEPROM.write(EEPROM_LASTPATCH, patch);
#if DEBUG_inteeprom
  Serial.print(F("Store Last bank intEEPROM addr ")); Serial.print(EEPROM_LASTBANK); Serial.print(" bank = "); Serial.println(bank, DEC);
  Serial.print(F("Store Last patch intEEPROM addr ")); Serial.print(EEPROM_LASTPATCH); Serial.print(F(" patch = " )); Serial.println(patch, DEC);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// recall last bank and patch number used in arduino mega EEPROM
////////////////////////////////////////////////////////////////////////////
void Recall_LastBankPatch_from_EEPROM(void)
{
  uBank = EEPROM.read(EEPROM_LASTBANK);
  uPatch = EEPROM.read(EEPROM_LASTPATCH);
#if DEBUG_inteeprom
  Serial.print(F("Recall Last bank intEEPROM addr ")); Serial.print(EEPROM_LASTBANK); Serial.print(F(" uBank = ")); Serial.println(uBank, DEC);
  Serial.print(F("Recall Last patch intEEPROM addr ")); Serial.print(EEPROM_LASTPATCH); Serial.print(F(" uPatch = " )); Serial.println(uPatch, DEC);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// load on boot
/////////////////////////////////////////////////////////////////////////////
void PATCH_Init(void)
{
  // recall last bank patch used in previous session
  Recall_LastBankPatch_from_EEPROM();

  // read editbuffer coresponding
  Read_Patch_From_BS(uBank, uPatch);

  app_flags.Display_DIN_Req = 1;

  // send editbuffer to Mxi
  SendEditBuffer(INTERFACE_SERIAL);
  // send editbuffer to core out
  SendEditBuffer(INTERFACE_SERIAL3);

  // send Unison detune value TO DO :
  //send.controlChange(interface etc);
  // load arp parameters
  ArpLoad();


#if DEBUG_eeprom
  Serial.print(F("PATCH_Init() "));
  Serial.println();
#endif
}


