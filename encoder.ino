/* read a rotary encoder with/without interrupts
   Encoder hooked up with common to GROUND,
   encoder0PinA to pin 2, encoder0PinB to pin 4 (or pin 3 see below)
   it does matter which encoder pin you use for A or C, set accordingly myEnc( ; )

   uses Arduino pullups on A & C channel outputs
   turning on the pullups saves having to hook up resistors
   to the A & B channel outputs

  https://www.arduino.cc/en/Reference/AttachInterrupt
*/

#include "softpanel.h"
#include <Encoder.h>
#include "define.h"

//#define ENCODER_DO_NOT_USE_INTERRUPTS
Encoder myEnc(2, 3); // encoder pin A and C. B to ground

signed char incrementer = 0; // value sent to various functions
long position  = -999; // track the encoder

bool encoderClic;
bool previousEncoderClic;

/////////////////////////////////////////////////////////
// ENCODER MANAGEMENT
/////////////////////////////////////////////////////////
void encoder() // Youhouhouuuu !!!! ça marcheeee :)
{
  static signed char smooth; // several *clics* of the encoder are needed to increment/decrement (nicer touch feel)
  long newPos = myEnc.read(); // reading encoder position

  if (newPos != position) {
    if (encoder_inverted) // if Encoder has D-shaft
    {
      if (position < newPos) { // CW turn
        --smooth; // count CCW turns
        if (smooth == 0 - SMOOTHING_ENCODER) {
          incrementer = -1; // at -(SMOOTHING_ENCODER)'s counts send decrementer
          smooth = 0; // and reset count
        }
        else
          incrementer = 0; // set a null incrementer
      }

      if (position > newPos) { // CCW turn
        ++smooth;
        if (smooth == 0 + SMOOTHING_ENCODER) {
          incrementer = 1; // send incrementer
          smooth = 0;
        }
        else
          incrementer = 0;
      }
    }
    else // encoder has knurled shaft
    {
      if (position > newPos) { // CCW turn
        --smooth; // count CCW turns
        if (smooth == 0 - SMOOTHING_ENCODER) {
          incrementer = -1; // at -(SMOOTHING_ENCODER)'s counts send decrementer
          smooth = 0; // and reset count
        }
        else
          incrementer = 0; // set a null incrementer
      }

      if (position < newPos) { // CW turn
        ++smooth;
        if (smooth == 0 + SMOOTHING_ENCODER) {
          incrementer = 1; // send incrementer
          smooth = 0;
        }
        else
          incrementer = 0;
      }
    }


    if (incrementer != 0) { // we only send +1 or 1 value, null is useless
      // NB : using Shift button we could set a new value to SMOOTHING_ENCODER to increment faster
      SoftPanel_Handler (-1, incrementer); // no pin
      //   last_encoder = encoder;
      app_flags.Display_ENC_Req = 1; // update display
    }
    position = newPos; // update position

#if DEBUG_encoder
    Serial.println(F("encoder() "));
    Serial.print(incrementer); Serial.print(F(" Position = ")); Serial.println(position);
    Serial.println ();
#endif
  }
}

/////////////////////////////////////////////////////////
// ENCODER CLIC management (use pull up)
/////////////////////////////////////////////////////////
void Encoder_Clic() // OK elle marche 15/12/2016
{
  // scan
  if (digitalRead(encoderClicPin) == HIGH)
    encoderClic = false; // pull up
  else
    encoderClic = true;

#if DEBUG_encoder
  if (encoderClic != previousEncoderClic)
  {
    Serial.print(F("encoderClic = ")); Serial.println(encoderClic, DEC);
  }
#endif

  // behaviour
  if (encoderClic != previousEncoderClic) // state has changed
  {
    if (encoderClic) // do something when we push only
      SoftPanel_Handler(SOFT_EDIT_ENC_CLIC, 0);
  }
  previousEncoderClic = encoderClic;
}




