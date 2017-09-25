//#include <cmios.h>
#include "device.h"
//#include "main.h"
#include "din.h"
#include "midi.h"
#include "chaosmatrix.h"
//#include "arp.h"
#include "ui_patch.h"
//#include "memo.h"
#include <EEPROM.h>



//matrix_model_t matrix_model;
bool matrix_model_A;
bool matrix_model_B;
bool matrix_model_C;
bool matrix_model_D;
bool matrix_modele; // indicate if matrix6 or 1000
unsigned char device; // matrix hardware var : define Matrix1000 connected to i2c output (up to 4 matrix1000 is possible)
unsigned char router_device;	// define IIC port
unsigned char device_arp;

//////////////////////////////////////////////////////////////////////////////
// initiate devices on boot (last device used days before)
//////////////////////////////////////////////////////////////////////////////
void Device_Init(unsigned char device)
{
  Matrix_Modele_Init(); // load modele 1000 or 6
  
  // set interface
  switch (device) {
    case Matrix_Device_A: INTERFACE_SERIAL = INTERFACE_SERIAL1; matrix_modele = matrix_model_A; break;
    case Matrix_Device_B: INTERFACE_SERIAL = INTERFACE_SERIAL2; matrix_modele = matrix_model_B; break;
#if SOFTSERIAL_ENABLED
    case Matrix_Device_C: INTERFACE_SERIAL = INTERFACE_SERIAL4; matrix_modele = matrix_model_C; break;
    case Matrix_Device_D: INTERFACE_SERIAL = INTERFACE_SERIAL5; matrix_modele = matrix_model_D; break;
#endif
    default: break;
  }
  
  if (matrix_modele == matrix_6)
    MIDI_EnterRemoteEditMode(INTERFACE_SERIAL);
    
  Show_Selected_Device(device);

//  SendEditBuffer(device);
//  
//  //  ChooseEditBufferOrig(device);
//  //  CopyEditBufferOrigToEditBuffer(device);
//  UpdateDinStates();



#if DEBUG_device
  Serial.print(F("Device_Init() "));
  Serial.print(F("/ read intEEPROM_addr[")); Serial.print(EEPROM_DEVICE, DEC); Serial.print(F("] is device : ")); Serial.println(device + 0x0a, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// define if M1000 or M6 on A B C or D
////////////////////////////////////////////////////////////////////////////
void Matrix_Modele_Init(void)
{
  matrix_model_A = EEPROM.read(EEPROM_MATRIX_MODELE_A);
  matrix_model_B = EEPROM.read(EEPROM_MATRIX_MODELE_B);
  matrix_model_C = EEPROM.read(EEPROM_MATRIX_MODELE_C);
  matrix_model_D = EEPROM.read(EEPROM_MATRIX_MODELE_D);

#if DEBUG_device
  Serial.print(F("Matrix_Modele_Init() / "));
  Serial.print(F("device A is "));
  if (matrix_model_A) Serial.print(F("Matrix6 / ")); else Serial.print(F("Matrix1000 / "));
  Serial.print(F("device B is "));
  if (matrix_model_B) Serial.print(F("Matrix6 / ")); else Serial.print(F("Matrix1000 / "));
  Serial.print(F("device C is "));
  if (matrix_model_C) Serial.print(F("Matrix6 / ")); else Serial.print(F("Matrix1000 / "));
  Serial.print(F("device D is "));
  if (matrix_model_D) Serial.print(F("Matrix6")); else Serial.print(F("Matrix1000"));
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// Define device
////////////////////////////////////////////////////////////////////////////
void Device_Select(unsigned char pin)  // unsigned char Device_Select(unsigned char pin) return device;
{
  //SoftPanel.Mode = Device;
  RefreshSoftPanel = true;

  ///////////////////////// commande : ///////////////////////////////////
  // set device
  switch (pin)
  {
    case DIN_MATRIX_A: //
      device = Matrix_Device_A; // port midi
      matrix_modele = matrix_model_A; // regle de gestion des sysex
      break;

    case DIN_MATRIX_B: //
      device = Matrix_Device_B;
      matrix_modele = matrix_model_B;
      break;

    case DIN_MATRIX_C: //
      device = Matrix_Device_C;
      matrix_modele = matrix_model_C;
      break;

    case DIN_MATRIX_D: //
      device = Matrix_Device_D;
      matrix_modele = matrix_model_D;
      break;

    default:
      break;

  }

#if DEBUG_device
  Serial.print(F("Device_Select() / "));
  Serial.print(F("device is ")); Serial.print(device + 0x0a, HEX); //Serial.print(F(", written in ROM addr = ")); Serial.println(EEPROM_DEVICE);
  if (matrix_modele == matrix_6)
    Serial.println(F(" & modele is Matrix 6 "));
  else
    Serial.println(F(" & modele is Matrix 1000 "));
#endif

  //  Choose EditBufferOrig, UnisonDetuneOrig, ArpParametersOrig, sequenceOrig :
  PATCH_Load(uBank[device], uPatch[device]);

  // store in eeprom which device is edited -> NO, only when a patch is saved , don't use too much write.eeprom

  // envoie le patch >>> surtout pas ! on perd les reglages sinon, et on coupe les notes
  //  SendEditBuffer();

  // define which EditBuffer[device] to use >> euhh ??? fait automatiquement dans le debut de la fonction par la variable device

  // set interface
  switch (device)
  {
    case Matrix_Device_A:
      INTERFACE_SERIAL = INTERFACE_SERIAL1;
      break;

    case Matrix_Device_B:
      INTERFACE_SERIAL = INTERFACE_SERIAL2;
      break;

#if SOFTSERIAL_ENABLED
    case Matrix_Device_C:
      INTERFACE_SERIAL = INTERFACE_SERIAL4;
      break;

    case Matrix_Device_D:
      INTERFACE_SERIAL = INTERFACE_SERIAL5;
      break;
#endif

    default:
      break;
  }
#if DEBUG_device
  Serial.print(F("interface serial = ")); Serial.println(INTERFACE_SERIAL, DEC); 
#endif

  // send a special sysex if device is a Matrix6 in order to receive Livepanel order
  if (matrix_modele == matrix_6)
    MIDI_EnterRemoteEditMode(INTERFACE_SERIAL);


  ///////////////////////// VISUELS : ///////////////////////////////////
  // set LED of Device area:
  Show_Selected_Device(device);
  // maj des LEDs Livepanel
  UpdateDinStates();
  // update display if in Patch, Edit, etc mode :
  SoftPanel_DisplayHandler();

}

////////////////////////////////////////////////////////////////////////////
//manage Led A B C D of selected Matrix device
///////////////////////////////////////////////////////////////////////////
void Show_Selected_Device(unsigned char device)
{
#if DEBUG_device
  Serial.print(F("Show_Selected_Device()/ LED_DEVICE On is : ")); Serial.println(device + 0x0A, HEX);
  Serial.println();
#endif

  switch (device) {
    case Matrix_Device_A: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_A].dout_pin); // set the A LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_B].dout_pin); // rest is Off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_C].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_D].dout_pin);
      break;

    case Matrix_Device_B: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_B].dout_pin); // set the B LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_A].dout_pin); // rest is Off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_C].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_D].dout_pin);
      break;

    case Matrix_Device_C: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_C].dout_pin); // set the C LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_A].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_B].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_D].dout_pin);
      break;

    case Matrix_Device_D: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_D].dout_pin); // set the D LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_A].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_B].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_C].dout_pin);
      break;
  }
}

////////////////////////////////////////////////////////////////////////////
// Manage EditBufferOrig[] when you switch device
///////////////////////////////////////////////////////////////////////////
void ChooseEditBufferOrig(unsigned char device)
{
#if DEBUG_device
  Serial.print(F("ChooseEditBufferOrig()")); Serial.print(F("/ device = ")); Serial.println(device + 0x0a, HEX);
#endif

  Read_Patch_From_BS(device, uBank[device], uPatch[device]); // read into BS
  //CopyEditBufferOrigToEditBuffer(device);
}

void ChooseUnisonDetuneOrig(unsigned char device)
{
  UnisonDetune[device] = UnisonDetuneOrig;
}
void ChooseArpParametersOrig(unsigned char device)
{

}
/////////////////////////////////////////////////////////////////
// Copy EditBufferOrig into EditBuffer
////////////////////////////////////////////////////////////////
void CopyEditBufferOrigToEditBuffer(unsigned char device)
{
#if DEBUG_device
  Serial.print(F("CopyEditBufferOrigToEditBuffer()")); Serial.print(F("/ device = ")); Serial.println(device + 0x0a, HEX);
#endif

  for (unsigned char i = 0; i < 134; ++i)
    EditBuffer[device][i] = EditBufferOrig[i];
  //memcpy(EditBuffer[device], EditBufferOrig, 134); // more C langage ;)
}

