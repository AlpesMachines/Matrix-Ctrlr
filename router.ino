#include "ui_arp.h"
#include "ui_cfg.h"
#include "arp.h"
#include "seq.h"
#include "oner.h"

///////////////////////////////////////////////////////////
// Globales variables
///////////////////////////////////////////////////////////
//extern byte mClock; // clock tick
byte seqPushedKey = 255; // key pushed while Seq played
bool SetAB;
byte aSplit; // split note A/B = C4 (3rd octave on Matrix 6 keyboard)

///////////////////////////////////////////////////////////
// routing midi note off msg
///////////////////////////////////////////////////////////
void HandleNoteOff(byte channel, byte pitch, byte velocity)
{
  if (inTest && booting)
  {
    Serial.println(F("Note Off"));

    lcd.setCursor(18, 0);
    lcd.write(254);

    return;
  }

  MIDI_Incoming = true; // show that we receive midi message

  if (channel == MIDI_CHANNEL)
  {
    // update split point: done in NoteOn
    SetAB = ui_SetAB;
    aSplit = ui_aSplit;

    // holds notesOff or pass thru:
    // if (ui_aHold) // previous version
    if (ui_aHold && SetAB && (pitch > aSplit))
      Feed_aChordLatch(pitch, velocity, channel);
    else if (ui_aHold && (SetAB == false) && (pitch < aSplit))
      Feed_aChordLatch(pitch, velocity, channel);
    else //   if(1)// ((arpN >= 7-1)||(arpN==0))
      MIDI1.sendNoteOff(pitch, velocity, channel); // let pass last noteOff after arpegio ***

    if (SetAB)
    {
      if ((pitch > aSplit) && router_arp_tag && (arpN - 1 < 7)) // ARP
      {
        // send to arp buffer
        Update_Arp( pitch, velocity, channel, 0);
        //if (arpN == 0) MIDI1.sendNoteOff(pitch, velocity, channel); // kill last note
        Play_Arp( pitch,  velocity,  channel, 0);

#if DEBUG_ARP
        Serial.println(F("ArpegiateOption false"));
#endif

      }
      else if ((pitch <= aSplit) && (ui_seqRec || ui_seqPlay)) // SEQ
      {
        if (ui_seqRec) // if we record a seq
        {
          //don't call the seqREC() with noteOff, only noteOn are useful
          MIDI1.sendNoteOff(pitch, velocity, channel); // to listen what you rec !
        }

        if (ui_seqPlay && seqMonoMode && (pitch == seqPushedKey)) // if we play a seq and we release the pushed key of NoteOn used before to trig the sequence // CA MARCHE TROP BIEN ! :)
        {
          // play notesOff from the sequence following midi clock and gate time and pattern and speed :
          // TO DO : it' done in SEQ() of seq.ino
          if (ui_toggleSeq)
          {
            return;
          }
          else
          {
            seqTrig = false; // stop trigging the sequence
            // send last note off of when we stopped the sequence : that must be done here
            // sends parameters to the Seq.ino
            Play_Seq(pitch, velocity, channel, false);
            // reset the event step count of Sequence : can be an option
            // reset seqPushedKey :
            seqPushedKey = 255;
          }
        }

        if (ui_seqPlay && seqMonoMode && (pitch != seqPushedKey)) // availability to play others notes while seq is running on the same keyboard
        {
          MIDI1.sendNoteOff(pitch, velocity, channel);
        }


#if DEBUG_ARP
        Serial.println(F("arpegiateOption FALSE"));
#endif
      }
      else // normal behaviour : play notes thru
        //MIDI1.sendNoteOff(pitch, velocity, channel); // ***
      return;

    }
    else
    {
      if ((pitch <= aSplit) && (router_arp_tag) && (arpN - 1 < 7)) // ARP
      {
        // send to arp buffer
        Update_Arp( pitch, velocity, channel, 0);
        //if (arpN == 0) MIDI1.sendNoteOff(pitch, velocity, channel); // kill last note
        Play_Arp( pitch,  velocity,  channel, 0);


#if DEBUG_ARP
        Serial.println(F("ArpegiateOption false"));
#endif

      }
      else if ((pitch > aSplit) && (ui_seqRec || ui_seqPlay)) // SEQ
      {
        if (ui_seqRec) // if we record a seq
        {
          //don't call the seqREC() with noteOff, only noteOn are useful
          MIDI1.sendNoteOff(pitch, velocity, channel); // to listen what you rec !
        }

        if (ui_seqPlay && seqMonoMode && (pitch == seqPushedKey)) // if we play a seq and we release the pushed key of NoteOn used before to trig the sequence // CA MARCHE TROP BIEN ! :)
        {
          // play notesOff from the sequence following midi clock and gate time and pattern and speed :
          // TO DO : it' done in SEQ() of seq.ino
          if (ui_toggleSeq)
          {
            return;
          }
          else
          {
            seqTrig = false; // stop trigging the sequence
            // send last note off of when we stopped the sequence : that must be done here
            // sends parameters to the Seq.ino
            Play_Seq(pitch, velocity, channel, false);
            // reset the event step count of Sequence : can be an option
            // reset seqPushedKey :
            seqPushedKey = 255;
          }
        }

        if (ui_seqPlay && seqMonoMode && (pitch != seqPushedKey)) // availability to play others notes while seq is running on the same keyboard
        {
          MIDI1.sendNoteOff(pitch, velocity, channel);
        }


#if DEBUG_ARP
        Serial.println(F("arpegiateOption FALSE"));
#endif
      }
      else // normal behaviour : play notes thru
        //MIDI1.sendNoteOff(pitch, velocity, channel); // *** double noteoff dans midimonitor donc commentaire necessaire (voir*** plus haut)
      return;
    }
  }
  if (channel == MIDI_CHANNEL + 1)
    MIDI2.sendNoteOff(pitch, velocity, channel);

  //  if (channel == MIDI_CHANNEL + 16)
  //    MIDI3.sendNoteOff(pitch, velocity, channel);

#if SOFTSERIAL_ENABLED
  if (channel == MIDI_CHANNEL + 2)
    MIDI4.sendNoteOff(pitch, velocity, channel);

  if (channel == MIDI_CHANNEL + 3)
    MIDI5.sendNoteOff(pitch, velocity, channel);
#endif

#if DEBUG_router
  Serial.print(F("Note OFF handled and sent : ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print(pitch, DEC); Serial.print(F(" ")); Serial.println (velocity, DEC);
#endif

  // external midi note trigger (e.g rimshot, note=37, channel=10)
  if (ui_external_clk == MTRGCLK && channel == MIDI_DRUMS_CHANNEL && pitch == MIDI_TRIGGER_NOTE)
  {
    ARP3(true);
    SEQ2(true);
    DOUT_PinSet(DOUT_ACTIVITY2, false); // tempo led
  }
}


///////////////////////////////////////////////////////////
// routing midi note on msg
///////////////////////////////////////////////////////////
void HandleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (inTest && booting)
  {
    // test mode
    Serial.println(F("Note On"));

    // display
    lcd.setCursor(18, 0);
    lcd.write((byte)5);

    return;
  }

  // rimshot = 37
  MIDI_Incoming = true;

  if (velocity == 0)
  { // This acts like a NoteOff:
    HandleNoteOff(channel, pitch, velocity);
  }
  else
  {
    if (channel == MIDI_CHANNEL)
    {
      // update split point:
      SetAB = ui_SetAB;
      aSplit = ui_aSplit;
      //      TrspA = ui_TrspA; // 3
      //      pitchA = pitch + ((TrspA << 2) + (TrspA << 3) - 36); // 12*x = 4*x+8*x = x<<2 + x<<3


      if (SetAB)
      {
        // ARPEGIO
        if ((pitch > aSplit) && (router_arp_tag) && (arpN < 7 - 1)) // was only 'if(active_arp)'
        {
          Update_Arp(pitch, velocity, channel , 1);
          Play_Arp( pitch,  velocity,  channel, 1);

#if DEBUG_ARP
          Serial.println(F("ArpegiateOption TRUE"));
#endif

        }
        // SEQUENCER
        else if ((pitch <= aSplit) && ((ui_seqRec == 1) || (ui_seqPlay == 1)))
        {
          if (ui_seqRec) // if we record a seq
          {
            Rec_Seq(pitch, velocity, channel, 0); // opt = 0 for the moment TO IMPROVE
            MIDI1.sendNoteOn(pitch, velocity, channel); // to listen what you rec !

#if DEBUG_SEQ
            Serial.println(F("we are in Rec_Seq() inside router"));
#endif

            return;
          }

          if (ui_seqPlay && seqMonoMode && (pitch != seqPushedKey)) // availability to play others notes while seq is running on the same keyboard
          {
            MIDI1.sendNoteOn(pitch, velocity, channel);
          }

          if (ui_seqPlay && (seqMonoMode == false)) // if we play a seq
          {
            seqPushedKey = pitch; // key you just pushed to trigger the sequence
            // play notes from the sequence following midi clock and gate time and pattern and speed :
            // TO DO
            seqTrig = true; // trig the sequence
            // sends parameters to the Seq.ino
            Play_Seq(pitch, velocity, channel, seqTrig);

#if DEBUG_SEQ
            Serial.println(F("we are in Play_Seq() inside router"));
            Serial.print(F("pitch/velo/chnl : $")); Serial.print(pitch, HEX); Serial.print(F(" ")); Serial.print(velocity, HEX); Serial.print(F(" ")); Serial.println (channel, HEX);
            Serial.println();
#endif
            return; // stop here and return
          }

#if DEBUG_ARP
          Serial.println(F("arpegiateOption FALSE"));
#endif
        }
        else // normal behabviour : play notes
          MIDI1.sendNoteOn(pitch, velocity, channel);
        return;
      }
      else
      {
        // ARPEGIO
        if ((pitch <= aSplit) && (router_arp_tag) && (arpN < 7 - 1)) // was only 'if(active_arp)'
        {
          Update_Arp(pitch, velocity, channel , 1);
          Play_Arp( pitch,  velocity,  channel, 1);

#if DEBUG_ARP
          Serial.println(F("ArpegiateOption TRUE"));
#endif

        }
        // SEQUENCER
        else if ((pitch > aSplit) && ((ui_seqRec == 1) || (ui_seqPlay == 1)))
        {
          if (ui_seqRec) // if we record a seq
          {
            Rec_Seq(pitch, velocity, channel, 0); // opt = 0 for the moment TO IMPROVE
            MIDI1.sendNoteOn(pitch, velocity, channel); // to listen what you rec !

#if DEBUG_SEQ
            Serial.println(F("we are in Rec_Seq() inside router"));
#endif
            return;
          }

          if (ui_seqPlay && seqMonoMode && (pitch != seqPushedKey)) // availability to play others notes while seq is running on the same keyboard
          {
            MIDI1.sendNoteOn(pitch, velocity, channel);
          }

          if (ui_seqPlay && (seqMonoMode == false)) // if we play a seq
          {
            seqPushedKey = pitch; // key you just pushed to trigger the sequence
            // play notes from the sequence following midi clock and gate time and pattern and speed :
            // TO DO
            seqTrig = true; // trig the sequence
            // sends parameters to the Seq.ino
            Play_Seq(pitch, velocity, channel, seqTrig);

#if DEBUG_SEQ
            Serial.println(F("we are in Play_Seq() inside router"));
            Serial.print(F("pitch/velo/chnl : $")); Serial.print(pitch, HEX); Serial.print(F(" ")); Serial.print(velocity, HEX); Serial.print(F(" ")); Serial.println (channel, HEX);
            Serial.println();
#endif
            return; // stop here and return
          }
#if DEBUG_ARP
          Serial.println(F("arpegiateOption FALSE"));
#endif
        }
        else // normal behabviour : play notes
          MIDI1.sendNoteOn(pitch, velocity, channel);
        return;
      }
    }
    if (channel == (MIDI_CHANNEL + 1))
      MIDI2.sendNoteOn(pitch, velocity, channel);

    //if(channel == (MIDI_CHANNEL+8))
    //     MIDI3.sendNoteOn(pitch,velocity,channel);

#if SOFTSERIAL_ENABLED
    if (channel == MIDI_CHANNEL + 2)
      MIDI4.sendNoteOn(pitch, velocity, channel);

    if (channel == MIDI_CHANNEL + 3)
      MIDI5.sendNoteOn(pitch, velocity, channel);
#endif
  }

#if DEBUG_router
  Serial.print(F("Note ON  handled and sent : ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print(pitch, DEC); Serial.print(F(" ")); Serial.println (velocity, DEC);
#endif

  // external midi note trigger (e.g rimshot, note=37, channel=10)
  if (ui_external_clk == MTRGCLK && channel == MIDI_DRUMS_CHANNEL && pitch == MIDI_TRIGGER_NOTE)
  {
    ARP3(false);
    SEQ2(false);
    DOUT_PinSet(DOUT_ACTIVITY2, true); // tempo led
  }
}

/////////////////////////////////////////////////////////////////////////
//  CC router handler
////////////////////////////////////////////////////////////////////////
void HandleControlChange(byte channel, byte controlNumber, byte value)
{
  MIDI_Incoming = true;

  // Here are managed Control change messages : Control change recognized by Matrix 1000 (named as Active Controllers in operating manual) are
  // directly forwarded to the appropriate Serial port selected by the device function. Control change not recognized by Matrix 1000 but
  // generated by DAW are forwarded to the appropriate serial port regarding channel. directly recorded by a DAW. Notice that bank select (CC=00)
  // Notice than the Arduino Matrix Controller does generate CC messages when sending param sysex messages for being
  // is moreover differently managed as it commands the bank select for the Arduino Matrix Controller.
  // pseudo code :
  //  if(CC == message classiques (breath, modwheel, etc))
  //    sendControlChange(controlNumber, value, channel)
  //
  //  else
  //    MIDI_SendVoiceParam(interface,param, value, state mThru_XCc) / interface defined by channel number (1 or 2)
  // (mThru_XCc == true) if Sysex/CC enabled.

#if DEBUG_router
  Serial.print(F("ControlChange handled: ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print (controlNumber, DEC); Serial.print(F(" ")); Serial.println(value, DEC);
#endif

  if ((channel >= MIDI_CHANNEL) && (channel <= (MIDI_CHANNEL + 3)) && (controlNumber < 128) && (value < 128))
  {
    switch (controlNumber)
    {
      case 0: // bank select
      case 31: // bank change
        if (value < 10) // limits to 9 banks
        {
          uBank[device] = value;
          SoftPanel.Mode = Patch;
          SoftPanel_DisplayHandler();
          return;
          // then handleProgramchange() will do the rest
        }
        break;

      case 1: // Modwheel = LEVER2
      case 2: // breath control = LEVER3
      case 4: // footcontrol = PEDAL1
      case 6: // data entry
      case 7: // midi Volume
      case 64: // Sustain (pedal 2)
      case 96: // data increment
      case 97: // data decrement
      case 98: // NRPN LSB
      case 99: // NRPN MSB
      case 100: // RPN LSB
      case 101: // RPN MSB
      case 120: // all sounds off
      case 121: // (All Controllers Off)
      case 122: // local on/off
      case 123: // all notes off
      case 124: // omni off
      case 125: // omni on
      case 126: // mode mono
      case 127: // mode poly
        if (channel == MIDI_CHANNEL + 0)
          MIDI1.sendControlChange(controlNumber, value, channel);

        if (channel == MIDI_CHANNEL + 1)
          MIDI2.sendControlChange(controlNumber, value, channel);

        //if(channel == MIDI_CHANNEL+8)
        //     MIDI3.sendControlChange(controlNumber,value,channel);

#if SOFTSERIAL_ENABLED
        if (channel == MIDI_CHANNEL + 2)
          MIDI4.sendControlChange(controlNumber, value, channel);

        if (channel == MIDI_CHANNEL + 3)
          MIDI5.sendControlChange(controlNumber, value, channel);
#endif
        //return;
        break;

      case 94: // GliGli Unison Detune
        UnisonDetune[channel + MIDI_CHANNEL - 1] = value;
        // transmit stuff
        if (channel == MIDI_CHANNEL + 0)
          MIDI1.sendControlChange(94, value, channel);

        if (channel == MIDI_CHANNEL + 1)
          MIDI2.sendControlChange(94, value, channel);

        //if(channel == MIDI_CHANNEL+8)
        //     MIDI3.sendControlChange(controlNumber,value,channel);

#if SOFTSERIAL_ENABLED
        if (channel == MIDI_CHANNEL + 2)
          MIDI4.sendControlChange(94, value, channel);

        if (channel == MIDI_CHANNEL + 3)
          MIDI5.sendControlChange(94, value, channel);
#endif
        break;

      default:
        if (channel == MIDI_CHANNEL && mThru_XCc) // case A
        {
          MIDI_SendVoiceParam(INTERFACE_SERIAL1, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][1]), value, false);
          update_EditBuffer(Matrix_Device_A, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][1]), value); 
          UpdateDinStates();
        }
        else if (channel == (MIDI_CHANNEL + 1) && mThru_XCc) // case B
        {
          MIDI_SendVoiceParam(INTERFACE_SERIAL2, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][1]), value, false);
          update_EditBuffer(Matrix_Device_B, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3]), value);
          UpdateDinStates();
        }
        else if (channel == (MIDI_CHANNEL + 2) && mThru_XCc) // case C
        {
          MIDI_SendVoiceParam(INTERFACE_SERIAL4, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][1]), value, false);
          update_EditBuffer(Matrix_Device_C, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3]), value);
          UpdateDinStates();
        }
        else if (channel == (MIDI_CHANNEL + 3) && mThru_XCc) // case D
        {
          MIDI_SendVoiceParam(INTERFACE_SERIAL5, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][1]), value, false);
          update_EditBuffer(Matrix_Device_D, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3]), value);
          UpdateDinStates();
        }
        else
        { // echo (optional)
          //MIDI_SendVoiceParam(INTERFACE_SERIAL3, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][2]), value, false);
        }
        // legacy code :
        //        MIDI_SendVoiceParam(channel - MIDI_CHANNEL + 1, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][2]), value, false); // WORKING !!! 0.99r
        //        update_EditBuffer(device, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3]), value);
        //        //EditBuffer[device][pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3])] = value; // update the edit buffer ! ^^
        //        UpdateDinStates();
        break;

#if DEBUG_router
        Serial.print(F("ControlChange MIDI_SendVoiceParam/EditBuffer[device] : ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print (controlNumber, DEC); Serial.print(F(" ")); Serial.println(value, DEC);
        Serial.print(F("EditBuffer[device][")); Serial.print(controlNumber, HEX); Serial.print(F("] = ")); Serial.println(value, HEX);
#endif

    }
  }
  else
  {
    // not concerned, so quit
    return;
  }
}

/////////////////////////////////////////////////////////////////////////
//  CC router handler
////////////////////////////////////////////////////////////////////////
void HandleProgramChange(byte channel, byte program)
{
  MIDI_Incoming = true;

  if ((channel >= MIDI_CHANNEL) && (channel <= MIDI_CHANNEL + 4) && (program < 100))
  {
    //if you want to display stuff received :
    // // device = channel + 0x09; /// pas bon P.M bug M1000 26.2.17
    device = channel + Matrix_Device_A - MIDI_CHANNEL;
    uPatch[channel + Matrix_Device_A - MIDI_CHANNEL] = program;
    Show_Selected_Device(device);
    //INTERFACE_SERIAL = channel;

#if DEBUG_midi
    Serial.print(F("channel = ")); Serial.println(channel, DEC);
    Serial.print(F("program = ")); Serial.println(program, DEC);
    Serial.print(F("device = ")); Serial.println(channel + Matrix_Device_A - MIDI_CHANNEL, DEC);
    Serial.print(F("INTERFACE_SERIAL = ")); Serial.println(INTERFACE_SERIAL, DEC);
    Serial.print(F("uPatch[device] = ")); Serial.println(uPatch[device], DEC);
#endif
    // Read_Patch_From_BS(channel + Matrix_Device_A - MIDI_CHANNEL, uBank[channel + Matrix_Device_A - MIDI_CHANNEL], uPatch[channel + Matrix_Device_A - MIDI_CHANNEL]);

    switch (device)
    {
      case Matrix_Device_A:
        ARP_GLOBAL_INIT(device);
        Reset_UI_ARP(); // stop arp parameters
        Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
        UpdateDinStates(); // mise à jour des Leds
        SendEditBuffer(device, INTERFACE_SERIAL1);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL1, UnisonDetune[device]); // send Unison detune value
        ArpParameters_Load(device); // load arp parameters
        break;

      case Matrix_Device_B:
        Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
        UpdateDinStates(); // mise à jour des Leds
        SendEditBuffer(device, INTERFACE_SERIAL2);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL2, UnisonDetune[device]); // send Unison detune value
        break;

      case Matrix_Device_C:
        Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
        UpdateDinStates(); // mise à jour des Leds
        SendEditBuffer(device, INTERFACE_SERIAL4);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL4, UnisonDetune[device]); // send Unison detune value
        break;

      case Matrix_Device_D:
        Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
        UpdateDinStates(); // mise à jour des Leds
        SendEditBuffer(device, INTERFACE_SERIAL5);
        MIDI_Send_UNISONDETUNE(INTERFACE_SERIAL5, UnisonDetune[device]); // send Unison detune value
        break;

    }

    SoftPanel.Mode = Patch;
    SoftPanel_DisplayHandler();
    // else delete all above except read patch from BS
    return;
  }
  else
  {
    // not concerned
    return ;
  }

}

/////////////////////////////////////////////////////////////////////////
//  aftertouch router handler
////////////////////////////////////////////////////////////////////////
void HandleAfterTouchChannel(byte channel, byte pressure)
{
  MIDI_Incoming = true;

  if (channel == MIDI_CHANNEL + 0)
    MIDI1.sendAfterTouch(pressure, channel);

  if (channel == MIDI_CHANNEL + 1)
    MIDI2.sendAfterTouch(pressure, channel);

  //if(channel == MIDI_CHANNEL+8)
  //     MIDI3.sendAfterTouch(pressure, channel);

#if SOFTSERIAL_ENABLED
  if (channel == MIDI_CHANNEL + 2)
    MIDI4.sendAfterTouch(pressure, channel);

  if (channel == MIDI_CHANNEL + 3)
    MIDI5.sendAfterTouch(pressure, channel);
#endif

#if DEBUG_router
  Serial.print(F("Aftertouch handled and sent : ")); Serial.print(channel, DEC); Serial.print(F(" aftertouch ")); Serial.println(pressure);
#endif
}

/////////////////////////////////////////////////////////////////////////
//  pitchbend router handler
////////////////////////////////////////////////////////////////////////
void HandlePitchBend(byte channel, int pitchValue)
{
  MIDI_Incoming = true;

  // ça merdouille :/
  if (channel == MIDI_CHANNEL + 0)
    MIDI1.sendPitchBend(pitchValue, channel);

  if (channel == MIDI_CHANNEL + 1)
    MIDI2.sendPitchBend(pitchValue, channel);

  //if(channel == MIDI_CHANNEL+8)
  //     MIDI3.sendPitchBend(pitchValue, channel);

#if SOFTSERIAL_ENABLED
  if (channel == MIDI_CHANNEL + 2)
    MIDI4.sendPitchBend(pitchValue, channel);

  if (channel == MIDI_CHANNEL + 3)
    MIDI5.sendPitchBend(pitchValue, channel);
#endif

#if DEBUG_router
  Serial.print(F("Pitchbend handled and sent : ")); Serial.print(channel, DEC); Serial.print(F(" pitchbend = ")); Serial.println(pitchValue, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////
//  F8 clock router handler
////////////////////////////////////////////////////////////////////////
void HandleClock(void)
{
  // don't show midi here : the heartbeat LED does
  // MIDI_Incoming = true;

  if (ui_external_clk == MIDCLK)
  {
    ARP2(); // we always call ARP2()

    SEQ(); // we always call seq()

    // each time a clock message 0xf8 is received on MIDI3 Input port we increment mClock :
    ++mClock;
    if (mClock > 23) // 24 ppqn
      mClock = 0; // reset

    // Flash on each beat during a half quarter (métronome) : 6 pulses x 4 = 24 ppqn
    // ||  0  1  2  3  4  5 |  6  7  8  9 10 11 | 12 13 14 15 16 17 | 18 19 20 21 22 23 ||  //
    if (mClock > 5)
      DOUT_PinSet(DOUT_ACTIVITY2, 0); // heartbeat led
    else
      DOUT_PinSet(DOUT_ACTIVITY2, 1);

  }
#if DEBUG_router
  Serial.print(F("midi Clock handled. "));
  Serial.print(F("mClock = ")); Serial.println(mClock);
#endif
}

/////////////////////////////////////////////////////////////////////////
//  FA start router handler
////////////////////////////////////////////////////////////////////////
void HandleStart(void)
{
  MIDI_Incoming = true;
  if (ui_external_clk == MIDCLK)
  {
    mClock = -1; // reset bar (= 255)
  }
#if DEBUG_router
  Serial.print(F("midi Start handled. "));
  Serial.print(F("mClock = ")); Serial.println(mClock);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////
//  FC stop router handler
////////////////////////////////////////////////////////////////////////
void HandleStop(void)
{
  MIDI_Incoming = true;
  if (ui_external_clk == MIDCLK)
  {
    mClock = 24; // end bar
    DOUT_PinSet(DOUT_ARP, 0); // light off Arp LED
  }
#if DEBUG_router
  Serial.print(F("midi Stop handled. "));
  Serial.print(F("mClock = ")); Serial.println(mClock);
  Serial.println();
#endif
}
/////////////////////////////////////////////////////////////////////////
//  FB continue router handler
////////////////////////////////////////////////////////////////////////
void HandleContinue(void)
{
  MIDI_Incoming = true;

  // ... what to do :/ ??? nop

#if DEBUG_router
  Serial.print(F("midi Continue handled. "));
  Serial.print(F("mClock = ")); Serial.println(mClock);
  Serial.println();
#endif
}


///////////////////////////////////////////////////////
// Midi SysEx handler
////////////////////////////////////////////////////////
void HandleSystemExclusive(byte * sysex, unsigned int length)
{
  // thanks to this forum topic :
  // https://forum.arduino.cc/index.php?topic=344674.0

#if DEBUG_sysexhandler
  Serial.println(millis());
#endif

  SR.Led_Pin_Write(DOUT_ACTIVITY, 1); // midi led ON

#if DEBUG_sysexhandler
  //Print Sysex on Serial0 on serial monitor window :
  Serial.println(F("HandleSystemExclusive : "));
  for (unsigned int i = 0; i < length; ++i) {
    Serial.print(sysex[i], HEX); Serial.print(" ");
  }
  Serial.println(F("EOX "));
  Serial.print(F("length = ")); Serial.println(length, DEC);
  Serial.println();
#endif

  //   disableMidiCallbacks(); // NRT : stable 2/4/2016
  HandleBank(sysex, length); // NRT : stable 2/4/2016
  //   Serial3.flush();// NRT : stable 2/4/2016
  //   enableMidiCallbacks();  // NRT : stable 2/4/2016

  SR.Led_Pin_Write(DOUT_ACTIVITY, 0); // midi led Off
}

///////////////////////////////////////////////////////
// Midi SysEx handler
////////////////////////////////////////////////////////
unsigned int HandleBank(byte * sysex, unsigned int length)
{
  if ((sysex[1] == 0x10) && (sysex[2] == 0x06))     // for matrix synths and Matrix Ctrlr units
  {
    switch (length)
    {
      case 275:
        switch (sysex[3])
        {
          case 0x01:
            // sysex[4]: numéro de patch 0-99
            // EXTRATRICK here : scoot patch 0, next will be 1 then delay, then 3 etc.
            // Initiate a second pass manually accepting zero, delay, 2, delay, 4 etc :
            if (sysex[4] > 99) return sysex[4]; // not concerned so quit

            if (MIDI_ReceivingBank) // asking bank to the Matrix "GET"
            {
              SR.Led_Pin_Write(DOUT_F1, 1); // f1 led ON
              for (unsigned char i = 0; i < 134; ++i)
              {
                EditBuffer[device][i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
              }

              Write_Bank_To_BS2(device, uBank[device], sysex[4]);

              //last patch received :
              // reset receivig bank status
              // create iterative name, default apr and seq and uni and write them to mem2 & mem4
              if (sysex[4] == 99)
              {
                MIDI_ReceivingBank = false ;

                lcd.setCursor(0, 1);
                lcd.print(F("Bank received !    "));
                elapsedTime = 0;  //reset tmpMessage

                Write_Default_Patchname(device, uBank[device], BankNumberDump);

              }

              SR.Led_Pin_Write(DOUT_F1, 0);
              return sysex[4];
            }
            else // classic behaviour
            {
              SR.Led_Pin_Write(DOUT_SHIFT, 1); // shift led ON
              for (unsigned char i = 0; i < 134; ++i)
              {
                EditBuffer[device][i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
              }

              UpdateDinStates(); // update LEDs at the end of the sysex

              // print temp message on display
              //lcd.clear();
              lcd.setCursor(0, 1);
              lcd.print(F("Patch received : "));
              LCD_PrintBCD3(sysex[4]); //lcd.print(F(" "));
              elapsedTime = 0;  //reset tmpMessage


              Write_Bank_To_BS(device, uBank[device], sysex[4]); // write received patch without setting its number bankpatch in internal EEPROM !
              // Wire.h de l'arduino n'est pas assez rapide et n'arrive pas à faire cette fonction v0.96 : introduction dun delai de 300ms lors de l upload
              // corrigé dans 1.00c avec MINIBUFFER 128 et delay(1). corrigé encore mieux avec extratrick dans 1.02

#if (DEBUG_sysexhandler)
              Serial.print(F("sysex[4]=patch num : ")); Serial.println(sysex[4], DEC);
              for (unsigned int i = 0; i < length; ++i) {
                Serial.print(sysex[i], HEX); Serial.print(F(" "));
              }
              Serial.println(F(" "));

              Serial.println(F("EditBuffer[device] : "));
              for (unsigned char i = 0; i < 134; ++i) {
                Serial.print(EditBuffer[device][i], HEX); Serial.print(F(" "));
              }
              Serial.println();
              Serial.println();

              Serial.print(F("END_MIDI_HandleBank "));
              Serial.println(sysex[4], DEC);
              Serial.println();
              Serial.println();
#endif

#if DEBUG_factory
              Serial.print(F("{"));
              for (unsigned char i = 0; i < 133; ++i) {
                Serial.print(F("0x")); if (EditBuffer[device][i] < 0x10) Serial.print(F("0")); Serial.print(EditBuffer[device][i], HEX); Serial.print(F(", "));
              }
              Serial.print(F("0x")); if (EditBuffer[device][133] < 0x10) Serial.print(F("0")); Serial.print(EditBuffer[device][133], HEX);
              Serial.print(F("},")); Serial.print(F(" // patch ")); Serial.println(sysex[4], DEC);
#endif
              SR.Led_Pin_Write(DOUT_SHIFT, 0); // shift led Off
              return sysex[4];
            }

            break;

          case 0x0d: //EditBuffer[device]
            SR.Led_Pin_Write(DOUT_EDIT, 1); // Shift LED ON

            // update EditBuffer[device]
            for (unsigned char i = 0; i < 134; ++i)
            {
              EditBuffer[device][i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
            }
            UpdateDinStates();

            // print temp message on display
            lcd.setCursor(0, 1);
            lcd.print(F("EditBuffer received "));
            elapsedTime = 0;  //reset tmpMessage

            // update matrix
            SendEditBuffer(device, INTERFACE_SERIAL);
            SR.Led_Pin_Write(DOUT_EDIT, 0); // Shift LED Off

#if DEBUG_sysexhandler
            Serial.println(F("EditBuffer[device] sysex msg type (received from DAW) !"));
            Serial.println(F("EditBuffer[device] sent to Mx : "));
            for (unsigned char i = 0; i < 134; ++i) {
              Serial.print(EditBuffer[device][i], HEX); Serial.print(F(" "));
            }
            Serial.println();
#endif

            return 1;
            break;

          default:
            return sysex[4];
            break;
        }
        break;

      case 351:
        if ((sysex[3] == 0x03) && (sysex[4] == 0x03))
        {
          SR.Led_Pin_Write(DOUT_SHIFT, 1); // shift led ON
          SR.Led_Pin_Write(DOUT_CFG, 1);
          //SR.Led_Pin_Write(DOUT_MATRIXMOD, 1);
          for (unsigned char i = 0; i < 172; ++i)
          {
            // define buffer
            GlobalParameters[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
            // write master parameters in internal EEPROM here :
            EEPROM.update(EEPROM_GLOBALPARAMETERS + i, GlobalParameters[i]);

#if DEBUG_inteeprom
            Serial.print(F("writing GlobalParameter[")); Serial.print(i, DEC); Serial.print(F("] on internal EEPROM addr : $")); Serial.println(EEPROM_GLOBALPARAMETERS + i , HEX);
#endif
          }

#if DEBUG_sysexhandler
          Serial.println(F("Master Parameters[ ] = { "));
          for (unsigned char i = 0; i < 172; ++i) {
            Serial.print(GlobalParameters[i], DEC); Serial.print(F(", ")); // to get it via serial debug and put into arduino IDE code easily ;)
          }
          Serial.println(F(" }"));
#endif
          // print temp message on display
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(F("Master received     "));
          elapsedTime = 0;  //reset tmpMessage

          SR.Led_Pin_Write(DOUT_SHIFT, 0); // shift led Off
          SR.Led_Pin_Write(DOUT_CFG, 0);
          //SR.Led_Pin_Write(DOUT_ARP, 0);

          return 3; // code3 = master parameters sysex
        }
        else
          return 350;

        break;

      case 6:
        if (sysex[3] == 0x0a)
        {
          uBank[device] = sysex[4];
          return 0x0a;
        }
        else
          return sysex[3];
        break;

      case 143:
        if (sysex[3] == 0x09) // arp sysex
        {
          SR.Led_Pin_Write(DOUT_ARP, 1); // arp led ON
          SR.Led_Pin_Write(DOUT_EDIT, 1);
          // get arrays
          for (unsigned char i = 0; i < 20; ++i)
          {
            ArpParametersOrig[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
          }
          for (unsigned char j = 0; j < 32; j++)
          {
            sequenceOrig[j][0] = (sysex[46 + (2 * j)] << 4) + (sysex[45 + (2 * j)] & 0x0f);
          }
          for (unsigned char k = 0; k < 32; k++)
          {
            sequenceOrig[k][1] = (sysex[78 + (2 * k)] << 4) + (sysex[77 + (2 * k)] & 0x0f);
          }
          //saving is done by the Bank dump,
          //so upload arpsysex BEFORE PatchData

          SR.Led_Pin_Write(DOUT_ARP, 0); // arp led ON
          SR.Led_Pin_Write(DOUT_EDIT, 0);
          return 0x09;
        }
        else
          return sysex[3];
        break;

      case 9:
        if (sysex[3] == 0x08) // Unison sysex
        {
          SR.Led_Pin_Write(DOUT_EDIT, 1);
          UnisonDetuneOrig = (sysex[6] << 4) + (sysex[5] & 0x0f);
          //saving is done by the Bank dump,
          //so upload Unisonsysex BEFORE PatchData
          SR.Led_Pin_Write(DOUT_EDIT, 0);
          return 6;
        }
        else
          return sysex[3];
        break;

      case 52: // SystemCfg
        if (sysex[3] == 0x0f) // // SystemCfg
        {
          SR.Led_Pin_Write(DOUT_CFG, 1);

          // update system cfg parameters in RAM
          MIDI_CHANNEL = sysex[4];
          FilterSustainMode = sysex[5];
          uBank[Matrix_Device_A] = sysex[6];
          uPatch[Matrix_Device_A] = sysex[7];
          uBank[Matrix_Device_B] = sysex[8];
          uPatch[Matrix_Device_B] = sysex[9];
          uBank[Matrix_Device_C] = sysex[10];
          uPatch[Matrix_Device_C] = sysex[11];
          uBank[Matrix_Device_D] = sysex[12];
          uPatch[Matrix_Device_D] = sysex[13];
          device = sysex[14];
          matrix_model_A = sysex[15];
          matrix_model_B = sysex[16];
          matrix_model_C = sysex[17];
          matrix_model_D = sysex[18];
          encoder_inverted = sysex[19];
          mThru_XCc = sysex[20];
          // dummy sysex[21] EEPROM.read(EEPROM_EXTCLOCK) -> not used
          for (unsigned char i = 0; i < 4; ++i) {
            for (unsigned char j = 0; j < 5; ++j) {
              ZONE[i][j] = sysex[22 + 5 * i + j];
            }
          }
          zActive = sysex[42];
          localControl = sysex[43];

#if DEBUG_sysexhandler
          Serial.println(F("SystemCfg msg received "));
          for (unsigned char i = 0; i < 52; ++i)
          {
            Serial.print(sysex[i], HEX); Serial.print(F(" ")); // to get it via serial debug and put into code easily ;)
          }
          Serial.println();
#endif

          // print temp message on display
          lcd.setCursor(0, 1);
          lcd.print(F("System received    "));
          elapsedTime = 0;  //reset tmpMessage

          // http://www.locoduino.org/spip.php?article72

          // update system cfg parameters in ROM
          EEPROM.update(EEPROM_MIDI_CHANNEL, MIDI_CHANNEL);
          EEPROM.update(EEPROM_FILTERSUSTAIN_MODE, FilterSustainMode); // save this
          EEPROM.update(EEPROM_LASTBANKA, uBank[Matrix_Device_A]);
          EEPROM.update(EEPROM_LASTPATCHA, uPatch[Matrix_Device_A]);
          EEPROM.update(EEPROM_LASTBANKB, uBank[Matrix_Device_B]);
          EEPROM.update(EEPROM_LASTPATCHB, uPatch[Matrix_Device_B]);
          EEPROM.update(EEPROM_LASTBANKC, uBank[Matrix_Device_C]);
          EEPROM.update(EEPROM_LASTPATCHC, uPatch[Matrix_Device_C]);
          EEPROM.update(EEPROM_LASTBANKD, uBank[Matrix_Device_D]);
          EEPROM.update(EEPROM_LASTPATCHD, uPatch[Matrix_Device_D]);
          EEPROM.update(EEPROM_DEVICE, device); // save last device used (note : don't use too much write eeprom)
          EEPROM.update(EEPROM_MATRIX_MODELE_A, matrix_model_A);
          EEPROM.update(EEPROM_MATRIX_MODELE_B, matrix_model_B);
          EEPROM.update(EEPROM_MATRIX_MODELE_C, matrix_model_C);
          EEPROM.update(EEPROM_MATRIX_MODELE_D, matrix_model_D);
          EEPROM.update(EEPROM_ENCODER_INVERTED, encoder_inverted);
          EEPROM.update(EEPROM_MTHRU_XCC, mThru_XCc);
          ZONE_Save();
          EEPROM.update(EEPROM_LOCAL_CONTROL, localControl);

#if DEBUG_inteeprom
          Serial.print(F("writing SystemCfg, ")); Serial.print(F("sysex type $")); Serial.println(sysex[3], HEX);

#endif

          // reboot system
          software_Reboot();

          return 52;
        }
        else
          return sysex[3];
        break;

      case 8: // diagnostic
        if (sysex[3] == 0x0f && sysex[4] == 0x01) // echo on core out ONLY
        {
          MIDI_Rcv_Diagnostics(INTERFACE_SERIAL3, sysex[5], sysex[6]);
          return 8;
        }

        else if (sysex[3] == 0x0f && sysex[4] == 0x01) // echo on all midi outputs
        {
          MIDI_Rcv_Diagnostics(INTERFACE_SERIAL3, sysex[5], sysex[6]);
          MIDI_Rcv_Diagnostics(INTERFACE_SERIAL1, sysex[5], sysex[6]);
          MIDI_Rcv_Diagnostics(INTERFACE_SERIAL2, sysex[5], sysex[6]);
          MIDI_Rcv_Diagnostics(INTERFACE_SERIAL4, sysex[5], sysex[6]);
          MIDI_Rcv_Diagnostics(INTERFACE_SERIAL5, sysex[5], sysex[6]);
          return 8;
        }
        // other messages below
        // etc ...
        else
          return sysex[4];
        break;

      default:
        return 0x10;
        break;
    }
  }
  else
  {
    // not matrix, it is something else, so return.
    return 0;
  }
#if DEBUG_sysexhandler
  Serial.println(F("END of MIDI_HandleBank() "));
#endif
}

///////////////////////////////////////////////////////////////////////////////////
// Activate all necessary MIDI callbacks
///////////////////////////////////////////////////////////////////////////////////
void enableMidiCallbacks(void)
{
  // Connect the midi callbacks to the library,
  // Core in
  //  MIDI3.setHandleNoteOff(HandleNoteOff);
  //  MIDI3.setHandleNoteOn(HandleNoteOn);

  MIDI3.setHandleNoteOff(ZoneNoteOff);
  MIDI3.setHandleNoteOn(ZoneNoteOn);

  MIDI3.setHandleControlChange(HandleControlChange);
  MIDI3.setHandleProgramChange(HandleProgramChange);
  MIDI3.setHandleAfterTouchChannel(HandleAfterTouchChannel);
  MIDI3.setHandlePitchBend(HandlePitchBend);
  MIDI3.setHandleSystemExclusive(HandleSystemExclusive);
  MIDI3.setHandleClock(HandleClock);
  MIDI3.setHandleStart(HandleStart);
  MIDI3.setHandleContinue(HandleContinue);
  MIDI3.setHandleStop(HandleStop);

  // Midi in A (deviceA)
  //  MIDI1.setHandleNoteOff(HandleNoteOff);
  //  MIDI1.setHandleNoteOn(HandleNoteOn);

  MIDI1.setHandleNoteOff(ZoneNoteOff);
  MIDI1.setHandleNoteOn(ZoneNoteOn);

  MIDI1.setHandleControlChange(HandleControlChange);
  MIDI1.setHandleProgramChange(HandleProgramChange);
  MIDI1.setHandleAfterTouchChannel(HandleAfterTouchChannel);
  MIDI1.setHandlePitchBend(HandlePitchBend);
  //MIDI1.setHandleSystemExclusive(HandleSystemExclusive);
}

////////////////////////////////////////////////////////////////////////////////////
// Deactivate all activated MIDI callbacks
////////////////////////////////////////////////////////////////////////////////////
void disableMidiCallbacks(void)
{
  // Core MIDI IN
  MIDI3.disconnectCallbackFromType(midi::NoteOn);
  MIDI3.disconnectCallbackFromType(midi::NoteOff);
  MIDI3.disconnectCallbackFromType(midi::ControlChange);
  MIDI3.disconnectCallbackFromType(midi::ProgramChange);
  MIDI3.disconnectCallbackFromType(midi::AfterTouchChannel);
  MIDI3.disconnectCallbackFromType(midi::PitchBend);
  MIDI3.disconnectCallbackFromType(midi::SystemExclusive);
  MIDI3.disconnectCallbackFromType(midi::Clock);
  MIDI3.disconnectCallbackFromType(midi::Start);
  MIDI3.disconnectCallbackFromType(midi::Continue);
  MIDI3.disconnectCallbackFromType(midi::Stop);

  // Midi IN A
  MIDI1.disconnectCallbackFromType(midi::NoteOn);
  MIDI1.disconnectCallbackFromType(midi::NoteOff);
  MIDI1.disconnectCallbackFromType(midi::ControlChange);
  MIDI1.disconnectCallbackFromType(midi::ProgramChange); // can be removed
  MIDI1.disconnectCallbackFromType(midi::AfterTouchChannel);
  MIDI1.disconnectCallbackFromType(midi::PitchBend);
  //MIDI1.disconnectCallbackFromType(midi::SystemExclusive);
}




//
//
// A VOIR SI ON LA GARDE, un peu débile comme fonction
//
void GetBank(unsigned char interface, unsigned char bankRequested)
{
  unsigned char programRequested;
  unsigned long startTime;
  //  byte sysex_patch[] = {0xf0, 0x10,0x06,0x04,0x01,programRequested,0xf7};

  // set bank to request
  if (matrix_modele == matrix_6)
  {
    bankRequested = 0;
  }
  else
  {
    bankRequested = bankRequested;
    MIDI_SetBank(interface, bankRequested);
    programRequested = 0;
  }

#if DEBUG_sysexbankhandler
  Serial.print(F("GetBank : ")); Serial.println(bankRequested);
  Serial.println();
#endif

  for (unsigned char i = 0; i < 101; ++i)
  {
    startTime = millis();
    // sysex_patch[5] = programRequested;
    // request a program, start at 0
    switch (interface) // sending program change as the polling doesn"t work :(
    {
      case INTERFACE_SERIAL1:
        MIDI1.sendProgramChange(programRequested, MIDI_CHANNEL);
        break;

      case INTERFACE_SERIAL2:
        MIDI2.sendProgramChange(programRequested, MIDI_CHANNEL + 1);
        break;

        //        case INTERFACE_SERIAL3:
        //          MIDI3.sendProgramChange(programRequested,MIDI_CHANNEL+8);
        //        break;
#if SOFTSERIAL_ENABLED
      case INTERFACE_SERIAL4:
        MIDI4.sendProgramChange(programRequested, MIDI_CHANNEL + 3);
        break;

      case INTERFACE_SERIAL5:
        MIDI5.sendProgramChange(programRequested, MIDI_CHANNEL + 4);
        break;
#endif

      default:
        break;
    }
    lcd.setCursor(11, 1); lcd.print(programRequested, DEC);
    //MIDI_RequestEditBuffer[device](interface, programRequested);
    //MIDI1.sendSysEx(sizeof(sysex_patch), sysex_patch, true); // enfin ! ça marche !!
    MIDI_RequestSinglePatch(interface, programRequested);
    while (millis() < (startTime + 150)) // Matrix6 uses 133 msec to request and send his patchbuffer !!!
    {
      // wait ... to get EditBuffer[device] thanks to MIDI_HandleBank()
    }
    // so write it onto extEEPROM : NE PAS CHANGER delay(5) dans la lib 24LC512 sinon ça marche plus ! delayMicroseconds useless !!
    Write_Patch_To_BS(device, bankRequested, programRequested - 1); // trick is here : -1
    // then wait as this process is long :/
    while (millis() < (startTime + 700)) // at least 670 msec
    {
      // wait ... wait ... wait ... have a smoke :v
    }
    ++programRequested; // i2c is free so we can increment again and reloop !
    // last = 101
  }

  if (programRequested == 101) { // 0,...,99, end. (program was 100 at the end of the loop)
    lcd.setCursor(8, 1);
    lcd.print(F("DONE! "));
    delay(800);

#if DEBUG_sysexbankhandler
    Serial.println(F("GetBank() finished !"));
#endif

    return; // finished !
  }
  else
  {
    lcd.setCursor(8, 1); lcd.print(F("error "));
    delay(2000);

#if DEBUG_sysexbankhandler
    Serial.println(); Serial.println(F("GetBank error !!!")); Serial.println();
#endif

    return;
  }
}


