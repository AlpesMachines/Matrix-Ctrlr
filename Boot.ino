#include "softpanel.h"
#include "livepanel.h"
#include "memo.h"

///////////////////////////////////////////////////////////
// Boot sequence
///////////////////////////////////////////////////////////
void Boot()
{

  booting = 1;

  delay(200);
  Serial.print(F("Boot v")); Serial.println(firmware);

  for (unsigned char i = 0; i < NBR_LED; ++i)
  {
    SR.Led_Pin_Write(i, 0); // éteint toutes les LEDS
    SR.Led_Pin_Write(i, 1); // allume d'abord toutes les LEDS
    delay(10);
  }

  LCD_Display_Init(); // show firmware version
  delay(500);

  for (unsigned char i = 0; i < NBR_LED; ++i)
    SR.Led_Pin_Write(i, 0); // enfin éteint les LEDS

  for (unsigned char k = 0; k < 10; k++) { // stabilise analog inputs
    ReadAnalog();
    ReadDigital();
  }

  // read CFG parameters from internal eeprom:
  encoder_inverted = EEPROM.read(EEPROM_ENCODER_INVERTED);  // read encoder config
  mThru_XCc = EEPROM.read(EEPROM_MTHRU_XCC);
  //ui_external_clk = true;

  //  InitEditBuffer(); // load default EditBuffer - > NO
  Device_Init(); // initialiser les ports  (choisir le port serie + device led)
  MIDI_Init(); // initialiser midi channel (rappeler le midi channel de base)

  if (MIDITHRU_OFF_BOOT)
  {
    MIDI3.turnThruOff(); // disable MIDI Thru on CORE OUT
    MIDI1.turnThruOff(); // disable MIDI Thru on Matrix A
  }

  // SendMasterParametersInit(INTERFACE_SERIAL1);
  // SendMasterParametersInit(INTERFACE_SERIAL2);
  // SendGlobalParametersInit(INTERFACE_SERIAL3);
  // SendMasterParameters(INTERFACE_SERIAL1);
  // SendMasterParameters(INTERFACE_SERIAL2);
  SendGlobalParameters(INTERFACE_SERIAL3);

  PATCH_Init(); // se remémorer le dernier truc (rappeler le dernier patch et bank sauvé, charger l'editbuffer depuis les 24LC512, l'envoyer)
  SoftPanel_Init(); // load softpanel (afficher tout ça)

  MIDI1.sendControlChange(123, 0, MIDI_CHANNEL); // all notes off (tout le monde se tait)
  MIDI2.sendControlChange(123, 0, MIDI_CHANNEL + 1); // all notes off
  MIDI3.sendControlChange(123, 0, MIDI_CHANNEL + 2); // all notes off
#if SOFTSERIAL_ENABLED
  MIDI4.sendControlChange(123, 0, MIDI_CHANNEL + 3); // all notes off
  MIDI5.sendControlChange(123, 0, MIDI_CHANNEL + 4); // all notes off
#endif

// METTRE CES 3 lignes dans une fonction accessible depuis la page ARP afin d'initilaiser le bazar:
//  ArpInit(); // set default arp parameters
//  Init_aChord();
//  Init_Seq();

  // on est prêt à démarrer :)
  send_start = 1;

  Serial.println(F("End of boot.")); Serial.println();

}
