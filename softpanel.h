#ifndef softpanel_h
#define softpanel_h

#define SOFT_PAGE1 0
#define SOFT_PAGE2 1
#define SOFT_PAGE3 2
#define SOFT_PAGE4 3
#define SOFT_PAGE5 4
#define SOFT_PAGE6 5

#define SOFT_PAGE11 11
#define SOFT_PAGE12 12
#define SOFT_PAGE13 13
#define SOFT_PAGE21 21
#define SOFT_PAGE22 22
#define SOFT_PAGE23 23
#define SOFT_PAGE31 31
#define SOFT_PAGE32 32
#define SOFT_PAGE41 41
#define SOFT_PAGE42 42
#define SOFT_PAGE51 51
#define SOFT_PAGE52 52

void SoftPanel_Init(void);
void SoftPanel_DisplayHandler(void);
void SoftPanel_Handler (unsigned char pin, unsigned char encval);
char SetEncoderValue(unsigned char *value, unsigned char max);
int SetEncoderValue2(unsigned int *value, unsigned int max);
char SetEncoder7Signed(unsigned char *value);

enum PanelMode_t
{
  Patch,
  //	Patch_Page2,
  Edit,
  Oscillators,
  Filter,
  Envelopes,
  Keyboard,
  Matrix,
  Arp, 
  Cfg,
  Device
};

enum EditButton_t
{
  Edit0,    // no button selected
  Edit1,
  Edit2,
  Edit3,
  Edit4,
  Edit5,
  EditShift,
  EditAlt,
  EditArp,
  EditCfg,
  EditInc,
  EditDec,
  EditEncClic,
  EditDeviceA,
  EditDeviceB,
  EditDeviceC,
  EditDeviceD
  
};

typedef struct
{
  enum PanelMode_t Mode;
  enum PanelMode_t LastMode;
  unsigned char Page;             // current page number
  unsigned char LastPage;         // last page number
  unsigned char IsNewPage;        // 1=yes, 0=no was the page recently changed?
  unsigned char Button;           // recent button push
  signed char   EncoderValue;     // encoder increment value
  enum EditButton_t EditButton;   // current selected edit button for the encoder
  unsigned char Selected_Osc;
  unsigned char Selected_Env;
  unsigned char Selected_LFO;
  unsigned char Selected_Ramp;
  unsigned char Selected_MatrixBus;

} SoftPanel_t;
extern SoftPanel_t SoftPanel;

//extern enum PanelMode_t SoftPanelMode;

#endif
