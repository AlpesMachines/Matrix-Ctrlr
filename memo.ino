/*
   memo.ino
   Manage read/write into external/internal EEPROM 24LC512 Microchip

   Created by Julien VOIRIN on 07/02/11. on midibox plateform
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
unsigned char dataPage[128]; // ext EEPROM

/////////////////////////////////////////////////////////
// This function read a patch stored into BS
// This function loads the patch structure from EEPROM/BankStick
/////////////////////////////////////////////////////////
unsigned char PATCH_Load(unsigned char bank, unsigned char patch)
{
  unsigned char i;

  // offset page = patch + 100 x bank, car il y a 100 patch par bank
  if (bank < 5)
  {
#if DEBUG_eeprom
    Serial.print(F("Patch Load()/ (extEEPROM)/ paging / offset = ")); Serial.print(patch + (bank * 100), DEC); Serial.println(F(", mem_02"));
#endif

    mem_0.readPage(patch + (bank * 100), dataPage); // mem1 page addr = 0 ... 511
    for (i = 8; i < 134; i++)
      EditBufferOrig[i] = dataPage[i - 8]; // data of EditBufferOrig

    mem_1.readPage(patch + (bank * 100), dataPage); // mem2 page addr = 0 ... 511
    for (i = 0; i < 8; i++)
      EditBufferOrig[i] = dataPage[i]; // patchname of EditBufferOrig

    UnisonDetuneOrig = dataPage[8]; // unison data

    for (i = 0; i < 20; i++)
      ArpParametersOrig[i] = dataPage[i + 9]; // arp data

    for (i = 0; i < 32; i++) {
      sequenceOrig[i][0] = dataPage[i + 29];
      sequenceOrig[i][1] = dataPage[i + 29 + 32]; // seq data
    }

  }
  else
  {
#if DEBUG_eeprom
    Serial.print(F("Patch Load()/ (extEEPROM)/ paging / offset = ")); Serial.print(patch + (bank * 100), DEC); Serial.println(F(", mem_24"));
#endif

    mem_2.readPage(patch + ((bank - 5) * 100), dataPage); // page addr = 0 ... 511
    for (i = 8; i < 134; i++)
      EditBufferOrig[i] = dataPage[i - 8]; // data

    mem_3.readPage(patch + ((bank - 5) * 100), dataPage);
    for (i = 0; i < 8; i++)
      EditBufferOrig[i] = dataPage[i]; // patchname

    UnisonDetuneOrig = dataPage[8];

    for (i = 0; i < 20; i++)
      ArpParametersOrig[i] = dataPage[i + 9];

    for (i = 0; i < 32; i++) {
      sequenceOrig[i][0] = dataPage[i + 29];
      sequenceOrig[i][1] = dataPage[i + 29 + 32]; // seq data
    }
  }
  return 1;

}

///////////////////////////////////////////////////////////////////////////////
// This function stores the patch structure into EEPROM/BankStick
///////////////////////////////////////////////////////////////////////////////
unsigned char PATCH_Store(unsigned char bank, unsigned char patch, bool method)
{
  unsigned char i;

  // trick:
  // we could use 2 pages to store the 256 bytes as 2x128 but that cost twice RAM
  // hence it is cheaper to use twice same RAM
  // time to write into EEPROM is the same. Ratio Cost/Time better this way

  if (method == PAGING) // fast writing
  {
    // NOTA : page offset = patch + 100xbank
    if (bank < 5)
    {
      for (i = 8; i < 134; i++)
        dataPage[i - 8] = EditBufferOrig[i]; // tone of EditBuffer[device]
      dataPage[126] = dataPage[127] = 0; // free bytes

      mem_0.writePage(patch + (bank * 100), dataPage );

      for (i = 0; i < 8; i++)
        dataPage[i] = EditBufferOrig[i]; // patchname of EditBuffer[device]

      dataPage[8] = UnisonDetuneOrig; // unison data

      for (i = 0; i < 20; i++)
        dataPage[i + 9] = ArpParametersOrig[i]; // arp data

      for (i = 0; i < 32; i++) {
        dataPage[i + 29] = sequenceOrig[i][0]; // seq data
        dataPage[i + 29 + 32] = sequenceOrig[i][1];
      }

      mem_1.writePage(patch + (bank * 100), dataPage );

#if DEBUG_eeprom
      Serial.print(F("Patch Store()/ extEEPROM paging FAST / offset = ")); Serial.print(patch + (bank * 100), DEC); Serial.println(F("/ mem_02"));
#endif

    }
    else
    {
      for (i = 8; i < 134; i++)
        dataPage[i - 8] = EditBufferOrig[i]; // tone
      dataPage[126] = dataPage[127] = 0; // free bytes

      mem_2.writePage(patch + ((bank - 5) * 100), dataPage );

      for (i = 0; i < 8; i++)
        dataPage[i] = EditBufferOrig[i]; // name

      dataPage[8] = UnisonDetuneOrig; // unison data

      for (i = 0; i < 20; i++)
        dataPage[i + 9] = ArpParametersOrig[i]; // arp data

      for (i = 0; i < 32; i++) {
        dataPage[i + 29] = sequenceOrig[i][0]; // seq data
        dataPage[i + 29 + 32] = sequenceOrig[i][1];
      }

      mem_3.writePage(patch + ((bank - 5) * 100), dataPage );

#if DEBUG_eeprom
      Serial.print(F("Patch Store()/ extEEPROM/ paging FAST / offset = ")); Serial.print(patch + (bank * 100), DEC); Serial.println(F("/ mem_24"));
#endif

    }
    return 1;
  }
  else // normal behaviour of original library, write byte to byte (slow)
  {
    // NOTA : page offset = patch + 100xbank
    if (bank < 5)
    {
      for (i = 8; i < 134; i++)
        dataPage[i - 8] = EditBufferOrig[i]; // data of EditBuffer[device]

      for (i = 0; i < 128; i++)
        mem_0.writeByte(bank * 100 * 128 + patch * 128 + i, dataPage[i] );

#if DEBUG_eeprom
      Serial.print(F("Patch StoreBYTE() extEEPROM mem_0 offset = ")); Serial.println(bank * 100 * 128 + patch * 128 , DEC);
#endif

      for (i = 0; i < 8; i++)
        dataPage[i] = EditBufferOrig[i]; // patchname of EditBuffer[device]

      dataPage[8] = UnisonDetuneOrig; // unison data

      for (i = 0; i < 20; i++)
        dataPage[i + 9] = ArpParametersOrig[i]; // arp data

      for (i = 0; i < 32; i++) {
        dataPage[i + 29] = sequenceOrig[i][0]; // seq data
        dataPage[i + 29 + 32] = sequenceOrig[i][1];
      }

      for (i = 0; i < 128; i++)
        mem_1.writeByte(bank * 100 * 128 + patch * 128 + i, dataPage[i] );

#if DEBUG_eeprom
      Serial.print(F("Patch StoreBYTE() extEEPROM mem_1 offset = ")); Serial.println(bank * 100 * 128 + patch * 128 , DEC);
#endif

    }
    else
    {
      for (i = 8; i < 134; i++)
        dataPage[i - 8] = EditBufferOrig[i]; // patch data

      for (i = 0; i < 128; i++)
        mem_2.writeByte(bank * 100 * 128 + patch * 128 + i, dataPage[i] );

#if DEBUG_eeprom
      Serial.print(F("Patch StoreBYTE() extEEPROM mem_2 offset = ")); Serial.println(bank * 100 * 128 + patch * 128  , DEC);
#endif

      for (i = 0; i < 8; i++)
        dataPage[i] = EditBufferOrig[i]; // patch name

      dataPage[8] = UnisonDetuneOrig; // unison data

      for (i = 0; i < 20; i++)
        dataPage[i + 9] = ArpParametersOrig[i]; // arp data

      for (i = 0; i < 32; i++) {
        dataPage[i + 29] = sequenceOrig[i][0]; // seq data
        dataPage[i + 29 + 32] = sequenceOrig[i][1];
      }

      for (i = 0; i < 128; i++)
        mem_3.writeByte(bank * 100 * 128 + patch * 128 + i, dataPage[i] );

#if DEBUG_eeprom
      Serial.print(F("Patch StoreBYTE() extEEPROM mem_3 offset = ")); Serial.println(bank * 100 * 128 + patch * 128 , DEC);
#endif
    }

    return 2;
  }

}

/////////////////////////////////////////////////////////////////////////////
/// read a patch from a BS
/////////////////////////////////////////////////////////////////////////////
void Read_Patch_From_BS(unsigned char device, unsigned char bank, unsigned char patch)
{
  unsigned char j;

  PATCH_Load(bank, patch); // we feed the  arrays[]

  // matrix patch
  for ( j = 0; j < 134; j++)
    EditBuffer[device][j] = EditBufferOrig[j];

  // unison detune
#if GliGliON
  if (UnisonDetuneOrig > UDETUNE_MAX) // limit
    UnisonDetuneOrig = UDETUNE_MAX;
  UnisonDetune[device] = UnisonDetuneOrig;
#else
  UnisonDetune[device] = UnisonDetuneOrig = 0;
#endif

  // arp parameters copied from Orig
  for ( j = 0; j < 20; j++)
    ArpParameters[device][j] = ArpParametersOrig[j];

  //    // shift the array into variables :
  //    ArpParameters_Load(device);

  //sequence associated to patch
  if (device == Matrix_Device_A)
  {
    for ( j = 0; j < 32; j++)
    {
      sequence[j][0] = sequenceOrig[j][0];
      sequence[j][1] = sequenceOrig[j][1];
    }
  }
  else
  {
    for ( j = 0; j < 32; j++)
    {
      sequence[j][0] = sequence[j][0];
      sequence[j][1] = sequence[j][1];
    }
  }


  //	Recall_LastBankPatch_from_EEPROM(); // <-- surtout ne pas mettre ça crée une boucle sur uBS uPatch[device]
  //UpdateDinStates();

#if DEBUG_eeprom
  Serial.print(F("Read_Patch_From_BS()/ extEEPROM")); Serial.print(F(" / device = ")); Serial.print(device + 0x0a, HEX); Serial.print(F(" / bank = ")); Serial.print(bank, DEC); Serial.print(F(" / patch = " )); Serial.println(patch, DEC);
  Serial.print(F("EditBuffer[device][] = { "));
  for (j = 0; j < 134; j++) {
    Serial.print(EditBuffer[device][j], HEX); Serial.print(F(" "));
  }
  Serial.println(F("}/ end"));

  Serial.print(F("UnisonDetune[device] =  ")); Serial.println(UnisonDetune[device], DEC);

  Serial.print(F("ArpParameters[device][] = { "));
  for (j = 0; j < 20; j++) {
    Serial.print(ArpParameters[device][j], DEC); Serial.print(F(", "));
  }
  Serial.println(F("} "));

  Serial.print(F("sequence[] = { "));
  for (j = 0; j < 32; j++) {
    Serial.print(F("("));
    Serial.print(sequence[j][0], DEC); Serial.print(F(", "));
    Serial.print(sequence[j][1], DEC); Serial.print(F("), "));
  }
  Serial.println(F("} "));
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// write a single patch to a BS
/////////////////////////////////////////////////////////////////////////////
void Write_Patch_To_BS(unsigned char device, unsigned char bank, unsigned char patch)
{
  unsigned char j;

  for ( j = 0; j < 134; j++)
    EditBufferOrig[j] = EditBuffer[device][j];

  // unison detune
#if GliGliON
  UnisonDetuneOrig = UnisonDetune[device];
#else
  UnisonDetuneOrig = 0;
#endif

  // arp parameters
  //ArpParameters_Store(device); // place the indiv var into the array, then cpy arrays between them
  for ( j = 0; j < 20; j++)
    ArpParametersOrig[j] = ArpParameters[device][j];

  //sequence associated to patch
  if (device == Matrix_Device_A)
  {
    for ( j = 0; j < 32; j++)
    {
      sequenceOrig[j][0] = sequence[j][0];
      sequenceOrig[j][1] = sequence[j][1];
    }
  }
  else
  {
    for ( j = 0; j < 32; j++) {
      sequenceOrig[j][0] = sequenceOrig[j][0];
      sequenceOrig[j][1] = sequenceOrig[j][1];
    }
  }

#if DEBUG_eeprom
  Serial.print(F("Write_Patch_To_BS()")); Serial.print(F(" / device = ")); Serial.print(device + 0x0a, HEX); Serial.print(F(" / bank = ")); Serial.print(bank, DEC); Serial.print(F(" / patch = " )); Serial.println(patch, DEC);
  Serial.print(F("EditBufferOrig[] = { "));
  for (j = 0; j < 134; j++) {
    Serial.print(EditBufferOrig[j], HEX); Serial.print(F(" "));
  }
  Serial.println(F("}/ end"));

  Serial.print(F("UnisonDetuneOrig =  ")); Serial.println(UnisonDetuneOrig, DEC);

  Serial.print(F("ArpParametersOrig[] = { "));
  for (j = 0; j < 20; j++) {
    Serial.print(ArpParametersOrig[j], DEC); Serial.print(F(", "));
  }
  Serial.println(F("} "));

  Serial.print(F("sequenceOrig[] = { "));
  for (j = 0; j < 32; j++) {
    Serial.print(F("(")); Serial.print(sequenceOrig[j][0], DEC); Serial.print(F("; ")); Serial.print(sequenceOrig[j][1], DEC); Serial.print(F("), "));
  }
  Serial.println(F("} "));
#endif

  //PATCH_Store(bank, patch, BITING);
  PATCH_Store(bank, patch, PAGING);

  //Store_LastBankPatch_to_EEPROM(device, bank, patch);

#if DEBUG_eeprom
  Serial.print(F("END OF Write_Patch_To_BS()")); Serial.print(F(" / bank = ")); Serial.print(bank, DEC); Serial.print(F(" / patch = " )); Serial.println(patch, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// write a bank dump (several patches) in the 24LC512 : DON'T DEBUG SYSEXHANDLER OR WON'T WORK
/////////////////////////////////////////////////////////////////////////////
void Write_Bank_To_BS(unsigned char device, unsigned char bank, unsigned char patch)
{
  unsigned char j;

#if DEBUG_eeprom
  Serial.print(F("Write_Bank_To_BS()/ device = ")); Serial.print(device, DEC); Serial.print(F("/ bank = ")); Serial.print(bank, DEC); Serial.print(F("/ patch = ")); Serial.println(patch, DEC);
  Serial.println();
#endif

  for ( j = 0; j < 134; j++)
    EditBufferOrig[j] = EditBuffer[device][j];

  // section below removed in 1.02c
  // when GET, set this section. Using a tag like MIDI_REceivingBank wouldn't work
  // when uploading bank from computer :
  // INIT, to load the default into Orig, then dump Matrix6 sysex bank from the computer
  // If dumping Matrix Ctrlr bank, don't INIT.
  // in order : arp, unison, seq, patch

  PATCH_Store(bank, patch, PAGING);

  //          for (unsigned char i = 0; i < 128; i++)
  //        dataPage[i] = EditBuffer[device][i+8]; // data of EditBuffer[device]
  //
  //      mem_0.writePage(patch + (bank * 100), dataPage );

#if DEBUG_eeprom
  Serial.print(F("END OF Write_Bank_To_BS()/ device = ")); Serial.print(device + 0x0a, HEX); Serial.print(F("/ bank = ")); Serial.print(bank, DEC); Serial.print(F("/ patch = ")); Serial.println(patch, DEC);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
void Write_Bank_To_BS2(unsigned char device, unsigned char bank, unsigned char patch)
{
  for (unsigned char i = 8; i < 134; i++)
    dataPage[i - 8] = EditBuffer[device][i]; // data of EditBuffer[device]

  if (bank < 5)
    mem_0.writePage(patch + (bank * 100), dataPage );
  else
    mem_2.writePage(patch +  ((bank - 5) * 100), dataPage );
}

/////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
void Write_Default_Patchname(unsigned char device, unsigned char bank, unsigned char bankrequest)
{
  unsigned char j;
  unsigned char d, u;
  d = u = 0;

  // name indicating bank and patch "Patch000" using 8 characters, 100x iterative
  for (unsigned char i = 0; i < 100; i++)
  {
    // patchname is "BNKx: yz" from Matrix1000 (where x = bank number, y = dozen et z = units)
    dataPage[0] = 66; // 'B'
    dataPage[1] = 78; // 'N'
    dataPage[2] = 75; // 'K'
    dataPage[3] = bankrequest + 48; 
    dataPage[4] = 58; // ':'
    dataPage[5] = 32; // ' '
    dataPage[6] = d + 48; 
    dataPage[7] = u + 48;

    // unison detune default
    dataPage[8] = 0;

    // default arp
    for ( j = 0; j < 20; j++)
      dataPage[9 + j] = pgm_read_byte_near (&Default_ArpParameters[j][0]);

    //sequence default (Carpenter)
    for ( j = 0; j < 32; j++)
      dataPage[ j + 29] = pgm_read_byte_near (&Default_Sequence[3][32][0]);

    for ( j = 0; j < 32; j++)
      dataPage[ j + 29 + 32] = pgm_read_byte_near (&Default_Sequence[3][32][1]);

    for ( j = 0; j < 34; j++)
      dataPage[ j + 29 + 32 + 32] = 0;


    if (bank < 5)
      mem_1.writePage(i + (bank * 100), dataPage );
    else
      mem_3.writePage(i +  ((bank - 5) * 100), dataPage );

    // increment yz
    u++;
    if (u == 10)
    {
      d++;
      u = 0;
    }

    if (d == 10) d = 0;
  }
}
/////////////////////////////////////////////////////////////////////////////
// store in arduino mega EEPROM last bank and patch number used
////////////////////////////////////////////////////////////////////////////
void Store_LastBankPatch_to_EEPROM(unsigned char device, unsigned char bank, unsigned char patch)
{
#if DEBUG_inteeprom
  Serial.print(F("Store_LastBankPatch_to_EEPROM() / "));
  Serial.print(F("device= ")); ; Serial.print(device + 0x0a, HEX);
  Serial.print(" / bank = "); Serial.print(bank, DEC);
  Serial.print(F(" / patch = " )); Serial.println(patch, DEC);
#endif

  switch (device)
  {
    case Matrix_Device_A:
      EEPROM.update(EEPROM_LASTBANKA, bank);
      EEPROM.update(EEPROM_LASTPATCHA, patch);
      break;

    case Matrix_Device_B:
      EEPROM.update(EEPROM_LASTBANKB, bank);
      EEPROM.update(EEPROM_LASTPATCHB, patch);
      break;

    case Matrix_Device_C:
      EEPROM.update(EEPROM_LASTBANKC, bank);
      EEPROM.update(EEPROM_LASTPATCHC, patch);
      break;

    case Matrix_Device_D:
      EEPROM.update(EEPROM_LASTBANKD, bank);
      EEPROM.update(EEPROM_LASTPATCHD, patch);
      break;

    default:
      break;
  }

  EEPROM.update(EEPROM_DEVICE, device);

}

/////////////////////////////////////////////////////////////////////////////
// recall last bank and patch number used in arduino mega EEPROM
////////////////////////////////////////////////////////////////////////////
void Recall_LastBankPatch_from_EEPROM(unsigned char device)
{
  switch (device)
  {
    case Matrix_Device_A:
      uBank[device] = EEPROM.read(EEPROM_LASTBANKA);
      uPatch[device] = EEPROM.read(EEPROM_LASTPATCHA);
      break;

    case Matrix_Device_B:
      uBank[device] = EEPROM.read(EEPROM_LASTBANKB);
      uPatch[device] = EEPROM.read(EEPROM_LASTPATCHB);
      break;

    case Matrix_Device_C:
      uBank[device] = EEPROM.read(EEPROM_LASTBANKC);
      uPatch[device] = EEPROM.read(EEPROM_LASTPATCHC);
      break;

    case Matrix_Device_D:
      uBank[device] = EEPROM.read(EEPROM_LASTBANKD);
      uPatch[device] = EEPROM.read(EEPROM_LASTPATCHD);
      break;

    default:
      break;
  }

  //  device = EEPROM.read(EEPROM_DEVICE);

#if DEBUG_inteeprom
  switch (device)
  {
    case Matrix_Device_A:
      Serial.print(F("Recall_LastBankPatch_from_EEPROM() / Matrix_Device_A / device = ")); Serial.print(device + 0x0a, HEX);
      Serial.print(F(" / uBank[device] = ")); Serial.print(uBank[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.print(EEPROM_LASTBANKA, DEC);
      Serial.print(F(" / uPatch[device] = ")); Serial.print(uPatch[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.println(EEPROM_LASTPATCHA, DEC);
      break;

    case Matrix_Device_B:
      Serial.print(F("Recall_LastBankPatch_from_EEPROM() / Matrix_Device_B / device = ")); Serial.print(device + 0x0a, HEX);
      Serial.print(F(" / uBank[device] = ")); Serial.print(uBank[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.print(EEPROM_LASTBANKB, DEC);
      Serial.print(F(" / uPatch[device] = ")); Serial.print(uPatch[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.println(EEPROM_LASTPATCHB, DEC);
      break;

    case Matrix_Device_C:
      Serial.print(F("Recall_LastBankPatch_from_EEPROM() / Matrix_Device_C / device = ")); Serial.print(device + 0x0a, HEX);
      Serial.print(F(" / uBank[device] = ")); Serial.print(uBank[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.print(EEPROM_LASTBANKC, DEC);
      Serial.print(F(" / uPatch[device] = ")); Serial.print(uPatch[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.println(EEPROM_LASTPATCHC, DEC);
      break;

    case Matrix_Device_D:
      Serial.print(F("Recall_LastBankPatch_from_EEPROM() / Matrix_Device_D / device = ")); Serial.print(device + 0x0a, HEX);
      Serial.print(F(" / uBank[device] = ")); Serial.print(uBank[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.print(EEPROM_LASTBANKD, DEC);
      Serial.print(F(" / uPatch[device] = ")); Serial.print(uPatch[device], DEC); Serial.print(F(" / in EEPROM.addr=")); Serial.println(EEPROM_LASTPATCHD, DEC);
      break;

    default:
      break;
  }
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// load on boot, get the EditBuffer and send to Matrix_i
/////////////////////////////////////////////////////////////////////////////
void PATCH_Init(unsigned char device)
{

#if DEBUG_eeprom
  Serial.println(F("PATCH_Init() "));
#endif
  switch (device)
  {
    case Matrix_Device_A:
      // send correct EditBuffer to Matrix A:
      Recall_LastBankPatch_from_EEPROM(Matrix_Device_A); // recall last ubank upatch used in previous session
      Read_Patch_From_BS(Matrix_Device_A, uBank[Matrix_Device_A], uPatch[Matrix_Device_A]); // read EditBuffer[device] coresponding
      //CopyEditBufferOrigToEditBuffer(Matrix_Device_A);
      SendEditBuffer(Matrix_Device_A, INTERFACE_SERIAL1); // send EditBuffer[device] to Mxi
      // DEBUG//SendEditBuffer(Matrix_Device_A, INTERFACE_SERIAL3); // send EditBuffer[device] to core out
      break;

    case Matrix_Device_B:
      // Matrix in B:
      Recall_LastBankPatch_from_EEPROM(Matrix_Device_B); // recall last ubank upatch used in previous session
      Read_Patch_From_BS(Matrix_Device_B, uBank[Matrix_Device_B], uPatch[Matrix_Device_B]); // read EditBuffer[device] coresponding
      //CopyEditBufferOrigToEditBuffer(Matrix_Device_B);
      SendEditBuffer(Matrix_Device_B, INTERFACE_SERIAL2); // send EditBuffer[device] to Mxi
      // DEBUG//SendEditBuffer(Matrix_Device_B, INTERFACE_SERIAL3); // send EditBuffer[device] to core out
      break;

    case Matrix_Device_C:
      // Matrix in C:
      Recall_LastBankPatch_from_EEPROM(Matrix_Device_C); // recall last ubank upatch used in previous session
      Read_Patch_From_BS(Matrix_Device_C, uBank[Matrix_Device_C], uPatch[Matrix_Device_C]); // read EditBuffer[device] coresponding
      //CopyEditBufferOrigToEditBuffer(Matrix_Device_C);
      SendEditBuffer(Matrix_Device_C, INTERFACE_SERIAL4); // send EditBuffer[device] to Mxi
      // DEBUG//SendEditBuffer(Matrix_Device_C, INTERFACE_SERIAL3); // send EditBuffer[device] to core out
      break;

    case Matrix_Device_D:
      // Matrix in D:
      Recall_LastBankPatch_from_EEPROM(Matrix_Device_D); // recall last ubank upatch used in previous session
      Read_Patch_From_BS(Matrix_Device_D, uBank[Matrix_Device_D], uPatch[Matrix_Device_D]); // read EditBuffer[device] coresponding
      //CopyEditBufferOrigToEditBuffer(Matrix_Device_D);
      SendEditBuffer(Matrix_Device_D, INTERFACE_SERIAL5); // send EditBuffer[device] to Mxi
      // DEBUG//SendEditBuffer(Matrix_Device_D, INTERFACE_SERIAL3); // send EditBuffer[device] to core out
      break;
    default: break;
  }

  //app_flags.Display_DIN_Req = 1;


}

/////////////////////////////////////////////////////////////////////////////
// clear or format 24LC512, Internal eeprom
/////////////////////////////////////////////////////////////////////////////
void FORMAT_Memory(unsigned char memory)
{
  lcd.clear();

  switch (memory)
  {
    case 1:
      // set data
      for (unsigned char i = 0; i < 128; i++)
        dataPage[i] = 0xff; // data

      // display
      lcd.setCursor(0, 0);
      lcd.print(F("CLEARING WITH 0xff   "));

      // write 512 pages on 4 eeprom 24LC512
      for (unsigned int page = 0; page < 512; ++page)
      {

        lcd.setCursor(0, 1);
        lcd.print(F("ext.EEPROM page "));
        LCD_PrintBCD3(page);

        mem_0.writePageFast(page, dataPage );
        mem_1.writePageFast(page, dataPage );
        mem_2.writePageFast(page, dataPage );
        mem_3.writePageFast(page, dataPage );

      }
#if DEBUG_eeprom
      Serial.println(F("FORMAT_Memory(1) / 512 pages extEEPROM x4 cleared"));
#endif
      software_Reboot();
      break;

    case 2:
      // display
      lcd.setCursor(0, 0);
      lcd.print(F("CLEARING WITH 0xff   "));

      for (unsigned int addr = 0; addr <= EEPROM.length(); ++addr)
      {
        EEPROM.write(addr, 0xff);

        lcd.setCursor(0, 1);
        lcd.print(F("int.EEPROM addr "));
        lcd.print(addr);
      }
#if DEBUG_inteeprom
      Serial.println(F("FORMAT_Memory(2) / intEEPROM cleared"));
#endif
      software_Reboot();
      break;

    case 3:
      // write all pages of odd eeproms
      // data
      for (unsigned char i = 8; i < 134; i++)
        dataPage[i - 8] = pgm_read_byte_near(&DefaultEditBuffer[i]); // data of EditBuffer[device]

      // remaining bytes
      dataPage[126] = dataPage[127] = 0;

      // display
      lcd.setCursor(0, 0);
      lcd.print(F("INITIALISE PATCH     "));

      for (unsigned int page = 0; page < 512; ++page)
      {
        lcd.setCursor(0, 1);
        lcd.print(F("ext.EEPROM page "));
        LCD_PrintBCD3(page);
        mem_0.writePageFast(page, dataPage );
        mem_2.writePageFast(page, dataPage );
      }
#if DEBUG_eeprom
      Serial.println(F("FORMAT_Memory(3) / 512 pages INITIALISE PATCH finished"));
#endif

      // write all pages of pair eeproms
      // name, arp, unison, seq
      for (unsigned char i = 0; i < 8; i++)
        dataPage[i] = pgm_read_byte_near(&DefaultEditBuffer[i]); // patchname of EditBuffer[device]

      dataPage[8] = 0; // unison data

      for (unsigned char i = 0; i < 20; i++)
        dataPage[i + 9] = pgm_read_byte_near(&Default_ArpParameters[i][0]); // arp data

      for (unsigned char i = 0; i < 32; i++)
      {
        dataPage[2 * i + 0 + 29] =  pgm_read_byte_near(&Default_Sequence[3][32][0]); // seq data
        dataPage[2 * i + 1 + 29] =  pgm_read_byte_near(&Default_Sequence[3][32][1]); // seq data

      }

      for (unsigned char i = 93; i < 128; i++)
        dataPage[i] = 0; // free space of page

      // display
      lcd.setCursor(0, 0);
      lcd.print(F("INITIALISE U&ARP&SEQ"));

      // write all pages of pair eeprom
      for (unsigned int page = 0; page < 512; ++page)
      {

        lcd.setCursor(0, 1);
        lcd.print(F("ext.EEPROM page "));
        LCD_PrintBCD3(page);

        mem_1.writePageFast(page, dataPage );
        mem_3.writePageFast(page, dataPage );
      }
#if DEBUG_eeprom
      Serial.println(F("FORMAT_Memory(3) / 512 pages INITIALISE ARP&SEQ finished"));
#endif


      break;

    case 4: // initialise eeprom http://www.locoduino.org/spip.php?article72
      // 1st : default system (including Zone)
      // display
      lcd.setCursor(0, 0);
      lcd.print(F("Default SYSCONFIG[] "));

      for (unsigned int addr = 0; addr < 48; ++addr)
      {
        EEPROM.write(addr, pgm_read_byte_near(&DefaultSystemConfig[addr]));

        lcd.setCursor(0, 1);
        lcd.print(F("int.EEPROM addr "));
        lcd.print(addr);
      }
#if DEBUG_inteeprom
      Serial.println(F("FORMAT_Memory(4) / DefaultSystemConfig[] finished"));
#endif

      // fill the gap
      // display
      lcd.setCursor(0, 0);
      lcd.print(F("CLEARING WITH NULL   "));

      for (unsigned int addr = 48; addr < 500; ++addr)
      {
        EEPROM.write(addr, 0);

        lcd.setCursor(0, 1);
        lcd.print(F("int.EEPROM addr "));
        lcd.print(addr);
      }
#if DEBUG_inteeprom
      Serial.println(F("FORMAT_Memory(4) / gap[] finished"));
#endif

      // 2nd : default master parameters, start at addr 500

      // display
      lcd.setCursor(0, 0);
      lcd.print(F("Default MASTERS[]   "));

      for (unsigned int addr = 500; addr < 672; ++addr)
      {
        EEPROM.write(addr, pgm_read_byte_near(&DefaultGlobalParameters[addr - 500]));

        lcd.setCursor(0, 1);
        lcd.print(F("int.EEPROM addr "));
        lcd.print(addr);
      }

#if DEBUG_inteeprom
      Serial.println(F("FORMAT_Memory(4) / DefaultGlobalParameters[] finished"));
#endif

      // 3rd : the rest 'til the end

      // display
      lcd.setCursor(0, 0);
      lcd.print(F("CLEARING WITH NULL   "));

      for (unsigned int addr = 672; addr <= EEPROM.length(); ++addr)
      {
        EEPROM.write(addr, 0);

        lcd.setCursor(0, 1);
        lcd.print(F("int.EEPROM addr "));
        lcd.print(addr);
      }
#if DEBUG_inteeprom
      Serial.println(F("FORMAT_Memory(4) / the rest 'til the end[] finished"));
#endif
      software_Reboot();
      break;

    default:
      break;
  }
}

//////////////////////////////////////////////////////////////////
// Check internal eeproms has been formatted
///////////////////////////////////////////////////////////////////
unsigned char Check_IntEEPROM_Format(void)
{
  // 'magic numbers' are equal to zero if formatted previously : 4 last internal eeprom bytes are supposed to be null
  if ((EEPROM.read(EEPROM.length() - 1 ) != 0) && (EEPROM.read(EEPROM.length() - 2) != 0) && (EEPROM.read(EEPROM.length() - 3) != 0))
  {
    Serial.println(F("Check_IntEEPROM_Format()/ missmatch !"));
    return 1;
  }
  else
  {
    Serial.println(F("Check_IntEEPROM_Format()/ match !"));
    return 0;
  }
}

//////////////////////////////////////////////////////////////////
// Check external 24xx512 eeproms have been formatted
///////////////////////////////////////////////////////////////////
unsigned char Check_ExtEEPROM_Format(unsigned char eeprom)
{
  unsigned char result = 0;

  switch (eeprom)
  {
    case 0:
      Serial.print(F("mem_0 / "));
      mem_0.readPage(511, dataPage);
      break;

    case 2:
      Serial.print(F("mem_2 / "));
      mem_2.readPage(511, dataPage);
      break;

    default:
      break;
  }

  for (unsigned char i = 8; i < 134; ++i)
  {
    if (dataPage[i - 8] != pgm_read_byte_near(&DefaultEditBuffer[i]))
    {
      ++result;
    }
  }

  if (result)
  {
    Serial.println(F("Check_ExtEEPROM_Format() / missmatch !"));
  }
  else
    Serial.println(F("Check_ExtEEPROM_Format() / match !"));

  return result;
}



/*
   additional features 1.07. problem seen by M.Dieckmann 28 06 2018 :
   GlobalParameters not updated and saved while edited
*/
///////////////////////////////////////////////////////////////////////////////
// store GlobalParameters into internal EEPROM
// #define EEPROM_GLOBALPARAMETERS 500 // 172 bytes ! start address 500 , end adress 671 included
/////////////////////////////////////////////////////////////////////////////////
void STORE_GlobalParameters(void)
{
  for (unsigned char i = (EEPROM_GLOBALPARAMETERS - EEPROM_GLOBALPARAMETERS); i < (EEPROM_GLOBALPARAMETERS + 172 - EEPROM_GLOBALPARAMETERS); ++i)
  {
    EEPROM.update(EEPROM_GLOBALPARAMETERS + i, GlobalParameters[i]);
  }

#if DEBUG_inteeprom
  Serial.println(F("STORE_GlobalParameters()"));
#endif
}

/////////////////////////////////////////////////////////////////////////////////
// read GlobalParameters from internal EEPROM
// #define EEPROM_GLOBALPARAMETERS 500 // 172 bytes ! start address 500 , end adress 671 included
/////////////////////////////////////////////////////////////////////////////////
void READ_GlobalParameters(void)
{
  for (unsigned char i = (EEPROM_GLOBALPARAMETERS - EEPROM_GLOBALPARAMETERS); i < (EEPROM_GLOBALPARAMETERS + 172 - EEPROM_GLOBALPARAMETERS); ++i)
  {
    GlobalParameters[i]  = EEPROM.read(EEPROM_GLOBALPARAMETERS);
  }

#if DEBUG_inteeprom
  Serial.println(F("READ_GlobalParameters()"));
#endif
}
