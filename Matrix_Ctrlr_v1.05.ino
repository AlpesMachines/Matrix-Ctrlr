

/*
  Arduino 64 AIN 128 DIN 128 DOUT ported on arduino using e-lecktronic libraries + SFC60 code
  it uses :
   Arduino MEGA 2560
   32 pots
   AINx4 module
   32 switches
   DINx4 module
   32 leds
   DOUTx4 module
   custom 3 ports in/out MIDI shield
   2x20 LCD compatble 44780 hitachi
*/

#include <elapsedMillis.h>
#include <TimerOne.h>
#include <Wire.h>
#include <EEPROM.h>
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <LiquidCrystal.h>
#include <EEPROM24LC256_512.h>
#include <SoftwareSerial.h>
#include <FlexiTimer2.h>

#include "arp.h"
#include <SRIO_mxctr.h>
#include "din.h"
#include "midi.h"
#include "Display.h"
#include "livepanel.h"
#include "device.h"
#include "chaosmatrix.h"
#include "ui_patch.h"
#include "memo.h"
#include "define.h"
//#include "math.h"
#include "softpanel.h"
#include "oner.h"


// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// LCD display defines ---------
#define firmware "1.05 "
#define  LCD_ROWS  2
#define  LCD_COLS  20
LiquidCrystal lcd(44, 45, 40, 43, 41, 42); // julien proto arduino (ne marche pas en 47/48) ... pour les 2 premieres entrées A4 A5 ok
bool appflag_updateDisplay;

// used to debug various stuff
#define DEBUG 0
#define DEBUG_looptime 0
#define DEBUG_analog   0
#define DEBUG_SRIO    0
#define DEBUG_din      0
#define DEBUG_dout     0
#define DEBUG_serialout 0
#define DEBUG_LCDparamvaluedescription 0
#define DEBUG_softpanel 0
#define DEBUG_eeprom 0
#define DEBUG_inteeprom 0
#define DEBUG_encoder 0
#define DEBUG_device 0
#define DEBUG_midi 0
#define DEBUG_router 0
#define DEBUG_sysexhandler 0 // put to zero to allow bank damp without computer Serial is to long and bug the trick
#define DEBUG_sysexbankhandler 0
#define DEBUG_bargraph 0
#define DEBUG_matrix 0
#define DEBUG_master 0
#define DEBUG_CLOCK 0
#define DEBUG_ARP 0
#define DEBUG_SEQ 0
#define DEBUG_SEQTIK 0
#define DEBUG_ARP2 0
#define DEBUG_ARPN 0
#define DEBUG_ARP2STEP 0
#define DEBUG_ARP2CHORD 0
#define DEBUG_ARPSTACK 0
#define DEBUG_TRIG 0
#define DEBUG_TRIG2 0
#define DEBUG_LCD 0
#define DEBUG_factory 0
#define DEBUG_wizz 1

#define RX1_pin 19
#define ANALOG_INPUTS_ACTIVE 1 // set to 0 for defective unit (accident coffre 206)
#define DIGITAL_INPUTS_ACTIVE 1
#define SOFTSERIAL_ENABLED 1 // NOTA : SoftSerial uses nearly 1KB of RAM ! but provides 2 additional MIDI ports (see below)
#define MIDITHRU_OFF_BOOT 1
#define INVERT_ENCODER 1
#define CHINESE_RED_LCD 1 // specific to red lcd from aliexpress
#define GliGliON 1


// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
#define FASTADC 0 // à essayer à 1 plus tard quand les ain non utilisés seront reliés à la masse
//defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

byte ain_pin;
byte ain_pin_value;
//byte din_pin;
//byte din_pin_value;
byte dout_pin;
byte dout_pin_value;
byte Shift;
byte Alt;



bool inTest;

bool RefreshSoftPanel = 0;
bool encoder_inverted = true;
bool mThru_XCc = false; // used for sysex/CC translator, translate if true, don't translate if false, stored in eeprom and recalled in boot()

// last ain/din/dout
unsigned char last_din_pin;
unsigned char last_dout_pin;
unsigned char last_encoder;
//-------------------------------------------------------//
//Define PINS numbers//
//-------------------------------------------------------//

// arduino ANALOG PIN :
byte Analog1 = A0;
byte Analog2 = A1;
byte Analog3 = A2;
byte Analog4 = A3;
//byte Analog5 = A4;
//byte Analog6 = A5;


////Analog MUXES
////Configure MUX adress pins S0 S1 S2 (datasheet HEF4051)
//byte SPINS[3] = {
//  40, 42, 44
//};

//Configure MUX OUT PIN
byte VOUT_ANA1 = Analog1; //read pin analog0
byte VOUT_ANA2 = Analog2; //read pin analog1
byte VOUT_ANA3 = Analog3; //read pin analog2
byte VOUT_ANA4 = Analog4; //read pin analog3
//byte VOUT_ANA5 = Analog5; //read pin analog4
//byte VOUT_ANA6 = Analog6; //read pin analog5

////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////
byte AnalogThreshold = 3; //threshold (6 = OK)
//elapsedMicros timer_analog = 0;
elapsedMicros timer_digital = 0;

byte AnalogSamplingPeriod = 1; // in microsec
unsigned int DigitalScanPeriod = 0; // 5000 in microsec

int timer_display = 0;


////MUX control array SFC_60_RevA_V1_8_no_presets
//int muxChannel[8][3] = { // HEF4051 truth table
//  {0, 0, 0},
//  {1, 0, 0},
//  {0, 1, 0},
//  {1, 1, 0},
//  {0, 0, 1},
//  {1, 0, 1},
//  {0, 1, 1},
//  {1, 1, 1},
//};

byte controlPins[] = { // set bit to control HFE4051 S0 S1 S2
  B00000000,
  B10000000,
  B01000000,
  B11000000,
  B00100000,
  B10100000,
  B01100000,
  B11100000
};

bool InitFlag = 0;
bool booting = 1;

bool running;
bool send_start;
bool send_stop;
bool send_continue;
bool send_tick;
///uint32_t tempo_delay;
unsigned char bpm;
double Pulses;
unsigned char ui_external_clk; // external clock event tag
bool trigger;
bool klock;
bool localControl;

void setPin(byte outputPin)  // function to select pin on 74HC4067
{
  PORTA = controlPins[outputPin];
}

// Note: https://www.arduino.cc/en/Reference/SoftwareSerial
// Not all pins on the Mega and Mega 2560 support change interrupts,
// so only the following can be used for RX:
// 10, 11, 12, 13,  -> used by DIN DOUT
// 50, 51, 52, 53, -> used by SD card
// 62(A8), 63, 64(A10), 65(A11), -> used by CV jacks (at least 64 et 65)
// 66(A12), 67, 68, 69(A15) -> use for SoftSerial
// alternative :
// http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

#if SOFTSERIAL_ENABLED
SoftwareSerial mSerial4 (A12, A13); // Rx Tx (MIDI4 in out ) A8/A9 marche tres bien
SoftwareSerial mSerial5 (A14, A15); // Rx Tx (MIDI5 in out ) A10/A11 aussi
#endif

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI1); // RX3 TX3 port IIC 1
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2); // RX2 TX2 port IIC 2
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI3); // RX1 TX1 port CORE OUT

#if SOFTSERIAL_ENABLED // additional midi ports
MIDI_CREATE_INSTANCE(SoftwareSerial, mSerial4, MIDI4); // RX4 TX4 port non native serial port IIC 3
MIDI_CREATE_INSTANCE(SoftwareSerial, mSerial5, MIDI5); // RX5 TX5 port non native serial port IIC 4
#endif

using namespace midi;

unsigned char INTERFACE_SERIAL;

//define eeprom chips (1 per chip)
EEPROM256_512 mem_1;
EEPROM256_512 mem_2;
EEPROM256_512 mem_3;
EEPROM256_512 mem_4;

elapsedMillis elapsedTime; // used for temporary message of pot movements
unsigned int interval = 1800; // 1800 milliseconds

void ResetTmpMsg() // used for temporary message of pot movements
{
  if (elapsedTime == interval)
  {
    // return to previous screen
    SoftPanel.Mode = SoftPanel.LastMode;
    SoftPanel.Page = SoftPanel.LastPage;
    SoftPanel.IsNewPage = 1;
    SoftPanel_DisplayHandler();
  }

}
////////////////////////////////////////////////////////////////////////////////////////////
// SETUP /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(1000); // let arduino wake up ;)

  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  //  ADCSRA |= PS_64;    // set our own prescaler to 64 if pots 100KB
  ADCSRA |= PS_32; // 32 prescaler if pots 10 KB
  // ADCSRA |= PS_16; // 16 prescaler


#if FASTADC
  // set prescale to 16
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  cbi(ADCSRA, ADPS0) ;
#endif

  // CONFIGURE SERIAL DEBUG
  Serial.begin(SERIAL_BUS_SPEED); // fast !

  //VOUT MUX PIN CONFIG
  pinMode(VOUT_ANA1, INPUT);
  pinMode(VOUT_ANA2, INPUT);
  pinMode(VOUT_ANA3, INPUT);
  pinMode(VOUT_ANA4, INPUT);
  //  pinMode(VOUT_ANA5, INPUT);
  //  pinMode(VOUT_ANA6, INPUT);

  pinMode(encoderClicPin, INPUT_PULLUP);

  // triggers (CV gate inputs)
  pinMode(TRIGGER_INPUT_Pin, INPUT_PULLUP); // pin trig for analog drummachine
  pinMode(ANALOGCLOCK_Pin, INPUT_PULLUP); // pin trig for analog clock

  //MUX adress PINS CONFIG
  //  for (int i = 0; i < 3; i++) {
  //    pinMode(SPINS[i], OUTPUT);
  //  }
  DDRA = B11111111; // set PORT A to outputs

  //RESET PINS CONFIG
  //  for (int i = 0; i < 3; i++) digitalWrite(SPINS[i], LOW);
  setPin(1);

  //Potentiometers init
#if FASTADC
  // set prescale to 16
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  cbi(ADCSRA, ADPS0) ;
#endif

  // Initialize SR in and out
  SR.Initialize();
  //-------------------------------------------------------//
  //READING DIGITAL INPUTS//
  //-------------------------------------------------------//
  if (DIGITAL_INPUTS_ACTIVE)
    ReadDigital();

  //-------------------------------------------------------//
  //TOGGLE SWITCHES AND DIGITAL MIDI
  //-------------------------------------------------------//
//  if (DIGITAL_INPUTS_ACTIVE)
//    DIN_NotifyChange();


  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);

#if SOFTSERIAL_ENABLED
  MIDI4.begin(MIDI_CHANNEL_OMNI);
  MIDI5.begin(MIDI_CHANNEL_OMNI);
#endif
  // Connect the midi callbacks to the library,
  enableMidiCallbacks();

  // set up the LCD's number of rows and columns:
  lcd.begin(20, 2);
  lcd.clear();
  LCD_Init(); // custom char

  Timer1.initialize(20000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second) // 20000 OK
  Timer1.attachInterrupt( timer1Isr );   // attach the service routine

  //  Timer3.initialize(100); // used for tempoClock() // was 500 // 200 ok
  //  Timer3.attachInterrupt(timer3Isr);

  // mclock
  running = false;
  send_start = false;
  send_stop = false;
  //  send_tick = false;
  send_tick = true;

  //  check_timing();
  FlexiTimer2::set(60, 1.000 / 1440, timer_callback); // bpm = 120
  FlexiTimer2::start();

  //  attachInterrupt(digitalPinToInterrupt(RX1_pin), midiReadISR, RISING); // interrupt to midi In of the controller -> plante Wire.h

  Wire.begin(); // activate i2c bus !
  //begin EEPROM with I2C Address :
  mem_1.begin(0, 1); //addr 0 (DEC) type 1 (defined as 24LC512)
  mem_2.begin(1, 1); //addr 1 (DEC) type 1 (defined as 24LC512)
  mem_3.begin(2, 1); //addr 2 (DEC) type 1 (defined as 24LC512)
  mem_4.begin(3, 1); //addr 3 (DEC) type 1 (defined as 24LC512)

  //SD.begin();

  Boot();

}


////////////////////////////////////////////////////////////////////////////////////////////
// LOOPS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

#if DEBUG_looptime
  long looptime, looptime1;
  looptime = looptime1 = micros();
#endif

  // first, read midi !
  MIDI3.read();  // NRT : stable 2/4/2016 // Core IN
  MIDI1.read(); // Matrix A IN

  //-------------------------------------------------------//
  //READING AND SMOOTHING ANALOG//
  //-------------------------------------------------------//
  if (ANALOG_INPUTS_ACTIVE)
    ReadAnalog();

  //-------------------------------------------------------//
  //READING DIGITAL INPUTS//
  //-------------------------------------------------------//
  if (DIGITAL_INPUTS_ACTIVE)
    ReadDigital();


  //-------------------------------------------------------//
  //SAMPLING, TESTING THRESHOLD AND SENDS MIDI//
  //-------------------------------------------------------//
  if (ANALOG_INPUTS_ACTIVE)
    AIN_NotifyChange();

  //-------------------------------------------------------//
  //TOGGLE SWITCHES AND DIGITAL MIDI
  //-------------------------------------------------------//
//  if (DIGITAL_INPUTS_ACTIVE)
//    DIN_NotifyChange();


  //------------------------------------------------------//
  //READING ENCODER
  //------------------------------------------------------//
  encoder();
  Encoder_Clic();

  //------------------------------------------------------//
  //READING TRIGGER (ext CV Gate)
  //------------------------------------------------------//
  Triggers();




  //-------------------------------------------------------//
  //RECALL INIT//
  //-------------------------------------------------------//
  Recall();

  Display_Tick();
  Tick();
  ResetTmpMsg();
  //    tempoClock();

  InitFlag = 0;
  booting = 0;

#if DEBUG_looptime
  looptime1 = micros();
  Serial.print (F("looptime (usec) : "));
  Serial.println(looptime1 - looptime);
#endif

}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timer1Isr()
{
  // Toggle LED
  LivePanel_BlinkLEDs();
  LivePanel_BlinkLFOs();
}

//void timer3Isr()
//{
//  tempoClock();
//
//}

/////////////////////////////////////////////////////////////////////////////
// This function is called in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void)
{
  // the timing of delayed midi transmits is a low priority
  LivePanel_HandleTransmitDelay();                 // live panel

  MIDI_HandleMatrixModTransmitDelay(INTERFACE_SERIAL);        // from soft panel
  MIDI_HandleDelayedVoiceParam(INTERFACE_SERIAL, true);             // from soft panel
  MIDI_HandleFilterSustainTransmitDelay(INTERFACE_SERIAL);    // from live panel
}

/////////////////////////////////////////////////////////////////////////////
// This function reboot arduino board (RISKY)
/////////////////////////////////////////////////////////////////////////////
// https://arduino103.blogspot.fr/2013/06/comment-un-reset-darduino-par-logiciel.html
void software_Reboot()
{
  // reboot system :
  Serial.println(F("rebooting system / software_Reboot() "));Serial.println();
  delay(200);

  // print temp message on display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F(" --SYSTEM REBOOT-- "));


  wdt_enable(WDTO_1S);

  while (1)
  {

  }
}


