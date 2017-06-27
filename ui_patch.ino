//#include <cmios.h>
#include "chaosmatrix.h"
#include "softpanel.h"
#include "lcd.h"
#include "din.h"
#include "midi.h"
//#include "patchnames.h"
//#include "memo.h"
#include "device.h"
#include <EEPROM.h>

unsigned char uBank;
unsigned char uPatch;
unsigned char ProgramNumberReq;
unsigned char BankNumberReq;
unsigned char BankNumberDump;

/////////////////////////////////////////////////////////////////////////////
//  Display patch pages, page 1 for bank and program changes,
//  page 2 for MIDI channel setup, edit buffer init and patch compare//
// page3 for Bank dump from Matrix 1000
/////////////////////////////////////////////////////////////////////////////
void UI_Display_Patch (void)
{
  switch (SoftPanel.Page) {
    case SOFT_PAGE1:
      unsigned char j;

      /*
        PATCH PAGE 1
        []  []   [ ]  []  []
        01234567890123456789
        Patch u0000 ________
        GET Init Org Edt SAV
      */
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
      LCD_PrintBCD1(uBank);
      LCD_PrintBCD2(uPatch);
      //      // some clean up :
      //      if (uBank < 10)
      //      {
      //        LCD_CursorSet(7 + LCD_Offset);
      //        LCD_PrintCString(F("0")); // place a zero here for values below 10
      //      }
      if (uPatch < 10)
      {
        LCD_CursorSet(9 + LCD_Offset);
        LCD_PrintCString(F("0")); // place a zero here for values below 10
      }
      //lcd.print(F("."));

      // display patchname
      LCD_CursorSet(12 + LCD_Offset);
      for (j = 0; j < 8; j++) {
        if (EditBuffer[j] < 0x20) { // cf ASCII tables
          EditBuffer[j] = EditBuffer[j] + 0x40; // +64 (0x40) compatible with Matrix 1000 patch names
        } else {
          EditBuffer[j] = EditBuffer[j]; //
        }
        LCD_PrintChar( EditBuffer[j]);
      }


      // 2nd line
      LCD_CursorSet(64 + LCD_Offset);
      LCD_PrintCString(F("GET INIT ORG EDT SAV"));

      //3rd line

      break;

    case SOFT_PAGE4:
      /*
        PATCH PAGE 4
        []  []   [ ]  []  []
        12345678901234567890
        CH    A   B   C   D
        00    0   0   1   0
      */
      DOUT_PinSet1(DIN_ConfigMap[DIN_PATCH].dout_pin);		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_EDIT].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_OSCILLATORS].dout_pin); // off
      DOUT_PinSet0(DIN_ConfigMap[DIN_FILTER].dout_pin); 		// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_ENVELOPES].dout_pin); 	// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_KEYBOARD].dout_pin); 	// off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX].dout_pin); 		// off
      DOUT_PinSet1(DIN_ConfigMap[DIN_PAGE].dout_pin); 		// on
      DOUT_PinSet0(DIN_ConfigMap[DIN_CFG].dout_pin);     // off

      lcd.clear();
      lcd.print("PATCH page4");
      break;

    case SOFT_PAGE2:
      /*
        3rd page : Get whole bank from M1000 ?
        PATCH PAGE 2
        []  []   [ ]  []  []
        12345678901234567890
        Bank:_          BS:_
        d  u     DUMP   d  u
       *  *       o    *  *
      */
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
      LCD_PrintCString(F("==>"));
      LCD_CursorSet(12 + LCD_Offset);
      LCD_PrintCString(F("uBank:"));
      LCD_PrintBCD2(uBank);
      //2nd line
      LCD_CursorSet(65 + LCD_Offset);
      LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(69 + LCD_Offset);
      LCD_PrintChar(CHAR_UP);
      LCD_CursorSet(72 + LCD_Offset);
      LCD_PrintCString(F("GET "));	//68
      LCD_CursorSet(78 + LCD_Offset);
      LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(82 + LCD_Offset);
      LCD_PrintChar(CHAR_UP);
      break;

    case SOFT_PAGE3:

      /*
        4th page : Get patch from M1000 and save to BS ?
        PATCH PAGE 4
        []  []   [ ]  []  []
        12345678901234567890
        m000 ________ u000
        d  u   SAVE  d   u
        *  *     o    *  *
      */
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
        LCD_PrintChar( EditBufferOrig[j]);
      }
      // storage destination :
      LCD_CursorSet(15 + LCD_Offset);
      // LCD_PrintCString(F("u"));
      LCD_PrintBCD1(uBank);
      LCD_PrintBCD2(uPatch);
      //      // some clean up : place a zero here for values below 10 :
      //      if (uBank < 10) { // Bank
      //        LCD_CursorSet(16 + LCD_Offset);
      //        LCD_PrintCString(F("0")); // place a zero here for values below 10
      //      }
      if (uPatch < 10) { // Patch
        LCD_CursorSet(16 + LCD_Offset);
        LCD_PrintCString(F("0"));
      }

      //2nd line
      LCD_CursorSet(65 + LCD_Offset);
      LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(69 + LCD_Offset);
      LCD_PrintChar(CHAR_UP);
      LCD_CursorSet(72 + LCD_Offset);
      LCD_PrintCString(F("SAVE"));
      LCD_CursorSet(78 + LCD_Offset);
      LCD_PrintChar(CHAR_DOWN);
      LCD_CursorSet(82 + LCD_Offset);
      LCD_PrintChar(CHAR_UP);
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
  //	ChooseEditBufferABCD(); // define which editbuffer to use

  // enable Auto Repeat for -/+ buttons :
  //	MIOS_DIN_PinAutoRepeatEnable(SOFT_EDIT_DEC); // turbo mode !!
  //	MIOS_DIN_PinAutoRepeatEnable(SOFT_EDIT_INC);

  if (SoftPanel.Page == SOFT_PAGE1) /////////////////////////////////////////////////// 1 ////////////////// PAGE 1
  {
    // functions available to page 1 :
    switch (SoftPanel.Button)
    {
      case SOFT_EDIT_F1:
        router_arp_tag = !router_arp_tag;
        if (router_arp_tag == 0) active_arp = false;
        break;

      case SOFT_EDIT_F2:
        ui_seqPlay = !ui_seqPlay;
        seqTick = 0;
        break;
    }

    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE2;
        break;

      case SOFT_EDIT_5: // save
        Write_Patch_To_BS(uBank, uPatch); // save patch buffer in BS
        Read_Patch_From_BS(uBank, uPatch); // read into BS to update editbuffer values
        Store_LastBankPatch_to_EEPROM(uBank, uPatch); // save patch number in eeprom
        EEPROM.write(EEPROM_DEVICE, device); // save last device used (note : don't use too much write eeprom)
        // send editbuffer to core out for copy/past if necessary
        SendEditBuffer(INTERFACE_SERIAL3);
        break;

      case SOFT_EDIT_4: // set edit
        //Read_Patch_From_BS(uBank, uPatch);
        SendEditBuffer(INTERFACE_SERIAL);
        // send editbuffer to core out
        SendEditBuffer(INTERFACE_SERIAL3);
        //SoftPanel.Selected_MatrixBus = MMOD_BUS1;
        break;

      case SOFT_EDIT_3: // set orig
        SendEditBufferOrig(INTERFACE_SERIAL);
        //SoftPanel.Selected_MatrixBus = MMOD_BUS1;
        break;

      case SOFT_EDIT_2: // init patch
        InitEditBuffer();
        SendEditBuffer(INTERFACE_SERIAL);
        //SoftPanel.Selected_MatrixBus = MMOD_BUS1;
        // send editbuffer to core out
        SendEditBuffer(INTERFACE_SERIAL3);
        break;

      case SOFT_EDIT_1: // set & request patch from the Matrix1000
        //MIDI_SendPatchBank(uBank);  // :: piss off as it let banklock enable
        MIDI_RequestEditBuffer(INTERFACE_SERIAL, uPatch );
        break;

      case SOFT_EDIT_INC: // increment 000-->001  099-->100 :: OK :)
        uPatch++;
        uPatch = uPatch + Shift ;
        if (uPatch > uPatch_max) {
          uPatch = 0;
          uBank++;
          if (uBank > uBank_max)
            uBank = 0;
        }
        Read_Patch_From_BS(uBank, uPatch); // read into BS
        SendEditBuffer(INTERFACE_SERIAL);  // send edit buffer :)

        // send Unison detune value TO DO :
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune);
        // load arp parameters
        ArpLoad();

        //		Store_LastBankPatch_to_EEPROM(uBank, uPatch); // save patch number in eeprom : mauvaise idée ça va vite user l'eeprom
        //SoftPanel.Selected_MatrixBus = MMOD_BUS1;

        // send Bank et Patch number to Core  Midi out
        MIDI_SendPatchNumber(INTERFACE_SERIAL3, uBank, uPatch);
        // send editbuffer to core out
        SendEditBuffer(INTERFACE_SERIAL3);
        break;

      case SOFT_EDIT_DEC: // decrement 000-->3199 100-->099 :: OK :)
        uPatch--;
        uPatch = uPatch - Shift ;
        if (uPatch == 255) {
          uPatch = uPatch_max;
          uBank--;
          if (uBank == 255)
            uBank = uBank_max;
        }
        Read_Patch_From_BS(uBank, uPatch); // read into BS
        SendEditBuffer(INTERFACE_SERIAL);

        // send Unison detune value TO DO :
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune);
        // load arp parameters
        ArpLoad();

        //		Store_LastBankPatch_to_EEPROM(uBank, uPatch); // save patch number in eeprom : mauvaise idée ça va vite user l'eeprom

        //SoftPanel.Selected_MatrixBus = MMOD_BUS1;

        // send Bank et Patch number to Core  Midi out
        MIDI_SendPatchNumber(INTERFACE_SERIAL3, uBank, uPatch);
        // send editbuffer to core out
        SendEditBuffer(INTERFACE_SERIAL3);
        break;

      case DIN_SHIFT:
        // nope
        break;

      case SOFT_EDIT_ENC_CLIC: // encoder clic : load patch
        Read_Patch_From_BS(uBank, uPatch); // read into BS
        SendEditBuffer(INTERFACE_SERIAL);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL, UnisonDetune); // send Unison detune value 
        ArpLoad(); // load arp parameters
        //    Store_LastBankPatch_to_EEPROM(uBank, uPatch); // save patch number in eeprom : mauvaise idée ça va vite user l'eeprom
        //SoftPanel.Selected_MatrixBus = MMOD_BUS1;

        MIDI_SendPatchNumber(INTERFACE_SERIAL3, uBank, uPatch);// send Bank et Patch number to Core  Midi out
        SendEditBuffer(INTERFACE_SERIAL3); // send editbuffer to core out
        UI_Display_Patch();
        
#if DEBUG_ENC
        Serial.println(F("SOFT_EDIT_ENC_CLIC load patch"));
        // if (encoderClic) Serial.println(F("SOFT_EDIT_ENC_CLIC=1"));
#endif

        break;
    }

    // encoder
    if (Shift) {
      uBank += SoftPanel.EncoderValue;
    } else {
      uPatch += SoftPanel.EncoderValue;
    }

    if (uPatch > 200) {
      uPatch = uPatch_max;
      uBank--;
    }

    if (uPatch > uPatch_max) {
      uPatch = 0;
      uBank++;
    }

    if (uBank > 200) {
      uBank = uBank_max;
    }

    if (uBank > uBank_max) {
      uBank = 0;
    }


#if DEBUG_softpanel
    Serial.print(F("ubs patch: ")); Serial.println(uPatch);
#endif

  } // end of page 1

  else if (SoftPanel.Page == SOFT_PAGE2) { //////////////////////////////////////////////// PAGE 2 ///////////////////////////

    switch (SoftPanel.Button) {

      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE3; // goto page 3
        break;

      case SOFT_EDIT_1: // bank decrement
        if (BankNumberDump == 0) {
          BankNumberDump = 9;
        } else {
          BankNumberDump--;
        }
        break;

      case SOFT_EDIT_2: // bank increment
        BankNumberDump++;
        if (BankNumberDump > 9) {
          BankNumberDump = 0;
        }
        break;

      case SOFT_EDIT_3: // click this button to store mBank into uBS
        // tell system we will receive a bank:
        MIDI_ReceivingBank = 1;
        MIDI_BankRequest(INTERFACE_SERIAL, BankNumberDump); // :: NEED A FIX :: FIXED in beta 0.70 :)
        // GetBank(INTERFACE_SERIAL, BankNumberDump); // c'est sequnetial et ça marche pas ! v0.96
        // then store :
        //it's done automatically in router.ino Handle(sysex)
        break;

      case SOFT_EDIT_4: // decrement BS
        if (uBank == 0)
        {
          uBank = uBank_max;
        }
        else
        {
          uBank--;
        }
        break;

      case SOFT_EDIT_5: // increment BS
        uBank++;
        if (uBank > uBank_max)
        {
          uBank = 0;
        }
        break;
    }
  }
  else if (SoftPanel.Page == SOFT_PAGE3) { // it's page 3  //////////////////////////////////////////////// PAGE 3 ////////////////////////////////////
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE1; // goto page 4 // retour debut
        break;

      case SOFT_EDIT_1: // decrement patch & bank
        ProgramNumber--;
        if (ProgramNumber == 255)
        {
          ProgramNumber = uPatch_max;
          BankNumber--;
        }
        if (BankNumber == 255)
        {
          BankNumber = uBank_max;
        }
        if (matrix_modele == matrix_6)
        { // and ask corresponding Matrix patch using a SysEx msg
          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);
        }
        else
        {
          MIDI_SendPatchBank(INTERFACE_SERIAL, BankNumber); // and ask corresponding Matrix patch using a SysEx msg
          //          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber); // OK 0.96
          //          MIDI_RequestEditBuffer(INTERFACE_SERIAL, ProgramNumber); // OK 0.96
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);

        }
        break;

      case SOFT_EDIT_2: // increment patch & bank
        ProgramNumber++;
        if (ProgramNumber > uPatch_max) {
          ProgramNumber = 0;
          BankNumber++;
        }
        if (BankNumber > uBank_max) {
          BankNumber = 0;
        }
        if (matrix_modele) {
          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber);
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);
        } else {
          MIDI_SendPatchBank(INTERFACE_SERIAL, BankNumber);
          //          MIDI_SendPatchProgram(INTERFACE_SERIAL, ProgramNumber); // OK 0.96
          //          MIDI_RequestEditBuffer(INTERFACE_SERIAL, ProgramNumber);// OK 0.96
          MIDI_RequestSinglePatch(INTERFACE_SERIAL, ProgramNumber);

        }
        break;

      case SOFT_EDIT_3: // click this button to store
        Write_Patch_To_BS(uBank, uPatch);
        break;

      case SOFT_EDIT_4: // decrement BP
        uPatch--;
        if (uPatch == 255) {
          uPatch = uPatch_max;
          uBank--;
        }
        if (uBank == 255) {
          uBank = uBank_max;
        }

        if (uBank > uBank_max) {
          uBank = 0;
        }
        break;

      case SOFT_EDIT_5: // increment BP
        uPatch++;
        if (uPatch > uPatch_max) {
          uPatch = 0;
          uBank++;
        }
        if (uBank == 255) {
          uBank = uBank_max;
        }

        if (uBank > uBank_max) {
          uBank = 0;
        }
        break;

    }

    // encoder
    uPatch += SoftPanel.EncoderValue;
    if (uPatch > uPatch_max) {
      uPatch = 0;
      uBank++;
    }

    if (uPatch == 255) {
      uPatch = uPatch_max;
      uBank--;
    }

    if (uBank == 255) {
      uBank = uBank_max;
    }

    if (uBank > uBank_max) {
      uBank = 0;
    }
  } // end of page3
  else if (SoftPanel.Page == SOFT_PAGE4) ////////////////////////////////////////// PAGE 4 ////////////////////////
  {
    switch (SoftPanel.Button) {
      case DIN_PAGE:
        SoftPanel.Page = SOFT_PAGE5;
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

      default:
        break;

    }
  }
}//end of UI_Handle_Patch()



