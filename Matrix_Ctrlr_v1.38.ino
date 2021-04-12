

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
// definitions for compiling :
#define RX1_pin 19
#define ANALOG_INPUTS_ACTIVE 1 // set to 0 for defective unit (accident coffre 206)
#define DIGITAL_INPUTS_ACTIVE 1
#define SOFTSERIAL_ENABLED 1 // NOTA : SoftSerial uses nearly 1KB of RAM ! but provides 2 additional MIDI ports (see below)
#define MIDITHRU_OFF_BOOT 1
#define softMIDITHRU 0 // echo midi data from input ports (Core & A) to Core out #customerrequest
#define INVERT_ENCODER 1
#define CHINESE_RED_LCD 1 // specific to red lcd from aliexpress
#define GliGliON 1

// external Arduino libraries
#include <elapsedMillis.h> // https://www.arduino.cc/reference/en/libraries/elapsedmillis/
// #include <TimerOne.h>
#include <TimerThree.h> // https://www.arduino.cc/reference/en/libraries/timerthree/
#include <Wire.h> // included in Arduino IDE
#include <EEPROM.h> // included in Arduino IDE
#include <MIDI.h> // https://www.arduino.cc/reference/en/libraries/midi-library/
#include <avr/pgmspace.h> // included in Arduino IDE
#include <avr/wdt.h> // included in Arduino IDE (watchdog)
#include <hd44780.h> // faster and universal liquidcrystal lib : https://github.com/duinoWitchery/hd44780
#include <hd44780ioClass/hd44780_pinIO.h>
#include <EEPROM24LC256_512.h> // https://github.com/mikeneiderhauser/I2C-EEPROM-Arduino
#include <SoftwareSerial.h> // included in Arduino IDE
//#include <FlexiTimer2.h> // https://playground.arduino.cc/Main/FlexiTimer2/
#include <uClock.h> // replace flexiTimer2 library, generate midi clock https://github.com/midilab/uClock
#include <SRIO_mxctr.h> // custom Alpes Machines library to manage serial Shift Registers In/Out (SRIO) of couple 74HC165/74HC595 (inspired by midibox.org)

// project internal libraries
#include "arp.h"
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

#define firmware "1.38 "

// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
#define FASTADC 0 // à essayer à 1 plus tard quand les ain non utilisés seront reliés à la masse
//defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// used to debug various stuff :
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
#define DEBUG_wizz 0
#define DEBUG_uEB 0



// LCD display defines
#define  LCD_ROWS  2
#define  LCD_COLS  20
hd44780_pinIO lcd(44, 45, 40, 43, 41, 42); // lcd(RS, Enable, D4, D5, D6, D7) julien proto arduino (ne marche pas en 47/48) ... pour les 2 premieres entrées A4 A5 ok
#define HD44780_LCDOBJECT

////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////
bool appflag_updateDisplay;

byte dout_pin;
byte dout_pin_value;
byte Shift;
byte Alt;
bool inTest;
bool RefreshSoftPanel = 0;
bool encoder_inverted = true;
bool mThru_XCc = false; // used for sysex/CC translator, translate if true, don't translate if false, stored in internal eeprom and recalled in boot()
byte ain_pin;
byte ain_pin_value;
unsigned char last_din_pin;
unsigned char last_dout_pin;
unsigned char last_encoder;

byte AnalogThreshold = 3; //potentiometers threshold (6 = OK)
//elapsedMicros timer_analog = 0;
//elapsedMicros timer_digital = 0;
//byte AnalogSamplingPeriod = 1; // in microsec
//unsigned int DigitalScanPeriod = 0; // 5000 in microsec
//int timer_display = 0;

bool InitFlag = 0;
bool booting = 1;

bool running;
bool send_start;
bool send_stop;
bool send_continue;
bool send_tick;
unsigned char bpm;
unsigned char systmClock; // midi clock choice for a patch : INTernal, MIDi external, s24 SYNC24, Trg TRIGGER jack, mTg midi trigger cannel 10 cowbell pitch
unsigned char patch_clock; // system midi clock choice. can be chosen in a patch
bool localControl;

// set bit to control cd4051 S0 S1 S2 :
byte controlPins[] = {
  B00000000,
  B10000000,
  B01000000,
  B11000000,
  B00100000,
  B10100000,
  B01100000,
  B11100000
};

// function to select pin on 74HC4067/4051
void setPin(byte outputPin)
{
  PORTA = controlPins[outputPin];
}


////////////////////////////////////////////////////////////////////////////////////////////
// MIDI INTERFACE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////

// Note: https://www.arduino.cc/en/Reference/SoftwareSerial
// Not all pins on the Mega and Mega 2560 support change interrupts,
// so only the following can be used for RX:
// 10, 11, 12, 13,  -> used by DIN DOUT
// 50, 51, 52, 53, -> used by SD card
// 62(A8), 63(A9), 64(A10), 65(A11), -> used by CV jacks (at least 64 et 65)
// 66(A12), 67, 68, 69(A15) -> use for SoftSerial
// alternative :
// http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
#if SOFTSERIAL_ENABLED
SoftwareSerial mSerial4 (A12, A13); // MIDI4 in out
SoftwareSerial mSerial5 (A14, A15); // MIDI5 in out
#endif
/*
   Some boards, like Teensy, Teensy++ and Arduino Mega, have more than 1 timer
   which is suitable for AltSoftSerial. You can configure which timer AltSoftSerial
   uses by editing "config/known_boards.h" within the library. This may allow
   AltSoftSerial to be used together with other libraries which require the
   timer which AltSoftSerial uses by default.

   define the Timer4 or Timer5 to use. Only ONE Altserial is available in the same sketch
*/
//#if ALTSOFTSERIAL_ENABLED
//AltSoftSerial mSerial4 (A12, A13);
//AltSoftSerial mSerial5 (A14, A15);
//#endif

//using namespace MIDI_NAMESPACE;
USING_NAMESPACE_MIDI;

// was hard to debug ! https://github.com/FortySevenEffects/arduino_midi_library/pull/151/commits/cadaa30fb5f46d5210ec523254fc982abe268e84
struct MatrixCtrlrSettings : public MIDI_NAMESPACE::DefaultSettings
{
  static const bool Use1ByteParsing = false; // set to false to process long sysex
  static const unsigned SysExMaxSize = 351;
  static const bool UseSenderActiveSensing = true;
  static const uint16_t SenderActiveSensingPeriodicity = 1000;
};
struct Matrix1000Settings : public MIDI_NAMESPACE::DefaultSettings
{
  static const bool Use1ByteParsing = false; // set to false to process long sysex
  static const unsigned SysExMaxSize = 351;
};
struct smallSysEx : public MIDI_NAMESPACE::DefaultSettings
{
  static const unsigned SysExMaxSize = 32;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI3,  MatrixCtrlrSettings); // CORE DIN Midi
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial3, MIDI1,  Matrix1000Settings); // A DIN Midi
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI1); // RX3 TX3 port A
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, MIDI2, smallSysEx); // port B DIN
// MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, midiUSB, Matrix1000Settings); // USB with HIDuino on m16u2 !

#if SOFTSERIAL_ENABLED // additional midi ports
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, mSerial4, MIDI4, smallSysEx); // TX4 port C out
MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, mSerial5, MIDI5, Matrix1000Settings); // TX5 port D out
#endif

//#if ALTSOFTSERIAL_ENABLED // additional midi ports
//MIDI_CREATE_CUSTOM_INSTANCE(AltSoftSerial, mSerial4, MIDI4, smallSysEx);
//MIDI_CREATE_CUSTOM_INSTANCE(AltSoftSerial, mSerial5, MIDI5, Matrix1000Settings);
//#endif

unsigned char INTERFACE_SERIAL;

////////////////////////////////////////////////////////////////////////////////////////////
//define eeprom chips (1 per chip)
EEPROM256_512 mem_0;
EEPROM256_512 mem_1;
EEPROM256_512 mem_2;
EEPROM256_512 mem_3;

elapsedMillis elapsedTime; // used for temporary message of pot movements
unsigned int interval = 1800; // 1800 milliseconds

void ResetTmpMsg() // used for temporary message of pot movements
{
  if (elapsedTime > interval && elapsedTime < 30000)
  {
    // return to previous screen
    SoftPanel.Mode = SoftPanel.LastMode;
    SoftPanel.Page = SoftPanel.LastPage;
    SoftPanel.IsNewPage = 1;
    SoftPanel_DisplayHandler();

    elapsedTime = 300001; // ugly, but it works
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// SETUP /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  delay(1000); // let arduino wake up ;)

  // set up the ADC :
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  // you can choose a prescaler from above : PS_16, PS_32, PS_64 or PS_128
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

  // analog configure
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // encoder clic
  pinMode(encoderClicPin, INPUT_PULLUP);

  // triggers (CV gate inputs)
  pinMode(TRIGGER_Pin, INPUT_PULLUP); // pin trig for analog drummachine
  pinMode(SYNC24_Pin, INPUT_PULLUP); // pin trig for analog clock

  /* https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
     attachInterrupt(digitalPinToInterrupt(pin), ISR, mode) (recommended)
     Mega, Mega2560, MegaADK DIGITAL PINS USABLE FOR INTERRUPTS : 2, 3, 18, 19, 20, 21
     2,3 encoder // 18,19 Serial1 CORE in/out // 20,21 i2c bus
  */
  //  // attach the new PinChangeInterrupts and enable event functions below
  //  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(SYNC24_Pin), Sync24TrigINT, RISING);
  //  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(TRIGGER_Pin), TrigINT, CHANGE);

  // CD4051 MUX adress PINS CONFIG
  DDRA = B11111111; // set PORT A to outputs
  setPin(1);

  // Initialize SR in and out
  SR.Initialize();
  if (DIGITAL_INPUTS_ACTIVE)
    ReadDigital(); //READING DIGITAL INPUTS//

  // init midi ports
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
  LCD_Clear();
  LCD_Init(); // custom char

  // init timer used for blinking UX LEDs
  Timer3.initialize(20000); //
  Timer3.attachInterrupt(timer3Isr);

  // initi Midi Clock generator
  mClockInit();

  Wire.begin(); // activate i2c bus !
  //begin EEPROM with I2C Address :
  mem_0.begin(0, 1); //addr 0 (DEC) type 1 (defined as 24LC512)
  mem_1.begin(1, 1); //addr 1 (DEC) type 1 (defined as 24LC512)
  mem_2.begin(2, 1); //addr 2 (DEC) type 1 (defined as 24LC512)
  mem_3.begin(3, 1); //addr 3 (DEC) type 1 (defined as 24LC512)

  Boot();
  InitGroupNote();
}

////////////////////////////////////////////////////////////////////////////////////////////
// LOOPS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{

#if DEBUG_looptime
  long looptime, looptime1;
  looptime = looptime1 = micros();
#endif

  // first, read midi !
  MIDI3.read();  // NRT : stable 2/4/2016 // Core IN
  MIDI1.read(); // Matrix A IN

  /*
     doesnt work 20210323 --> better to use callbacks
    // MIDI THRU in software : MIDI IN A -> MIDI OUT CORE (no filtering) :
    if (softMIDITHRU) {
      if (MIDI1.read()) {
        MIDI3.send(MIDI1.getType(),
                   MIDI1.getData1(),
                   MIDI1.getData2(),
                   MIDI1.getChannel());
      }
    }
  */
  //-------------------------------------------------------//
  //READING AND SMOOTHING ANALOG//
  //-------------------------------------------------------//
  if (ANALOG_INPUTS_ACTIVE)
    ReadAnalog();

  //-------------------------------------------------------//
  //READING DIGITAL INPUTS//
  //-------------------------------------------------------//
  if (DIGITAL_INPUTS_ACTIVE)
    ReadDigital(); //    DIN_NotifyChange();

  //-------------------------------------------------------//
  //SAMPLING, TESTING THRESHOLD AND SENDS MIDI//
  //-------------------------------------------------------//
  if (ANALOG_INPUTS_ACTIVE)
    AIN_NotifyChange();

  //------------------------------------------------------//
  //READING ENCODER
  //------------------------------------------------------//
  encoder();
  Encoder_Clic();

  //------------------------------------------------------//
  //READING TRIGGER (ext CV Gate)
  //------------------------------------------------------//
  //  enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(SYNC24_Pin));
  //  enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(TRIGGER_Pin));
  Triggers();
  //-------------------------------------------------------//
  //RECALL INIT//
  //-------------------------------------------------------//
  Recall();

  Display_Tick();
  Ticker();
  ResetTmpMsg();

  InitFlag = 0;
  booting = 0;

#if DEBUG_looptime
  looptime1 = micros();
  Serial.print (F("looptime (usec) : "));
  Serial.println(looptime1 - looptime);
#endif

}

/// --------------------------
/// Custom ISR Timer3 Routine
/// --------------------------
void timer3Isr()
{
  // Toggle LED
  LivePanel_BlinkLEDs();
  LivePanel_BlinkLFOs();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Ticker(void)
{
  // the timing of delayed midi transmits is a low priority
  LivePanel_HandleTransmitDelay(); // live panel
  MIDI_HandleMatrixModTransmitDelay(INTERFACE_SERIAL); // from soft panel
  MIDI_HandleDelayedVoiceParam(INTERFACE_SERIAL, true); // from soft panel
  MIDI_HandleFilterSustainTransmitDelay(INTERFACE_SERIAL); // from live panel
}

/////////////////////////////////////////////////////////////////////////////
// This function reboot arduino board (RISKY)
// https://arduino103.blogspot.fr/2013/06/comment-un-reset-darduino-par-logiciel.html
/////////////////////////////////////////////////////////////////////////////
void software_Reboot()
{
  // reboot system :
  Serial.println(F("rebooting system / software_Reboot() ")); Serial.println();
  delay(200);

  // print temp message on display
  LCD_Clear();
  lcd.setCursor(0, 0);
  lcd.print(F(" --SYSTEM REBOOT-- "));

  wdt_enable(WDTO_1S);

  while (1)
  {
  }
}
