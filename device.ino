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



matrix_model_t matrix_model;
unsigned char matrix_modele; // indicate if matrix6 or 1000
unsigned char device; // matrix hardware var : define Matrix1000 connected to i2c output (up to 4 matrix1000 is possible)
unsigned char router_device;	// define IIC port
unsigned char device_arp;

//////////////////////////////////////////////////////////////////////////////
// initiate devices on boot (last device used days before)
//////////////////////////////////////////////////////////////////////////////
void Device_Init()
{
  Matrix_Modele_Init();
  device = EEPROM.read(EEPROM_DEVICE);

  // set interface
  switch (device) {
    case Matrix_Device_A: INTERFACE_SERIAL = INTERFACE_SERIAL1; break;
    case Matrix_Device_B: INTERFACE_SERIAL = INTERFACE_SERIAL2; break;
#if SOFTSERIAL_ENABLED
    case Matrix_Device_C: INTERFACE_SERIAL = INTERFACE_SERIAL4; break;
    case Matrix_Device_D: INTERFACE_SERIAL = INTERFACE_SERIAL5; break;
#endif
    default: break;
  }

  Show_Selected_Device(device);
  if (matrix_modele == matrix_6) MIDI_EnterRemoteEditMode(INTERFACE_SERIAL);

#if DEBUG_device
  Serial.println(F("Device_Init()"));
  Serial.print(F("ROM read addr ")); Serial.print(EEPROM_DEVICE); Serial.print(F(" is device = ")); Serial.println(device, HEX);
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// define if M1000 or M6 on A B C or D
////////////////////////////////////////////////////////////////////////////
void Matrix_Modele_Init(void)
{
  matrix_model.A = EEPROM.read(EEPROM_MATRIX_MODELE_A);
  matrix_model.B = EEPROM.read(EEPROM_MATRIX_MODELE_B);
  matrix_model.C = EEPROM.read(EEPROM_MATRIX_MODELE_C);
  matrix_model.D = EEPROM.read(EEPROM_MATRIX_MODELE_D);

#if DEBUG_device
  Serial.println(F("Matrix_Modele_Init()"));
  Serial.println();
#endif
}

/////////////////////////////////////////////////////////////////////////////
/// Define device
////////////////////////////////////////////////////////////////////////////
void Device_Select(unsigned char pin)
{
  //  unsigned char i;

  // set device
  switch (pin) {

    case DIN_MATRIX_A: //
      device = Matrix_Device_A; // port midi
      matrix_modele = matrix_model.A; // regle de gestion des sysex
      //	  for(i=0; i<134; i++) EditBuffer[i] = EditBufferA[i]; // buffers to use
      //	EditBufferOrig[i] = EditBufferOrigA[i];
      break;

    case DIN_MATRIX_B: //
      device = Matrix_Device_B;
      matrix_modele = matrix_model.B;
      //	  for(i=0; i<134; i++) EditBuffer[i] = EditBufferB[i];
      //	EditBufferOrig[i] = EditBufferOrigB[i];
      break;

    case DIN_MATRIX_C: //
      device = Matrix_Device_C;
      matrix_modele = matrix_model.C;
      //	  for(i=0; i<134; i++) EditBuffer[i] = EditBufferC[i];
      //	EditBufferOrig[i] = EditBufferOrigC[i];
      break;

    case DIN_MATRIX_D: //
      device = Matrix_Device_D;
      matrix_modele = matrix_model.D;
      //	  for(i=0; i<134; i++) EditBuffer[i] = EditBufferD[i];
      //	EditBufferOrig[i] = EditBufferOrigD[i];
      // store in eeprom which device is edited
      break;

    default:
      break;

  }

  // store in eeprom which device is edited : ONLY, only when a patch is saved
  // EEPROM.write(EEPROM_DEVICE, device); // note : don't use too much write eeprom

#if DEBUG_device
  Serial.println (F("Device_Select() "));
  Serial.print(F("device ")); Serial.print(device, HEX); Serial.print(F(" written in ROM addr ")); Serial.println(EEPROM_DEVICE);
  Serial.println ();
#endif

  // set interface
  switch (device) {
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
  Serial.print(F("interface serial = ")); Serial.println(INTERFACE_SERIAL, DEC); Serial.println();
#endif

  if (matrix_modele == matrix_6) {
    MIDI_EnterRemoteEditMode(INTERFACE_SERIAL);
  }

  Show_Selected_Device(device);
  UI_Display_Patch();
  // envoie le patch >>> surtout pas ! on perd les reglages sinon, et on coupe les notes
  //	SendEditBuffer();


  //  ChooseEditBufferABCD(device, 1); // define which editbuffer to use >> euhh ???

  // maj des LEDs
  UpdateDinStates();

  //	ChooseEditBufferABCD(); // define which editbuffer to use >> euhh ???

}

////////////////////////////////////////////////////////////////////////////
//manage Led A B C D of selected Matrix device
///////////////////////////////////////////////////////////////////////////
void Show_Selected_Device(unsigned char device)
{
  //  unsigned char i;
  //  unsigned char EditBufferA [134];
  //  unsigned char EditBufferB [134];
  //  unsigned char EditBufferC [134];
  //  unsigned char EditBufferD [134];

  switch (device) {
    case Matrix_Device_A: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_A].dout_pin); // set the A LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_B].dout_pin); // rest is Off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_C].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_D].dout_pin);

      matrix_modele = matrix_model.A;
      //      //stock the editbuffer
      //      for (unsigned char i = 0; i < 134; ++i)
      //      {
      //        EditBufferB[i] = EditBuffer[i];
      //        EditBuffer[i] = EditBufferA[i];
      //      }
      break;

    case Matrix_Device_B: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_B].dout_pin); // set the B LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_A].dout_pin); // rest is Off
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_C].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_D].dout_pin);

      matrix_modele = matrix_model.B;
      //      // stock the editbuffer
      //      for (unsigned char i = 0; i < 134; ++i)
      //      {
      //        EditBufferA[i] = EditBuffer[i];
      //        EditBuffer[i] = EditBufferB[i];
      //      }
      break;

    case Matrix_Device_C: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_C].dout_pin); // set the C LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_A].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_B].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_D].dout_pin);
      matrix_modele = matrix_model.C;
      //      // stock the editbuffer
      //      for (unsigned char i = 0; i < 134; ++i)
      //      {
      //        EditBufferC[i] = EditBuffer[i];
      //        EditBuffer[i] = EditBufferC[i];
      //      }
      break;

    case Matrix_Device_D: //
      DOUT_PinSet1(DIN_ConfigMap[DIN_MATRIX_D].dout_pin); // set the D LED On
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_A].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_B].dout_pin);
      DOUT_PinSet0(DIN_ConfigMap[DIN_MATRIX_C].dout_pin);
      matrix_modele = matrix_model.D;
      //      // stock the editbuffer
      //      for (unsigned char i = 0; i < 134; ++i)
      //      {
      //        EditBufferD[i] = EditBuffer[i];
      //        EditBuffer[i] = EditBufferD[i];
      //      }
      break;

  }
#if DEBUG_device
  Serial.print(F("Show_Selected_Device() : ")); Serial.println(device, HEX);
  Serial.println();
#endif
}

////////////////////////////////////////////////////////////////////////////
// Manage tmpEditBuffers[]
///////////////////////////////////////////////////////////////////////////
void ChooseEditBufferABCD(unsigned char device, bool process)
{
  static unsigned char tmpEditBuffer[4][134];
  switch (process)
  {
    case 0:

      switch (device)
      {
        case Matrix_Device_A:
          for (unsigned char i = 0; i < 134; ++i)
          {
            EditBuffer[i] = tmpEditBuffer[0][i];
          }
          break;

        case Matrix_Device_B:
          for (unsigned char i = 0; i < 134; ++i)
          {
            EditBuffer[i] = tmpEditBuffer[1][i];
          }
          break;

        case Matrix_Device_C:
          for (unsigned char i = 0; i < 134; ++i)
          {
            EditBuffer[i] = tmpEditBuffer[2][i];
          }
          break;

        case Matrix_Device_D:
          for (unsigned char i = 0; i < 134; ++i)
          {
            EditBuffer[i] = tmpEditBuffer[3][i];
          }
          break;
      }
      break;

    case 1:
      switch (device)
      {
        case Matrix_Device_A:
          for (unsigned char i = 0; i < 134; ++i)
          {
            tmpEditBuffer[0][i] = EditBuffer[i];
          }
          break;

        case Matrix_Device_B:
          for (unsigned char i = 0; i < 134; ++i)
          {
            tmpEditBuffer[1][i] = EditBuffer[i];
          }
          break;

        case Matrix_Device_C:
          for (unsigned char i = 0; i < 134; ++i)
          {
            tmpEditBuffer[2][i] = EditBuffer[i];
          }
          break;

        case Matrix_Device_D:
          for (unsigned char i = 0; i < 134; ++i)
          {
            tmpEditBuffer[3][i] = EditBuffer[i];
          }
          break;
      }
      break;
  }

}


