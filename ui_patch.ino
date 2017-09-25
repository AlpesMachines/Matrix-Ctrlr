 
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
#include "device.h"
#include <EEPROM.h>

unsigned char uBank[4];
unsigned char uPatch[4];
unsigned char ProgramNumberReq;
unsigned char BankNumberReq;
unsigned char BankNumberDump;

// not enough memory so a single 128 bytes buffer for copy/paste :
unsigned char copyBuffer[136];
static bool copy, paste;


/////////////////////////////////////////////////////////////////////////////
//  Display patch pages, page 1 for bank and program changes,
//  page 2 for MIDI channel setup, edit buffer init and patch compare//
// page3 for Bank dump from Matrix 1000
/////////////////////////////////////////////////////////////////////////////
void UI_Display_Patch (void)
{
  switch (SoftPanel.Page) {

    /*
      PATCH PAGE 1
      []  []   [ ]  []  []
      01234567890123456789
      Patch u0000 ________
      GET Init Org Edt SAV
    */
    case SOFT_PAGE1:
      unsigned char j;

      DOUT_PinSet1(DIN_ConfigMap[DIN_PATCH].dout_pin);		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 	// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      LCD_Clear();
      //1st line
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("PATCH  "));
      switch (device) {
        case Matrix_Device_A:
          lcd.print(F("A"));
          break;
        case Matrix_Device_B:
          lcd.print(F("B"));
          break;
#if SOFTSERIAL_ENABLED
        case Matrix_Device_C:
          lcd.print(F("C"));
          break;
        case Matrix_Device_D:
          lcd.print(F("D"));
          break;
#endif
        default:
          break;
      }
      //lcd.print(F("-"));
      LCD_PrintBCD1(uBank[device]);
      LCD_PrintBCD2(uPatch[device]);
      if (uPatch[device] < 10)
      {
        LCD_CursorSet(9 + LCD_Offset);
        LCD_PrintCString(F("0")); // place a zero here for values below 10
      }

      // display patchname
      LCD_CursorSet(12 + LCD_Offset);
      for (j = 0; j < 8; j++) {
        if (EditBuffer[device][j] < 0x20) // cf ASCII tables
          EditBuffer[device][j] = EditBuffer[device][j] + 0x40; // +64 (0x40) compatible with Matrix 1000 patch names
        else
          EditBuffer[device][j] = EditBuffer[device][j];
        LCD_PrintChar( EditBuffer[device][j]);
      }

      // 2nd line
      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintCString(F("GET INIT ORG EDT SAV"));
      break;

    /*
      2nd page : Get whole bank from M1000 ?
      PATCH PAGE 2
      []  []   [ ]  []  []
      12345678901234567890
      mBank:_      uBank:_
      d  u     GET   d  u
      *  *      o    *  *
    */
    case SOFT_PAGE2:
      DOUT_PinSet1(DIN_ConfigMap[DIN_PATCH].dout_pin);		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin); 		// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	// on "store to ?"
      DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 	// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      //1st line
      LCD_Clear();
      LCD_CursorSet(0 + LCD_Offset);
      LCD_PrintCString(F("mBank:"));
      LCD_PrintBCD1(BankNumberDump);
      LCD_CursorSet(8 + LCD_Offset);
      lcd.write(255); lcd.write(255); lcd.write((byte)0); //LCD_PrintCString(F("==>"));
      LCD_CursorSet(12 + LCD_Offset);
      LCD_PrintCString(F("cBank"));
      LCD_PrintBCD2(uBank[device]);
      //2nd line
      LCD_CursorSet(65 + LCD_Offset);
      lcd.write((byte)6); //LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(69 + LCD_Offset);
      lcd.write((byte)7); //LCD_PrintChar(CHAR_UP);
      LCD_CursorSet(72 + LCD_Offset);
      LCD_PrintCString(F("GET "));	//68
      LCD_CursorSet(78 + LCD_Offset);
      lcd.write((byte)6); //LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(82 + LCD_Offset);
      lcd.write((byte)7); //LCD_PrintChar(CHAR_UP);


      if (MIDI_ReceivingBank)
      {
        lcd.setCursor(0, 1);
        lcd.print(F("Getting MTRX Bank "));
        LCD_PrintBCD1(BankNumberDump);
        lcd.print(F(" "));
      }
      break;

    /*
      3rd page : Get patch from M1000 and save to BS ?
      PATCH PAGE 4
      []  []   [ ]  []  []
      12345678901234567890
      m000 ________ u000
      d  u   SAVE  d   u
      *  *     o    *  *
    */
    case SOFT_PAGE3:
      DOUT_PinSet1(DIN_ConfigMap[DIN_PATCH].dout_pin);		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin); 		// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	// on "store to ?"
      DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 	// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      //1st line
      LCD_Clear();
      // patch number of M1000 :
      LCD_CursorSet(1 + LCD_Offset);
      LCD_PrintCString(F("m"));
      LCD_PrintBCD1(BankNumber);
      LCD_PrintBCD2(ProgramNumber);
      if (ProgramNumber < 10) {
        LCD_CursorSet(3 + LCD_Offset);
        LCD_PrintCString(F("0"));
      } // place a zero here for values below 10

      // name of patch :
      LCD_CursorSet(6 + LCD_Offset);
      for (j = 0; j < 8; j++) {
        if (EditBufferOrig[j] < 0x20) { // cf ASCII tables
          EditBufferOrig[j] = EditBufferOrig[j] + 0x40; // +64 (0x40) compatible with Matrix 1000 patch names
        } else {
          EditBufferOrig[j] = EditBufferOrig[j]; //
        }
        LCD_PrintChar(EditBufferOrig[j]);
      }
      // storage destination :
      LCD_CursorSet(15 + LCD_Offset);
      // LCD_PrintCString(F("u"));
      LCD_PrintBCD1(uBank[device]);
      LCD_PrintBCD2(uPatch[device]);
      if (uPatch[device] < 10) { // Patch
        LCD_CursorSet(16 + LCD_Offset);
        LCD_PrintCString(F("0"));
      }

      //2nd line
      LCD_CursorSet(65 + LCD_Offset);
      lcd.write((byte)6); //LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(69 + LCD_Offset);
      lcd.write((byte)7); //LCD_PrintChar(CHAR_UP);
      LCD_CursorSet(72 + LCD_Offset);
      LCD_PrintCString(F("SAVE"));
      LCD_CursorSet(78 + LCD_Offset);
      lcd.write((byte)6); //LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(82 + LCD_Offset);
      lcd.write((byte)7); //LCD_PrintChar(CHAR_UP);
      break;

    /*
      PATCH PAGE 4
      []  []   [ ]  []  []
      12345678901234567890
      Copy&Paste EB/AP/SEQ
      CPY PAST EDT ARP SEQ
    */
    case SOFT_PAGE4:
      DOUT_PinSet1(DIN_ConfigMap[DIN_PATCH].dout_pin);    // on
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin);     // on
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);     // off

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("COPY & PASTE :      "));
      lcd.setCursor(0, 1);
      lcd.print(F("Cpy Past Edt/Arp/Seq"));
      DOUT_PinSet(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin, copy);
      DOUT_PinSet(DIN_ConfigMap[DIN_FILTER].dout_pin, paste);
      break;

    case SOFT_PAGE5:
      DOUT_PinSet1(DIN_ConfigMap[DIN_PATCH].dout_pin);    // on
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin);    // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off
      DOUT_PinSet_Keypanel(0, 0, 0, 0, 0, 1);
      // 1st line
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("PATCH page5"));
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  Send MIDI for the patch pages, display patch name
/////////////////////////////////////////////////////////////////////////////
void UI_Handle_Patch(void)
{
  //	ChooseEditBuffer[device]ABCD(); // define which EditBuffer[device] to use

  if (SoftPanel.Page == SOFT_PAGE1) /////////////////////////////////////////////////// 1 ////////////////// PAGE 1
  {
    // functions available to page 1 :
    switch (SoftPanel.Button)
    {
      case SOFT_EDIT_F1:
        if (Shift)
        {
          if (ui_aHold)
            Release_aChordLatch(ui_aHold);
          ui_aHold = !ui_aHold;
        }
        else
        {
          router_arp_tag = !router_arp_tag;
          if (router_arp_tag == 0)
            active_arp = false;
        }
        break;

      case SOFT_EDIT_F2:
        if (Shift)
        {
          ui_toggleSeq = !ui_toggleSeq;
          if (seqPushedKey != 255) HandleNoteOff(seqPushedKey, 0x00, MIDI_CHANNEL);
        }
        else
        {
          ui_seqPlay = !ui_seqPlay;
          seqTick = 0;
        }
        break;
    }

    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE2; // go to next page
        break;

      case SOFT_EDIT_5: // SAV

        lcd.setCursor(0, 0);
        lcd.print(F("EditBuffer, Unison, "));
        lcd.setCursor(0, 1);
        lcd.print(F("Arp & Seq saved !   "));
        elapsedTime = 0; // temp msg

        ArpParameters_Store(device); // copy the ui_ in buffer[device] done in WritePatchtoBS() below
        Write_Patch_To_BS(device, uBank[device], uPatch[device]); // save patch buffer in BS
        Store_LastBankPatch_to_EEPROM(device, uBank[device], uPatch[device]); // save device & patch numbers in eeprom
        //Read_Patch_From_BS(device, uBank[device], uPatch[device]); // reload what's just been saved
        UpdateDinStates(); // mise à jour des Leds
        // send EditBuffer[device] to core out for copy/past in MidiMonitor.app if necessary
        SendEditBuffer(device, INTERFACE_SERIAL3);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune[device]);
        break;

      case SOFT_EDIT_4: // send edit
        if (Shift)
          WizardEditBuffer(device, 6);

        if (localControl)
        {
          SendEditBuffer(device, INTERFACE_SERIAL);
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune[device]);
        }
        // send EditBuffer[device] to core out for copy/past in MidiMonitor.app if necessary
        SendEditBuffer(device, INTERFACE_SERIAL3);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL3, UnisonDetune[device]);
        break;

      case SOFT_EDIT_3: // send Orig
        if (localControl)
        {
          SendEditBufferOrig(INTERFACE_SERIAL);
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetuneOrig);
        }
        SendEditBufferOrig(INTERFACE_SERIAL3);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL3, UnisonDetuneOrig);
        break;

      case SOFT_EDIT_2: // init patch in EditBuffer
        InitEditBuffer();
        ARP_GLOBAL_INIT(device);
        if (localControl)
        {
          SendEditBuffer(device, INTERFACE_SERIAL);
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, 0);
        }
        SendEditBuffer(device, INTERFACE_SERIAL3);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL3, 0);
        break;

      case SOFT_EDIT_1: // set & request patch from the Matrix1000
        MIDI_RequestEditBuffer(device, INTERFACE_SERIAL, uPatch[device]);
        break;

      case SOFT_EDIT_INC: // increment 000-->001  099-->100 :: OK :)
        uPatch[device]++;
        uPatch[device] = uPatch[device] + Shift ;
        if (uPatch[device] > uPATCH_MAX) {
          uPatch[device] = 0;
          uBank[device]++;
          if (uBank[device] > uBANK_MAX)
            uBank[device] = 0;
        }
        if (localControl)
        {
          ARP_GLOBAL_INIT(device);
          Reset_UI_ARP(); // stop arp parameters

          Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
          UpdateDinStates(); // mise à jour des Leds
          SendEditBuffer(device, INTERFACE_SERIAL);  // send edit buffer :)
          // send Unison detune value :
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune[device]);
          // load arp parameters
          ArpParameters_Load(device);
    
          // send EditBuffer[device] to core out
          SendEditBuffer(device, INTERFACE_SERIAL3);
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL3, UnisonDetune[device]);
        }
        // send Bank et Patch number & EditBuffer[device] to Core  Midi out :
        MIDI_SendPatchNumber(INTERFACE_SERIAL3, uBank[device], uPatch[device]);

        break;

      case SOFT_EDIT_DEC: // decrement 000-->3199 100-->099 :: OK :)
        uPatch[device]--;
        uPatch[device] = uPatch[device] - Shift ;
        if (uPatch[device] == 255) {
          uPatch[device] = uPATCH_MAX;
          uBank[device]--;
          if (uBank[device] == 255)
            uBank[device] = uBANK_MAX;
        }
        if (localControl)
        {
          ARP_GLOBAL_INIT(device);
          Reset_UI_ARP(); // stop arp parameters
          Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
          UpdateDinStates(); // mise à jour des Leds
          SendEditBuffer(device, INTERFACE_SERIAL);
          // send Unison detune value TO DO :
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune[device]);
          // load arp parameters
          ArpParameters_Load(device);
          // send EditBuffer[device] to core out
          SendEditBuffer(device, INTERFACE_SERIAL3);
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL3, UnisonDetune[device]);
        }
        // send Bank et Patch number to Core  Midi out
        MIDI_SendPatchNumber(INTERFACE_SERIAL3, uBank[device], uPatch[device]);

        break;

      case DIN_SHIFT:
        // nope
        break;

      case SOFT_EDIT_ENC_CLIC: // encoder clic : load patch
#if DEBUG_encoder
        Serial.println(F("SOFT_EDIT_ENC_CLIC load patch"));
        // if (encoderClic) Serial.println(F("SOFT_EDIT_ENC_CLIC=1"));
#endif
        if (localControl)
        {
          ARP_GLOBAL_INIT(device);
          Reset_UI_ARP(); // stop arp parameters
          Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
          UpdateDinStates(); // mise à jour des Leds
          SendEditBuffer(device, INTERFACE_SERIAL);
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune[device]); // send Unison detune value
          ArpParameters_Load(device); // load arp parameters
          SendEditBuffer(device, INTERFACE_SERIAL3); // send  to core out
          MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL3, UnisonDetune[device]);
          UI_Display_Patch(); // to update patch name on display
        }
        // send Bank et Patch number & EditBuffer[device] to Core  Midi out :
        MIDI_SendPatchNumber(INTERFACE_SERIAL3, uBank[device], uPatch[device]);

        break;
    }

    // encoder
    if (Shift) {
      uBank[device] += SoftPanel.EncoderValue;
    } else {
      uPatch[device] += SoftPanel.EncoderValue;
    }

    if (uPatch[device] > 200)
    {
      uPatch[device] = uPATCH_MAX;
      uBank[device]--;
    }

    if (uPatch[device] > uPATCH_MAX)
    {
      uPatch[device] = 0;
      uBank[device]++;
    }

    if (uBank[device] > 200)
    {
      uBank[device] = uBANK_MAX;
    }

    if (uBank[device] > uBANK_MAX) {
      uBank[device] = 0;
    }


#if DEBUG_softpanel
    Serial.print(F("uPatch = ")); Serial.println(uPatch[device]);
#endif

  } // end of page 1

  else if (SoftPanel.Page == SOFT_PAGE2) //////////////////////////////////////////////// PAGE 2 ///////////////////////////
  {
    switch (SoftPanel.Button)
    {
      case DIN_PAGE:
        MIDI_ReceivingBank = false;
        SoftPanel.Page = SOFT_PAGE3; // goto page 3
        break;

      case SOFT_EDIT_1: // bank decrement
        BankNumberDump--;
        if (BankNumberDump == 255)
          BankNumberDump = 9;
        MIDI_SetBank(INTERFACE_SERIAL, BankNumberDump);
        break;

      case SOFT_EDIT_2: // bank increment
        BankNumberDump++;
        if (BankNumberDump > 9)
          BankNumberDump = 0;
        MIDI_SetBank(INTERFACE_SERIAL, BankNumberDump);
        break;

      case SOFT_EDIT_3: // click this button to store mBank into uBS
        // tell system we will receive a bank:
        MIDI_ReceivingBank = true; // toggle state
        MIDI_SetBank(INTERFACE_SERIAL, BankNumberDump);
        MIDI_BankRequest(INTERFACE_SERIAL, BankNumberDump);
        // then store is done automatically in router.ino Handle(sysex)
        break;

      case SOFT_EDIT_4: // decrement BS
        if (uBank[device] == 0)
          uBank[device] = uBANK_MAX;
        else
          uBank[device]--;
        break;

      case SOFT_EDIT_5: // increment BS
        uBank[device]++;
        if (uBank[device] > uBANK_MAX)
          uBank[device] = 0;
        break;
    }
  }
  else if (SoftPanel.Page == SOFT_PAGE3) // it's page 3  //////////////////////////////////////////////// PAGE 3 ////////////////////////////////////
  {
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE4; // goto page 4
        break;

      case SOFT_EDIT_1: // decrement patch & bank
        ProgramNumber--;
        if (ProgramNumber == 255)
        {
          ProgramNumber = uPATCH_MAX;
          BankNumber--;
        }
        if (BankNumber == 255)
        {
          BankNumber = uBANK_MAX;
        }
        if (matrix_modele == matrix_6)
        { // and ask corresponding Matrix patch using a SysEx msg
          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);
        }
        else
        {
          // and ask corresponding Matrix patch using a SysEx msg
          MIDI_SetBank(INTERFACE_SERIAL, BankNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);

        }
        break;

      case SOFT_EDIT_2: // increment patch & bank
        ProgramNumber++;
        if (ProgramNumber > uPATCH_MAX) {
          ProgramNumber = 0;
          BankNumber++;
        }
        if (BankNumber > uBANK_MAX) {
          BankNumber = 0;
        }
        if (matrix_modele == matrix_6) {
          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);
        }
        else
        {
          MIDI_SetBank(INTERFACE_SERIAL, BankNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber); // 0.99 marche tres bien ne pas changer
        }
        break;

      case SOFT_EDIT_3: // click this button to store a patch sent by the Matrix1000 in the Ctrlr memory :
        // set arp, uni & seq to default
        Set_OrigArpUniSeq_toDefault();
        // then save
        Write_Patch_To_BS(device, uBank[device], uPatch[device]);
        break;

      case SOFT_EDIT_4: // decrement BP
        uPatch[device]--;
        if (uPatch[device] == 255) {
          uPatch[device] = uPATCH_MAX;
          uBank[device]--;
        }
        if (uBank[device] == 255) {
          uBank[device] = uBANK_MAX;
        }

        if (uBank[device] > uBANK_MAX) {
          uBank[device] = 0;
        }
        break;

      case SOFT_EDIT_5: // increment BP
        uPatch[device]++;
        if (uPatch[device] > uPATCH_MAX) {
          uPatch[device] = 0;
          uBank[device]++;
        }
        if (uBank[device] == 255) {
          uBank[device] = uBANK_MAX;
        }

        if (uBank[device] > uBANK_MAX) {
          uBank[device] = 0;
        }
        break;

      case SOFT_EDIT_ENC_CLIC: // encoder clic : load patch
        if (matrix_modele == matrix_6)
        { // and ask corresponding Matrix patch using a SysEx msg
          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);
        }
        else
        {
          // and ask corresponding Matrix patch using a SysEx msg
          MIDI_SetBank(INTERFACE_SERIAL, BankNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);
        }
        break;

      default: break;
    }

    // encoder
    if (Shift)
    {
      uPatch[device] += SoftPanel.EncoderValue;
      if (uPatch[device] > uPATCH_MAX) {
        uPatch[device] = 0;
        uBank[device]++;
      }

      if (uPatch[device] == 255) {
        uPatch[device] = uPATCH_MAX;
        uBank[device]--;
      }

      if (uBank[device] == 255) {
        uBank[device] = uBANK_MAX;
      }

      if (uBank[device] > uBANK_MAX) {
        uBank[device] = 0;
      }
    }
    else
    {
      ProgramNumber += SoftPanel.EncoderValue;
      if (ProgramNumber > uPATCH_MAX) {
        ProgramNumber = 0;
        BankNumber++;
      }
      if (BankNumber > uBANK_MAX) {
        BankNumber = 0;
      }

      if (ProgramNumber == 255)
      {
        ProgramNumber = uPATCH_MAX;
        BankNumber--;
      }
      if (BankNumber == 255)
      {
        BankNumber = uBANK_MAX;
      }
    }
  } // end of page3
  else if (SoftPanel.Page == SOFT_PAGE4) ////////////////////////////////////////// PAGE 4 ////////////////////////
  {
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1;
        break;

      default:
        break;
    }
  }
  else //////////////////////////////////////////////// PAGE 5 /////////////////////////
  {
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1; // goto page 1
        break;

      case SOFT_EDIT_1: // cpy
        copy = true;
        break;

      case SOFT_EDIT_2: // past
        paste = true;
        break;

      case SOFT_EDIT_3: // EditBuffer copying
        if (copy)        // copying
        {
          for (unsigned char i = 0; i < 134; i++)
          {
            copyBuffer[i] = EditBuffer[device][i]; // patch
          }
          copyBuffer[134] = UnisonDetune[device]; // unison
          copyBuffer[135] = 1; // typ EB
          copy = false;
        }

        if (paste && (copyBuffer[135] == 1))        //pasting
        {
          for (unsigned char i = 0; i < 128; i++)
          {
            EditBuffer[device][i] = copyBuffer[i];
          }
          UnisonDetune[device] = copyBuffer[134];
          paste = false;
        }
        else
          return; // quit

        break;

      case SOFT_EDIT_4:
        if (copy)        // copying
        {
          for (unsigned char i = 0; i < 20; i++)
          {
            copyBuffer[i] = ArpParameters[device][i];
          }
          copyBuffer[135] = 2; // typ ArpParam
          copy = false;
        }

        if (paste && (copyBuffer[135] == 2))        //pasting
        {
          for (unsigned char i = 0; i < 20; i++)
          {
            ArpParameters[device][i] = copyBuffer[i];
          }
          paste = false;
        }
        else return;
        break;

      case SOFT_EDIT_5:
        if (copy)        // copying
        {
          for (unsigned char i = 0; i < 32; i++)
          {
            for (unsigned char j = 0; j < 2; j++)
            {
              copyBuffer[2 * i + j] = sequence[i][j];
            }
          }
          copyBuffer[135] = 3;
          copy = false;
        }

        if (paste && (copyBuffer[135] == 3))        //pasting
        {
          for (unsigned char i = 0; i < 32; i++)
          {
            for (unsigned char j = 0; j < 2; j++)
            {
              sequence[i][j] = copyBuffer[2 * i + j];
            }
          }
          paste = false;
        }
        else return;
        break;

      default:
        break;

    }
  }
}//end of UI_Handle_Patch()



