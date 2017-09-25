#include "ui_arp.h"
#include "arp.h"
#include "seq.h"

#define seqLengthMAX 32
// https://programmingelectronics.com/tutorial-24-multi-dimensional-arrays-aka-matrix-old-version/
// default seq pattern :
const unsigned char Default_Sequence[10][32][2] PROGMEM = { // 32 steps (could be longer later), {note, velo, rest/tie}
  { //  NULL - 0
    {60, 100}, // step16
    {60, 128}, //
    {59, 100}, //
    {59, 128}, //
    {57, 100}, //
    {57, 128}, //
    {55, 100}, //
    {55, 128}, //

    {53, 100}, // step24
    {53, 128}, //
    {52, 100}, //
    {52, 128}, //
    {50, 100}, //
    {50, 128}, //
    {48, 100}, //
    {0, 0}, // step31

    {48, 100}, // step0
    {48, 128}, //
    {50, 100}, //
    {50, 128}, //
    {52, 100}, //
    {52, 128}, //
    {53, 100}, //
    {53, 128}, //

    {55, 100}, // step8
    {55, 128}, //
    {57, 100}, //
    {57, 128}, //
    {59, 100}, //
    {59, 128}, //
    {60, 100}, //
    {0, 0} //
  },
  { //  BASIC - 1
    {48, 100}, // step0
    {48, 128}, //
    {50, 100}, //
    {50, 128}, //
    {52, 100}, //
    {52, 128}, //
    {53, 100}, //
    {53, 128}, //

    {55, 100}, // step8
    {55, 128}, //
    {57, 100}, //
    {57, 128}, //
    {59, 100}, //
    {59, 128}, //
    {60, 100}, //
    {0, 0}, //

    {60, 100}, // step16
    {60, 128}, //
    {59, 100}, //
    {59, 128}, //
    {57, 100}, //
    {57, 128}, //
    {55, 100}, //
    {55, 128}, //

    {53, 100}, // step24
    {53, 128}, //
    {52, 100}, //
    {52, 128}, //
    {50, 100}, //
    {50, 128}, //
    {48, 100}, //
    {0, 0} // step31
  },
  { //  Halloween - J.Carpenter (1978) - 3
    {48, 80}, // step0
    {41, 80}, //
    {41, 80}, //
    {48, 80}, //
    {41, 80}, //
    {41, 80}, //
    {49, 80}, //
    {41, 80}, //

    {48, 80}, // step8
    {41, 80}, //
    {41, 80}, //
    {48, 80}, //
    {41, 80}, //
    {41, 80}, //
    {49, 80}, //
    {41, 80}, //

    {46, 80}, // step16
    {39, 80}, //
    {39, 80}, //
    {46, 80}, //
    {39, 80}, //
    {39, 80}, //
    {47, 80}, //
    {39, 80}, //

    {46, 80}, // step24
    {39, 80}, //
    {39, 80}, //
    {46, 80}, //
    {39, 80}, //
    {39, 80}, //
    {47, 80}, //
    {39, 80} // step31
  },
  // more below
  { // Billie Jean - M.Jackson - 4
    {67, 53}, // step0
    {60, 128},
    {62, 50}, //
    {60, 128},
    {65, 39}, //
    {60, 128},
    {67, 45}, //
    {60, 128},
    {65, 49}, //
    {60, 128},
    {62, 51}, //
    {60, 128},
    {60, 55}, //
    {60, 128},
    {62, 50}, //
    {60, 128},

    {67, 53}, // step0
    {60, 128},
    {62, 50}, //
    {60, 128},
    {65, 39}, //
    {60, 128},
    {67, 45}, //
    {60, 128},
    {65, 49}, //
    {60, 128},
    {62, 51}, //
    {60, 128},
    {60, 55}, //
    {60, 128},
    {62, 50}, //
    {60, 128}

  },
  { // Thriller - M.Jackson - 5
    {60, 104}, // step0
    {60, 128}, //
    {62, 82 }, //
    {62, 128}, //
    {65, 86 }, //
    {65, 128}, //
    {67, 104}, //
    {67, 128}, //

    {62, 104}, // step16
    {0, 0}, //
    {0, 0}, //
    {50, 74}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {62, 74}, //

    {60, 104}, // repeat
    {60, 128}, //
    {62, 82 }, //
    {62, 128}, //
    {65, 86 }, //
    {65, 128}, //
    {67, 104}, //
    {67, 128}, //

    {62, 104}, // repeat
    {0, 0}, //
    {0, 0}, //
    {50, 74}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {62, 74} //

  },
  { // I feel love - G.Moroder - 6
    {48, 57},
    {48, 52},
    {48, 54},
    {48, 55},
    {55, 62},
    {55, 62},
    {58, 54},
    {58, 55},

    {48, 57},
    {48, 52},
    {48, 54},
    {48, 55},
    {55, 62},
    {55, 62},
    {58, 54},
    {58, 55},

    {53, 57},
    {53, 52},
    {53, 54},
    {53, 55},
    {60, 62},
    {60, 62},
    {63, 54},
    {63, 55},

    {55, 57},
    {55, 52},
    {55, 54},
    {55, 55},
    {62, 62},
    {62, 62},
    {65, 54},
    {65, 55}
  },

  { //  Moskow disko - Telex - https://www.youtube.com/watch?v=z900sXr3bf0
    {55, 65 }, // step0
    {55, 59 }, //
    {53, 49 }, //
    {53, 42 }, //
    {55, 51 }, //
    {55, 50 }, //
    {53, 53 }, //
    {53, 54 }, //

    {55, 57 }, // step8
    {55, 64 }, //
    {53, 67 }, //
    {55, 46 }, //
    { 0, 0}, //
    { 0, 0}, //
    { 0, 0}, //
    { 0, 0}, //

    {58, 46 }, // step16
    {58, 41 }, //
    {55, 59 }, //
    {55, 57 }, //
    {58, 55 }, //
    { 0, 0}, //
    { 0, 0}, //
    { 0, 0}, //

    {60, 41  }, // step34
    {60, 46 }, //
    {58, 34 }, //
    {58, 39 }, //
    {60, 67 }, //
    { 0, 0}, //
    { 0, 0}, //
    { 0, 0} // step31
  },

  { //  Fame bassline https://www.youtube.com/watch?v=qjA_pDFoHnc
    {36, 80}, // step0
    {36, 80}, //
    {36, 97}, //
    {36, 80}, //
    {36, 80}, //
    {36, 80}, //
    {48, 97}, //
    {48, 80}, //

    {36, 80}, // step8
    {36, 80}, //
    {36, 80}, //
    {36, 80}, //
    {48, 80}, //
    {48, 97}, //
    {48, 80}, //
    {48, 80}, //

    {36, 80}, // repeat
    {36, 80}, //
    {36, 97}, //
    {36, 80}, //
    {36, 80}, //
    {36, 80}, //
    {48, 97}, //
    {48, 80}, //

    {36, 80}, // 
    {36, 80}, //
    {36, 80}, //
    {36, 80}, //
    {48, 80}, //
    {48, 97}, //
    {48, 80}, //
    {48, 80}, //

  },
  { //  NULL - 9
    {0, 0}, // step0
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //

    {0, 0}, // step8
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //

    {0, 0}, // step16
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //

    {0, 0}, // step34
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0} // step31
  },
  { //  NULL - 10
    {0, 0}, // step0
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //

    {0, 0}, // step8
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //

    {0, 0}, // step16
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //

    {0, 0}, // step34
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0}, //
    {0, 0} // step31
  }

};

// sequence pattern populated with zeros :
unsigned char sequence[32][2]; //
unsigned char sequenceOrig[32][2]; //
unsigned char seqNbr;
unsigned char seqLength;
unsigned char seqRecStep;
unsigned char seqPlayStep;
unsigned char seqOption; // false, rest or tie
unsigned char seqTick;
byte TrspB;
bool ui_seqRec; // true or false, used in router.c and in ui_arp.ino
bool ui_seqPlay; // true or false, used in router and ui_arp
bool seqTrig; // sequence trigger using note On/Off
bool seqMonoMode; // one note at a time on the keyboard while we play a sequence
unsigned char seqSpeed; // 0 - 255
unsigned char sGate;
unsigned char playSeqTrigger[4]; // pitch, velo, channel, seqOption provided by NoteOn in router.ino
signed char skipSeqStep;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTA recording a sequence :
//    1 : initialise the sequence when you press the 'rec mode' button
//    2 : fill the array by entering pitches with your keyboard on the right midi channel (need a routing in router while noteOn/Off}
//    3 : define the length by the quantity of received pitches
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////
// sequence Reset (or Init)
/////////////////////////////////////////////////////////////////////////////////////////
void Init_Seq(void)
{
  MIDI1.sendControlChange(123, 0, MIDI_CHANNEL); // all notes off msg

  //  seqSpeed = 3;

  // reset to default array
  for (unsigned char i = 0; i < 32; ++i)
  {
    sequence[i][0] = pgm_read_byte_near(&Default_Sequence[1][i][0]);
    sequence[i][1] = pgm_read_byte_near(&Default_Sequence[1][i][1]);
  }

  // reset seq length to zero
  seqLength = 32;
  seqRecStep = 0;

#if DEBUG_SEQ
  Serial.println(F("sequence init"));
  SerialPrintSeq();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
// Load default seq
/////////////////////////////////////////////////////////////////////////////////////////
void DefaultSeq_Load(unsigned char index)
{
  MIDI1.sendControlChange(123, 0, MIDI_CHANNEL); // all notes off msg

  // set to default array depending of index
  for (unsigned char i = 0; i < 32; ++i)
  {
    sequence[i][0] = pgm_read_byte_near(&Default_Sequence[index][i][0]);
    sequence[i][1] = pgm_read_byte_near(&Default_Sequence[index][i][1]);
  }

  seqSpeed = ArpParametersOrig[18];

  // reset seq length to zero
  seqLength = 32;
  seqRecStep = 0;

#if DEBUG_SEQ
  Serial.println(F("DefaultSeq_Load()"));
  SerialPrintSeq();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
// print sequence array on serial port (DEBUG)
/////////////////////////////////////////////////////////////////////////////////////////
void SerialPrintSeq(void)
{
#if DEBUG_SEQ
  Serial.println(F("SeqStack = {"));
  for (unsigned char i = 0; i < 32; ++i)
  {
    Serial.print(F("(")); Serial.print(sequence[i][0], HEX); Serial.print(F(",")); Serial.print(sequence[i][1], HEX); Serial.print(F("),"));

  }
  Serial.println(F("};"));
  Serial.print(F("SeqLength : ")); Serial.println(seqLength, DEC);
  Serial.println();

#endif
}

/////////////////////////////////////////////////////////////////////////////////////
// Populate the sequnce array with pitch, velocity and seqOption (rest or tie)
//////////////////////////////////////////////////////////////////////////////////////
void Rec_Seq(byte pitch, byte velo, byte channel, unsigned char opt) // 0.97a c'est bon tout marche pour remplir le tableau
{

  if (ui_seqRec && (channel == MIDI_CHANNEL)) // if we record the sequence and receive on right channel
  {
    // we can't play sequence
    ui_seqPlay = false;

    // passing from play to Rec does reset the sequence to 128 : LATER

    // passing from play to Rec does reset seqLength to zero : LATER

    // each time is called that function :
    // we receive notes to populate the sequence array :
    // notice that seqRecStep starts at 0
    sequence[seqRecStep][0] = pitch; // 1st element is note number
    sequence[seqRecStep][1] = velo; // 2nd element is velocity of that note,rest or tie
    //  sequence[seqRecStep][2] = opt; // TO EDIT LATER and replace by false, rest or tie i.e 0, 1 or 2

    // you added a note to the sequence, i.e that its sequence length increments for the next loop
    ++seqRecStep;

    // seqLength is (secReqStep - 1) as we just incremented
    seqLength = seqRecStep; // ou - 1  ?

    if (seqLength > 32) {
      // we have reach the end of the seq
      // do we initialise the array or keep for further modification ?
      Init_Seq();
      // i choose to init, but we could imagine navigate inside the steps with +/- keys in order to change pitch values on specific step
      // TO DO & IMPROVE
      seqLength = seqRecStep = 0; // rtz if higher than max

    }

#if DEBUG_SEQ
    SerialPrintSeq();
#endif

    // update display lcd :
    if (SoftPanel.Mode == Arp) UI_Display_Arp();
  }
  else if (ui_seqRec == 0) // if we don't record the sequence
  {
    seqLength = 0;
  }

  else
    return;
}
/////////////////////////////////////////////////////////////////////////////////////////
// insert fake element in Seq Array (silent note) or Tie (liaison)
/////////////////////////////////////////////////////////////////////////////////////////
void Insert_SeqRest(unsigned char param)
{
#if DEBUG_SEQ
  Serial.print(F("param = ")); Serial.println(param, DEC);
#endif

  if (ui_seqRec ) // if we record the sequence and receive on right channel
  {
    // we can't play sequence
    ui_seqPlay = false;


    sequence[seqRecStep][0] = 60; // 1st element is note number
    sequence[seqRecStep][1] = param; // 2nd element is velocity of that note, rest or tie i.e 0, 1 or 2

    //sequence[seqRecStep][2] = param; // TO EDIT LATER and replace by false,
    // you added a note to the sequence, i.e that its sequence length increments for the next loop
    ++seqRecStep;

    // seqLength is (secReqStep - 1) as we just incremented
    seqLength = seqRecStep; // ou - 1  ?

    if (seqLength > 32) {
      // we have reach the end of the seq
      // do we initialise the array or keep for further modification ?
      Init_Seq();
      // i choose to init, but we could imagine navigate inside the steps with +/- keys in order to change pitch values on specific step
      // TO DO & IMPROVE
      seqLength = seqRecStep = 0; // rtz if higher than max

    }

#if DEBUG_SEQ
    SerialPrintSeq();
#endif

    // update display lcd :
    if (SoftPanel.Mode == Arp) UI_Display_Arp();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// used to play the seq
/////////////////////////////////////////////////////////////////////////////////////////
void Play_Seq(byte pitch, byte velocity, byte channel, bool trigger)
{
  if (ui_seqPlay && (channel == MIDI_CHANNEL))
  {
    // we can't record :
    ui_seqRec = false;

    seqMonoMode = !seqMonoMode; // change state bool

#if DEBUG_SEQ
    Serial.print(F("seqMonoMode = ")); Serial.println(seqMonoMode);
#endif

    seqTick = 0; // reset du sequencer tick

    playSeqTrigger[0] = pitch;
    playSeqTrigger[1] = velocity;
    playSeqTrigger[2] = channel;
    playSeqTrigger[3] = trigger; // = seqTrig (note on/off = true/false)

    // update display lcd :
    if (SoftPanel.Mode == Arp) UI_Display_Arp();

  }
  else
    return;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// similar to ARP(), called on each tick of the clock 0xf9 (int or ext)
/////////////////////////////////////////////////////////////////////////////////////////////
void SEQ(void)
{
  //unsigned char lastPlayedSeqStep=0;
  // static unsigned char sGate;
  static unsigned char skipBack;

  if (seqTick == 0) // == 0
  {
#if DEBUG_SEQTIK
    Serial.print(F("seqTick = ")); Serial.println( seqTick);
#endif

    // update parameters :
    sGate = seqSpeed;
    TrspB = ui_TrspB;
    seqPlayStep = seqPlayStep + skipSeqStep;

    // extract an elemnt E contained in sequence, and play it as NoteOn
    // apres une certaine temporisation (correspond à la resolution):
    if (ui_seqPlay && seqTrig)
    {
      //MIDI1.sendNoteOn(sequence[seqPlayStep][0], sequence[seqPlayStep][1], MIDI_CHANNEL); // sequence[seqPlayStep][1] dans velo // ça marche 0.97b quand on ne prend pas le parametre rest/tie
      // new model with triggers:
      // pitch = Seq[i] - Seq[0] + trigger
      // idem velocity,
      // same channel but we could extract from trigger[3]
      if (sequence[seqPlayStep][1] == 0)
      {
        // skip do nothing
      }
      else if (sequence[seqPlayStep][1] > 127) // if a silent note or a  tie
      {
        // do nothing
        // if >127 increment a var skipback that you will use in the note off instezad of step-1
        ++skipBack;
      }
      else // else normal note
      {
        MIDI1.sendNoteOn(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        // NOTA : limits 0 & 127 are automatically made by sendNoteOn function Edit : pas tout à fait, ça revient à zero si >127
        //      lastPlayedSeqStep = seqPlayStep; // for very last resting note
        if (arp_send_notes)
        {
          MIDI3.sendNoteOn(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        }
      }

      // update display lcd : if you like but you'll permanently switch from other menus while playing otherwise
      if (SoftPanel.Mode == Arp && SoftPanel.Page == SOFT_PAGE2)
        UI_Display_Arp();
#if DEBUG_SEQ
      Serial.print(F("seqPlayStep = ")); Serial.println(seqPlayStep, DEC); Serial.println();
      Serial.print(F("play_Seq de seq.ino NteOn interface 1 :: pitch: $")); Serial.print(sequence[seqPlayStep][0], HEX); Serial.print(F(" velo: $")); Serial.print(sequence[seqPlayStep][1], HEX); Serial.print(F(" channel: $")); Serial.println (MIDI_CHANNEL, HEX);

#endif
    }
  }

  // before we reach the gate time, we remove our finger from the key, so we need to kill the last note played (or it will sustain) :
  // kill last resting note when latching Key
  if (ui_seqPlay && (playSeqTrigger[3] == false))
  {
    //if it was a tie note, get previous pitch and send note off
    if (sequence[seqPlayStep][1] > 127)
    {
      MIDI1.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      }
    }
    else if (sequence[seqPlayStep][1] != 0)
    {
      MIDI1.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      }
    }
    else
    {
      // it was a silent so do nothing
    }

    // chnage state of released trigger
    playSeqTrigger[3] = true;
    // reset seq step to beginning
    seqPlayStep = 0;
    skipSeqStep = 0; //reset

#if DEBUG_SEQ
    Serial.print(F("seqPlayStep = ")); Serial.println(seqPlayStep, DEC); Serial.println();
    Serial.print(F("kill last resting note when latching Key, pitch = $"));
    Serial.print(sequence[seqPlayStep][0], HEX); Serial.print(F(" velo = $")); Serial.print(sequence[seqPlayStep][1], HEX); Serial.print(F(" channel = $")); Serial.println (MIDI_CHANNEL, HEX);
#endif
  }

  // we reach gate time :
  if (seqTick == sGate) // can be modulated by seqSpeed (= 6) || (playSeqTrigger[3] == false)
  {
#if DEBUG_SEQTIK
    Serial.print(F("seqTick = ")); Serial.println( seqTick);
#endif
    // extract an element E contained in sequence array, and play it as NoteOff after a delay corresponding to sGate
    if (ui_seqPlay && seqTrig)
    {
      if ((sequence[seqPlayStep][1] == 0) || (sequence[seqPlayStep + 1][1] > 127)) // if  a 'rest' or next note is a tie
      {
        // do nothing
      }
      else if (sequence[seqPlayStep][1] > 127) // if it's a 'tie'
      {
        // play pitch of previous note with a zero velocity
        MIDI1.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
        if (arp_send_notes)
        {
          MIDI3.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
        }
        // reset counter
        skipBack = 0;
      }
      else // play note off normally
      {
        MIDI1.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        if (arp_send_notes)
        {
          MIDI3.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        }
      }

      // chnage state of released trigger
      playSeqTrigger[3] = true;
      // increment next step to play :
      ++seqPlayStep;
      skipSeqStep = 0; // reset
      if (seqPlayStep > seqLength - 1) // ça MARCHE !!! 0.97b :) RTZ when reaching end
        seqPlayStep = 0;
#if DEBUG_SEQ
      Serial.print(F("seqPlayStep = ")); Serial.println(seqPlayStep, DEC); Serial.println();
      Serial.print(F("play_Seq de seq.ino NteOff interface 1 :: pitch: $")); Serial.print(sequence[seqPlayStep][0], HEX); Serial.print(F(" velo: $")); Serial.print(sequence[seqPlayStep][1], HEX); Serial.print(F(" channel: $")); Serial.println (MIDI_CHANNEL, HEX);
#endif
    }
  }

  // increment the tick of seq
  ++seqTick;

  // loop reset :
  if (seqTick == (sGate + sGate)) // modulé par ui_aSpeed (=24) arp_div[arp_div_index] // non ! on fixe la durée de note et le reset se fera au double. travailler sur les gates plutot
  {
    seqTick = 0;
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////
// similar to ARP(), called on each tick of the clock 0xf9 (int or ext)
/////////////////////////////////////////////////////////////////////////////////////////////
void SEQ2(bool trig)
{
  //unsigned char lastPlayedSeqStep=0;
  // static unsigned char sGate;
  static unsigned char skipBack;

  if (trig == true) // play noteOn
  {
#if DEBUG_SEQTIK
    Serial.print(F("seqTick = ")); Serial.println( seqTick);
#endif

    // update parameters :
    sGate = seqSpeed = 0;
    TrspB = ui_TrspB;

    seqPlayStep = seqPlayStep + skipSeqStep;

    // extract an elemnt E contained in sequence, and play it as NoteOn
    // apres une certaine temporisation (correspond à la resolution):
    if (ui_seqPlay && seqTrig)
    {
      //      // increment next step to play :
      ++seqPlayStep;
      skipSeqStep = 0; // reset
      if (seqPlayStep > seqLength - 1) // ça MARCHE !!! 0.99x :) RTZ when reaching end
        seqPlayStep = 0;
      //MIDI1.sendNoteOn(sequence[seqPlayStep][0], sequence[seqPlayStep][1], MIDI_CHANNEL); // sequence[seqPlayStep][1] dans velo // ça marche 0.97b quand on ne prend pas le parametre rest/tie
      // new model with triggers:
      // pitch = Seq[i] - Seq[0] + trigger
      // idem velocity,
      // same channel but we could extract from trigger[3]
      if (sequence[seqPlayStep][1] == 0)
      {
        // skip do nothing
      }
      else if (sequence[seqPlayStep][1] > 127) // if a silent note or a  tie
      {
        // do nothing
        // if >127 increment a var skipback that you will use in the note off instezad of step-1
        ++skipBack;
      }
      else // else normal note
      {
        MIDI1.sendNoteOn(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        // NOTA : limits 0 & 127 are automatically made by sendNoteOn function Edit : pas tout à fait, ça revient à zero si >127
        //      lastPlayedSeqStep = seqPlayStep; // for very last resting note
        if (arp_send_notes)
        {
          MIDI3.sendNoteOn(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        }
      }

      // update display lcd : if you like but you'll permanently switch from other menus while playing otherwise
      if (SoftPanel.Mode == Arp && SoftPanel.Page == SOFT_PAGE2)
        UI_Display_Arp();
#if DEBUG_SEQ
      Serial.print(F("seqPlayStep = ")); Serial.println(seqPlayStep, DEC); Serial.println();
      Serial.print(F("play_Seq de seq.ino NteOn interface 1 :: pitch: $")); Serial.print(sequence[seqPlayStep][0], HEX); Serial.print(F(" velo: $")); Serial.print(sequence[seqPlayStep][1], HEX); Serial.print(F(" channel: $")); Serial.println (MIDI_CHANNEL, HEX);

#endif
    }
  }

  // before we reach the gate time, we remove our finger from the key, so we need to kill the last note played (or it will sustain) :
  // kill last resting note when latching Key
  if (ui_seqPlay && (playSeqTrigger[3] == false))
  {
    //if it was a tie note, get previous pitch and send note off
    if (sequence[seqPlayStep][1] > 127)
    {
      MIDI1.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      }
    }
    else if (sequence[seqPlayStep][1] != 0)
    {
      MIDI1.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
      }
    }
    else
    {
      // it was a silent so do nothing
    }

    // chnage state of released trigger
    playSeqTrigger[3] = true;
    // reset seq step to beginning
    seqPlayStep = 0;
    skipSeqStep = 0; //reset

#if DEBUG_SEQ
    Serial.print(F("seqPlayStep = ")); Serial.println(seqPlayStep, DEC); Serial.println();
    Serial.print(F("kill last resting note when latching Key, pitch = $"));
    Serial.print(sequence[seqPlayStep][0], HEX); Serial.print(F(" velo = $")); Serial.print(sequence[seqPlayStep][1], HEX); Serial.print(F(" channel = $")); Serial.println (MIDI_CHANNEL, HEX);
#endif
  }

  // we reach gate time :
  if (trig == false) // play noteOff
  {
#if DEBUG_SEQTIK
    Serial.print(F("seqTick = ")); Serial.println( seqTick);
#endif
    // extract an element E contained in sequence array, and play it as NoteOff after a delay corresponding to sGate
    if (ui_seqPlay && seqTrig)
    {
      if ((sequence[seqPlayStep][1] == 0) || (sequence[seqPlayStep + 1][1] > 127)) // if  a 'rest' or next note is a tie
      {
        // do nothing
      }
      else if (sequence[seqPlayStep][1] > 127) // if it's a 'tie'
      {
        // play pitch of previous note with a zero velocity
        MIDI1.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
        if (arp_send_notes)
        {
          MIDI3.sendNoteOff(sequence[seqPlayStep - skipBack][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), 0, MIDI_CHANNEL);
        }
        // reset counter
        skipBack = 0;
      }
      else // play note off normally
      {
        MIDI1.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        if (arp_send_notes)
        {
          MIDI3.sendNoteOff(sequence[seqPlayStep][0] - sequence[0][0] + playSeqTrigger[0] + ((TrspB << 2) + (TrspB << 3) - 36), sequence[seqPlayStep][1] - sequence[0][1] + playSeqTrigger[1], MIDI_CHANNEL);
        }
      }

      // chnage state of released trigger
      playSeqTrigger[3] = true;

#if DEBUG_SEQ
      Serial.print(F("seqPlayStep = ")); Serial.println(seqPlayStep, DEC); Serial.println();
      Serial.print(F("play_Seq de seq.ino NteOff interface 1 :: pitch: $")); Serial.print(sequence[seqPlayStep][0], HEX); Serial.print(F(" velo: $")); Serial.print(sequence[seqPlayStep][1], HEX); Serial.print(F(" channel: $")); Serial.println (MIDI_CHANNEL, HEX);
#endif
    }
  }

  //  // increment the tick of seq
  //  ++seqTick;
  //
  //  // loop reset :
  //  if (seqTick == (sGate + sGate)) // modulé par ui_aSpeed (=24) arp_div[arp_div_index] // non ! on fixe la durée de note et le reset se fera au double. travailler sur les gates plutot
  //  {
  //    seqTick = 0;
  //  }

}
