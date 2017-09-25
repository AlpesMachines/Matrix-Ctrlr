#ifndef midi_h
#define midi_h

extern bool MIDI_Incoming;
extern bool MIDI_ReceivingEditBuffer;
extern bool MIDI_ReceivingBank;
extern byte MIDI_CHANNEL;

void MIDI_Init(void);
extern void MIDI_SendSysex(unsigned char interface, byte* sysex);
void MIDI_SendChannel(void);
void MIDI_SendPatchProgram(unsigned char interface, unsigned char program);
void MIDI_SendPatchNumber(unsigned char interface, unsigned char bank, unsigned char program);
void MIDI_RequestEditBuffer(unsigned char device,unsigned char interface, unsigned char program);
void MIDI_RequestSinglePatch(unsigned char interface, unsigned char program);
void MIDI_RequestMasterParameters(unsigned char interface);
void MIDI_BankRequest(unsigned char interface, unsigned char bank_req);
void MIDI_SendVoiceParam(unsigned char interface, unsigned char param, unsigned char value, bool midiThru);
void MIDI_SendDelayedVoiceParam(unsigned char param, unsigned char value);
void MIDI_HandleDelayedVoiceParam(unsigned char interface, bool midiThru);
void MIDI_SendMatrixModParam(unsigned char mmbus, unsigned char mmsrc, unsigned char mmdest, unsigned char value);
void MIDI_HandleMatrixModTransmitDelay(unsigned char interface);
void MIDI_HandleFilterSustainTransmitDelay(unsigned char interface);
void MIDI_SendSustainBuffer(unsigned char value);
void MIDI_Send_BreathController(unsigned char interface, unsigned char value);
void MIDI_SetBank(unsigned char interface, unsigned char bank);

//void MIDI_SavePatch(unsigned char banksaved, unsigned char programsaved);
void MIDI_SendCC(unsigned char pin, unsigned int pin_value);
void MIDI_EnterRemoteEdit(unsigned char interface);

// void MIDI_SendSysex(unsigned char interface, byte *sysex); NE MARCHE PAS !

extern void MIDI_SendControlMessages(unsigned char deviceCC, unsigned char param, unsigned char value);
extern void MIDI_Control2Sysex( unsigned char evnt0, unsigned char param, unsigned char evnt2);

void MIDI_HandleRequestData(unsigned char byte);
void MIDI_HandleRemoteParameterEdit(unsigned char byte);
void MIDI_HandleRemoteModulationEdit(unsigned char byte);

#endif


