/*******************
 * kill_screen.cpp *
 *******************/

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
#include "../ui_lib/grid_layout.h"

using namespace ExtUI;

// The kill screen is an oddball that happens after Marlin has killed the events
// loop. So we only have a show() method rather than onRedraw(). The KillScreen
// should not be used as a model for other UI screens as it is an exception.

void KillScreen::show(const char *message) {
  cmd_dlstart();
  clear_color_rgb(Theme::bg_color);
  clear(true,true,true);
  tag(0);

  #define GRID_COLS 4
  #define GRID_ROWS 8

  font(Theme::font_large);
  color_rgb(Theme::bg_text_enabled);
  text(BTN_POS(1,2), BTN_SIZE(4,1), message);
  text(BTN_POS(1,3), BTN_SIZE(4,1), (const char *)GET_TEXT_F(MSG_HALTED));
  text(BTN_POS(1,6), BTN_SIZE(4,1), (const char *)GET_TEXT_F(MSG_PLEASE_RESET));

  #undef GRID_COLS
  #undef GRID_ROWS

  dl_display();
  cmd_swap();
  cmd_execute();

  InterfaceSoundsScreen::playEventSound(InterfaceSoundsScreen::PRINTING_FAILED, PLAY_SYNCHRONOUS);
}

#endif // TOUCH_UI_FTDI_EVE
