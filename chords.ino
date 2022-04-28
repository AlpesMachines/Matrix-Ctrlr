/*
   chords memory functinn based on Alpha Juno
   3 memories available, picth & velocity are saved

*/

unsigned char defaultChord[6][2] = {
  {0, 100}, // C
  {2, 80},  // D
  {4, 120}, // E
  {5, 100}, // F
  {7, 80},  // G
  {9, 120}, // A
};
unsigned char chordsMemoArray[3][6][2]; // 3 memories of 6 notes chords including pitch and velocity
unsigned char currentChords; // cm1, cm2, cm3
static unsigned char chordSize[3]; // number or real notes inside the chord
#define CHORDS_MEM0 0 // NULL
#define CHORDS_MEM1 1
#define CHORDS_MEM2 2
#define CHORDS_MEM3 3
bool learningChord;

///////////////////////////////////////////////////////////////////
// initialisation of chords
///////////////////////////////////////////////////////////////////
void initChords(void)
{
  for (unsigned char i = 0; i < 3; i++)
  {
    for (unsigned char j = 0; j < 6; j++)
    {
      for (unsigned char k = 0; k < 2; k++)
      {
        chordsMemoArray[i][j][k] = 0; //defaultChord[j][k];
      }
    }
  }

  for (unsigned char i = 0; i < 3; i++)
    chordSize[i] = 0;

  learningChord = false;
}

///////////////////////////////////////////////////////////////////
// status
///////////////////////////////////////////////////////////////////
bool learnChord(bool state)
{
  // active en ON quand on appuie sur KEY1 et en Off des la premiere noteOff reçue
  // sera utilisé par l'affichage pour mettre * ou o

  //learningChord = !learningChord;
  return state;
}

///////////////////////////////////////////////////////////////////
// empty chords
///////////////////////////////////////////////////////////////////
void CLEAN_CHORD(unsigned char chordmem)
{

  for (unsigned char i = 0; i < 6; i++) {
    LEARN_CHORD(0, 0, MIDI_CHANNEL, chordmem, true);
  }

  //  for (unsigned char i = 0; i < 3; i++)
  //    chordSize[chordmem] = 1;
}

///////////////////////////////////////////////////////////////////
// populate chords
///////////////////////////////////////////////////////////////////
void LEARN_CHORD(byte pitch, byte velocity, byte channel, unsigned char chordmem, bool status)
{
  static unsigned char chordPosition;
  if ( chordPosition >= 6) chordPosition = 0; // cyclic rule

  // status Off :
  if (channel == MIDI_CHANNEL && chordmem && status == OFF) {
    // all notes have been received, then, order the stack http://www.vipan.com/htdocs/bitwisehelp.html (faster, no swaptmp)
    for (unsigned char i = 0; i < chordSize[chordmem]; i++) {
      for (unsigned char j = i + 1; j < chordSize[chordmem]; j++) {
        if (chordsMemoArray[chordmem - 1][i][0] > chordsMemoArray[chordmem - 1][j][0]) { // from lowest to highest
          chordsMemoArray[chordmem - 1][i][0] ^= chordsMemoArray[chordmem - 1][j][0];
          chordsMemoArray[chordmem - 1][i][1] ^= chordsMemoArray[chordmem - 1][j][1];

          chordsMemoArray[chordmem - 1][j][0] ^= chordsMemoArray[chordmem - 1][i][0];
          chordsMemoArray[chordmem - 1][j][1] ^= chordsMemoArray[chordmem - 1][i][1];

          chordsMemoArray[chordmem - 1][i][0] ^= chordsMemoArray[chordmem - 1][j][0];
          chordsMemoArray[chordmem - 1][i][1] ^= chordsMemoArray[chordmem - 1][j][1];
        }
      }
    }
#if DEBUG_CHORDM
    //print whole array sorted :
    Serial.println(F("sorted chordsMemoArray[] : "));
    for (unsigned char i = 0; i < chordSize[chordmem]; i++) {
      Serial.print(F("cm")); Serial.print(chordmem, DEC); Serial.print(F(" ")); Serial.print(F("/chordPosition = ")); Serial.print(i, DEC); Serial.print(F(" ")); Serial.print(F("/pitch = ")); Serial.print(chordsMemoArray[chordmem - 1][i][0], DEC); Serial.print(F(" ")); Serial.print(F("/velocity = ")); Serial.println(chordsMemoArray[chordmem - 1][i][1], DEC);
    }
#endif

    // rapport harmonique des differents membres du tableau :
    /* // méthode Alpes Machines
      for (unsigned char i = chordPosition + 1; i > 0; --i) { // for (int i = 10; i > 0; --i) {
      unsigned char newPitch = chordsMemoArray[chordmem - 1][i - 1][0] - chordsMemoArray[chordmem - 1][0][0];
      chordsMemoArray[chordmem - 1][i - 1][0] = newPitch;
      }
    */
    // méthode Roland Juno2
    signed char transposition = 60 - chordsMemoArray[chordmem - 1][0][0]; // pour ramener l'accord enregistré sur le C4 central
    for (unsigned char i = chordSize[chordmem] + 1; i > 0; --i) { // for (int i = 10; i > 0; --i) {
      unsigned char newPitch = chordsMemoArray[chordmem - 1][i - 1][0] - chordsMemoArray[chordmem - 1][0][0];
      chordsMemoArray[chordmem - 1][i - 1][0] = newPitch - transposition;
    }
    
#if DEBUG_CHORDM
    //print whole array sorted :
    Serial.println(F("sorted chordsMemoArray[] -> rapport harmonique : "));
    for (unsigned char i = 0; i < chordSize[chordmem]; i++) {
      Serial.print(F("cm")); Serial.print(chordmem, DEC); Serial.print(F(" ")); Serial.print(F("/chordPosition = ")); Serial.print(i, DEC); Serial.print(F(" ")); Serial.print(F("/pitch = ")); Serial.print(chordsMemoArray[chordmem - 1][i][0], DEC); Serial.print(F(" ")); Serial.print(F("/velocity = ")); Serial.println(chordsMemoArray[chordmem - 1][i][1], DEC);
    }
#endif

    // reset for next time by a noteOff msg
    transposition = 0; // reset
    chordPosition = 0;
#if DEBUG_CHORDM
    Serial.print(F("reset for next time by a noteOff msg /chordPosition = ")); Serial.println(chordPosition, DEC);
#endif

  }

  // status On :
  if (channel == MIDI_CHANNEL && chordmem && status == ON) {
    chordsMemoArray[chordmem - 1][chordPosition][0] = pitch;
    chordsMemoArray[chordmem - 1][chordPosition][1] = velocity;

#if DEBUG_CHORDM
    Serial.print(F("cm")); Serial.print(chordmem, DEC); Serial.print(F(" ")); Serial.print(F("/chordPosition = ")); Serial.print(chordPosition, DEC); Serial.print(F(" ")); Serial.print(F("/pitch = ")); Serial.print(chordsMemoArray[chordmem - 1][chordPosition][0], DEC); Serial.print(F(" ")); Serial.print(F("/velocity = ")); Serial.println(chordsMemoArray[chordmem - 1][chordPosition][1], DEC);
#endif

    chordPosition++; // increment for next time
    chordSize[chordmem] = chordPosition;

#if DEBUG_CHORDM
    Serial.print(F("chordSize[")); Serial.print(chordmem, DEC); Serial.print(F("] = ")); Serial.println(chordSize[chordmem], DEC);
    Serial.print(F("chordPosition = ")); Serial.println(chordPosition, DEC);
#endif
  }


}

///////////////////////////////////////////////////////////////////
// playing chords : noteOn & noteOff !
///////////////////////////////////////////////////////////////////
void PLAY_CHORD(byte pitch, byte velocity, byte channel, unsigned char chordmem, unsigned char noteType)
{
  /*
     int size = sizeof prices / sizeof prices[0];
     int size = sizeof prices / sizeof *prices;
  */
  switch (noteType) {
    case ON:
      // for (unsigned char i = 0; i < 6; i++) {
      for (unsigned char i = 0; i < chordSize[chordmem]; i++) {
        MIDI1.sendNoteOn (pitch + chordsMemoArray[chordmem - 1][i][0], velocity, channel); // fixed velocity + chordsMemoArray[chordmem - 1][i][1]
      }

#if DEBUG_CHORDM
      //print played notes :
      Serial.println(F("played noteOn : "));
      for (unsigned char i = 0; i < chordSize[chordmem]; i++) {
        Serial.print(F("noteOn  nbr")); Serial.print(i, DEC); Serial.print(F(" ")); Serial.print(F("/pitch = ")); Serial.print(pitch + chordsMemoArray[chordmem - 1][i][0], DEC); Serial.print(F(" ")); Serial.print(F("/velo = ")); Serial.print(velocity, DEC); Serial.print(F(" ")); Serial.print(F("/channel = ")); Serial.println(channel, DEC);
      }
#endif
      break;

    case OFF:
      for (unsigned char i = 0; i < chordSize[chordmem]; i++) {
        MIDI1.sendNoteOff(chordsMemoArray[chordmem - 1][i][0] + pitch, velocity, channel);
      }

#if DEBUG_CHORDM
      //print played notes :
      Serial.println(F("played noteOff : "));
      for (unsigned char i = 0; i < chordSize[chordmem]; i++) { //(sizeof chordsMemoArray / sizeof * chordsMemoArray) - 2
        Serial.print(F("noteOff nbr")); Serial.print(i, DEC); Serial.print(F(" ")); Serial.print(F("/pitch = ")); Serial.print(pitch + chordsMemoArray[chordmem - 1][i][0], DEC); Serial.print(F(" ")); Serial.print(F("/velo = ")); Serial.print(velocity, DEC); Serial.print(F(" ")); Serial.print(F("/channel = ")); Serial.println(channel, DEC);
      }
#endif
      break;

    default: break;
  }
}
