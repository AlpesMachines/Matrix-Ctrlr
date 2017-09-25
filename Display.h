
#ifndef Display_h
#define Display_h

// status of  application
typedef union 
{
  struct 
  {
    unsigned ALL: 8;
  };

  struct {
    unsigned DISPLAY_UPDATE_REQ: 1;	// requests a display update :: A VIRER
    unsigned Display_DIN_Req: 1;
    unsigned Display_Pot_Req: 1;
    unsigned Display_DOUT_Req: 1;		// not used
    unsigned Display_ENC_Req: 1;
    //unsigned Transmit_Delayed_Pot:1;	//added for matrix
  };
} app_flags_t;

#endif
