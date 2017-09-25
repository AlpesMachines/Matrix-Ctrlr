
bool triggerInput;
bool previousTriggerInput;

bool analogClock;
bool previousAnalogClock;


/////////////////////////////////////////////////////////
// ext CV triggers management (use pull up)
/////////////////////////////////////////////////////////
void Triggers()
{
  /////// state machine : get info ///////////////////////////////
  // clock pulse from analog oscillator
  if (digitalRead(ANALOGCLOCK_Pin) == HIGH)
  {
    analogClock = false; // pull up
  }
  else
  {
    analogClock = true;
  }

  // S-Trig or V-Trig from drummachine : trigger out
  if (digitalRead(TRIGGER_INPUT_Pin) == HIGH)
  {
    triggerInput = false; // pull up
  }
  else
  {
    triggerInput = true;
  }
  
  // state machine : launch player ///////////////////////////////
  // http://www.doepfer.de/faq/gen_faq.htm#Sync
  //////// case SYNC24 ////////////////////////////////////////////////
  if ((ui_external_clk == S24CLK) && (previousAnalogClock != analogClock)) // same behaviour as midi msg 0xF8
  {
    klock = !klock; // sinon ça compte 2 par 2

    if (klock)
    {
#if DEBUG_TRIG
      Serial.print(F("klock!"));
#endif
      // each time a clock is received we increment mClock :
      ++mClock;
      if (mClock > 23) // 24 ppqn
        mClock = 0; // reset
      // Flash on each beat during a half quarter (métronome) : 6 pulses x 4 = 24 ppqn
      // ||  0  1  2  3  4  5 |  6  7  8  9 10 11 | 12 13 14 15 16 17 | 18 19 20 21 22 23 ||  //
      if (mClock > 5)
        DOUT_PinSet(DOUT_ACTIVITY2, 0); // heartbeat led
      else
        DOUT_PinSet(DOUT_ACTIVITY2, 1);

      MIDI3.sendRealTime(Clock); // send a midi clock on core out
      ARP2(); // we always call ARP2()
      SEQ(); // we always call seq()

#if DEBUG_TRIG
      Serial.print(F("sync24 clock handled. "));
      Serial.print(F("mClock = ")); Serial.println(mClock);
#endif
    }
  } // WORK 16.6.17

  //////// case ROLAND TRIGGER ////////////////////////////////////////////////
  if ((ui_external_clk == TRGCLK) && (previousTriggerInput != triggerInput))
  {
    // rules to trig a noteOn or a noteOff :
    if (previousTriggerInput == true && triggerInput == false)
    {
#if DEBUG_TRIG
      Serial.println(F("TRIG On"));
#endif

      ARP3(true);
      SEQ2(true);
      // display it is a trig On event (like a midi in event) :
      trigger = true;
      DOUT_PinSet(DOUT_ACTIVITY2, trigger); // tempo led
    }
    else if (previousTriggerInput == false && triggerInput == true)
    {
#if DEBUG_TRIG
      Serial.println(F("TRIG Off"));
#endif

      ARP3(false);
      SEQ2(false);
      DOUT_PinSet(DOUT_ACTIVITY2, trigger); // tempo led
    }
    else
    {
      // nop
      
#if DEBUG_TRIG
      Serial.println(F("TRIG pending"));
#endif
    }

#if DEBUG_TRIG2
    if (trigger)
      Serial.println(F("trigger!"));
#endif
  }

#if DEBUG_TRIG2
  if (previousAnalogClock != analogClock)
  {
    Serial.print(F("analogClock = "));
    Serial.println(analogClock, DEC);
  }

  if (previousTriggerInput != triggerInput)
  {
    Serial.print(F("triggerInput = "));
    Serial.println(triggerInput, DEC);
  }
#endif

  // save the states for next loop
  previousTriggerInput = triggerInput;
  previousAnalogClock = analogClock;
}

