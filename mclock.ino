/*
   midiclock stuff inspired by sparkfun and mstimer2 lib

   other solution : https://github.com/DieterVDW/arduino-midi-clock/blob/master/MIDI-Clock.ino

   https://www.arduino.cc/en/Reference/Long
   https://github.com/thomasfredericks/Metro-Arduino-Wiring/tree/master/Metro

   solution : http://playground.arduino.cc/Main/FlexiTimer2 avec PPQNDIV dans la resolution  2880 pulses/min quand on est à 120bpm

*/
#include "arp.h"
#include <uClock.h>
#define PPQNDIV 96

//static uint32_t loops = 0;
static uint8_t  ticks, prev_ticks;

//////////////////////////////////////////////////////////////////////////////////////////
// initialisation de mClock
//////////////////////////////////////////////////////////////////////////////////////////
void mClockInit()
{
  // Inits the clock
  uClock.init();
  running = false;
  send_start = false;
  send_stop = true;
  send_tick = false;
  // Set the callback function for the clock output to send MIDI Sync message.
  uClock.setClock96PPQNOutput(ClockOut96PPQN);
  // Set the callback function for MIDI Start and Stop messages.
  uClock.setOnClockStartOutput(onClockStart);
  uClock.setOnClockStopOutput(onClockStop);
  // Set the clock BPM to 126 BPM
  uClock.setTempo(bpm);

  // Starts the clock, tick-tac-tick-tac...
  uClock.start();
}

//////////////////////////////////////////////////////////////////////////////////////////
// The callback function wich will be called when clock starts by using Clock.start() method.
////////////////////////////////////////////////////////////////////////////////////////////
void onClockStart()
{
  MIDI3.sendRealTime(Start);
  send_start = false;
  Serial.println("send_start");
  ticks = 0;

  // Next tick comes immediately...
  // it also resets the timer
  send_tick = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// The callback function wich will be called when clock stops by using Clock.stop() method.
////////////////////////////////////////////////////////////////////////////////////////////
void onClockStop()
{
  //MIDI3.sendRealTime(Stop);
  send_stop = false;
  prev_ticks = ticks ;
  Serial.println("send_stop");
}

////////////////////////////////////////////////////////////////////////////////////////////
// The callback function wich will be called by Clock each Pulse of 96PPQN clock resolution.
//////////////////////////////////////////////////////////////////////////////////////////
void ClockOut96PPQN(uint32_t * tick_)
{
  if (send_tick)
  {
    if (systmClock == INTCLK)    // if internal MIDI clock source
    {
      // Send MIDI_CLOCK to external gears -> core out
      MIDI3.sendRealTime(Clock);

      // heartbeat :
      if (ticks > 5)
        DOUT_PinSet(DOUT_ACTIVITY2, 0); // heartbeat led
      else
        DOUT_PinSet(DOUT_ACTIVITY2, 1);

      // increment tick count
      ++ticks;
      if (ticks >= 24)
        ticks = 0;
        
      // arpegiate from the beginning
      ARP2();
      
      // sequence from the beginning
      SEQ();
    }
  }

}
