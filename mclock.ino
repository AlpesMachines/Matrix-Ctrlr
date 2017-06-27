/*
   midiclock stuff inspired by sparkfun and mstimer2 lib

   other solution : https://github.com/DieterVDW/arduino-midi-clock/blob/master/MIDI-Clock.ino

   https://www.arduino.cc/en/Reference/Long
   https://github.com/thomasfredericks/Metro-Arduino-Wiring/tree/master/Metro

   solution : http://playground.arduino.cc/Main/FlexiTimer2 avec PPQN dans la resolution  2880 pulses/min quand on est à 120bpm

*/
#include "arp.h"

#define PPQN 96

void timer_callback()
{
  send_tick = true;
  tempoClock();
}


void tempoClock()
{

  static uint32_t loops = 0;
  static uint8_t  ticks = 0;
  static uint8_t  prev_ticks = 0;
  //  static unsigned char clockLoop = 0;
  bool reset_timer = false;

  // uint32_t calc; // NOT USED

  // put your main code here, to run repeatedly:

  // process inputs
  if (send_start)
  {
    //MIDI3.sendRealTime(Start);
    send_start = false;
    Serial.println("send_start");

    ticks = 0;

    // Next tick comes immediately...
    // it also resets the timer
    send_tick = true;

  }

  if (send_continue)
  {
    //MIDI3.sendRealTime(Continue);
    send_continue = false;
    Serial.println("send_continue");

    // Restore the LED blink counter
    ticks = prev_ticks;

    // Next tick comes immediately...
    // it also resets the timer
    send_tick = true;
  }

  if (send_stop)
  {
    //MIDI3.sendRealTime(Stop);
    send_stop = false;
    prev_ticks = ticks ;
    Serial.println("send_stop");
  }

  if (send_tick)
  {
    if (ui_external_clk == INTCLK)    // if internal clock source : (use ui_external_clk = false here)
    {
      if ((loops == 0) && (PPQN == 96)) // we run 4x standard 24ppqn midi clock (better accuracy)
      {
        MIDI3.sendRealTime(Clock);
        // arpegiate from the beginning
        // if(router_arp_tag && playArpTrigger[3]) ARP2();
        ARP2();
        // sequence from the beginning
        SEQ();
      }
      else
      {
        //  dummy
      }
    }
    // else ARP() is called in router.ino at HandleClock()
    send_tick = false;

    // heartbeat if internal Timer :
    if (ui_external_clk == INTCLK)    // if internal clock source : (use ui_external_clk = false here)
    {
      if (ticks < (PPQN >> 2))
      {
        DOUT_PinSet(DOUT_ACTIVITY2, DIN_STATE_ON);
      }
      else if (ticks == (PPQN >> 2))
      {
        DOUT_PinSet(DOUT_ACTIVITY2, DIN_STATE_OFF);
      }
      else if (ticks >= PPQN)
      {
        ticks = 0;
      }

      ++ticks;
      // calc = 60L * 1000 / PPQN / bpm; // FORMULE OK mais pas utilisée
      Pulses = (double)(bpm * PPQN); // OK
      //Pulses = ((bpm << 3) + (bpm << 2)) << 1; // 24*x = (x*8 + x*4)*2

      // DEBUG
#if DEBUG_CLOCK
      if (!bpm)
        Serial.print(F("DEBUG CLOCK ")); Serial.print(1.000 / Pulses, 10); Serial.print(F("  bpm = ")); Serial.println(bpm);
#endif

      reset_timer = true;
    }
  }
  /*
    // créer un condition de external_clk = 1 pour couper le heartbeat et le passer sur l'external clock ICI -> voir router.ino
    if (ui_external_clk == true)
    {
      if (ticks < 6)
      {
        DOUT_PinSet(DOUT_ACTIVITY2, DIN_STATE_ON);
      }
      else if (ticks == 6)
      {
        DOUT_PinSet(DOUT_ACTIVITY2, DIN_STATE_OFF);
      }
      else if (ticks >= 24)
      {
        ticks = 0;
      }
    }
  */
  if (reset_timer)
  {
    FlexiTimer2::stop();
    FlexiTimer2::set(60, 1.000000000000 / (Pulses + 120), timer_callback); // how much pulses do you send in 60 sec ? answer = bpm*ppqn
    FlexiTimer2::start();
    reset_timer = false;
  }

  ++loops;
  if (loops > 3)
  {
    loops = 0;
  }
}

