#define GROUPNONE 0
#define GROUPSOLO     1
#define GROUPDUO      2
#define GROUPTRIO     3
#define GROUPQUARTET  4
#define GROUPDOUBLEDUO 5

unsigned char groupMode;
unsigned char ui_groupMode;

// 4 groups of 6 voices, each note having pitch + velocity. implicitly first group is channel A, second group channel B etc
byte groupNote[4][6][2];

/////////////////////////////////////////////////////////
// initialise groupNote[] array
/////////////////////////////////////////////////////////
void InitGroupNote()
{
  unsigned char i, j, k;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 6; j++) {
      for (k = 0; k < 2; k++) {
        groupNote[i][j][k] = 255;
      }
    }
  }
}

/////////////////////////////////////////////////////////
// fill groupNote[] array with noteOn recieved
/////////////////////////////////////////////////////////
unsigned char FillGroupNote(byte pitch, byte velocity, byte channel)
{
  unsigned char i, j;

  for (i = 0; i < groupMode; i++) {
    for (j = 0; j < 6; j++) {
      if (groupNote[i][j][0] == 255) {
        groupNote[i][j][0] = pitch; // new pitch in the array
        groupNote[i][j][1] = velocity; // new velocity in array
      }
    }
  }
  return i; // return the group of the note i.e the interfaceserial where to send the noteOn
}

/////////////////////////////////////////////////////////
// empty groupNote[] array from noteOff received
/////////////////////////////////////////////////////////
unsigned char EmptyGroupNote(byte pitch, byte velocity, byte channel)
{
  unsigned char i, j;

  for (i = 0; i < groupMode; i++) {
    for (j = 0; j < 6; j++) {
      if (groupNote[i][j][0] == pitch) {
        groupNote[i][j][0] = 255; // reset pitch
        groupNote[i][j][1] = 255; // reset velocity
      }
    }
  }
  return i; // return the group of the note i.e the interfaceserial where to send the noteOff
}


