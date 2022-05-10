/*****************
 * main_menu.cpp *
 *****************/

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

#if ENABLED(TOUCH_UI) && !defined(TOUCH_UI_LULZBOT_BIO)

#include "screens.h"
#include "../ui_lib/constants.h"
#include "../ui_lib/commands.h"

using namespace ExtUI;
using namespace Theme;

void back_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) SaveSettingsDialogBox::promptToSaveSettings();
}
void autohome_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) SpinnerDialogBox::enqueueAndWait_P(F("G28"));
}
void move_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) GOTO_SCREEN(MoveAxisScreen);
}
void dis_motor_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) GOTO_SCREEN(MoveAxisScreen);
}
void temp_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) GOTO_SCREEN(TemperatureScreen);
}
void level_btn_cb(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_CLICKED)
  {
  #ifdef HAS_LEVELING
    SpinnerDialogBox::enqueueAndWait_P(F(
      #ifdef BED_LEVELING_COMMANDS
        BED_LEVELING_COMMANDS
      #else
        "G29"
      #endif
    ));
    #endif
  }
}
void about_btn_cb(lv_obj_t * obj, lv_event_t event)
{
  //printf("about_btn_cb EVENT=%d\n",event);
  if(event == LV_EVENT_CLICKED)
  {
    //printf("about_btn_cb event == LV_EVENT_CLICKED\n");
    GOTO_SCREEN(AboutScreen);
  }
  //printf("about_btn_cb exit\n");
}
void clean_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
    #if ENABLED(TOUCH_UI_COCOA_PRESS)
      GOTO_SCREEN(PreheatMenu);
    #elif ENABLED(NOZZLE_CLEAN_FEATURE)
      injectCommands_P(PSTR("G12"));
      GOTO_SCREEN(StatusScreen);
    #endif
    }
}
void refill_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
    #if ENABLED(TOUCH_UI_COCOA_PRESS) && HAS_CASE_LIGHT
      GOTO_SCREEN(CaseLightScreen);
    #else
      GOTO_SCREEN(ChangeFilamentScreen);
    #endif
    }
}
void level_x_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    #ifdef AXIS_LEVELING_COMMANDS
      if(event == LV_EVENT_CLICKED) SpinnerDialogBox::enqueueAndWait_P(F(AXIS_LEVELING_COMMANDS));
    #endif
}
void adv_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) GOTO_SCREEN(AdvancedSettingsMenu);
}

void MainMenu::onRedraw(draw_mode_t what) {
  if (what & BACKGROUND) {
    //clear_color_rgb(Theme::bg_color);
    //clear(true,true,true);
  }
  lv_obj_clean(lv_scr_act());

  #ifdef TOUCH_UI_PORTRAIT
    #define GRID_ROWS 8
    #define GRID_COLS 2
    #define ABOUT_PRINTER_POS     BTN_POS(1,1), BTN_SIZE(2,1)
    #define ADVANCED_SETTINGS_POS BTN_POS(1,2), BTN_SIZE(2,1)
    #define FILAMENTCHANGE_POS    BTN_POS(1,3), BTN_SIZE(2,1)
    #define TEMPERATURE_POS       BTN_POS(1,4), BTN_SIZE(2,1)
    #define MOVE_AXIS_POS         BTN_POS(1,5), BTN_SIZE(1,1)
    #define DISABLE_STEPPERS_POS  BTN_POS(2,5), BTN_SIZE(1,1)
    #define AUTO_HOME_POS         BTN_POS(1,6), BTN_SIZE(1,1)
    #define CLEAN_NOZZLE_POS      BTN_POS(2,6), BTN_SIZE(1,1)
    #define LEVEL_BED_POS         BTN_POS(1,7), BTN_SIZE(1,1)
    #define LEVEL_AXIS_POS        BTN_POS(2,7), BTN_SIZE(1,1)
    #define BACK_POS              BTN_POS(1,8), BTN_SIZE(2,1)
  #else
    #define GRID_ROWS 6
    #define GRID_COLS 2
    #define ADVANCED_SETTINGS_POS BTN_POS(1,1), BTN_SIZE(1,1)
    #define ABOUT_PRINTER_POS     BTN_POS(2,1), BTN_SIZE(1,1)
    #define AUTO_HOME_POS         BTN_POS(1,2), BTN_SIZE(1,1)
    #define CLEAN_NOZZLE_POS      BTN_POS(2,2), BTN_SIZE(1,1)
    #define MOVE_AXIS_POS         BTN_POS(1,3), BTN_SIZE(1,1)
    #define DISABLE_STEPPERS_POS  BTN_POS(2,3), BTN_SIZE(1,1)
    #define TEMPERATURE_POS       BTN_POS(1,4), BTN_SIZE(1,1)
    #define FILAMENTCHANGE_POS    BTN_POS(2,4), BTN_SIZE(1,1)
    #define LEVEL_BED_POS         BTN_POS(1,5), BTN_SIZE(1,1)
    #define LEVEL_AXIS_POS        BTN_POS(2,5), BTN_SIZE(1,1)
    #define BACK_POS              BTN_POS(1,6), BTN_SIZE(2,1)
  #endif

  if (what & FOREGROUND) {
    /*colors(normal_btn);
    font(Theme::font_medium);
    tag(2);button( AUTO_HOME_POS, (const char*)GET_TEXT_F(MSG_AUTO_HOME));*/
    draw_btn(AUTO_HOME_POS,(const char*)GET_TEXT_F(MSG_AUTO_HOME),autohome_btn_cb);
    enabled(
      #if ANY(NOZZLE_CLEAN_FEATURE, TOUCH_UI_COCOA_PRESS)
        1
      #endif
    );
    /*tag(3);button( CLEAN_NOZZLE_POS, (const char*)GET_TEXT_F(
      #if ENABLED(TOUCH_UI_COCOA_PRESS)
        MSG_PREHEAT_1
      #else
        MSG_CLEAN_NOZZLE
      #endif
    ));*/
    draw_btn(CLEAN_NOZZLE_POS,(const char*)GET_TEXT_F(
      #if ENABLED(TOUCH_UI_COCOA_PRESS)
        MSG_PREHEAT_1
      #else
        MSG_CLEAN_NOZZLE
      #endif
    ),NULL);
    //tag(4);button( MOVE_AXIS_POS,        (const char*)GET_TEXT_F(MSG_MOVE_AXIS));
    draw_btn(MOVE_AXIS_POS,(const char*)GET_TEXT_F(MSG_MOVE_AXIS),move_btn_cb);
    //tag(5);button( DISABLE_STEPPERS_POS, (const char*)GET_TEXT_F(MSG_DISABLE_STEPPERS));
    draw_btn(DISABLE_STEPPERS_POS,(const char*)GET_TEXT_F(MSG_DISABLE_STEPPERS),dis_motor_btn_cb);
    //tag(6);button( TEMPERATURE_POS,      (const char*)GET_TEXT_F(MSG_TEMPERATURE));
    draw_btn(TEMPERATURE_POS,(const char*)GET_TEXT_F(MSG_TEMPERATURE),temp_btn_cb);
    enabled(
      #if DISABLED(TOUCH_UI_LULZBOT_BIO)
        1
      #endif
    );
    /*tag(7);button( FILAMENTCHANGE_POS, (const char*)GET_TEXT_F(
      #if ENABLED(TOUCH_UI_COCOA_PRESS)
          MSG_CASE_LIGHT
      #else
          MSG_FILAMENTCHANGE
      #endif
    ));*/
    draw_btn(FILAMENTCHANGE_POS,(const char*)GET_TEXT_F(
      #if ENABLED(TOUCH_UI_COCOA_PRESS)
          MSG_CASE_LIGHT
      #else
          MSG_FILAMENTCHANGE
      #endif
    ),refill_btn_cb);
    //tag(8);button( ADVANCED_SETTINGS_POS, (const char*)GET_TEXT_F(MSG_ADVANCED_SETTINGS));
    draw_btn(ADVANCED_SETTINGS_POS,(const char*)GET_TEXT_F(MSG_ADVANCED_SETTINGS),adv_btn_cb);
    enabled(
      #ifdef PRINTCOUNTER
        1
      #endif
    );
    enabled(
      #ifdef AXIS_LEVELING_COMMANDS
        1
      #endif
    );
    //tag(9);button( LEVEL_AXIS_POS, (const char*)GET_TEXT_F(MSG_AUTOLEVEL_X_AXIS));
    draw_btn(LEVEL_AXIS_POS,(const char*)GET_TEXT_F(MSG_AUTOLEVEL_X_AXIS),level_x_btn_cb);
    enabled(
      #ifdef HAS_LEVELING
        1
      #endif
    );
    //tag(10);button( LEVEL_BED_POS, (const char*)GET_TEXT_F(MSG_LEVEL_BED));
    draw_btn(LEVEL_BED_POS,(const char*)GET_TEXT_F(MSG_LEVEL_BED),level_btn_cb);
    //tag(11);button( ABOUT_PRINTER_POS, (const char*)GET_TEXT_F(MSG_INFO_MENU));
    draw_btn(ABOUT_PRINTER_POS,(const char*)GET_TEXT_F(MSG_INFO_MENU),about_btn_cb);
    //colors(action_btn);
    //tag(1);button( BACK_POS, (const char*)GET_TEXT_F(MSG_BACK));
    draw_btn(BACK_POS,(const char*)GET_TEXT_F(MSG_BACK),back_btn_cb);
  }
}

bool MainMenu::onTouchEnd(uint8_t tag) {
  using namespace ExtUI;

  switch (tag) {
    case 1:  SaveSettingsDialogBox::promptToSaveSettings();           break;
    case 2:  SpinnerDialogBox::enqueueAndWait_P(F("G28"));            break;
    #if ENABLED(TOUCH_UI_COCOA_PRESS)
    case 3:  GOTO_SCREEN(PreheatMenu);                                break;
    #elif ENABLED(NOZZLE_CLEAN_FEATURE)
    case 3: injectCommands_P(PSTR("G12")); GOTO_SCREEN(StatusScreen); break;
    #endif
    case 4:  GOTO_SCREEN(MoveAxisScreen);                             break;
    case 5:  injectCommands_P(PSTR("M84"));                           break;
    case 6:  GOTO_SCREEN(TemperatureScreen);                          break;
    #if ENABLED(TOUCH_UI_COCOA_PRESS) && HAS_CASE_LIGHT
    case 7:  GOTO_SCREEN(CaseLightScreen);                            break;
    #else
    case 7:  GOTO_SCREEN(ChangeFilamentScreen);                       break;
    #endif
    case 8:  GOTO_SCREEN(AdvancedSettingsMenu);                       break;
    #ifdef AXIS_LEVELING_COMMANDS
    case 9: SpinnerDialogBox::enqueueAndWait_P(F(AXIS_LEVELING_COMMANDS)); break;
    #endif
    #ifdef HAS_LEVELING
    case 10:  SpinnerDialogBox::enqueueAndWait_P(F(
      #ifdef BED_LEVELING_COMMANDS
        BED_LEVELING_COMMANDS
      #else
        "G29"
      #endif
    ));            break;
    #endif
    case 11: GOTO_SCREEN(AboutScreen);                                break;
    default:
      return false;
  }
  return true;
}

#endif // TOUCH_UI
