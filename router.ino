#include "ui_arp.h"
#include "arp.h"
#include "seq.h"


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
        // MIDI1.sendNoteOff(pitch, velocity, channel); // ***
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
        //  MIDI1.sendNoteOff(pitch, velocity, channel);
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
      //      TrspB = ui_TrspB;
      //      pitchA = pitch + ((TrspA << 2) + (TrspA << 3) - 36); // 12*x = 4*x+8*x = x<<2 + x<<3
      //      pitchB = pitch + ((TrspB << 2) + (TrspB << 3) - 36);

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

  if ((channel >= MIDI_CHANNEL) && (channel <= MIDI_CHANNEL + 4) && (controlNumber < 128) && (value < 128))
  {
    switch (controlNumber)
    {
      case 0: // bank select
      case 31: // bank change
        if (value < 10) // limits to 9 banks
        {
          uBank = value;
          SoftPanel.Mode = Patch;
          SoftPanel_DisplayHandler();

          return;
          // then handleProgramchange will do the rest
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
        if (channel == MIDI_CHANNEL + 3)
          MIDI4.sendControlChange(controlNumber, value, channel);

        if (channel == MIDI_CHANNEL + 4)
          MIDI5.sendControlChange(controlNumber, value, channel);
#endif
        //return;
        break;

      case 94: // GliGli Unison Detune
        UnisonDetune = value;
        // transmit stuff
        if (channel == MIDI_CHANNEL + 0)
          MIDI1.sendControlChange(94, UnisonDetune, channel);

        if (channel == MIDI_CHANNEL + 1)
          MIDI2.sendControlChange(94, UnisonDetune, channel);

        //if(channel == MIDI_CHANNEL+8)
        //     MIDI3.sendControlChange(controlNumber,value,channel);

#if SOFTSERIAL_ENABLED
        if (channel == MIDI_CHANNEL + 3)
          MIDI4.sendControlChange(94, UnisonDetune, channel);

        if (channel == MIDI_CHANNEL + 4)
          MIDI5.sendControlChange(94, UnisonDetune, channel);
#endif
        break;

      default:
        MIDI_SendVoiceParam(channel - MIDI_CHANNEL + 1, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][2]), value, false); // WORKING !!! 0.99r
        update_editbuffer(pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3]), value);
        //EditBuffer[pgm_read_word_near(&singlePatchDataFormatX[controlNumber][3])] = value; // update the edit buffer ! ^^
        UpdateDinStates();
        break;

#if DEBUG_router
        Serial.print(F("ControlChange MIDI_SendVoiceParam/editBuffer : ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print (controlNumber, DEC); Serial.print(F(" ")); Serial.println(value, DEC);
        Serial.print(F("editbuffer[")); Serial.print(controlNumber, HEX); Serial.print(F("] = ")); Serial.println(value, HEX);
#endif

    }
  }
  else
  {
    // not concerned, so quit
    return;
  }

}
/*
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

  // if Active Controllers recognized by matrix 1000 :
  if ((   controlNumber == 1 // or Modwheel = LEVER2
          || controlNumber == 2 // or breath control = LEVER3
          || controlNumber == 4 // or footcontrol = PEDAL1
          || controlNumber == 6 // or data entry
          || controlNumber == 7 // or volume
          //          || controlNumber == 31 // or bank change enable
          || controlNumber == 64 // or sustain (pedal2)
          || controlNumber == 94 // Unison detune, MIDI CC #94 (Celeste Level) [new in Firmware 1.16]
          || controlNumber == 96 // or increment
          || controlNumber == 97 // or decrement
          || controlNumber == 98 // or non reg parm LSB
          || controlNumber == 99 // or non reg parm MSB
          || controlNumber == 100 // or reg parm LSB
          || controlNumber == 101 // or reg parm MSB
          || controlNumber > 119 // or channel mode messages
      ) && (channel < (MIDI_CHANNEL + 4)) && (channel >= MIDI_CHANNEL) // avoid sending unnecessary stuff to Matrix 1000 (latency)
     )
  {
    //  if (midiThru)
    // MIDI3.sendControlChange( last_delayed_enc_param, last_delayed_enc_value, MIDI_CHANNEL + interface - 1);
    // singlePatchDataFormat[][]
    //MIDI3.sendControlChange( singlePatchDataFormat[param][4], value, MIDI_CHANNEL + interface - 1);

    if (channel == MIDI_CHANNEL + 0)
      MIDI1.sendControlChange(controlNumber, value, channel);

    if (channel == MIDI_CHANNEL + 1)
      MIDI2.sendControlChange(controlNumber, value, channel);

    //if(channel == MIDI_CHANNEL+8)
    //     MIDI3.sendControlChange(controlNumber,value,channel);

  #if SOFTSERIAL_ENABLED
    if (channel == MIDI_CHANNEL + 3)
      MIDI4.sendControlChange(controlNumber, value, channel);

    if (channel == MIDI_CHANNEL + 4)
      MIDI5.sendControlChange(controlNumber, value, channel);
  #endif
    return;
  }
  else if (((controlNumber == 0) || (controlNumber == 31)) && (channel < (MIDI_CHANNEL + 5)) && (channel >= MIDI_CHANNEL)) // Bank select or Bank change
  {
    //    // here we managed the bank change of the ext EEPROM (old method that increment every 4 banks, a bit stupid)
    //    if (value > (4 * uBank_max))
    //    {
    //      uBank = uBank_max;
    //    }
    //    else
    //    {
    //      uBank = value >> 2;
    //    }
    if (value < 10) // limits to 9 banks
    {
      uBank = value;
      SoftPanel.Mode = Patch;
      SoftPanel_DisplayHandler();

      return;
      // then handleProgramchange will do the rest
    }
  }
  else
  {
    //    for (unsigned char i = 0; i < 128; ++i) // scanning the 128  CC possible
    //    {
    //      if (controlNumber == pgm_read_word_near(&singlePatchDataFormatCC[i][4]))
    //      {
    MIDI_SendVoiceParam(channel, pgm_read_word_near(&singlePatchDataFormatX[controlNumber][2]), value, false); // WORKING !!! 0.99r
    // EditBuffer[pgm_read_word_near(&singlePatchDataFormatEB[controlNumber][3])] = value;
    //  }
    //}
    //    MIDI_SendVoiceParam(channel, controlNumber, value, false);
    //    EditBuffer[controlNumber] = value; // update the edit buffer ! ^^
  #if DEBUG_router
    Serial.print(F("ControlChange MIDI_SendVoiceParam/editBuffer : ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print (controlNumber, DEC); Serial.print(F(" ")); Serial.println(value, DEC);
    Serial.print(F("editbuffer[")); Serial.print(controlNumber, HEX); Serial.print(F("] = ")); Serial.println(value, HEX);
  #endif
    return;

  }
  #if DEBUG_router
  Serial.print(F("ControlChange handled: ")); Serial.print(channel, DEC); Serial.print(F(" ")); Serial.print (controlNumber, DEC); Serial.print(F(" ")); Serial.println(value, DEC);
  #endif
  }
*/
/////////////////////////////////////////////////////////////////////////
//  CC router handler
////////////////////////////////////////////////////////////////////////
void HandleProgramChange(byte channel, byte program)
{
  MIDI_Incoming = true;

  if ((channel >= MIDI_CHANNEL) && (channel <= MIDI_CHANNEL + 4) && (program < 100))
  {
    //if you want to display stuff received :
    //  device = channel + 0x09; /// pas bon P.M bug M1000 26.2.17
    device = channel + 0x0a - MIDI_CHANNEL;
    Show_Selected_Device(device);
    INTERFACE_SERIAL = channel;
    uPatch = program;
#if DEBUG_midi
    Serial.print(F("channel = ")); Serial.println(channel, DEC);
    Serial.print(F("program = ")); Serial.println(program, DEC);
    Serial.print(F("device = ")); Serial.println(device, DEC);
    Serial.print(F("INTERFACE_SERIAL = ")); Serial.println(INTERFACE_SERIAL, DEC);
    Serial.print(F("uPatch = ")); Serial.println(uPatch, DEC);
#endif
    Read_Patch_From_BS(uBank, program);
    // then send midi sysex !
    SendEditBuffer(INTERFACE_SERIAL);
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
  if (channel == MIDI_CHANNEL + 3)
    MIDI4.sendAfterTouch(pressure, channel);

  if (channel == MIDI_CHANNEL + 4)
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
  if (channel == MIDI_CHANNEL + 3)
    MIDI4.sendPitchBend(pitchValue, channel);

  if (channel == MIDI_CHANNEL + 4)
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
void HandleSystemExclusive(byte* sysex, unsigned int length)
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
unsigned int HandleBank(byte* sysex, unsigned int length)
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

            SR.Led_Pin_Write(DOUT_SHIFT, 1); // shift led ON
            for (unsigned char i = 0; i < 134; ++i)
            {
              EditBuffer[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
            }

            UpdateDinStates(); // update LEDs at the end of the sysex

            Write_Bank_To_BS(uBank, sysex[4]); // write received patch without setting its number bankpatch in internal EEPROM !
            // Wire.h de l'arduino n'est pas assez rapide et n'arrive pas à faire cette fonction v0.96 : introduction dun delai de 300ms lors de l upload

#if (DEBUG_sysexhandler)
            Serial.print(F("sysex[4]=patch num : ")); Serial.println(sysex[4], DEC);
            for (unsigned int i = 0; i < length; ++i) {
              Serial.print(sysex[i], HEX); Serial.print(F(" "));
            }
            Serial.println(F(" "));

            Serial.println(F("editbuffer : "));
            for (unsigned char i = 0; i < 134; ++i) {
              Serial.print(EditBuffer[i], HEX); Serial.print(F(" "));
            }
            Serial.println();
            Serial.println();

            Serial.print(F("END_MIDI_HandleBank "));
            Serial.println(sysex[4], DEC);
            Serial.println();
            Serial.println();
#endif

            SR.Led_Pin_Write(DOUT_SHIFT, 0); // shift led Off
            return sysex[4];
            break;

          case 0x0d: //editbuffer
            SR.Led_Pin_Write(DOUT_SHIFT, 1); // Shift LED ON
            // update editbuffer
            for (unsigned char i = 0; i < 134; ++i)
            {
              EditBuffer[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
            }
            UpdateDinStates();
            SendEditBuffer(INTERFACE_SERIAL);
            SR.Led_Pin_Write(DOUT_SHIFT, 0); // Shift LED Off

#if DEBUG_sysexhandler
            Serial.println(F("EditBuffer sysex msg type (received from DAW) !"));
            Serial.println(F("Editbuffer sent to Mx : "));
            for (unsigned char i = 0; i < 134; ++i) {
              Serial.print(EditBuffer[i], HEX); Serial.print(F(" "));
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
          SR.Led_Pin_Write(DOUT_EDIT, 1);
          SR.Led_Pin_Write(DOUT_MATRIXMOD, 1);
          for (unsigned char i = 0; i < 172; ++i)
          {
            // define buffer
            GlobalParameters[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
            // write master parameters in internal EEPROM here :
            EEPROM.write(EEPROM_GLOBALPARAMETERS + i, GlobalParameters[i]);

#if DEBUG_inteeprom
            Serial.print(F("writing GlobalParameter[")); Serial.print(i, DEC); Serial.print(F("] on internal EEPROM addr : $")); Serial.println(EEPROM_GLOBALPARAMETERS + i , HEX);
            Serial.println();
#endif
          }

#if DEBUG_sysexhandler
          Serial.println(F("Master Parameters[ ] = "));
          for (unsigned char i = 0; i < 172; ++i) {
            Serial.print(GlobalParameters[i], DEC); Serial.print(F(",")); // to get it via serial debug and put into code easily ;)
          }
          Serial.println();
#endif

          SR.Led_Pin_Write(DOUT_SHIFT, 0); // shift led Off
          SR.Led_Pin_Write(DOUT_EDIT, 0);
          SR.Led_Pin_Write(DOUT_ARP, 0);

          return 3; // code3 = master parameters sysex
        }
        else
          return 350;

        break;

      case 6:
        if (sysex[3] == 0x0a)
        {
          uBank = sysex[4];
          return 0x0a;
        }
        else return sysex[3];
        break;

      case 143:
        if (sysex[3] == 0x09) // arp sysex
        {
          SR.Led_Pin_Write(DOUT_ARP, 1); // arp led ON
          SR.Led_Pin_Write(DOUT_EDIT, 1);
          // get arrays
          for (unsigned char i = 0; i < 20; ++i)
          {
            ArpParameters[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
          }
          for (unsigned char j = 0; j < 32; j++)
          {
            sequence[j][0] = (sysex[46 + (2 * j)] << 4) + (sysex[45 + (2 * j)] & 0x0f);
          }
          for (unsigned char k = 0; k < 32; k++)
          {
            sequence[k][1] = (sysex[78 + (2 * k)] << 4) + (sysex[77 + (2 * k)] & 0x0f);
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
          UnisonDetune = (sysex[6] << 4) + (sysex[5] & 0x0f);
          //saving is done by the Bank dump,
          //so upload Unisonsysex BEFORE PatchData
          return 6;
        }
        else
          return sysex[3];
        break;

      case 20: // SystemCfg
        if (sysex[3] == 0x0f) // // SystemCfg
        {
          // update system cfg parameters in RAM
          MIDI_CHANNEL = sysex[4];
          FilterSustainMode = sysex[5];
          uBank = sysex[6];
          uPatch = sysex[7];
          device = sysex[8];
          matrix_model.A = sysex[9];
          matrix_model.B = sysex[10];
          matrix_model.C = sysex[11];
          matrix_model.D = sysex[12];
          encoder_inverted = sysex[13];
          mThru_XCc = sysex[14];

#if DEBUG_sysexhandler
          Serial.println(F("SystemCfg msg received "));
          for (unsigned char i = 0; i < 20; ++i)
          {
            Serial.print(sysex[i], HEX); Serial.print(F(" ")); // to get it via serial debug and put into code easily ;)
          }
          Serial.println();
#endif

          // update system cfg parameters in ROM
          EEPROM.write(EEPROM_MIDI_CHANNEL, MIDI_CHANNEL);
          EEPROM.write(EEPROM_FILTERSUSTAIN_MODE, FilterSustainMode); // save this
          EEPROM.write(EEPROM_LASTBANK, uBank);
          EEPROM.write(EEPROM_LASTPATCH, uPatch);
          EEPROM.write(EEPROM_DEVICE, device); // save last device used (note : don't use too much write eeprom)
          EEPROM.write(EEPROM_MATRIX_MODELE_A, matrix_model.A);
          EEPROM.write(EEPROM_MATRIX_MODELE_B, matrix_model.B);
          EEPROM.write(EEPROM_MATRIX_MODELE_C, matrix_model.C);
          EEPROM.write(EEPROM_MATRIX_MODELE_D, matrix_model.D);
          EEPROM.write(EEPROM_ENCODER_INVERTED, encoder_inverted);
          EEPROM.write(EEPROM_MTHRU_XCC, mThru_XCc);

#if DEBUG_inteeprom
          Serial.print(F("writing SystemCfg, ")); Serial.print(F("sysex type $")); Serial.println(sysex[3], HEX);

#endif
          // reboot system :
          Serial.println(F("rebooting"));
          software_Reboot();

          return 20;
        }
        else
          return sysex[3];
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
  //    Serial.print(F("Sysex Handled type ")); Serial.println(HandleBank,DEC);
  Serial.println(F("END_MIDI_HandleBank "));
#endif
}
///////////////////////////////////////////////////////////////////////////////////
// Activate all necessary MIDI callbacks
///////////////////////////////////////////////////////////////////////////////////
void enableMidiCallbacks(void)
{
  // Connect the midi callbacks to the library,
  // Core in
  MIDI3.setHandleNoteOff(HandleNoteOff);
  MIDI3.setHandleNoteOn(HandleNoteOn);
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
  MIDI1.setHandleNoteOff(HandleNoteOff);
  MIDI1.setHandleNoteOn(HandleNoteOn);
  MIDI1.setHandleControlChange(HandleControlChange);
  MIDI1.setHandleProgramChange(HandleProgramChange);
  MIDI1.setHandleAfterTouchChannel(HandleAfterTouchChannel);
  MIDI1.setHandlePitchBend(HandlePitchBend);
  MIDI1.setHandleSystemExclusive(HandleSystemExclusive);
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
  MIDI1.disconnectCallbackFromType(midi::SystemExclusive);
}












//////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////
//unsigned int HandleBank(byte* sysex, unsigned int length)
//{
//  // BANK DUMP (100 PATCHES)
//  if ((length == 275) && (sysex[1] == 0x10) && (sysex[2] == 0x06) && (sysex[3] == 0x01)) {
//    // sysex[4]: numéro de patch 0-99
//
//    SR.Led_Pin_Write(DOUT_SHIFT, 1); // shift led ON
//    for (unsigned char i = 0; i < 134; ++i) {
//      EditBuffer[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
//    }
//
//    UpdateDinStates(); // update LEDs at the end of the sysex
//
//    Write_Bank_To_BS(uBank, sysex[4]); // write received patch without setting its number bankpatch in internal EEPROM !
//    // Wire.h de l'arduino n'est pas assez rapide et n'arrive pas à faire cette fonction v0.96 : introduction dun delai de 300ms lors de l upload
//
//#if (DEBUG_sysexhandler)
//    Serial.print(F("sysex[4]=patch num : ")); Serial.println(sysex[4], DEC);
//    for (unsigned int i = 0; i < length; ++i) {
//      Serial.print(sysex[i], HEX); Serial.print(F(" "));
//    }
//    Serial.println(F(" "));
//
//    Serial.println(F("editbuffer : "));
//    for (unsigned char i = 0; i < 134; ++i) {
//      Serial.print(EditBuffer[i], HEX); Serial.print(F(" "));
//    }
//    Serial.println();
//    Serial.println();
//
//    Serial.print(F("END_MIDI_HandleBank "));
//    Serial.println(sysex[4], DEC);
//    Serial.println();
//    Serial.println();
//#endif
//
//    SR.Led_Pin_Write(DOUT_SHIFT, 0); // shift led Off
//    return sysex[4];
//  } /// EDITBUFFER
//  else if ((length == 275) && (sysex[1] == 0x10) && (sysex[2] == 0x06) && (sysex[3] == 0x0d)) // it's an editbuffer, sent by your DAW for example ;)
//  {
//    SR.Led_Pin_Write(DOUT_SHIFT, 1); // Shift LED ON
//    // update editbuffer
//    for (unsigned char i = 0; i < 134; ++i) {
//      EditBuffer[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
//    }
//    UpdateDinStates();
//    SendEditBuffer(INTERFACE_SERIAL);
//    SR.Led_Pin_Write(DOUT_SHIFT, 0); // Shift LED Off
//
//#if DEBUG_sysexhandler
//    Serial.println(F("EditBuffer sysex msg type (received from DAW) !"));
//    Serial.println(F("Editbuffer sent to Mx : "));
//    for (unsigned char i = 0; i < 134; ++i) {
//      Serial.print(EditBuffer[i], HEX); Serial.print(F(" "));
//    }
//    Serial.println(F("END_MIDI_HandleBank "));
//    Serial.println();
//#endif
//
//    return 1;
//  }
//  else if ((length == 351) && (sysex[1] == 0x10) && (sysex[2] == 0x06) && (sysex[3] == 0x03) && (sysex[4] == 0x03)) // Treatment of Master parameters sysex can be done here
//  {
//    SR.Led_Pin_Write(DOUT_SHIFT, 1); // shift led ON
//    SR.Led_Pin_Write(DOUT_EDIT, 1);
//    SR.Led_Pin_Write(DOUT_MATRIXMOD, 1);
//    for (unsigned char i = 0; i < 172; ++i)
//    {
//      // define buffer
//      GlobalParameters[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
//      // write master parameters in internal EEPROM here :
//      EEPROM.write(EEPROM_GLOBALPARAMETERS + i, GlobalParameters[i]);
//
//#if DEBUG_inteeprom
//      Serial.print(F("writing GlobalParameter[")); Serial.print(i, DEC); Serial.print(F("] on internal EEPROM addr : $")); Serial.println(EEPROM_GLOBALPARAMETERS + i , HEX);
//      Serial.println();
//#endif
//    }
//
//#if DEBUG_sysexhandler
//    Serial.println(F("Master Parameters[ ] = "));
//    for (unsigned char i = 0; i < 172; ++i) {
//      Serial.print(GlobalParameters[i], DEC); Serial.print(F(",")); // to get it via serial debug and put into code easily ;)
//    }
//    Serial.println();
//#endif
//
//    SR.Led_Pin_Write(DOUT_SHIFT, 0); // shift led Off
//    SR.Led_Pin_Write(DOUT_EDIT, 0);
//    SR.Led_Pin_Write(DOUT_ARP, 0);
//
//    return 3; // code3 = master parameters sysex
//  }
//  else if ((length == 6) && (sysex[1] == 0x10) && (sysex[2] == 0x06) && (sysex[3] == 0x0a))
//  {
//    uBank = sysex[4];
//    return 4;
//  }
//  else if ((length == 143) && (sysex[1] == 0x10) && (sysex[2] == 0x06) && (sysex[3] == 0x09)) // arp sysex
//  {
//    SR.Led_Pin_Write(DOUT_ARP, 1); // arp led ON
//    SR.Led_Pin_Write(DOUT_EDIT, 1);
//    // get arrays
//    for (unsigned char i = 0; i < 20; ++i) {
//      ArpParameters[i] = (sysex[5 + 2 * i + 1] << 4) + (sysex[5 + 2 * i] & 0x0f);
//    }
//    for (unsigned char j = 0; j < 32; j++) {
//      sequence[j][0] = (sysex[46 + (2 * j)] << 4) + (sysex[45 + (2 * j)] & 0x0f);
//    }
//    for (unsigned char k = 0; k < 32; k++) {
//      sequence[k][1] = (sysex[78 + (2 * k)] << 4) + (sysex[77 + (2 * k)] & 0x0f);
//    }
//    //saving is done by the Bank dump,
//    //so upload arpsysex BEFORE PatchData
//
//    SR.Led_Pin_Write(DOUT_ARP, 0); // arp led ON
//    SR.Led_Pin_Write(DOUT_EDIT, 0);
//    return 5;
//  }
//  else if ((length == 9) && (sysex[1] == 0x10) && (sysex[2] == 0x06) && (sysex[3] == 0x08)) // Unison sysex
//  {
//    UnisonDetune = (sysex[6] << 4) + (sysex[5] & 0x0f);
//    //saving is done by the Bank dump,
//    //so upload Unisonsysex BEFORE PatchData
//    return 6;
//  }
//  else // it is something else, so return.
//    return 0;
//
//
//#if DEBUG_sysexhandler
//  //    Serial.print(F("Sysex Handled type ")); Serial.println(HandleBank,DEC);
//#endif
//}








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
    MIDI_SendPatchBank(interface, bankRequested);
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
    //MIDI_RequestEditBuffer(interface, programRequested);
    //MIDI1.sendSysEx(sizeof(sysex_patch), sysex_patch, true); // enfin ! ça marche !!
    MIDI_RequestSinglePatch(interface, programRequested);
    while (millis() < (startTime + 150)) // Matrix6 uses 133 msec to request and send his patchbuffer !!!
    {
      // wait ... to get EditBuffer thanks to MIDI_HandleBank()
    }
    // so write it onto extEEPROM : NE PAS CHANGER delay(5) dans la lib 24LC512 sinon ça marche plus ! delayMicroseconds useless !!
    Write_Patch_To_BS(bankRequested, programRequested - 1); // trick is here : -1
    // then wait as this process is long :/
    while (millis() < (startTime + 700)) // at lesat 670 msec
    {
      // wait ... wait ... wait ... have a smoke :v
    }
    ++programRequested; // i2c is free so we can increment again and reloop !
    // last = 101
  }

  if (programRequested == 101) { // 0,...,99, end. (program was 100 at the end of the loop)
    lcd.setCursor(8, 1); lcd.print(F("DONE! "));
    delay(800);

#if DEBUG_sysexbankhandler
    Serial.println(); Serial.println(F("GetBank finished !")); Serial.println();
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


