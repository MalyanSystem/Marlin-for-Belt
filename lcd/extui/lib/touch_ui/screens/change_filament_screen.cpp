/******************************
 * change_filament_screen.cpp *
 ******************************/

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
#include "screen_data.h"
#include "../ui_lib/constants.h"
#include "../ui_lib/commands.h"
#include "string_format.h"

using namespace ExtUI;
using namespace Theme;

#define COOL_TEMP  40
#define LOW_TEMP  180
#define MED_TEMP  200
#define HIGH_TEMP 220

/****************** COLOR SCALE ***********************/

uint32_t getWarmColor(uint16_t temp, uint16_t cool, uint16_t low, uint16_t med, uint16_t high) {
  rgb_t R0, R1, mix;

  float t;
  if (temp < cool) {
    R0 = cool_rgb;
    R1 = low_rgb;
    t  = 0;
  }
  else if (temp < low) {
    R0 = cool_rgb;
    R1 = low_rgb;
    t = (float(temp)-cool)/(low-cool);
  }
  else if (temp < med) {
    R0 = low_rgb;
    R1 = med_rgb;
    t = (float(temp)-low)/(med-low);
  }
  else if (temp < high) {
    R0 = med_rgb;
    R1 = high_rgb;
    t = (float(temp)-med)/(high-med);
  }
  else if (temp >= high) {
    R0 = med_rgb;
    R1 = high_rgb;
    t = 1;
  }
  rgb_t::lerp(t, R0, R1, mix);
  return mix;
}

void ChangeFilamentScreen::drawTempGradient(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  /*cmd.cmd(SCISSOR_XY   (x, y))
     .cmd(SCISSOR_SIZE (w, h/2))
     .gradient         (x, y,     high_rgb, x, y+h/2, med_rgb)
     .cmd(SCISSOR_XY   (x, y+h/2))
     .cmd(SCISSOR_SIZE (w, h/2))
     .gradient         (x, y+h/2, med_rgb,  x, y+h, low_rgb)
     .cmd(SCISSOR_XY   ())
     .cmd(SCISSOR_SIZE ());*/
  printf("drawTempGradient\n");
}

void ChangeFilamentScreen::onEntry() {
  screen_data.ChangeFilamentScreen.e_tag = ExtUI::getActiveTool() + 10;
  screen_data.ChangeFilamentScreen.t_tag = 0;
  screen_data.ChangeFilamentScreen.repeat_tag = 0;
  screen_data.ChangeFilamentScreen.saved_extruder = getActiveTool();
  #if FILAMENT_UNLOAD_PURGE_LENGTH > 0
    screen_data.ChangeFilamentScreen.need_purge = true;
  #endif
}

void ChangeFilamentScreen::onExit() {
  setActiveTool(screen_data.ChangeFilamentScreen.saved_extruder, true);
}

void ChangeFilamentScreen::onRedraw(draw_mode_t what) {

  #ifdef TOUCH_UI_PORTRAIT
    #define GRID_COLS 2
    #define GRID_ROWS 11
  #else
    #define GRID_COLS 4
    #define GRID_ROWS 6
  #endif

  if (what & BACKGROUND) {
    clear_color_rgb(bg_color);
    clear(true,true,true);
    color_rgb(bg_text_enabled);
    tag(0);
    #ifdef TOUCH_UI_PORTRAIT
       font(font_large);
    #else
       font(font_medium);
    #endif
       text(BTN_POS(1,1), BTN_SIZE(2,1), (const char*)GET_TEXT_F(MSG_EXTRUDER_SELECTION));
    #ifdef TOUCH_UI_PORTRAIT
       text(BTN_POS(1,7), BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_CURRENT_TEMPERATURE));
    #else
       text(BTN_POS(3,1), BTN_SIZE(2,1), (const char*)GET_TEXT_F(MSG_CURRENT_TEMPERATURE));
       font(font_small);
    #endif
       text(BTN_POS(1,3), BTN_SIZE(2,1), (const char*)GET_TEXT_F(MSG_REMOVAL_TEMPERATURE));
    drawTempGradient(BTN_POS(1,4), BTN_SIZE(1,3));
  }

  if (what & FOREGROUND) {
    const extruder_t e = getExtruder();

    char e_str[15];
    if (isHeaterIdle(e))
      format_temp_and_idle(e_str, getActualTemp_celsius(e));
    else
      format_temp_and_temp(e_str, getActualTemp_celsius(e), getTargetTemp_celsius(e));

    const rgb_t tcol = getWarmColor(getActualTemp_celsius(e), COOL_TEMP, LOW_TEMP, MED_TEMP, HIGH_TEMP);
    color_rgb(tcol);
       tag(15);
    #ifdef TOUCH_UI_PORTRAIT
       rectangle(BTN_POS(2,7), BTN_SIZE(1,1));
    #else
       rectangle(BTN_POS(3,2), BTN_SIZE(2,1));
    #endif
       color_rgb(tcol.luminance() > 128 ? 0x000000 : 0xFFFFFF);
       font(font_medium);
    #ifdef TOUCH_UI_PORTRAIT
       text(BTN_POS(2,7), BTN_SIZE(1,1), e_str);
    #else
       text(BTN_POS(3,2), BTN_SIZE(2,1), e_str);
    #endif
       colors(normal_btn);

    const bool t_ok = getActualTemp_celsius(e) > getSoftenTemp() - 10;

    if (screen_data.ChangeFilamentScreen.t_tag && !t_ok) {
      text(BTN_POS(1,6), BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_HEATING));
    } else if (getActualTemp_celsius(e) > 100) {
      color_rgb(0xFF0000);
         text(BTN_POS(1,4), BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_CAUTION));
         colors(normal_btn);
         text(BTN_POS(1,6), BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_HOT));
    }

    #define TOG_STYLE(A) colors(A ? action_btn : normal_btn)

    const bool tog2  = screen_data.ChangeFilamentScreen.t_tag == 2;
    const bool tog3  = screen_data.ChangeFilamentScreen.t_tag == 3;
    const bool tog4  = screen_data.ChangeFilamentScreen.t_tag == 4;
    const bool tog10 = screen_data.ChangeFilamentScreen.e_tag == 10;
    #if HOTENDS > 1
    const bool tog11 = screen_data.ChangeFilamentScreen.e_tag == 11;
    #endif

    #ifdef TOUCH_UI_PORTRAIT
      font(font_large);
    #else
      font(font_medium);
    #endif
       TOG_STYLE(tog10);
       tag(10);button (BTN_POS(1,2), BTN_SIZE(1,1), (const char*)F("1"));
    #if HOTENDS < 2
       enabled(false);
    #else
       TOG_STYLE(tog11);
    #endif
       tag(11);button (BTN_POS(2,2), BTN_SIZE(1,1), (const char*)F("2"));

    if (!t_ok) reset_menu_timeout();

    const bool tog7 = screen_data.ChangeFilamentScreen.repeat_tag == 7;
    const bool tog8 = screen_data.ChangeFilamentScreen.repeat_tag == 8;

    font(
      #ifdef TOUCH_UI_PORTRAIT
        font_large
      #else
        font_small
      #endif
    );

    {
      char str[30];

      format_temp_and_material(str, LOW_TEMP, GET_TEXT(MSG_MATERIAL_PLA));
      tag(2);TOG_STYLE(tog2);button (BTN_POS(2,6), BTN_SIZE(1,1), str);

      format_temp_and_material(str, MED_TEMP, GET_TEXT(MSG_MATERIAL_ABS));
      tag(3);TOG_STYLE(tog3);button (BTN_POS(2,5), BTN_SIZE(1,1), str);

      format_temp_and_material(str, HIGH_TEMP, GET_TEXT(MSG_MATERIAL_HIGH_TEMP));
      tag(4);TOG_STYLE(tog4);button (BTN_POS(2,4), BTN_SIZE(1,1), str);
    }
    colors(normal_btn);

    // Add tags to color gradient
    color_mask(0,0,0,0);
    tag(2);rectangle(BTN_POS(1,6), BTN_SIZE(1,1));
    tag(3);rectangle(BTN_POS(1,5), BTN_SIZE(1,1));
    tag(4);rectangle(BTN_POS(1,4), BTN_SIZE(1,1));
    color_mask(1,1,1,1);

    color_rgb(t_ok ? bg_text_enabled : bg_text_disabled);
    #ifdef TOUCH_UI_PORTRAIT
       font(font_large);
       tag(0);text   (BTN_POS(1,8),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_UNLOAD_FILAMENT));
       text   (BTN_POS(2,8),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_LOAD_FILAMENT));
       tag(5);enabled(t_ok);button (BTN_POS(1,9),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_MOMENTARY));
       tag(6);enabled(t_ok);button (BTN_POS(2,9),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_MOMENTARY));
       tag(7);TOG_STYLE(tog7);enabled(t_ok);button (BTN_POS(1,10), BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_CONTINUOUS));
       tag(8);TOG_STYLE(tog8);enabled(t_ok);button (BTN_POS(2,10), BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_CONTINUOUS));
       tag(1);colors(action_btn)           ;button (BTN_POS(1,11), BTN_SIZE(2,1), (const char*)GET_TEXT_F(MSG_BACK));
    #else
       font(font_small);
       tag(0)                              ;text   (BTN_POS(3,3),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_UNLOAD_FILAMENT));
                                           ;text   (BTN_POS(4,3),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_LOAD_FILAMENT));
       tag(5)                ;enabled(t_ok);button (BTN_POS(3,4),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_MOMENTARY));
       tag(6)                ;enabled(t_ok);button (BTN_POS(4,4),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_MOMENTARY));
       tag(7);TOG_STYLE(tog7);enabled(t_ok);button (BTN_POS(3,5),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_CONTINUOUS));
       tag(8);TOG_STYLE(tog8);enabled(t_ok);button (BTN_POS(4,5),  BTN_SIZE(1,1), (const char*)GET_TEXT_F(MSG_CONTINUOUS));
       font(font_medium);
       tag(1);colors(action_btn)           ;button (BTN_POS(3,6),  BTN_SIZE(2,1), (const char*)GET_TEXT_F(MSG_BACK));
    #endif
  }
  #undef GRID_COLS
  #undef GRID_ROWS
}

uint8_t ChangeFilamentScreen::getSoftenTemp() {
  switch (screen_data.ChangeFilamentScreen.t_tag) {
    case 2:  return LOW_TEMP;
    case 3:  return MED_TEMP;
    case 4:  return HIGH_TEMP;
    default: return EXTRUDE_MINTEMP;
  }
}

ExtUI::extruder_t ChangeFilamentScreen::getExtruder() {
  switch (screen_data.ChangeFilamentScreen.e_tag) {
    case 13: return ExtUI::E3;
    case 12: return ExtUI::E2;
    case 11: return ExtUI::E1;
    default: return ExtUI::E0;
  }
}

void ChangeFilamentScreen::doPurge() {
  #if FILAMENT_UNLOAD_PURGE_LENGTH > 0
    constexpr float purge_distance_mm = FILAMENT_UNLOAD_PURGE_LENGTH;
    if (screen_data.ChangeFilamentScreen.need_purge) {
      screen_data.ChangeFilamentScreen.need_purge = false;
      MoveAxisScreen::setManualFeedrate(getExtruder(), purge_distance_mm);
      ExtUI::setAxisPosition_mm(ExtUI::getAxisPosition_mm(getExtruder()) + purge_distance_mm, getExtruder());
    }
  #endif
}

bool ChangeFilamentScreen::onTouchStart(uint8_t tag) {
  // Make the Momentary and Continuous buttons slightly more responsive
  switch (tag) {
    case 5: case 6: case 7: case 8:
      #if FILAMENT_UNLOAD_PURGE_LENGTH > 0
        if (tag == 5 || tag == 7) doPurge();
      #endif
      return ChangeFilamentScreen::onTouchHeld(tag);
    default:
      return false;
  }
}

bool ChangeFilamentScreen::onTouchEnd(uint8_t tag) {
  using namespace ExtUI;
  switch (tag) {
    case 1: GOTO_PREVIOUS(); break;
    case 2:
    case 3:
    case 4:
      // Change temperature
      screen_data.ChangeFilamentScreen.t_tag = tag;
      setTargetTemp_celsius(getSoftenTemp(), getExtruder());
      break;
    case 7:
      screen_data.ChangeFilamentScreen.repeat_tag = (screen_data.ChangeFilamentScreen.repeat_tag == 7) ? 0 : 7;
      break;
    case 8:
      screen_data.ChangeFilamentScreen.repeat_tag = (screen_data.ChangeFilamentScreen.repeat_tag == 8) ? 0 : 8;
      break;
    case 10:
    case 11:
      // Change extruder
      screen_data.ChangeFilamentScreen.e_tag      = tag;
      screen_data.ChangeFilamentScreen.t_tag      = 0;
      screen_data.ChangeFilamentScreen.repeat_tag = 0;
      #if FILAMENT_UNLOAD_PURGE_LENGTH > 0
        screen_data.ChangeFilamentScreen.need_purge = true;
      #endif
      setActiveTool(getExtruder(), true);
      break;
    case 15: GOTO_SCREEN(TemperatureScreen); break;
  }
  return true;
}

bool ChangeFilamentScreen::onTouchHeld(uint8_t tag) {
  if (ExtUI::isMoving()) return false; // Don't allow moves to accumulate
  constexpr float increment = 1;
  #define UI_INCREMENT_AXIS(axis) UI_INCREMENT(AxisPosition_mm, axis);
  #define UI_DECREMENT_AXIS(axis) UI_DECREMENT(AxisPosition_mm, axis);
  switch (tag) {
    case 5: case 7: UI_DECREMENT_AXIS(getExtruder()); break;
    case 6: case 8: UI_INCREMENT_AXIS(getExtruder()); break;
    default: return false;
  }
  #undef UI_DECREMENT_AXIS
  #undef UI_INCREMENT_AXIS
  return false;
}

void ChangeFilamentScreen::onIdle() {
  reset_menu_timeout();
  if (screen_data.ChangeFilamentScreen.repeat_tag) onTouchHeld(screen_data.ChangeFilamentScreen.repeat_tag);
  if (refresh_timer.elapsed(STATUS_UPDATE_INTERVAL)) {
    onRefresh();
    refresh_timer.start();
  }
  BaseScreen::onIdle();
}

#endif // TOUCH_UI
