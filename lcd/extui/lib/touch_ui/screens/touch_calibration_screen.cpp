/********************************
 * touch_calibration_screen.cpp *
 ********************************/

/****************************************************************************
 *   Written By Mark Pelletier  2017 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
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

using namespace ExtUI;
using namespace Theme;

#define GRID_COLS 4
#define GRID_ROWS 16

void TouchCalibrationScreen::onEntry() {
  BaseScreen::onEntry();

  if (is_touching()) {
    // Ask the user to release the touch before starting,
    // as otherwise the first calibration point could
    // be misinterpreted.
    cmd_dlstart();
    clear_color_rgb(bg_color);
    clear(true,true,true);
    color_rgb(bg_text_enabled);

    draw_text_box(BTN_POS(1,1), BTN_SIZE(4,16), (const char*)GET_TEXT_F(MSG_TOUCH_CALIBRATION_START), OPT_CENTER, font_large);
    dl_display();
    cmd_swap();
    cmd_execute();

    while (is_touching()) {
      #if ENABLED(TOUCH_UI_DEBUG)
        SERIAL_ECHO_MSG("Waiting for touch release");
      #endif
    }
  }

  // Force a refresh
  cmd_dlstart();
  onRedraw(FOREGROUND);
  dl_display();
  cmd_execute();
}

void TouchCalibrationScreen::onRefresh() {
  // Don't do the regular refresh, as this would
  // cause the calibration be restarted on every
  // touch.
}

void TouchCalibrationScreen::onRedraw(draw_mode_t) {
  clear_color_rgb(bg_color);
  clear(true,true,true);
  color_rgb(bg_text_enabled);

  draw_text_box(BTN_POS(1,1), BTN_SIZE(4,16), GET_TEXT_F(MSG_TOUCH_CALIBRATION_PROMPT), OPT_CENTER, font_large);
  cmd_calibrate();
}

void TouchCalibrationScreen::onIdle() {
  if (!is_touching()) {
    GOTO_PREVIOUS();
  }
}

#endif // TOUCH_UI
