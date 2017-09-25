/*
  // Arpegiator 
  // created by Alpes Machines 2016
  // for Matrix Ctrlr embedded
  //
  //

*/

#include <avr/pgmspace.h>

#include "memo.h"
#include "ui_patch.h"
#include "ui_arp.h"
#include "seq.h"
#include "arp.h"
#include "ui_arp.h"
#include "din.h"
#include "device.h"
#include "define.h"
#include "chaosmatrix.h"

////////////////////////////////////
// Globals
////////////////////////////////////
byte mClock; // clock tick
byte playArpTrigger[4];
unsigned int ui_aSpeed;
unsigned char ui_aMotif;
unsigned char ui_aMltp;
unsigned char ui_aGroove;
unsigned char ui_aGate;      // duration of each arpegiated note
unsigned char ui_aOctave;    // transpose value
bool ui_aHold;

bool active_arp;
bool router_arp_tag; // tag which indicates if arp if active while routing midi events
bool router_seq_tag;
bool arp_send_notes;
unsigned char pattern_number;

unsigned char arpN; // length aChord
byte aChord[8][2]; // 8 notes chord
const byte default_aChord[8][2] PROGMEM = {
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0}
};
byte aChordSorted[8][2]; // sorted chord
byte aChordPlay[128][2]; // played chord
byte aChordLatch[8][3]; // array to store notes to release when (un)latching
unsigned char latchCounter; // counter for latch release
unsigned char aTik;
unsigned char aGate;
unsigned char aDiv; // time division of the arp
unsigned char aStep;
unsigned char aOctave;
unsigned char aMltp = 1;
unsigned char aStepMax;
bool weakTap;
unsigned char aGroove;
unsigned char aPattern;
byte TrspA;
byte lastNote[4] = {0, 0, 0, 0};

unsigned char  ArpParameters[4][20]; // each device has its own ArpParameters[4][20]
unsigned char  ArpParametersOrig[20];

const unsigned char Default_ArpParameters[20][3] PROGMEM = {
  // default, min, max
  {0, 0 , 5},   // ui_ext_clock state bool
  {120, 20, 230},   // bpm value
  {false, false, true},  // thru : arp_send_notes
  {0, 0, 1},  // grv
  {false, false, true}, // ui_aHold
  {true, false, true},  // ui_SetAB
  {59, 1, 126}, // ui_aSplit = 59; // B before C3
  {3, 0, 6},  //  ui_TrspA = 3
  {3, 0, 6},  //  ui_TrspB = 3
  {2, 0, 10}, //  SEQ nbr
  {0, 0, 5},  //  typ
  {5, 1, 96}, //  arp div index
  {1, 1,  4}, //  arp_mltp
  {3, 1, 95}, //  arpgate
  {0, 0, 3},  //  ui_aOctave 8va
  {0, 0, 1},  // seq toggle
  {false, false, true}, // active arp
  {false, false, true}, // active seq
  {3, 1, 47}, // seq speed max could be 96 (2mes)
  {0, 0, 32}, // seqLength
};

//////////////////////////////////////////////////////////////////////////////////////
// fill ui_* with ArpParameters[device]
//////////////////////////////////////////////////////////////////////////////////////
void ArpParameters_Load(unsigned char device)
{
  // only work when device = A
  if (device != Matrix_Device_A)
    return; // quit

  else
  {
#if DEBUG_eeprom
    Serial.print(F("ArpParameters_Load(")); Serial.print(device + 0x0a, HEX); Serial.println(F(")"));
#endif

    // trick to compensate the problem of the eeprom reading while the arp hasn't been set previously (normal behaviour) :
    // arp[] has been loaded from eeprom, compare that the value is between min & max. if not set to default
    for (unsigned char k = 0; k < 20; ++k)
    {
      if ((ArpParametersOrig[k] < (pgm_read_byte_near (&Default_ArpParameters[k][1]))) || (ArpParametersOrig[k] > (pgm_read_byte_near (&Default_ArpParameters[k][2]))))
        ArpParametersOrig[k] = pgm_read_byte_near (&Default_ArpParameters[k][0]);
      else
        ArpParametersOrig[k]  = ArpParametersOrig[k]; // leave unchanged (useful line for later, like random or smtg)
    }

    //initialise achord
    Init_aChord();

    // load indiv arp.ino & ui_arp.ino variblaes with value array
    ui_external_clk = ArpParameters[device][0];
    bpm = ArpParameters[device][1];
    arp_send_notes = ArpParameters[device][2];
    ui_aGroove = ArpParameters[device][3];
    ui_aHold = ArpParameters[device][4];
    ui_SetAB = ArpParameters[device][5];
    ui_aSplit = ArpParameters[device][6];
    ui_TrspA = ArpParameters[device][7];
    ui_TrspB = ArpParameters[device][8];
    seqNbr = ArpParameters[device][9];
    ui_aMotif = ArpParameters[device][10];
    arp_div_index = ArpParameters[device][11];
    ui_aMltp = ArpParameters[device][12];
    ui_aGate = ArpParameters[device][13];
    ui_aOctave = ArpParameters[device][14];
    //seqToggle = ArpParameters[device][15]; // not implemnted yet
    router_arp_tag = ArpParameters[device][16];
    ui_seqPlay = ArpParameters[device][17];
    seqSpeed = ArpParameters[device][18];
    seqLength = ArpParameters[device][19];
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// store ui_* into ArpParameters[device]
//////////////////////////////////////////////////////////////////////////////////////
void ArpParameters_Store(unsigned char device)
{
  // only do job when device = A
  if (device != Matrix_Device_A)
    return; // quit
  else
  {
#if DEBUG_eeprom
    Serial.print(F("ArpParameters_Store(")); Serial.print(device + 0x0a, HEX); Serial.println(F(")"));
#endif

    // store to Orig[] -> ArpParametersOrig[k] = ArpParameters[device][k] = ui*_;
    ArpParametersOrig[0 ] = ArpParameters[device][0] = ui_external_clk ;// Clock source
    ArpParametersOrig[1 ] = ArpParameters[device][1] = bpm ;
    ArpParametersOrig[2 ] = ArpParameters[device][2] = arp_send_notes;
    ArpParametersOrig[3 ] = ArpParameters[device][3] = ui_aGroove ;
    ArpParametersOrig[4 ] = ArpParameters[device][4] = ui_aHold ;
    ArpParametersOrig[5 ] = ArpParameters[device][5] = ui_SetAB ;
    ArpParametersOrig[6 ] = ArpParameters[device][6] = ui_aSplit ;
    ArpParametersOrig[7 ] = ArpParameters[device][7] = ui_TrspA ;
    ArpParametersOrig[8 ] = ArpParameters[device][8] = ui_TrspB ;
    ArpParametersOrig[9 ] = ArpParameters[device][9] = seqNbr ;
    ArpParametersOrig[10 ] = ArpParameters[device][10] = ui_aMotif ;
    ArpParametersOrig[11 ] = ArpParameters[device][11] = arp_div_index ;
    ArpParametersOrig[12 ] = ArpParameters[device][12] = ui_aMltp ;
    ArpParametersOrig[13 ] = ArpParameters[device][13] = ui_aGate ;
    ArpParametersOrig[14 ] = ArpParameters[device][14] = ui_aOctave ;
    ArpParametersOrig[15 ] = ArpParameters[device][15] = 0; // not implemnted yet
    ArpParametersOrig[16 ] = ArpParameters[device][16] = router_arp_tag ;
    ArpParametersOrig[17 ] = ArpParameters[device][17] = ui_seqPlay ;
    ArpParametersOrig[18 ] = ArpParameters[device][18] = seqSpeed ;
    ArpParametersOrig[19 ] = ArpParameters[device][19] = seqLength ;


    // then, store to bankstick : done in PATCH_Store() -> dataPage = ArpParametersOrig[k]
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Init the ui_*, ArpParameters[device] & ArpParametersOrig with Default_ArpParameters
//////////////////////////////////////////////////////////////////////////////////////
void ArpParameters_Init(unsigned char device)
{
  // only work when device = A
  if (device != Matrix_Device_A)
    return; // quit
  else
  {
#if DEBUG_eeprom
    Serial.print(F("ArpParameters_Init(")); Serial.print(device + 0x0a, HEX); Serial.println(F(")"));
#endif

    // copy default array
    for (unsigned char k = 0; k < 20; ++k)
    {
      ArpParameters[device][k] = ArpParametersOrig[k] = pgm_read_byte_near(&Default_ArpParameters[k][0]); // [k] instead of +k
    }

    // place values into various ui_var
    ui_external_clk = ArpParameters[device][0];
    bpm = ArpParameters[device][1];
    arp_send_notes = ArpParameters[device][2];
    ui_aGroove = ArpParameters[device][3];
    ui_aHold = ArpParameters[device][4];
    ui_SetAB = ArpParameters[device][5];
    ui_aSplit = ArpParameters[device][6];
    ui_TrspA = ArpParameters[device][7];
    ui_TrspB = ArpParameters[device][8];
    seqNbr = ArpParameters[device][9];
    ui_aMotif = ArpParameters[device][10];
    arp_div_index = ArpParameters[device][11];
    ui_aMltp = ArpParameters[device][12];
    ui_aGate = ArpParameters[device][13];
    ui_aOctave = ArpParameters[device][14];
    // seqToggle = ArpParameters[device][15]; // not implemnted yet
    router_arp_tag = ArpParameters[device][16];
    ui_seqPlay = ArpParameters[device][17];
    seqSpeed = ArpParameters[device][18];
    seqLength = ArpParameters[device][19];
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// reset and clean the arp parameters with default & null values
//////////////////////////////////////////////////////////////////////////////////////
void ARP_GLOBAL_INIT(unsigned char device)
{
#if DEBUG_eeprom
  Serial.print(F("ARP_GLOBAL_INIT(")); Serial.print(device + 0x0a, HEX); Serial.println(F(")"));
#endif

  // only work when device = A
  if (device != Matrix_Device_A)
    return; // quit

  ARP2(); // does empty the stack

  Init_aChord();

  arpN = aStepMax = aStep = 0;
  ArpParameters_Init(device); // set default arp parameters

  Init_Seq();

}

/////////////////////////////////////////////////////////////////////////////////////
// fill the arpegio chord with null
/////////////////////////////////////////////////////////////////////////////////////
void Init_aChord(void)
{
  // only work when device = A
  if (device != Matrix_Device_A)
    return; // quit

  for (byte i = 0; i < 8; i++)
  {
    for (byte j = 0; j < 2; j++)
    {
      aChord[i][j] = 0;
    }
  }


#if DEBUG
  Serial.println(F("Init_aChord()"));
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
/// This function activate the arpegiator
//////////////////////////////////////////////////////////////////////////////////////
void Active_Arp(unsigned char state)
{
  switch (state)
  {
    case true: // = 1
      send_stop = false;
      send_start = true; // = MCLOCKDoplay
      router_arp_tag = true;
      active_arp = true;
      break;

    case false: // = 0
      send_start = false;
      send_stop = true;
      router_arp_tag = false;
      active_arp = false;
      break;

    default:
      break;
  }
#if DEBUG_ARP
  Serial.print(F("Active_Arp() =  "));
  Serial.println(active_arp, DEC);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////
// update aChord array (from router.ino)
/////////////////////////////////////////////////////////////////////////////////////
void Update_Arp(unsigned char pitch, unsigned char velocity, unsigned char channel, unsigned char type)
{
  if (arpN < 7) // limit the quantity of notes in the Chord
  {

    // updating the notestack
    switch (type)
    {
      case 0: // noteOff
        // if hold (arp) is active and keys released, don't forward event to aChord
        if (ui_aHold)
        {
          // feed the arp latch  buffer to release notes later, and return
          Feed_aChordLatch(pitch, velocity, channel);
          return;
        }
        else
        {
          // remove element from aChord
          for (unsigned char i = 0; i <= arpN; i++) // we compare all the pitch of the arp to the pitch of the Note Off in order to find which to remove
          {
            if (aChord[i][0] == pitch) // replace ième note by i+1ème  until end of array
            {
              for (unsigned char j = i; j <= arpN; j++)
              {
                aChord [j][0] = aChord [j + 1][0];
                aChord [j][1] = aChord [j + 1][1];
              }
              // decrease length
              --arpN; // "there are null elements in the chord (arpN = 0)"
              if (arpN == 255) arpN = 0;
              break; // return;
            }
          }
        }
        break;

      case 1: // noteOn
        for (unsigned char i = 0; i <= arpN; i++)
        {
          if (pitch == aChord[i][0])
          {
            aChord[i][0] = pitch; // overwrite Note already here
            aChord[i][1] = velocity;
            // don't increment chord length,
            // don't change order,
            // quit
            return;
          }
          else
          {
            // there is one more element in the Chord
            aChord[arpN][0] = pitch;
            aChord[arpN][1] = velocity;
            ++arpN; // "there are arpN elements in the chord (at least one, i.e arpN=1)
            break; // on se casse
          }
        }
        break;

      case 3: // HOLD Notes released (act like notesOff events)
        // remove element from aChord
        for (unsigned char i = 0; i <= arpN; i++) // we compare all the pitch of the arp to the pitch of the Note Off in order to find which to remove
        {
          if (aChord[i][0] == pitch) // replace ième note by i+1ème  until end of array
          {
            for (unsigned char j = i; j <= arpN; j++)
            {
              aChord [j][0] = aChord [j + 1][0];
              aChord [j][1] = aChord [j + 1][1];
            }
            // decrease length
            --arpN; // "there are null elements in the chord (arpN = 0)"
            if (arpN < 0) arpN = 0;
            break; // return;
          }
        }
        break;

      default:
        break;
    }
#if DEBUG_ARPN
    Serial.println(F("Update_Arp()"));
    Serial.print(F("arpN = ")); Serial.println(arpN, DEC); Serial.println();
#endif
  }
  else // there are too much element in aChord, so return
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////
void Play_Arp(byte pitch, byte velocity, byte channel, bool trigger)
{
  if (router_arp_tag && channel == MIDI_CHANNEL && (arpN < 7))
  {
    switch (trigger)
    {
      case 0 : // NoteOff
        if (arpN == 0) // commented in 0.99h
        {
          active_arp = false;
          aTik = 0; // ajout 099h
        }
#if DEBUG_ARPN
        Serial.print(F("PlayArp()::NoteOff, arpN = ")); Serial.println(arpN, DEC); Serial.println();
#endif
        break;

      case 1 : // NoteOn
        // we can't record :
        //   ui_seqRec = false;
        active_arp = true;
        if (arpN == 1)
        {
          aTik = 0; // reset arp tick at first note received
          weakTap = false;
        }
#if DEBUG_ARPN
        Serial.print(F("PlayArp()::Note On arpN = ")); Serial.println(arpN, DEC); Serial.println();
#endif

        playArpTrigger[0] = pitch;
        playArpTrigger[1] = velocity;
        playArpTrigger[2] = channel;
        playArpTrigger[3] = trigger; // = seqTrig (on/off = true/false)
        break;

      default :
        break;
    }
  }
  else
    return;
}

/////////////////////////////////////////////////////////////////////////////////////
// order (or not) the aChord stack
/////////////////////////////////////////////////////////////////////////////////////
void Sort_aChord(unsigned char typ, unsigned char stepmax, unsigned char transpose)
{
  // https://openclassrooms.com/forum/sujet/ordonner-tableau-tableau-en-c
  // http://www.commentcamarche.net/forum/affich-22003423-langage-c-classement-ordre-croissant-decroi

  if (typ) // sort pitches from low to high
  {
    // 1st, copy the stack
    for (unsigned char i = 0; i < arpN; i++)
    {
      aChordSorted[i][0] = aChord[i][0] + ((transpose << 2) + (transpose << 3) - 36); // pitch
      aChordSorted[i][1] = aChord[i][1];  // velocity
    }
    // then, order the stack
    for (unsigned char i = 0; i < arpN; i++)
    {
      for (unsigned char j = i + 1; j < arpN; j++)
      {
        if (aChordSorted[i][0] > aChordSorted[j][0]) // from lowest to highest
        {
          // http://www.vipan.com/htdocs/bitwisehelp.html (faster, no swaptmp)
          aChordSorted[i][0] ^= aChordSorted[j][0];
          aChordSorted[i][1] ^= aChordSorted[j][1];

          aChordSorted[j][0] ^= aChordSorted[i][0];
          aChordSorted[j][1] ^= aChordSorted[i][1];

          aChordSorted[i][0] ^= aChordSorted[j][0];
          aChordSorted[i][1] ^= aChordSorted[j][1];
        }
      }
    }
  }
  else // don't change (PlayOrder)
  {
    for (unsigned char i = 0; i < arpN; i++)
    {
      aChordSorted[i][0] = aChord[i][0] + ((transpose << 2) + (transpose << 3) - 36); // pitch
      aChordSorted[i][1] = aChord[i][1];  // velocity
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Arpegiate function (notes to play)
/////////////////////////////////////////////////////////////////////////////////////
void ARP2(void)
{
  if (active_arp == true)// && (arpN) && (aChord[aStep][0] != 255)) // une seule condition sinon probleme qd aStep == 0
  {
    ///////////////////////////////////////////
    // trigger : 1st pass is not weak, 2nd pass is weak, and so on...
    if (aTik == (0 + aGroove * weakTap))
    {
      // update parameters :
      // aGroove = ui_aGroove;
      aDiv = arp_div_index;
      aGate = ui_aGate;
      aOctave = ui_aOctave;
      aMltp = ui_aMltp;
      aPattern = ui_aMotif;
      TrspA = ui_TrspA;
      aStepMax = arpN * (aOctave + 1) * aMltp; // define max aStep
      if (aStepMax < 0) aStepMax = 0;


#if DEBUG_ARPN
      Serial.println(F("ARP2()"));
      Serial.print("aStepMax = "); Serial.println(aStepMax);
#endif

      // sort aChord
      Sort_aChord(aPattern, aStepMax, TrspA);
      // set the step
      // what to play ? aChordPlay ! new version with aChordsorted (0.99i)
      // to modulate by pattern : TO DO
      for (unsigned char j = 0; j <= aOctave; ++j) {
        for (unsigned char k = 1; k <= aMltp ; ++k) { // nbr of repeat
          for (unsigned char i = 0; i < arpN; ++i) {
            aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][0] = aChordSorted[i][0] + 12 * j; // k-1 parce que k commence à 1
            aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][1] = aChordSorted[i][1];
          }
        }
      }
#if DEBUG_ARP2CHORD
      Serial.println(F("ARP2()"));
      Serial.println("aChordPlay :");
      for (unsigned char j = 0; j <= aOctave; ++j) {
        for (unsigned char k = 1; k <= aMltp ; ++k) { // nbr of repeat
          for (unsigned char i = 0; i < arpN; ++i) {
            Serial.print(aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][0], HEX); Serial.print(", "); Serial.println(aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][0][1], HEX);
          }
        }
      }
#endif

      // send Note On event:
      MIDI1.sendNoteOn(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOn(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      }

      // notice last note to kill at the very end:
      lastNote[0] = aChordPlay[aStep][0];
      lastNote[1] = aChordPlay[aStep][1];
      lastNote[2] = MIDI_CHANNEL;
      lastNote[3] = true; // remind you to kill last note if necessary

#if DEBUG_ARP2
      Serial.println(F("ARP2()"));
      Serial.print(F("Note On : ")); Serial.print(aChord[aStep][0], DEC); Serial.print(F(" ")); Serial.print(aChord[aStep][1], DEC); Serial.print(F(" ch: ")); Serial.println(MIDI_CHANNEL, DEC);
#endif
    }

    ///////////////////////////////////////////
    // gate is ending
    if (aTik == aGate)
    {
      //if(AB_ArpSeq_Cfg)
      MIDI1.sendNoteOff(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOff(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      }
      lastNote[0] = aChordPlay[aStep][0];
      lastNote[1] = aChordPlay[aStep][1];
      lastNote[2] = MIDI_CHANNEL;
      lastNote[3] = false; // no need to kill the note as it has just been done
      weakTap = !weakTap; // alternate strong/weak tap (for swing or groove)
      aGroove = ui_aGroove;


#if DEBUG_ARP2
      Serial.println(F("ARP2()"));
      Serial.print(F("Note Off : ")); Serial.print(aChordPlay[aStep][0], DEC); Serial.print(F(" ")); Serial.print(aChordPlay[aStep][1], DEC); Serial.print(F(" ch: ")); Serial.println(MIDI_CHANNEL, DEC);
#endif

#if DEBUG_ARP2STEP
      Serial.println(F("ARP2()"));
      Serial.print(F("aStep = ")); Serial.println(aStep, DEC);
#endif

    }

    ///////////////////////////////////////////
    // time to pass to next note of Chord and define next step (end of loop)
    //    if (aTik == (aGate + aGate))
    if (aTik == aDiv) // at least = 1
    {
      aTik = 0; // reset

#if DEBUG_ARPN
      Serial.print(F("aTik==aDiv./ arpN = ")); Serial.println(arpN, DEC);
#endif

      ARPMODE2(aPattern, aStepMax);

#if DEBUG_ARP2STEP
      Serial.print(F("reset aStep = ")); Serial.println(aStep, DEC);
#endif

      return;
    }

    ///////////////////////////////////////////
    // increment tiks, and loop if necessary DON'T CHANGE THIS STEP PLACEMENT 0.99h or mute note
    ++aTik;

    if (arpN == 0)
    {
      aStep = 0;
      aTik = 0;
      active_arp = false; // commmentr 0.99l
    }
  }
  else
  {
    // nope
    if (arpN == 0)
    {
      aStep = 0;
      aTik = 0;
      active_arp = false; // commmentr  0.99l
    }

#if DEBUG_ARP2
    //  Serial.print(F("else de ARP2() : ")); Serial.println();
#endif
  }


  ///////////////////////////////////////////
  // kill last note, reset aStep, aTik
  if ((lastNote[3] == true) && (arpN == 0)) // prior 0.99l
    //  if ((lastNote[3] == true) && (active_arp = false)) // since 0.99l2 BUGGY
  {
    MIDI1.sendNoteOff(lastNote[0], lastNote[1], lastNote[2]);
    if (arp_send_notes)
    {
      MIDI3.sendNoteOff(lastNote[0], lastNote[1], lastNote[2]);
    }
    //active_arp = false;
    aTik = aStep = 0;
    Init_aChord();
    lastNote[3] = false;
    weakTap = !weakTap; // alternate strong/weak tap (swing or groove)
    aGroove = ui_aGroove;

#if DEBUG_ARP2
    Serial.print(F("kill last note")); Serial.println();
#endif
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Arpegiate function (notes to play) with CV Trigger
/////////////////////////////////////////////////////////////////////////////////////
void ARP3(bool trig)
{
  if (active_arp == true)// && (arpN) && (aChord[aStep][0] != 255)) // une seule condition sinon probleme qd aStep == 0
  {
    ///////////////////////////////////////////
    // trigger : 1st pass is not weak, 2nd pass is weak, and so on...
    if (trig == true)
    {
      // update parameters :
      aDiv = arp_div_index = 0;
      aOctave = ui_aOctave;
      aMltp = ui_aMltp;
      aPattern = ui_aMotif;
      TrspA = ui_TrspA;
      aStepMax = arpN * (aOctave + 1) * aMltp;
      if (aStepMax < 0) aStepMax = 0;

#if DEBUG_ARPN
      Serial.println(F("ARP3()"));
      Serial.print("aStepMax = "); Serial.println(aStepMax);
#endif

      // sort aChord
      Sort_aChord(aPattern, aStepMax, TrspA);

      // what to play ? aChordPlay ! new version with aChordsorted (0.99i)
      // to modulate by pattern : TO DO
      for (unsigned char j = 0; j <= aOctave; ++j) {
        for (unsigned char k = 1; k <= aMltp ; ++k) { // nbr of repeat
          for (unsigned char i = 0; i < arpN; ++i) {
            aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][0] = aChordSorted[i][0] + 12 * j; // k-1 parce que k commence à 1
            aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][1] = aChordSorted[i][1];
          }
        }
      }
#if DEBUG_ARP2CHORD
      Serial.println("aChordPlay :");
      for (unsigned char j = 0; j <= aOctave; ++j) {
        for (unsigned char k = 1; k <= aMltp ; ++k) { // nbr of repeat
          for (unsigned char i = 0; i < arpN; ++i) {
            Serial.print(aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][0], HEX); Serial.print(", "); Serial.println(aChordPlay[(arpN * j * aMltp) + (i * aMltp + (k - 1))][0][1], HEX);
          }
        }
      }
#endif

      // send Note On event:
      MIDI1.sendNoteOn(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOn(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      }

      // notice last note to kill at the very end:
      lastNote[0] = aChordPlay[aStep][0];
      lastNote[1] = aChordPlay[aStep][1];
      lastNote[2] = MIDI_CHANNEL;
      lastNote[3] = true; // remind you to kill last note if necessary

#if DEBUG_ARP2
      Serial.print(F("Note On : ")); Serial.print(aChord[aStep][0], DEC); Serial.print(F(" ")); Serial.print(aChord[aStep][1], DEC); Serial.print(F(" ch: ")); Serial.println(MIDI_CHANNEL, DEC);
#endif
    }



    ///////////////////////////////////////////
    // gate is ending
    if (trig == false)
    {
      //if(AB_ArpSeq_Cfg)
      MIDI1.sendNoteOff(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      if (arp_send_notes)
      {
        MIDI3.sendNoteOff(aChordPlay[aStep][0], aChordPlay[aStep][1], MIDI_CHANNEL);
      }
      lastNote[0] = aChordPlay[aStep][0];
      lastNote[1] = aChordPlay[aStep][1];
      lastNote[2] = MIDI_CHANNEL;
      lastNote[3] = false; // no need to kill the note as it has just been done

#if DEBUG_ARP2
      Serial.print(F("Note Off : ")); Serial.print(aChordPlay[aStep][0], DEC); Serial.print(F(" ")); Serial.print(aChordPlay[aStep][1], DEC); Serial.print(F(" ch: ")); Serial.println(MIDI_CHANNEL, DEC);
#endif

#if DEBUG_ARP2STEP
      Serial.print(F("aStep = ")); Serial.println(aStep, DEC);
#endif

      ///////////////////////////////////////////
      // time to pass to next note of Chord and define next step (end of loop)

      ARPMODE2(aPattern, aStepMax);

#if DEBUG_ARP2STEP
      Serial.print(F("reset aStep = ")); Serial.println(aStep, DEC);
#endif

      return;
    }

    ///////////////////////////////////////////
    // increment tiks, and loop if necessary DON'T CHANGE THIS STEP PLACEMENT 0.99h or mute note
    //++aTik;

    if (arpN == 0)
    {
      aStep = 0;
      //aTik = 0;
      active_arp = false; // commmentr 0.99l
    }
  }
  else
  {
    // nope
    if (arpN == 0)
    {
      aStep = 0;
      //aTik = 0;
      trig = true;
      active_arp = false; // commmentr  0.99l
    }

#if DEBUG_ARP2
    //  Serial.print(F("else de ARP2() : ")); Serial.println();
#endif
  }


  ///////////////////////////////////////////
  // kill last note, reset aStep, aTik
  if ((lastNote[3] == true) && (arpN == 0)) // prior 0.99l
    //  if ((lastNote[3] == true) && (active_arp = false)) // since 0.99l2 BUGGY
  {
    MIDI1.sendNoteOff(lastNote[0], lastNote[1], lastNote[2]);
    if (arp_send_notes)
    {
      MIDI3.sendNoteOff(lastNote[0], lastNote[1], lastNote[2]);
    }
    //active_arp = false;
    aTik = aStep = 0;
    Init_aChord();
    lastNote[3] = false;


#if DEBUG_ARP2
    Serial.print(F("kill last note")); Serial.println();
#endif
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Hold captured Notes off
/////////////////////////////////////////////////////////////////////////////////////
void Feed_aChordLatch(byte pitch, byte velocity, byte channel)
{
  aChordLatch[latchCounter][0] = pitch;
  aChordLatch[latchCounter][1] = velocity;
  aChordLatch[latchCounter][2] = channel;

  ++latchCounter;
}


/////////////////////////////////////////////////////////////////////////////////////
// release captured Notes off
/////////////////////////////////////////////////////////////////////////////////////
void Release_aChordLatch(bool state)
{
  //unsigned char latchCounter = sizeof(aChordLatch) / sizeof(aChordLatch[0]); // plante l'arduino !
  unsigned char i = 0; // our counter

  if (state) // if it's true : switch state, release the array and then reset the counter
  {
    if (router_arp_tag)
    {
      while (i < latchCounter)
      {
        Update_Arp( aChordLatch[i][0], aChordLatch[i][1], aChordLatch[i][2], 3);
        Play_Arp( aChordLatch[i][0],  aChordLatch[i][1],  aChordLatch[i][2], 0);
        ++i;
      }
      latchCounter = 0; // reset
    }
    else
    {
      while (i < latchCounter)
      {
        MIDI1.sendNoteOff( aChordLatch[i][0], aChordLatch[i][1], aChordLatch[i][2] );
        ++i;
      }
      latchCounter = 0; // reset
    }
  }
  else
    return;

}

/////////////////////////////////////////////////////////////////////////////////////
// the different arpegio motifs
// https://github.com/wbajzek/arduino-arpeggiator/blob/master/arpeggiator.ino
/////////////////////////////////////////////////////////////////////////////////////
void ARPMODE2(unsigned char type, unsigned char stepmax)
{
  // bool arpUp;
  static bool arpUp;

  // no note, quit the function
  if (arpN == 0)
  {
    return;
  }

  // single note, repeat it
  if (arpN == 1)
  {
    ++aStep;
    if (aStep > (stepmax - 1))
      aStep = 0;
  }

  //  // 2 notes, alternate whatever pattern is
  //  if (arpN == 2)
  //  {
  //    ++aStep;
  //    if (aStep == 2)
  //    {
  //      aStep = 0;
  //    }
  //  }

  // there start some pattern arrangemnts
  if (arpN > 1)
  {
    switch (type)
    {
      case 0: // playorder // OK 0.99i
        ++aStep;
        if (aStep >= stepmax)
          aStep = 0;
        break;

      case 1: // up // OK 0.99i
        ++aStep;
        if (aStep >= stepmax)
          aStep = 0;
        break;

      case 2: //down //OK 0.99i
        if (aStep == 0)
        {
          aStep = stepmax - 1;
          //          while (aChordPlay[aStep][0] == 0)
          //          {
          //            --aStep;
          //          }
        }
        else
        {
          --aStep;
        }
        break;

      case 3: // bounce OK 0.99i
        if (arpUp)
        {
          ++aStep;
        }
        else
        {
          if (aStep == 0)
          {
            ++aStep;
            arpUp = true;
          }
          else
          {
            --aStep;
            arpUp = false;
          }
        }
        if (aStep == stepmax ) // si pas -1 ca joue 2 fois le stepmax (pour UpDn)
        {
          --aStep;
          --aStep;
          arpUp = false;
        }
        break;


      case 4: // updown OK 0.99i
        if (arpUp)
        {
          if (aStep == stepmax - 1)
          {
            //--aStep;
            arpUp = false;
          }
          else
            ++aStep;
        }
        else
        {
          if (aStep == 0)
          {
            arpUp = true;
            aStep = aStep;
          }
          else
            --aStep;
        }
        break;

      case 5: // random OK 0.99k
        aStep = random(0, stepmax - 1); // too easy ^^
        break;

      case 6: // 1>>3 pas bon
        if (arpUp)
          aStep += 2;
        else
          --aStep;

        arpUp = !arpUp;

        if (aChordPlay[aStep][0] == 0)
        {
          aStep = 0;
          arpUp = true;
        }
        break;

      case 9: // 1<<3 pas bon
        if (aChordPlay[aStep + 1][0] == 0) {
          aStep = 0;
          arpUp = true;
          return;
        }
        if (!arpUp)
          aStep += 2;
        else
          --aStep;

        arpUp = !arpUp;
        break;

      case 7: // bounce down completemnt buggé
        if (arpUp)
        {
          ++aStep;
          if (aStep > (stepmax - 1))
          {
            --aStep;
            --aStep;
            arpUp = false;
          }

        }
        else
        {
          //--aStep;
          if (aStep == 0)
          {
            aStep = stepmax - 1;
            arpUp = false;
          }
          else
          {
            --aStep;
            if (aStep == 0)
              arpUp = true;
          }
        }

        //return;
        break;

      case 8: // donwup pas bon
        if (arpUp)
        {
          ++aStep;
          if (aStep > (stepmax - 1))
          {
            --aStep;
            //--aStep;
            arpUp = false;
          }
          if (aStep == 0) return; // 2nd pass

        }
        else
        {
          --aStep;
          if (aStep < 0)
          {
            aStep = stepmax - 1;
            arpUp = false;
          }
          else
          {
            //--aStep;
            if (aStep == 0)
            {
              // 1st pass
              arpUp = true;
            }
          }
        }

        //return;
        break;

      default: // add more motifs, Up by default
        ++aStep;
        if (aStep >= stepmax)
          aStep = 0;
        break;
    }
  }
}

