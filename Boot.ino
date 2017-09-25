#include "softpanel.h"
#include "livepanel.h"
#include "memo.h"
#include "oner.h"


///////////////////////////////////////////////////////////
// Boot sequence
///////////////////////////////////////////////////////////
void Boot(void)
{
  booting = 1;

  delay(200);
  Serial.print(F("Matrix Ctrlr v")); Serial.println(firmware);
  Serial.println(F("Boot() "));

  for (unsigned char i = 0; i < NBR_LED; ++i)
  {
    SR.Led_Pin_Write(i, 0); // éteint toutes les LEDS
    SR.Led_Pin_Write(i, 1); // allume d'abord toutes les LEDS
    delay(10);
  }

  // show firmware version on display
  Display_FW(); delay(500);

  lcd.setCursor(0, 0);
  lcd.print(F("--- Boot  System ---"));

  // stabilise analog inputs
  for (unsigned char k = 0; k < 32; k++)
  {
    ReadAnalog();
    ReadDigital();
  }


  /////////////////////// check formatting ////////////////////////
  
  // check 24LC512 as blank or formatted before previously
  if (Check_ExtEEPROM_Format(0) || Check_ExtEEPROM_Format(2))
    FORMAT_Memory(3);

  // check internal eeprom formated previously
  if ( Check_IntEEPROM_Format())
    FORMAT_Memory(4);

    // automatically reboot if Formatting necessary


  /////////////////////// recall system config ////////////////////////
  Serial.println(F("recall system config"));

  lcd.setCursor(0, 1);
  lcd.print(F("recall system config"));

  // recall interface behaviour:
  encoder_inverted = EEPROM.read(EEPROM_ENCODER_INVERTED);  // read encoder config
  mThru_XCc = EEPROM.read(EEPROM_MTHRU_XCC);
  localControl = EEPROM.read(EEPROM_LOCAL_CONTROL);
  //ui_external_clk = true;

  // recall base midi channel and thru lib functionnality
  MIDI_Init();

  if (MIDITHRU_OFF_BOOT)
  {
    MIDI3.turnThruOff(); // disable MIDI Thru on CORE OUT
    MIDI1.turnThruOff(); // disable MIDI Thru on Matrix A
  }
  delay(300);
  // send global parameters to DAW
  SendGlobalParameters(INTERFACE_SERIAL3);

  // and to devices
  SendGlobalParameters(INTERFACE_SERIAL1);
  SendGlobalParameters(INTERFACE_SERIAL2);
  SendGlobalParameters(INTERFACE_SERIAL4);
  SendGlobalParameters(INTERFACE_SERIAL5);

  // enfin éteint les LEDS
  for (unsigned char i = 0; i < NBR_LED; ++i)
    SR.Led_Pin_Write(i, 0);

  // load softpanel (default display)
  //SoftPanel_Init();

  //  recall Zone[] definition
  ZONE_Init();

  /////////////////////// recall patch config ////////////////////////
  /*
    pour chaque device :
    1. charger le(s) patch dans notre tableau EditBuffer[4][134]
    2. charger le(s) matrix_modele, passer en MIDI_EnterRemoteEditMode si besoin
    3. envoyer le(s) editbuffer
    puis :
    4. chercher le dernier device sauvé
    5. afficher le dernier device sauvé
    6. choisir l'editBufferOrig correspondant
    7. selectionner l'editBuffer corespondant
    8. (mettre à jour les LEDs)
    9. récupérer les ui_ARP à partir de MatrixA
  */
  Serial.println(F("recall patch config"));

  lcd.setCursor(0, 1);
  lcd.print(F("recall patch config "));

  Matrix_Modele_Init();

  // on each device first :
  for (unsigned char d = Matrix_Device_A; d <= Matrix_Device_D; d++)
  {
    // recall device configuration
    Device_Init(d);
    // recall last patch (rappeler le dernier patch et bank sauvé, charger l'EditBuffer[device] depuis les 24LC512, l'envoyer)
    PATCH_Init(d);
    // leds, later
    //UpdateDinStates();
  }

  // then on the lastdevice saved :
  device = EEPROM.read(EEPROM_DEVICE);
#if DEBUG_device
  Serial.print(F("Boot() / device = ")); Serial.println(device, DEC);
#endif

  // load patch and set device of the lastdevice saved :
  Device_Init(device);
  PATCH_Init(device);

  // load EditBufferOrig, (done previously above)
  // send Unison detune value TO DO to A, B, C, D : done in patch_init() in the loop

  // load &ArpParameters into ui*_arpVar of last device (is this particular case : MtrixA)
  ArpParameters_Load(Matrix_Device_A);
  Init_aChord();

  delay(200);

  /////////////////////// recall silent notes ////////////////////////
  Serial.println(F("recall silent notes"));

  lcd.setCursor(0, 1);
  lcd.print(F("-- all notes off --"));

  // all notes off
  MIDI1.sendControlChange(123, 0, MIDI_CHANNEL); //(tout le monde se tait)
  MIDI2.sendControlChange(123, 0, MIDI_CHANNEL + 1); // all notes off
  MIDI3.sendControlChange(123, 0, MIDI_CHANNEL + 2); // all notes off
#if SOFTSERIAL_ENABLED
  MIDI4.sendControlChange(123, 0, MIDI_CHANNEL + 3); // all notes off
  MIDI5.sendControlChange(123, 0, MIDI_CHANNEL + 4); // all notes off
#endif

  // METTRE CES 3 lignes dans une fonction accessible depuis la page ARP afin d'initilaiser ARP&SEQ : done above
  //  ArpParameters_Init(); // set default arp parameters
  //  Init_aChord();
  //  Init_Seq();

  delay(200);
  // on est prêt à démarrer :)
  send_start = 1;

  // load softpanel (default display)
  SoftPanel_Init();
  UpdateDinStates();


  Serial.println(F("End of Boot.")); Serial.println();

}
