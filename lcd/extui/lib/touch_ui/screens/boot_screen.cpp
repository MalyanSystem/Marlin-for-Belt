/*******************
 * boot_screen.cpp *
 *******************/

/****************************************************************************
 *   Written By Mark Pelletier  2017 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2019 - Cocoa Press                          *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <http://www.gnu.org/licenses/>.                              *
 ****************************************************************************/

#include "../config.h"

#if ENABLED(TOUCH_UI)

#include "screens.h"
#include "../ui_lib/constants.h"
#include "../ui_lib/commands.h"

//#include "../ui_lib/poly_ui.h"
//#include "../archim2-flash/flash_storage.h"

#ifdef SHOW_CUSTOM_BOOTSCREEN
  #ifdef TOUCH_UI_PORTRAIT
    #include "../theme/_bootscreen_portrait.h"
  #else
    #include "../theme/_bootscreen_landscape.h"
  #endif
#else
  #ifdef TOUCH_UI_PORTRAIT
    #include "../theme/marlin_bootscreen_portrait.h"
  #else
    #include "../theme/marlin_bootscreen_landscape.h"
  #endif
#endif

using namespace ExtUI;
using namespace Theme;

void BootScreen::onRedraw(draw_mode_t) {
  clear_color_rgb(0);
  clear(true,true,true);

  turn_on_backlight();
  SoundPlayer::set_volume(255);
}

void BootScreen::onIdle() {
  if (is_touching()) {
    // If the user is touching the screen at startup, then
    // assume the user wants to re-calibrate the screen.
    // This gives the user the ability to recover a
    // miscalibration that has been stored to EEPROM.

    // Also reset display parameters to defaults, just
    // in case the display is borked.
    InterfaceSettingsScreen::failSafeSettings();

    GOTO_SCREEN(TouchCalibrationScreen);
    current_screen.forget();
    PUSH_SCREEN(StatusScreen);
    StatusScreen::setStatusMessage(GET_TEXT_F(WELCOME_MSG));
  } else {
    printf("Todo: we check eeprom and init here.\n");
    /*if (!UIFlashStorage::is_valid()) {
      StatusScreen::loadBitmaps();
      SpinnerDialogBox::show(GET_TEXT_F(MSG_PLEASE_WAIT));
      UIFlashStorage::format_flash();
      SpinnerDialogBox::hide();
    }*/

    #if DISABLED(TOUCH_UI_NO_BOOTSCREEN)
      /*if (UIData::animations_enabled()) {
        // If there is a startup video in the flash SPI, play
        // that, otherwise show a static splash screen.
        if (!MediaPlayerScreen::playBootMedia())
          showSplashScreen();
      }*/
      showSplashScreen();
    #endif

    #ifdef TOUCH_UI_LULZBOT_BIO
      GOTO_SCREEN(BioConfirmHomeXYZ);
      current_screen.forget();
      PUSH_SCREEN(StatusScreen);
      PUSH_SCREEN(BioConfirmHomeE);
    #elif NUM_LANGUAGES > 1
      StatusScreen::setStatusMessage(GET_TEXT_F(WELCOME_MSG));
      GOTO_SCREEN(LanguageMenu);
    #else
      StatusScreen::setStatusMessage(GET_TEXT_F(WELCOME_MSG));
      GOTO_SCREEN(StatusScreen);
    #endif
  }
}

void BootScreen::showSplashScreen() {
  printf("showSplashScreen()\n");
  cmd_dlstart();
  clear_color_rgb(LOGO_BACKGROUND);
  clear(true,true,true);

  /*#define POLY(A) PolyUI::poly_reader_t(A, sizeof(A)/sizeof(A[0]))
  #define LOGO_PAINT_PATH(rgb, path) cmd.cmd(COLOR_RGB(rgb)); ui.fill(POLY(path));

  PolyUI ui(cmd);

  LOGO_PAINT_PATHS*/

  printf("Let's render logo here\n");

  dl_display();
  cmd_swap();
  cmd_execute();

  ExtUI::delay_ms(2500);
}

#endif // TOUCH_UI
