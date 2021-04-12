
#include "midi.h"
#include "din.h"
#include "chaosmatrix.h"
#include "device.h"
#include "memo.h"
#include "ui_patch.h"
#include <EEPROM.h>

bool MIDI_Incoming;
bool MIDI_ReceivingEditBuffer;
bool MIDI_ReceivingBank;
byte MIDI_CHANNEL;
bool buffer_received;

unsigned int encoder_send_counter;
unsigned char mmod_in_queue;

unsigned int filtersustain_send_counter;
unsigned char filtersustain_in_queue;
unsigned char filtersustain_value;
unsigned char last_filtersustain;

unsigned char last_mmbus;
unsigned char last_mmsrc;
unsigned char last_mmval;
unsigned char last_mmdest;
unsigned char last_delayed_enc_param;
unsigned char last_delayed_enc_value;
unsigned char param_in_queue;


/////////////////////////////////////////////////////////////////////////////
//  Initializes MIDI variables
/////////////////////////////////////////////////////////////////////////////
void MIDI_Init()
{
  MIDI_Incoming = false; // handles midi indicator led

  //  load saved midi channel
  MIDI_CHANNEL = EEPROM.read(EEPROM_MIDI_CHANNEL);
  if (MIDI_CHANNEL > 16 || MIDI_CHANNEL == 0 )
  {
    MIDI_CHANNEL = 1;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_Init()/ ")); Serial.print (F("MIDI_CHANNEL = ")); Serial.println(MIDI_CHANNEL, DEC);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send program change on current MIDI channel
/////////////////////////////////////////////////////////////////////////////
void MIDI_SendPatchProgram(unsigned char interface, unsigned char program)
{
  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendProgramChange(program, MIDI_CHANNEL);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendProgramChange(program, MIDI_CHANNEL + 1);
      break;

    case INTERFACE_SERIAL3:
      MIDI3.sendProgramChange(program, MIDI_CHANNEL + device - Matrix_Device_A);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendProgramChange(program, MIDI_CHANNEL + 2);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendProgramChange(program, MIDI_CHANNEL + 3);
      break;
#endif

    default:
      break;
  }
#if DEBUG_midi
  Serial.print(F("MIDI_SendPatchProgram()/ interface ")); Serial.print(interface, DEC); Serial.print (F("/ program = ")); Serial.println(program, DEC);
#endif

}

//////////////////////////////////////////////////////////////////////////////
//  Send bank/program number on default core out
/////////////////////////////////////////////////////////////////////////////
void MIDI_SendPatchNumber(unsigned char interface, unsigned char bank, unsigned char program)
{
  // send this type of message (3 at a time)
  // bank select MSB - CC00 :: bank (0-9)
  // bank select LSB - CC32 :: 0 (always)
  // program change - PC :: program
  // http://www.music-software-development.com/midi-tutorial.html

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendControlChange (0x00, bank, MIDI_CHANNEL);
      MIDI1.sendControlChange (0x20, 0, MIDI_CHANNEL);
      MIDI1.sendProgramChange(program, MIDI_CHANNEL);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendControlChange (0x00, bank, MIDI_CHANNEL + 1);
      MIDI2.sendControlChange (0x20, 0, MIDI_CHANNEL + 1);
      MIDI2.sendProgramChange(program, MIDI_CHANNEL + 1);
      break;

    case INTERFACE_SERIAL3:
      MIDI3.sendControlChange (0x00, bank, MIDI_CHANNEL + device - Matrix_Device_A);
      MIDI3.sendControlChange (0x20, 0, MIDI_CHANNEL + device - Matrix_Device_A);
      MIDI3.sendProgramChange(program, MIDI_CHANNEL + device - Matrix_Device_A);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendControlChange (0x20, bank, MIDI_CHANNEL + 2);
      MIDI4.sendControlChange (0x20, 0, MIDI_CHANNEL + 2);
      MIDI4.sendProgramChange(program, MIDI_CHANNEL + 2);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendControlChange (0x20, bank, MIDI_CHANNEL + 3);
      MIDI5.sendControlChange (0x20, 0, MIDI_CHANNEL + 3);
      MIDI5.sendProgramChange(program, MIDI_CHANNEL + 3);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_SendPatchNumber()/ interface ")); Serial.print(interface, DEC); Serial.print (F("/ bank = ")); Serial.print(bank, DEC);
  Serial.print(F("/ program = ")); Serial.println(program, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send request a single patch from the current bank
/////////////////////////////////////////////////////////////////////////////
void MIDI_RequestSinglePatch(unsigned char interface, unsigned char number)
{
  // Request data <type> = 0x01 : a single patch from the current bank
  byte sysex[7] = { 0xf0, 0x10, 0x06, 0x04, 0x01, number, 0xF7};
  MIDI_ReceivingEditBuffer = 1;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = systmClock;
      systmClock = MIDCLK; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      systmClock = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_RequestSinglePatch()/ interface ")); Serial.print(interface, DEC); Serial.print (F("/ number = ")); Serial.println(number, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send request for Global Parameters 352 bytes (see Matrix 1000 manual page 54)
/////////////////////////////////////////////////////////////////////////////
void MIDI_RequestMastersParameters(unsigned char interface)
{
  static byte sysex[7] = { 0xf0, 0x10, 0x06, 0x04, 0x03, 0x00, 0xF7};

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_RequestMastersParameters()/ ")); Serial.print (F("interface = ")); Serial.println(interface, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send request edit buffer for a specified program
/////////////////////////////////////////////////////////////////////////////
void MIDI_RequestEditBuffer(unsigned char device, unsigned char interface, unsigned char number)
{
  byte sysex[7] = { 0xf0, 0x10, 0x06, 0x04, 0x04, number, 0xF7};
  MIDI_ReceivingEditBuffer = 1;

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_RequestEditBuffer()/ [device] ")); Serial.print(device, HEX);
  Serial.print (F("/ interface = ")); Serial.print(interface, HEX); Serial.print (F("/ number = ")); Serial.println(number, DEC);
#endif

}

/////////////////////////////////////////////////////////////////////////////
// Matrix6 specific 05H = Enter Remote Edit Mode (Receive Only) Patch Edit mode
/////////////////////////////////////////////////////////////////////////////
void MIDI_EnterRemoteEditMode(unsigned char interface)
{
  static byte sysex[] = {0xf0, 0x10, 0x06, 0x05, 0xf7};

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_EnterRemoteEditMode()/ ")); Serial.print (F("interface = ")); Serial.println(interface, HEX);
  Serial.println();
#endif
}


/////////////////////////////////////////////////////////////////////////////
//  Send a 'send me your whole Bank' request to the matrix
/////////////////////////////////////////////////////////////////////////////
void MIDI_BankRequest(unsigned char interface, unsigned char bank_req)
{
  static byte sysex_setbank[] = { 0xf0, 0x10, 0x06, 0x0a, bank_req, 0xF7};
  static byte sysex_request[] = { 0xf0, 0x10, 0x06, 0x04, 0x00, 0x00, 0xF7};
  static byte sysex_delock[] = { 0xf0, 0x10, 0x06, 0x0c, 0xF7};

  switch (interface) {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex_setbank), sysex_setbank, true);
      MIDI1.sendSysEx (sizeof(sysex_request), sysex_request, true);
      MIDI1.sendSysEx (sizeof(sysex_delock), sysex_delock, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex_setbank), sysex_setbank, true);
      MIDI2.sendSysEx (sizeof(sysex_request), sysex_request, true);
      MIDI2.sendSysEx (sizeof(sysex_delock), sysex_delock, true);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex_setbank), sysex_setbank, true);
      MIDI4.sendSysEx (sizeof(sysex_request), sysex_request, true);
      MIDI4.sendSysEx (sizeof(sysex_delock), sysex_delock, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex_setbank), sysex_setbank, true);
      MIDI5.sendSysEx (sizeof(sysex_request), sysex_request, true);
      MIDI5.sendSysEx (sizeof(sysex_delock), sysex_delock, true);
      break;
#endif

    default:
      break;
  }

#if DEBUG_midi
  Serial.print(F("MIDI_BankRequest()/ ")); Serial.print (F("/ interface = ")); Serial.print(interface, HEX); Serial.print (F("/ bank_req = ")); Serial.println(bank_req, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send a matrix sysex parameter value
/////////////////////////////////////////////////////////////////////////////
void MIDI_SendVoiceParam(unsigned char interface, unsigned char param, unsigned char value, bool midiThru)
{
  static unsigned char lastparam;
  static unsigned char lastvalue;

  byte sysex[] = { 0xf0, 0x10, 0x06, 0x06, 0, 0, 0xf7 };

  sysex[4] = param;
  sysex[5] = value;

  // don't send the same message twice in a row
  if (param != lastparam || value != lastvalue)
  {
    //    //update_EditBuffer(device, param, value);
    //    switch (interface)
    //    {
    //      case  INTERFACE_SERIAL1: update_EditBuffer(Matrix_Device_A, param, value); break;
    //      case  INTERFACE_SERIAL2: update_EditBuffer(Matrix_Device_B, param, value); break;
    #if SOFTSERIAL_ENABLED
    //      case  INTERFACE_SERIAL4: update_EditBuffer(Matrix_Device_C, param, value); break;
    //      case  INTERFACE_SERIAL5: update_EditBuffer(Matrix_Device_D, param, value); break;
    #endif
    //      default: break;
    //    }
#if DEBUG_matrix
    Serial.println(F("inside MIDI_SendVoiceParam()"));
#endif

#if DEBUG_serialout
    // print debug sysex +
    Serial.print(F("inside MIDI_SendVoiceParam()/ "));
    Serial.print(F("interface = ")); Serial.print(interface, DEC); Serial.print(F("/ MIDI_SendVoiceParam - SysEx[]= "));
    Serial.print(sysex[0], HEX); Serial.print(' '); Serial.print(sysex[1], HEX); Serial.print(' '); Serial.print(sysex[2], HEX); Serial.print(' ');
    Serial.print(sysex[3], HEX); Serial.print(' '); Serial.print(sysex[4], HEX); Serial.print(' '); Serial.print(sysex[5], HEX); Serial.print(' '); Serial.print(sysex[6], HEX); Serial.print(" * ");
    Serial.print(F("param/value (DEC,DEC) : ")); Serial.print(param); Serial.print(F(",")); Serial.println(value, DEC);
    Serial.println();
#endif

    // Unison detune [new in Firmware 1.16], controlled by MIDI CC #94 (Celeste Level), value from 0 (no detune) to 127 (strongly out of tune).
    // It is always active, even in poly mode.
    // Values around 2-6 add a slow VCO-like detune that helps liven up the sound and keeps poly mode sounding in tune.
    // http://gliglisynth.blogspot.fr/2014/11/matrix-1000-rom-only-upgrade-v116.html
    
    // UNISON DETUNE :
    if (param == SX_UNISON_DETUNE)
    {
      switch (interface) // update UnisonDetune[device] with value;
      {
        case INTERFACE_SERIAL1:
          UnisonDetune[0] = value;
          break;

        case INTERFACE_SERIAL2:
          UnisonDetune[1] = value;
          break;

#if SOFTSERIAL_ENABLED
        case INTERFACE_SERIAL4:
          UnisonDetune[2] = value;
          break;

        case INTERFACE_SERIAL5:
          UnisonDetune[3] = value;
          break;
#endif
        default:
          break;
      }
      MIDI_Send_UNISONDETUNE(interface, value); // send value to device
    }
    else // rest of the parameters
    {
      switch (interface)
      {
        case  INTERFACE_SERIAL1:
          MIDI1.sendSysEx (sizeof(sysex), sysex, true);
          break;

        case INTERFACE_SERIAL2:
          MIDI2.sendSysEx (sizeof(sysex), sysex, true);
          break;

#if SOFTSERIAL_ENABLED
        case INTERFACE_SERIAL4:
          MIDI4.sendSysEx (sizeof(sysex), sysex, true);
          break;

        case INTERFACE_SERIAL5:
          MIDI5.sendSysEx (sizeof(sysex), sysex, true);
          break;
#endif

        default:
          break;
      }
    }
    // send CC to DAW :
    // filtrer les CC reconnus par le m1000 et les mettre sur un autre CC de façon à pouvoir recupérer les automations du DAW dans handleControlChange() & les CC affectants réellement le son
    // depend of the midiThru condition (SysEx/CC translator) 0.97d
    if (midiThru && (Translate_SX_CC(param) == 1 // or Modwheel = LEVER2 / exclude those specific Control number as they are used for something else
                     || Translate_SX_CC(param) == 2 // or breath control = LEVER3
                     || Translate_SX_CC(param) == 4 // or footcontrol = PEDAL1
                     || Translate_SX_CC(param) == 6 // or data entry
                     || Translate_SX_CC(param) == 7 // or volume
                     || Translate_SX_CC(param) == 31 // or bank change Matrix 1000
                     || Translate_SX_CC(param) == 0 // or bank select MSB
                     || Translate_SX_CC(param) == 32 // or bank select LSB
                     || Translate_SX_CC(param) == 64 // or sustain (pedal2)
                     // || Translate_SX_CC(param) == SX_UNISON_DETUNE // Unison detune, MIDI CC #94 (Celeste Level) 
                     || Translate_SX_CC(param) == 96 // or increment
                     || Translate_SX_CC(param) == 97 // or decrement
                     || Translate_SX_CC(param) == 98 // or non reg parm LSB
                     || Translate_SX_CC(param) == 99 // or non reg parm MSB
                     || Translate_SX_CC(param) == 100 // or reg parm LSB
                     || Translate_SX_CC(param) == 101 // or reg parm MSB
                     || Translate_SX_CC(param) > 119 ))// or channel mode messages
    {
      // DO NOP
    }
    else if (midiThru) // accepted values CC_ with Translate_SX_CC(param)
    {
      switch (interface)
      {
        case INTERFACE_SERIAL1:
          MIDI3.sendControlChange( Translate_SX_CC(param), value, MIDI_CHANNEL );
          break;
        case INTERFACE_SERIAL2:
          MIDI3.sendControlChange( Translate_SX_CC(param), value, MIDI_CHANNEL + 1);
          break;
        case INTERFACE_SERIAL4:
          MIDI3.sendControlChange( Translate_SX_CC(param), value, MIDI_CHANNEL + 2 );
          break;
        case INTERFACE_SERIAL5:
          MIDI3.sendControlChange( Translate_SX_CC(param), value, MIDI_CHANNEL + 3);
          break;
        default: break;
      }
    }
    else
    {
      // do nop
    }

  }
  lastparam = param;
  lastvalue = value;
}

/////////////////////////////////////////////////////////////////////////////
//  this is called from the soft panel encoder
//  some values can't be sent on every turn, delay the transmission of these
/////////////////////////////////////////////////////////////////////////////
void MIDI_SendDelayedVoiceParam(unsigned char param, unsigned char value)
{
  last_delayed_enc_value = value;
  last_delayed_enc_param = param;
  encoder_send_counter = 0;
  param_in_queue = 1;

#if DEBUG_midi
  Serial.print(F("MIDI_SendDelayedVoiceParam() "));
  Serial.print(F("/ param = ")); Serial.print(param, HEX);
  Serial.print(F("/ value = ")); Serial.println(value, HEX);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send matrix mod sysex parameters when the encoder stops moving, about a 3rd second
//  This is called from tick, it sends parameter changes from the soft panel
//  encoder
/////////////////////////////////////////////////////////////////////////////
void MIDI_HandleDelayedVoiceParam(unsigned char interface, bool midiThru)
{
  byte sysex[] = {0xf0, 0x10, 0x06, 0x06, last_delayed_enc_param, last_delayed_enc_value, 0xf7};

  if (!param_in_queue)
    return;
    
#if DEBUG_midi
  Serial.print(F("MIDI_HandleMatrixModTransmitDelay()/")); Serial.print (F(" interface = ")); Serial.println(interface, HEX);
#endif

  if (encoder_send_counter > 300)   // delay about a 3rd second 5120
  {
    switch (interface)
    {
      case  INTERFACE_SERIAL1:
        MIDI1.sendSysEx (sizeof(sysex), sysex, true);
        update_EditBuffer(Matrix_Device_A, last_delayed_enc_param, last_delayed_enc_value);
        break;

      case INTERFACE_SERIAL2:
        MIDI2.sendSysEx (sizeof(sysex), sysex, true);
        update_EditBuffer(Matrix_Device_B, last_delayed_enc_param, last_delayed_enc_value);
        break;

#if SOFTSERIAL_ENABLED
      case INTERFACE_SERIAL4:
        MIDI4.sendSysEx (sizeof(sysex), sysex, true);
        update_EditBuffer(Matrix_Device_C, last_delayed_enc_param, last_delayed_enc_value);
        break;

      case INTERFACE_SERIAL5:
        MIDI5.sendSysEx (sizeof(sysex), sysex, true);
        update_EditBuffer(Matrix_Device_D, last_delayed_enc_param, last_delayed_enc_value);
        break;
#endif

      default:
        break;
    }
    //    update_EditBuffer(device, last_delayed_enc_param, last_delayed_enc_value);
    encoder_send_counter = 0;
    param_in_queue = 0;

    // send CC to DAW
    // filtrer les CC reconnus par le m1000 et les mettre sur un autre CC de façon à pouvoir recupérer
    // les automations du DAW dans handleControlChange() & les CC affectants réellement le son
    if (midiThru) // accepted values CC_ with Translate_SX_CC(last_delayed_enc_param)
    {
      // MIDI3.sendControlChange( last_delayed_enc_param, last_delayed_enc_value, MIDI_CHANNEL + interface - 1);
      switch (interface)
      {
        case INTERFACE_SERIAL1:
          MIDI3.sendControlChange( Translate_SX_CC(last_delayed_enc_param), last_delayed_enc_value, MIDI_CHANNEL );
          break;
        case INTERFACE_SERIAL2:
          MIDI3.sendControlChange( Translate_SX_CC(last_delayed_enc_param), last_delayed_enc_value, MIDI_CHANNEL + 1);
          break;
        case INTERFACE_SERIAL4:
          MIDI3.sendControlChange( Translate_SX_CC(last_delayed_enc_param), last_delayed_enc_value, MIDI_CHANNEL + 2 );
          break;
        case INTERFACE_SERIAL5:
          MIDI3.sendControlChange( Translate_SX_CC(last_delayed_enc_param), last_delayed_enc_value, MIDI_CHANNEL + 3);
          break;
        default: break;
      }
    }
  }

  encoder_send_counter++;

#if DEBUG_matrix
  Serial.print(F("inside MIDI_HandleMatrixModTransmitDelay(), encoder_send_counter = ")); Serial.println(encoder_send_counter);
#endif

}

/////////////////////////////////////////////////////////////////////////////
//  Send matrix mod sysex parameters when the encoder stops moving, about a 3rd second
/////////////////////////////////////////////////////////////////////////////
void MIDI_HandleMatrixModTransmitDelay(unsigned char interface)
{
  byte sysex[] = {0xf0, 0x10, 0x06, 0x0b, last_mmbus, last_mmsrc, last_mmval, last_mmdest, 0xf7};

  if (!mmod_in_queue)
    return;

  if (encoder_send_counter > 300)   // delay about a 3rd second : 5120
  {
    if (matrix_modele == matrix_6)
    {
      // MIDI_EnterRemoteEditMode(interface);
      SendEditBuffer(device, interface); // BUG of M6 : can't handle those sysex modmatrix messages :( #hack
    }
    else
    {
      switch (interface) {
        case  INTERFACE_SERIAL1:
          MIDI1.sendSysEx (sizeof(sysex), sysex, true);
          break;

        case INTERFACE_SERIAL2:
          MIDI2.sendSysEx (sizeof(sysex), sysex, true);
          break;

#if SOFTSERIAL_ENABLED
        case INTERFACE_SERIAL4:
          MIDI4.sendSysEx (sizeof(sysex), sysex, true);
          break;

        case INTERFACE_SERIAL5:
          MIDI5.sendSysEx (sizeof(sysex), sysex, true);
          break;
#endif

        default:
          break;
      }
    }
    // send CC to DAW here ...
    // how managing 4 parameters : bus src dest val ?
    MIDI3.sendSysEx (sizeof(sysex), sysex, true); // first the sysex
    //MIDI3.sendControlChange ( 110 + last_mmbus, last_mmval, MIDI_CHANNEL + interface - 1); // 10 buses -> 119 OK then a CC
    // send a Cc corresponding to sysex. on CORE out. message is based on interface_serial: OK 1.02a 28/8/17
    // 27/09/17 using those CC will conflict with singlePatchDataFormatX[] when XCc
    // let comment until a new solution is found :
    /*
      switch (interface)
      {
      case INTERFACE_SERIAL1:
        MIDI3.sendControlChange ( 110 + last_mmbus, last_mmval, MIDI_CHANNEL);
        break;
      case INTERFACE_SERIAL2:
        MIDI3.sendControlChange ( 110 + last_mmbus, last_mmval, MIDI_CHANNEL + 1);
        break;
      case INTERFACE_SERIAL3:
        MIDI3.sendControlChange ( 110 + last_mmbus, last_mmval, MIDI_CHANNEL + 8); // normally never used
        break;
      case INTERFACE_SERIAL4:
        MIDI3.sendControlChange ( 110 + last_mmbus, last_mmval, MIDI_CHANNEL + 2 );
        break;
      case INTERFACE_SERIAL5:
        MIDI3.sendControlChange ( 110 + last_mmbus, last_mmval, MIDI_CHANNEL + 3);
        break;
      default: break;
      }
    */
    encoder_send_counter = 0;
    mmod_in_queue = 0;
  }

  encoder_send_counter++;
}

/////////////////////////////////////////////////////////////////////////////
//  We shouldn't send mmod sysex on every click of an encoder.
//  Keep track of the current value, we'll send it later when the encoder stops.
//  see MIDI_HandleMatrixModTransmitDelay()
/////////////////////////////////////////////////////////////////////////////
void MIDI_SendMatrixModParam(unsigned char mmbus, unsigned char mmsrc, unsigned char mmdest, unsigned char value)
{
  last_mmbus = mmbus;
  last_mmsrc = mmsrc;
  last_mmval = value;
  last_mmdest = mmdest;

  encoder_send_counter = 0;
  mmod_in_queue = 1;

#if DEBUG_midi
  Serial.println(F("MIDI_SendMatrixModParam()/ "));
  Serial.print(F("bus = ")); Serial.println(mmbus, HEX);
  Serial.print(F("/src = ")); Serial.println(mmsrc, HEX);
  Serial.print(F("/dest = ")); Serial.println(mmdest, HEX);
  Serial.print(F("/value = ")); Serial.println(value, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Filter sustain sysex parameter doesn't work. Instead, send the entire edit
//  buffer when the pot stops.
/////////////////////////////////////////////////////////////////////////////
void MIDI_HandleFilterSustainTransmitDelay(unsigned char interface)
{
  if (!filtersustain_in_queue)
    return;

  if (filtersustain_send_counter > 150) // about a second 15000
  {
    last_filtersustain = filtersustain_value;
    SendEditBuffer(device, interface);
    filtersustain_in_queue = 0;
    filtersustain_send_counter = 0;
  }

  filtersustain_send_counter++;

#if DEBUG_midi
  Serial.print(F("MIDI_HandleFilterSustainTransmitDelay()/ "));
  Serial.print(F("/ interface = ")); Serial.println(interface, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Keep track of filter sustain, when the pot stops moving,
//  send sysex from MIDI_HandleFilterSustainTransmitDelay()
/////////////////////////////////////////////////////////////////////////////
void MIDI_SendSustainBuffer(unsigned char value)
{
  filtersustain_value = value;
  filtersustain_send_counter = 0;
  filtersustain_in_queue = 1;

#if DEBUG_midi
  Serial.print(F("MIDI_SendSustainBuffer()/ "));
  Serial.print(F("value = ")); Serial.println(value, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Filter frequency potentiometer can optionally send out cc lever 3 (breath controller)
/////////////////////////////////////////////////////////////////////////////
void MIDI_Send_BreathController(unsigned char interface, unsigned char value)
{
  // Control change : breath controller CC02

  switch (interface) {
    case INTERFACE_SERIAL1:
      MIDI1.sendControlChange(0x02, value, MIDI_CHANNEL);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendControlChange(0x02, value, MIDI_CHANNEL + 1);
      break;

      //    case INTERFACE_SERIAL3:
      //      MIDI3.sendControlChange(0x02, value, MIDI_CHANNEL + 2);
      //      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendControlChange(0x02, value, MIDI_CHANNEL + 2);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendControlChange(0x02, value, MIDI_CHANNEL + 3);
      break;
#endif

    default:
      break;

  }
#if DEBUG_midi
  Serial.print(F("MIDI_Send_BreathController()/ "));
  Serial.print(F("/ interface = ")); Serial.print(interface, HEX); Serial.print(F("/ value = ")); Serial.println(value, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  Send Unison detune CC94 introduced by Gliggli
// http://gliglisynth.blogspot.fr/2014/11/matrix-1000-rom-only-upgrade-v116.html
/////////////////////////////////////////////////////////////////////////////
void MIDI_Send_UNISONDETUNE(unsigned char interface, unsigned char value)
{
  switch (interface) {
    case INTERFACE_SERIAL1:
      MIDI1.sendControlChange(94, value, MIDI_CHANNEL);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendControlChange(94, value, MIDI_CHANNEL + 1);
      break;

    case INTERFACE_SERIAL3:
      MIDI3.sendControlChange(94, value, MIDI_CHANNEL + 8);
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendControlChange(94, value, MIDI_CHANNEL + 2);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendControlChange(94, value, MIDI_CHANNEL + 3);
      break;
#endif

    default:
      break;
  }
#if DEBUG_midi
  Serial.print(F("MIDI_Send_UNISONDETUNE()/ "));
  Serial.print(F("/ interface = ")); Serial.print(interface, HEX); Serial.print(F("/ value = ")); Serial.println(value, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// send a midi sysex message Set Banl (manuel p44)
/////////////////////////////////////////////////////////////////////////////
void MIDI_SetBank(unsigned char interface, unsigned char bank)
{
  byte sysex[] = {0xf0, 0x10, 0x06, 0x0a, bank, 0xf7};

  switch (interface)
  {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = systmClock;
      systmClock = MIDCLK; // doing this unactivate $F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      systmClock = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }
#if DEBUG_midi
  Serial.print(F("MIDI_SetBank() (as a sysex)/ "));
  Serial.print(F("/ interface = ")); Serial.print(interface, HEX); Serial.print(F("/ bank = ")); Serial.println(bank, HEX);
  Serial.println();
#endif
}


/////////////////////////////////////////////////////////////////////////////
// diagnostics (proper to Matrix Ctrlr)
/////////////////////////////////////////////////////////////////////////////
void MIDI_Send_Diagnostics(unsigned char interface, byte order1, byte order2)
{
  byte sysex[8] = { 0xf0, 0x10, 0x06, 0x0f, 0x01, order1, order2, 0xf7};

  // 0x0f : system
  // 0x01 : diag


#if DEBUG_midi
  Serial.print(F("MIDI_Send_Diagnostics() / "));
  Serial.print(F("/ interface = ")); Serial.print(interface, HEX); Serial.print(F("/ order1 = ")); Serial.println(order1, HEX); Serial.print(F("/ order2 = ")); Serial.println(order2, HEX);
#endif

  switch (interface)
  {
    case  INTERFACE_SERIAL1:
      MIDI1.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL2:
      MIDI2.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL3:
      unsigned char stamp;
      stamp = systmClock;
      systmClock = MIDCLK; // doing this desactive F8 on serial port
      MIDI3.sendSysEx (sizeof(sysex), sysex, true);
      systmClock = stamp; // retrieve state
      break;

#if SOFTSERIAL_ENABLED
    case INTERFACE_SERIAL4:
      MIDI4.sendSysEx (sizeof(sysex), sysex, true);
      break;

    case INTERFACE_SERIAL5:
      MIDI5.sendSysEx (sizeof(sysex), sysex, true);
      break;
#endif

    default:
      break;
  }

}

/////////////////////////////////////////////////////////////////////
// translate inside code a SX_ element into a CC_ element
/////////////////////////////////////////////////////////////////////
byte Translate_SX_CC( unsigned char param)
{
  byte control;

  switch (param)
  {
    case SX_KEYBOARD_MODE         : control = CC_KEYBOARD_MODE; break;
    case SX_OSC1_FREQ             : control = CC_OSC1_FREQ; break;
    case SX_OSC1_WAVESHAPE        : control = CC_OSC1_WAVESHAPE; break;
    case SX_OSC1_PULSEWIDTH       : control = CC_OSC1_PULSEWIDTH; break;
    case SX_OSC1_LEVERMOD         : control = CC_OSC1_LEVERMOD; break;
    case SX_OSC1_WAVEFORM         : control = CC_OSC1_WAVEFORM; break;
    case SX_OSC2_FREQ             : control = CC_OSC2_FREQ; break;
    case SX_OSC2_WAVESHAPE        : control = CC_OSC2_WAVESHAPE; break;
    case SX_OSC2_PULSEWIDTH       : control = CC_OSC2_PULSEWIDTH; break;
    case SX_OSC2_LEVERMOD         : control = CC_OSC2_LEVERMOD; break;
    case SX_OSC2_WAVEFORM         : control = CC_OSC2_WAVEFORM; break;
    case SX_OSC2_DETUNE           : control = CC_OSC2_DETUNE; break;
    case SX_MIX                   : control = CC_MIX ; break;
    case SX_OSC1_KEYBOARDMOD      : control = CC_OSC1_KEYBOARDMOD ; break;
    case SX_OSC1_CLICK            : control = CC_OSC1_CLICK ; break;
    case SX_OSC2_KEYBOARDMOD      : control = CC_OSC2_KEYBOARDMOD ; break;
    case SX_OSC2_CLICK            : control = CC_OSC2_CLICK; break;
    case SX_OSC_SYNCMODE          : control = CC_OSC_SYNCMODE; break;
    case SX_FILTER_FREQ           : control = CC_FILTER_FREQ; break;
    case SX_FILTER_RESONANCE      : control = CC_FILTER_RESONANCE; break;
    case SX_FILTER_LEVERMOD       : control = CC_FILTER_LEVERMOD; break;
    case SX_FILTER_KEYBOARDMOD    : control = CC_FILTER_KEYBOARDMOD; break;
    case SX_FILTER_FM_AMOUNT      : control = CC_FILTER_FM_AMOUNT; break;
    case SX_VCA1_AMOUNT           : control = CC_VCA1_AMOUNT; break;
    case SX_PORTAMENTO_RATE       : control = CC_PORTAMENTO_RATE; break;
    case SX_PORTAMENTO_MODE       : control = CC_PORTAMENTO_MODE; break;
    case SX_PORTAMENTO_LEGATOENABLE: control = CC_PORTAMENTO_LEGATOENABLE; break;
    case SX_LFO1_SPEED            : control = CC_LFO1_SPEED; break;
    case SX_LFO1_TRIGGERMODE      : control = CC_LFO1_TRIGGERMODE; break;
    case SX_LFO1_LAGENABLE        : control = CC_LFO1_LAGENABLE ; break;
    case SX_LFO1_WAVESHAPE        : control = CC_LFO1_WAVESHAPE ; break;
    case SX_LFO1_RETRIGGERPOINT   : control = CC_LFO1_RETRIGGERPOINT; break;
    case SX_LFO1_SAMPLEDSOURCE    : control = CC_LFO1_SAMPLEDSOURCE; break;
    case SX_LFO1_AMPLITUDE        : control = CC_LFO1_AMPLITUDE ; break;
    case SX_LFO2_SPEED            : control = CC_LFO2_SPEED; break;
    case SX_LFO2_TRIGGERMODE      : control = CC_LFO2_TRIGGERMODE; break;
    case SX_LFO2_LAGENABLE        : control = CC_LFO2_LAGENABLE; break;
    case SX_LFO2_WAVESHAPE        : control = CC_LFO2_WAVESHAPE ; break;
    case SX_LFO2_RETRIGGERPOINT   : control = CC_LFO2_RETRIGGERPOINT ; break;
    case SX_LFO2_SAMPLEDSOURCE    : control = CC_LFO2_SAMPLEDSOURCE ; break;
    case SX_LFO2_AMPLITUDE        : control = CC_LFO2_AMPLITUDE   ; break;
    case SX_ENV1_TRIGGERMODE      : control = CC_ENV1_TRIGGERMODE   ; break;
    case SX_ENV1_DELAY            : control = CC_ENV1_DELAY ; break;
    case SX_ENV1_ATTACK           : control = CC_ENV1_ATTACK  ; break;
    case SX_ENV1_DECAY            : control = CC_ENV1_DECAY   ; break;
    case SX_ENV1_SUSTAIN          : control = CC_ENV1_SUSTAIN ; break;
    case SX_ENV1_RELEASE          : control = CC_ENV1_RELEASE ; break;
    case SX_ENV1_AMPLITUDE        : control = CC_ENV1_AMPLITUDE; break;
    case SX_ENV1_LFOTRIGGERMODE   : control = CC_ENV1_LFOTRIGGERMODE ; break;
    case SX_ENV1_RELEASEMODE      : control = CC_ENV1_RELEASEMODE ; break;
    case SX_ENV2_TRIGGERMODE      : control = CC_ENV2_TRIGGERMODE ; break;
    case SX_ENV2_DELAY            : control = CC_ENV2_DELAY; break;
    case SX_ENV2_ATTACK           : control = CC_ENV2_ATTACK; break;
    case SX_ENV2_DECAY            : control = CC_ENV2_DECAY; break;
    case SX_ENV2_SUSTAIN          : control = CC_ENV2_SUSTAIN ; break;
    case SX_ENV2_RELEASE          : control = CC_ENV2_RELEASE; break;
    case SX_ENV2_AMPLITUDE        : control = CC_ENV2_AMPLITUDE; break;
    case SX_ENV2_LFOTRIGGERMODE   : control = CC_ENV2_LFOTRIGGERMODE; break;
    case SX_ENV2_RELEASEMODE      : control = CC_ENV2_RELEASEMODE; break;
    case SX_ENV3_TRIGGERMODE      : control = CC_ENV3_TRIGGERMODE; break;
    case SX_ENV3_DELAY            : control = CC_ENV3_DELAY ; break;
    case SX_ENV3_ATTACK           : control = CC_ENV3_ATTACK; break;
    case SX_ENV3_DECAY            : control = CC_ENV3_DECAY; break;
    case SX_ENV3_SUSTAIN          : control = CC_ENV3_SUSTAIN; break;
    case SX_ENV3_RELEASE          : control = CC_ENV3_RELEASE; break;
    case SX_ENV3_AMPLITUDE        : control = CC_ENV3_AMPLITUDE; break;
    case SX_ENV3_LFOTRIGGERMODE   : control = CC_ENV3_LFOTRIGGERMODE; break;
    case SX_ENV3_RELEASEMODE      : control = CC_ENV3_RELEASEMODE; break;
    case SX_TRACK_INPUT           : control = CC_TRACK_INPUT; break;
    case SX_TRACK_POINT1          : control = CC_TRACK_POINT1; break;
    case SX_TRACK_POINT2          : control = CC_TRACK_POINT2; break;
    case SX_TRACK_POINT3          : control = CC_TRACK_POINT3; break;
    case SX_TRACK_POINT4          : control = CC_TRACK_POINT4; break;
    case SX_TRACK_POINT5          : control = CC_TRACK_POINT5; break;
    case SX_RAMP1_RATE            : control = CC_RAMP1_RATE; break;
    case SX_RAMP1_MODE            : control = CC_RAMP1_MODE; break;
    case SX_RAMP2_RATE            : control = CC_RAMP2_RATE; break;
    case SX_RAMP2_MODE            : control = CC_RAMP2_MODE; break;
    case SX_OSC1_FREQBYLFO1AMOUNT : control = CC_OSC1_FREQBYLFO1AMOUNT  ; break;
    case SX_OSC1_PWBYLFO2AMOUNT   : control = CC_OSC1_PWBYLFO2AMOUNT      ; break;
    case SX_OSC2_FREQBYLFO1AMOUNT : control = CC_OSC2_FREQBYLFO1AMOUNT  ; break;
    case SX_OSC2_PWBYLFO2AMOUNT   : control = CC_OSC2_PWBYLFO2AMOUNT      ; break;
    case SX_FILTER_FREQBYENV1AMOUNT: control = CC_FILTER_FREQBYENV1AMOUNT; break;
    case SX_FILTER_FREQBYPRESSURE : control = CC_FILTER_FREQBYPRESSURE  ; break;
    case SX_VCA1_AMPBYVELOCITY    : control = CC_VCA1_AMPBYVELOCITY        ; break;
    case SX_VCA2_AMPBYENV2        : control = CC_VCA2_AMPBYENV2; break;
    case SX_ENV1_AMPBYVELO        : control = CC_ENV1_AMPBYVELO; break;
    case SX_ENV2_AMPBYVELO        : control = CC_ENV2_AMPBYVELO; break;
    case SX_ENV3_AMPBYVELO        : control = CC_ENV3_AMPBYVELO; break;
    case SX_LFO1_AMPMODULATION    : control = CC_LFO1_AMPMODULATION; break;
    case SX_LFO2_AMPMODULATION    : control = CC_LFO2_AMPMODULATION; break;
    case SX_PORTAMENTO_SPEEDBYVELOCITY: control = CC_PORTAMENTO_SPEEDBYVELOCITY; break;
    case SX_FILTER_FMBYENV3       : control = CC_FILTER_FMBYENV3; break;
    case SX_FILTER_FMBYPRESSURE   : control = CC_FILTER_FMBYPRESSURE; break;
    case SX_LFO1_SPEEDMODULATION  : control = CC_LFO1_SPEEDMODULATION; break;
    case SX_LFO2_SPEEDMODULATION  : control = CC_LFO2_SPEEDMODULATION; break;

    case SX_UNISON_DETUNE         : control = CC_UNISON_DETUNE; break;

    default: control = param; break;

  }
  return control;
}


/////////////////////////////////////////////////////////////////////
// translate inside code a CC_ element into a SX_ element
/////////////////////////////////////////////////////////////////////
byte Translate_CC_SX(unsigned char ctrl)
{
  byte sx_param;

  switch (ctrl)
  {
    case CC_KEYBOARD_MODE         : sx_param = SX_KEYBOARD_MODE; break;
    case CC_OSC1_FREQ             : sx_param = SX_OSC1_FREQ; break;
    case CC_OSC1_WAVESHAPE        : sx_param = SX_OSC1_WAVESHAPE; break;
    case CC_OSC1_PULSEWIDTH       : sx_param = SX_OSC1_PULSEWIDTH; break;
    case CC_OSC1_LEVERMOD         : sx_param = SX_OSC1_LEVERMOD; break;
    case CC_OSC1_WAVEFORM         : sx_param = SX_OSC1_WAVEFORM; break;
    case CC_OSC2_FREQ             : sx_param = SX_OSC2_FREQ; break;
    case CC_OSC2_WAVESHAPE        : sx_param = SX_OSC2_WAVESHAPE; break;
    case CC_OSC2_PULSEWIDTH       : sx_param = SX_OSC2_PULSEWIDTH; break;
    case CC_OSC2_LEVERMOD         : sx_param = SX_OSC2_LEVERMOD; break;
    case CC_OSC2_WAVEFORM         : sx_param = SX_OSC2_WAVEFORM; break;
    case CC_OSC2_DETUNE           : sx_param = SX_OSC2_DETUNE; break;
    case CC_MIX                   : sx_param = SX_MIX ; break;
    case CC_OSC1_KEYBOARDMOD      : sx_param = SX_OSC1_KEYBOARDMOD ; break;
    case CC_OSC1_CLICK            : sx_param = SX_OSC1_CLICK ; break;
    case CC_OSC2_KEYBOARDMOD      : sx_param = SX_OSC2_KEYBOARDMOD ; break;
    case CC_OSC2_CLICK            : sx_param = SX_OSC2_CLICK; break;
    case CC_OSC_SYNCMODE          : sx_param = SX_OSC_SYNCMODE; break;
    case CC_FILTER_FREQ           : sx_param = SX_FILTER_FREQ; break;
    case CC_FILTER_RESONANCE      : sx_param = SX_FILTER_RESONANCE; break;
    case CC_FILTER_LEVERMOD       : sx_param = SX_FILTER_LEVERMOD; break;
    case CC_FILTER_KEYBOARDMOD    : sx_param = SX_FILTER_KEYBOARDMOD; break;
    case CC_FILTER_FM_AMOUNT      : sx_param = SX_FILTER_FM_AMOUNT; break;
    case CC_VCA1_AMOUNT           : sx_param = SX_VCA1_AMOUNT; break;
    case CC_PORTAMENTO_RATE       : sx_param = SX_PORTAMENTO_RATE; break;
    case CC_PORTAMENTO_MODE       : sx_param = SX_PORTAMENTO_MODE; break;
    case CC_PORTAMENTO_LEGATOENABLE: sx_param = SX_PORTAMENTO_LEGATOENABLE; break;
    case CC_LFO1_SPEED            : sx_param = SX_LFO1_SPEED; break;
    case CC_LFO1_TRIGGERMODE      : sx_param = SX_LFO1_TRIGGERMODE; break;
    case CC_LFO1_LAGENABLE        : sx_param = SX_LFO1_LAGENABLE ; break;
    case CC_LFO1_WAVESHAPE        : sx_param = SX_LFO1_WAVESHAPE ; break;
    case CC_LFO1_RETRIGGERPOINT   : sx_param = SX_LFO1_RETRIGGERPOINT; break;
    case CC_LFO1_SAMPLEDSOURCE    : sx_param = SX_LFO1_SAMPLEDSOURCE; break;
    case CC_LFO1_AMPLITUDE        : sx_param = SX_LFO1_AMPLITUDE ; break;
    case CC_LFO2_SPEED            : sx_param = SX_LFO2_SPEED; break;
    case CC_LFO2_TRIGGERMODE      : sx_param = SX_LFO2_TRIGGERMODE; break;
    case CC_LFO2_LAGENABLE        : sx_param = SX_LFO2_LAGENABLE; break;
    case CC_LFO2_WAVESHAPE        : sx_param = SX_LFO2_WAVESHAPE ; break;
    case CC_LFO2_RETRIGGERPOINT   : sx_param = SX_LFO2_RETRIGGERPOINT ; break;
    case CC_LFO2_SAMPLEDSOURCE    : sx_param = SX_LFO2_SAMPLEDSOURCE ; break;
    case CC_LFO2_AMPLITUDE        : sx_param = SX_LFO2_AMPLITUDE   ; break;
    case CC_ENV1_TRIGGERMODE      : sx_param = SX_ENV1_TRIGGERMODE   ; break;
    case CC_ENV1_DELAY            : sx_param = SX_ENV1_DELAY ; break;
    case CC_ENV1_ATTACK           : sx_param = SX_ENV1_ATTACK  ; break;
    case CC_ENV1_DECAY            : sx_param = SX_ENV1_DECAY   ; break;
    case CC_ENV1_SUSTAIN          : sx_param = SX_ENV1_SUSTAIN ; break;
    case CC_ENV1_RELEASE          : sx_param = SX_ENV1_RELEASE ; break;
    case CC_ENV1_AMPLITUDE        : sx_param = SX_ENV1_AMPLITUDE; break;
    case CC_ENV1_LFOTRIGGERMODE   : sx_param = SX_ENV1_LFOTRIGGERMODE ; break;
    case CC_ENV1_RELEASEMODE      : sx_param = SX_ENV1_RELEASEMODE ; break;
    case CC_ENV2_TRIGGERMODE      : sx_param = SX_ENV2_TRIGGERMODE ; break;
    case CC_ENV2_DELAY            : sx_param = SX_ENV2_DELAY; break;
    case CC_ENV2_ATTACK           : sx_param = SX_ENV2_ATTACK; break;
    case CC_ENV2_DECAY            : sx_param = SX_ENV2_DECAY; break;
    case CC_ENV2_SUSTAIN          : sx_param = SX_ENV2_SUSTAIN ; break;
    case CC_ENV2_RELEASE          : sx_param = SX_ENV2_RELEASE; break;
    case CC_ENV2_AMPLITUDE        : sx_param = SX_ENV2_AMPLITUDE; break;
    case CC_ENV2_LFOTRIGGERMODE   : sx_param = SX_ENV2_LFOTRIGGERMODE; break;
    case CC_ENV2_RELEASEMODE      : sx_param = SX_ENV2_RELEASEMODE; break;
    case CC_ENV3_TRIGGERMODE      : sx_param = SX_ENV3_TRIGGERMODE; break;
    case CC_ENV3_DELAY            : sx_param = SX_ENV3_DELAY ; break;
    case CC_ENV3_ATTACK           : sx_param = SX_ENV3_ATTACK; break;
    case CC_ENV3_DECAY            : sx_param = SX_ENV3_DECAY; break;
    case CC_ENV3_SUSTAIN          : sx_param = SX_ENV3_SUSTAIN; break;
    case CC_ENV3_RELEASE          : sx_param = SX_ENV3_RELEASE; break;
    case CC_ENV3_AMPLITUDE        : sx_param = SX_ENV3_AMPLITUDE; break;
    case CC_ENV3_LFOTRIGGERMODE   : sx_param = SX_ENV3_LFOTRIGGERMODE; break;
    case CC_ENV3_RELEASEMODE      : sx_param = SX_ENV3_RELEASEMODE; break;
    case CC_TRACK_INPUT           : sx_param = SX_TRACK_INPUT; break;
    case CC_TRACK_POINT1          : sx_param = SX_TRACK_POINT1; break;
    case CC_TRACK_POINT2          : sx_param = SX_TRACK_POINT2; break;
    case CC_TRACK_POINT3          : sx_param = SX_TRACK_POINT3; break;
    case CC_TRACK_POINT4          : sx_param = SX_TRACK_POINT4; break;
    case CC_TRACK_POINT5          : sx_param = SX_TRACK_POINT5; break;
    case CC_RAMP1_RATE            : sx_param = SX_RAMP1_RATE; break;
    case CC_RAMP1_MODE            : sx_param = SX_RAMP1_MODE; break;
    case CC_RAMP2_RATE            : sx_param = SX_RAMP2_RATE; break;
    case CC_RAMP2_MODE            : sx_param = SX_RAMP2_MODE; break;
    case CC_OSC1_FREQBYLFO1AMOUNT : sx_param = SX_OSC1_FREQBYLFO1AMOUNT  ; break;
    case CC_OSC1_PWBYLFO2AMOUNT   : sx_param = SX_OSC1_PWBYLFO2AMOUNT      ; break;
    case CC_OSC2_FREQBYLFO1AMOUNT : sx_param = SX_OSC2_FREQBYLFO1AMOUNT  ; break;
    case CC_OSC2_PWBYLFO2AMOUNT   : sx_param = SX_OSC2_PWBYLFO2AMOUNT      ; break;
    case CC_FILTER_FREQBYENV1AMOUNT: sx_param = SX_FILTER_FREQBYENV1AMOUNT; break;
    case CC_FILTER_FREQBYPRESSURE : sx_param = SX_FILTER_FREQBYPRESSURE  ; break;
    case CC_VCA1_AMPBYVELOCITY    : sx_param = SX_VCA1_AMPBYVELOCITY        ; break;
    case CC_VCA2_AMPBYENV2        : sx_param = SX_VCA2_AMPBYENV2; break;
    case CC_ENV1_AMPBYVELO        : sx_param = SX_ENV1_AMPBYVELO; break;
    case CC_ENV2_AMPBYVELO        : sx_param = SX_ENV2_AMPBYVELO; break;
    case CC_ENV3_AMPBYVELO        : sx_param = SX_ENV3_AMPBYVELO; break;
    case CC_LFO1_AMPMODULATION    : sx_param = SX_LFO1_AMPMODULATION; break;
    case CC_LFO2_AMPMODULATION    : sx_param = SX_LFO2_AMPMODULATION; break;
    case CC_PORTAMENTO_SPEEDBYVELOCITY: sx_param = SX_PORTAMENTO_SPEEDBYVELOCITY; break;
    case CC_FILTER_FMBYENV3       : sx_param = SX_FILTER_FMBYENV3; break;
    case CC_FILTER_FMBYPRESSURE   : sx_param = SX_FILTER_FMBYPRESSURE; break;
    case CC_LFO1_SPEEDMODULATION  : sx_param = SX_LFO1_SPEEDMODULATION; break;
    case CC_LFO2_SPEEDMODULATION  : sx_param = SX_LFO2_SPEEDMODULATION; break;

    case CC_UNISON_DETUNE         : sx_param = SX_UNISON_DETUNE; break;

    default: sx_param = ctrl; break;

  }
  return sx_param;
}
