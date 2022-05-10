/********************
 * about_screen.cpp *
 ********************/

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

#define GRID_COLS 4
#define GRID_ROWS 7

using namespace Theme;
using namespace ExtUI;

static void back_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) GOTO_PREVIOUS();
}
static void infostat_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
    #if ENABLED(PRINTCOUNTER)
      GOTO_SCREEN(StatisticsScreen);
    #endif
    }
}

void AboutScreen::onEntry() {
  BaseScreen::onEntry();
  sound.play(chimes, PLAY_ASYNCHRONOUS);
}

void AboutScreen::onRedraw(draw_mode_t) {
  /*clear_color_rgb(bg_color);
  clear(true,true,true);
  color_rgb(bg_text_enabled);
  tag(0);*/
  //esp_backtrace_print(50);
  lv_obj_clean(lv_scr_act());

  #define HEADING_POS BTN_POS(1,2), BTN_SIZE(4,1)
  #define FW_VERS_POS BTN_POS(1,3), BTN_SIZE(4,1)
  #define FW_INFO_POS BTN_POS(1,4), BTN_SIZE(4,1)
  #define LICENSE_POS BTN_POS(1,5), BTN_SIZE(4,2)
  #define STATS_POS   BTN_POS(1,7), BTN_SIZE(2,1)
  #define BACK_POS    BTN_POS(3,7), BTN_SIZE(2,1)

  #define _INSET_POS(x,y,w,h) x + w/10, y, w - w/5, h
  #define INSET_POS(pos) _INSET_POS(pos)

  #ifdef TOOLHEAD_NAME
    // If MSG_ABOUT_TOUCH_PANEL_2 has %s, substitute in the toolhead name.
    // But this is optional, so squelch the compiler warning here.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-extra-args"
    char about_str[
      strlen_P(GET_TEXT(MSG_ABOUT_TOUCH_PANEL_2)) +
      strlen_P(TOOLHEAD_NAME) + 1
    ];
    sprintf_P(about_str, GET_TEXT(MSG_ABOUT_TOUCH_PANEL_2), TOOLHEAD_NAME);
    #pragma GCC diagnostic pop
  #else
    char about_str[
      strlen_P(GET_TEXT(MSG_ABOUT_TOUCH_PANEL_2)) + 1
    ];
    strcpy_P(about_str, GET_TEXT(MSG_ABOUT_TOUCH_PANEL_2));
  #endif

  draw_text_box(HEADING_POS,
    #ifdef CUSTOM_MACHINE_NAME
      (const char*)F(CUSTOM_MACHINE_NAME)
    #else
      (const char*)GET_TEXT_F(MSG_ABOUT_TOUCH_PANEL_1)
    #endif
    , OPT_CENTER, font_xlarge
  );
  draw_text_box(FW_VERS_POS, progmem_str(getFirmwareName_str()), OPT_CENTER, font_medium);
  draw_text_box(FW_INFO_POS, about_str, OPT_CENTER, font_medium);
  draw_text_box(INSET_POS(LICENSE_POS), (const char*)GET_TEXT_F(MSG_LICENSE), OPT_CENTER, font_tiny);

  font(font_medium);
  colors(normal_btn);
  tag(2);button(STATS_POS, (const char*)GET_TEXT_F(MSG_INFO_STATS_MENU));
  colors(action_btn);
  tag(1);button(BACK_POS,  (const char*)GET_TEXT_F(MSG_BACK));;

  draw_btn(STATS_POS,(const char*)GET_TEXT_F(MSG_INFO_STATS_MENU),infostat_btn_cb);
  draw_btn(BACK_POS,(const char*)GET_TEXT_F(MSG_BACK),back_btn_cb);
}

bool AboutScreen::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 1: GOTO_PREVIOUS(); break;
    #if ENABLED(PRINTCOUNTER)
      case 2: GOTO_SCREEN(StatisticsScreen); break;
    #endif
    #if ENABLED(TOUCH_UI_DEVELOPER_MENU)
      case 3: GOTO_SCREEN(DeveloperMenu); break;
    #endif
    default: return false;
  }
  return true;
}

#endif // TOUCH_UI
