#include "Display.h"

#include "define.h"
#include "pots.h"
#include "softpanel.h"
#include "livepanel.h"

app_flags_t app_flags;

///////////////////////////////////////////////////////////
// affichage démarage du firmware
///////////////////////////////////////////////////////////
void Display_FW(void) {
  lcd.setCursor(0, 0); lcd.print(F("ALPES MACHINES 2017 "));
  lcd.setCursor(0, 1); lcd.print(F("Matrix Ctrlr ")); lcd.print(firmware);
  delay(255); // to read it ^^
  //lcd.clear(); // flush lcd
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
//
// the live panel should only display on the lcd when we're in patch mode.
/////////////////////////////////////////////////////////////////////////////
void Display_Tick(void) {

  // display encoder movements
  if (app_flags.Display_ENC_Req)
  {
    SoftPanel_DisplayHandler();
    app_flags.Display_ENC_Req = 0; // handled
  }

  // display analog pot movements
  if (app_flags.Display_Pot_Req && (SoftPanel.Mode == Patch))
  {
    //		LivePanel_DisplayAin();
    app_flags.Display_Pot_Req = 0; // handled
  }

  // display buttons
  if (app_flags.Display_DIN_Req)
  {
    // NE PAS METTRE CETTE SECTION SINON DCO2 ClIC affiché au démarrage :
    //    if (DIN_ConfigMap[last_din_pin].group == BUTGRP_LIVE && SoftPanel.Mode == Patch )
    //     LivePanel_DisplayDin(last_din_pin);
    //  else
    SoftPanel_DisplayHandler ();
    app_flags.Display_DIN_Req = 0; // handled
  }
}

void LCD_Display(void)
{

}


