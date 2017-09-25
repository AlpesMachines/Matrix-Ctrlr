#ifndef device_h
#define device_h

// declare the Oberheim matrix hardware :
#define Matrix_Device_A 0 //0x0a // matrix connected to Serial1 
#define Matrix_Device_B 1 //0x0b // matrix connected to Serial2
#define Matrix_Device_C 2 //0x0c // matrix connected to Serial4
#define Matrix_Device_D 3 //0x0d // matrix connected to Serial5
/*
  #define matrix_model_A matrix_1000
  #define matrix_model_B matrix_1000
  #define matrix_model_C matrix_6
  #define matrix_model_D matrix_1000
*/
//// status of matrix_model
//typedef union {
//  struct {
//    unsigned ALL: 8;
//  };
//  struct {
//    unsigned A: 1;
//    unsigned B: 1;
//    unsigned C: 1;
//    unsigned D: 1;
//  };
//} matrix_model_t;
extern bool matrix_model_A;
extern bool matrix_model_B;
extern bool matrix_model_C;
extern bool matrix_model_D;

#define matrix_6    1		// 1 if Matrix6/6R , 0 if Matrix 1000
#define matrix_1000 0

//device var used in chaosmatrix.c and else
extern bool matrix_modele; // indicate if matrix6 or 1000
extern unsigned char device; // set the Matrix Device A, B, C or D regarding i2c output
extern unsigned char router_device;	// define IIC port

extern unsigned char device_arp; // set the Matrix Device A, B, C or D regarding i2c output
extern unsigned char device_seq; //

// function to set the device :
extern void Matrix_Modele_Init(void);

extern void Device_Select(unsigned char pin);
extern void Show_Selected_Device(unsigned char device);
extern void Device_Init(unsigned char device);
extern void Router(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2);
extern void Router_Arp(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2);

//extern matrix_model_t matrix_model;

#endif




