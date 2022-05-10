/*********************
 * status_screen.cpp *
 *********************/

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

#if ENABLED(TOUCH_UI) && NONE(TOUCH_UI_LULZBOT_BIO, TOUCH_UI_COCOA_PRESS)

#include "screens.h"
#include "screen_data.h"
#include "../ui_lib/constants.h"
#include "../ui_lib/commands.h"
#include "string_format.h"

using namespace ExtUI;
using namespace Theme;

#ifdef TOUCH_UI_PORTRAIT
    #define GRID_ROWS 8
#else
    #define GRID_ROWS 8
#endif

lv_obj_t * label_e0;
lv_obj_t * label_e1;
lv_obj_t * label_bed;
lv_obj_t * label_fan;
lv_obj_t * status_message;
lv_obj_t * label_time;
lv_obj_t * label_progress;

static char _x_str[15],_y_str[15],_z_str[15];
void StatusScreen::draw_axis_position() {

  #define GRID_COLS 3

  #ifdef TOUCH_UI_PORTRAIT
    #define X_LBL_POS  BTN_POS(1,5), BTN_SIZE(1,1)
    #define Y_LBL_POS  BTN_POS(1,6), BTN_SIZE(1,1)
    #define Z_LBL_POS  BTN_POS(1,7), BTN_SIZE(1,1)
    #define X_VAL_POS  BTN_POS(2,5), BTN_SIZE(2,1)
    #define Y_VAL_POS  BTN_POS(2,6), BTN_SIZE(2,1)
    #define Z_VAL_POS  BTN_POS(2,7), BTN_SIZE(2,1)
  #else
    #define X_LBL_POS  BTN_POS(1,5), BTN_SIZE(1,1)
    #define Y_LBL_POS  BTN_POS(2,5), BTN_SIZE(1,1)
    #define Z_LBL_POS  BTN_POS(3,5), BTN_SIZE(1,1)
    #define X_VAL_POS  BTN_POS(1,6), BTN_SIZE(1,1)
    #define Y_VAL_POS  BTN_POS(2,6), BTN_SIZE(1,1)
    #define Z_VAL_POS  BTN_POS(3,6), BTN_SIZE(1,1)
  #endif

  #define _UNION_POS(x1,y1,w1,h1,x2,y2,w2,h2) x1,y1,max(x1+w1,x2+w2)-x1,max(y1+h1,y2+h2)-y1
  #define UNION_POS(p1, p2) _UNION_POS(p1, p2)

  /*if (what & BACKGROUND) {
    tag(6);
    fgcolor(Theme::axis_label);
    font(Theme::font_large);
    button( UNION_POS(X_LBL_POS, X_VAL_POS), (const char*)F(""), OPT_FLAT);
    button( UNION_POS(Y_LBL_POS, Y_VAL_POS), (const char*)F(""), OPT_FLAT);
    button( UNION_POS(Z_LBL_POS, Z_VAL_POS), (const char*)F(""), OPT_FLAT);
    font(Theme::font_medium);
    fgcolor(Theme::x_axis);button( X_VAL_POS, (const char*)F(""), OPT_FLAT);
    fgcolor(Theme::y_axis);button( Y_VAL_POS, (const char*)F(""), OPT_FLAT);
    fgcolor(Theme::z_axis);button( Z_VAL_POS, (const char*)F(""), OPT_FLAT);
    font(Theme::font_small);
    text  ( X_LBL_POS, (const char*)GET_TEXT_F(MSG_AXIS_X), OPT_CENTER);
    text  ( Y_LBL_POS, (const char*)GET_TEXT_F(MSG_AXIS_Y), OPT_CENTER);
    text  ( Z_LBL_POS, (const char*)GET_TEXT_F(MSG_AXIS_Z), OPT_CENTER);
    colors(normal_btn);
  }*/
  {
    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(1,5));
    lv_obj_set_width(label1, BTN_W(1));
    lv_label_set_text(label1, (const char*)GET_TEXT_F(MSG_AXIS_X));

    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label2, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(2,5));
    lv_obj_set_width(label2, BTN_W(1));
    lv_label_set_text(label2, (const char*)GET_TEXT_F(MSG_AXIS_Y));

    lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label3, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(3,5));
    lv_obj_set_width(label3, BTN_W(1));
    lv_label_set_text(label3, (const char*)GET_TEXT_F(MSG_AXIS_Z));
  }

  {
    using namespace ExtUI;
    char x_str[15];
    char y_str[15];
    char z_str[15];

    if (isAxisPositionKnown(X))
      format_position(x_str, getAxisPosition_mm(X));
    else
      strcpy_P(x_str, PSTR("?"));

    if (isAxisPositionKnown(Y))
      format_position(y_str, getAxisPosition_mm(Y));
    else
      strcpy_P(y_str, PSTR("?"));

    if (isAxisPositionKnown(Z))
      format_position(z_str, getAxisPosition_mm(Z));
    else
      strcpy_P(z_str, PSTR("?"));

    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(1,6));
    lv_obj_set_width(label1, BTN_W(1));
    lv_label_set_text(label1, x_str);

    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label2, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(2,6));
    lv_obj_set_width(label2, BTN_W(1));
    lv_label_set_text(label2, y_str);

    lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label3, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(3,6));
    lv_obj_set_width(label3, BTN_W(1));
    lv_label_set_text(label3, z_str);

    strncpy(_x_str,x_str,15);
    strncpy(_y_str,y_str,15);
    strncpy(_z_str,z_str,15);
  }

  #undef GRID_COLS
}

#ifdef TOUCH_UI_PORTRAIT
  #define GRID_COLS 8
#else
  #define GRID_COLS 12
#endif

static char _e0_str[20],_e1_str[20],_bed_str[20],_fan_str[20];
LV_IMG_DECLARE(Extruder_Icon);
LV_IMG_DECLARE(Bed_Heat_Icon);
LV_IMG_DECLARE(Fan_Icon);

void StatusScreen::draw_temperature() {
  using namespace Theme;

  #define TEMP_RECT_1 BTN_POS(1,1), BTN_SIZE(4,2)
  #define TEMP_RECT_2 BTN_POS(1,1), BTN_SIZE(8,1)
  #define NOZ_1_POS   BTN_POS(1,1), BTN_SIZE(4,1)
  #define NOZ_2_POS   BTN_POS(5,1), BTN_SIZE(4,1)
  #define BED_POS     BTN_POS(1,2), BTN_SIZE(4,1)
  #define FAN_POS     BTN_POS(5,2), BTN_SIZE(4,1)

  #define _ICON_POS(x,y,w,h) x, y, w/4, h
  #define _TEXT_POS(x,y,w,h) x + w/4, y, w - w/4, h
  #define ICON_POS(pos) _ICON_POS(pos)
  #define TEXT_POS(pos) _TEXT_POS(pos)

  lv_obj_t *img1 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img1, &Extruder_Icon);
  lv_img_set_zoom(img1, ICON_SCALE);
  lv_obj_align(img1, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(1,1));

  lv_obj_t *img2 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img2, &Extruder_Icon);
  lv_img_set_zoom(img2, ICON_SCALE);
  lv_obj_align(img2, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(5,1));

  lv_obj_t *img3 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img3, &Bed_Heat_Icon);
  lv_img_set_zoom(img3, ICON_SCALE);
  lv_obj_align(img3, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(1,2));

  lv_obj_t *img4 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img4, &Fan_Icon);
  lv_img_set_zoom(img4, ICON_SCALE);
  lv_obj_align(img4, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(5,2));
  
  {
    using namespace ExtUI;

    /*tag(5);
    font(font_medium);
    update_text(TEXT_POS(NOZ_1_POS), e0_str,_e0_str);
    #if EXTRUDERS == 2
    update_text(TEXT_POS(NOZ_2_POS), e1_str,_e1_str);
    #endif
    update_text(TEXT_POS(BED_POS), bed_str,_bed_str);
    update_text(TEXT_POS(FAN_POS), fan_str,_fan_str);*/

    /*static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_16);*/

    label_e0 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label_e0, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(2,1));
    lv_obj_set_width(label_e0, BTN_W(3));
    //lv_label_set_text(label_e0, e0_str);

    label_e1 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label_e1, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(6,1));
    lv_obj_set_width(label_e1, BTN_W(3));
    //lv_label_set_text(label_e1, e1_str);

    label_bed = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label_bed, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(2,2));
    lv_obj_set_width(label_bed, BTN_W(3));
    //lv_label_set_text(label_bed, bed_str);

    label_fan = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label_fan, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(6,2));
    lv_obj_set_width(label_fan, BTN_W(3));
    //lv_label_set_text(label_fan, fan_str);

  }
}

static char _time_str[10],_progress_str[10];
void StatusScreen::draw_progress() {
  using namespace ExtUI;
  using namespace Theme;

  #if ENABLED(TOUCH_UI_PORTRAIT)
    #define TIME_POS     BTN_POS(1,3), BTN_SIZE(4,1)
    #define PROGRESS_POS BTN_POS(5,3), BTN_SIZE(4,1)
  #else
    #define TIME_POS     BTN_POS(9,1), BTN_SIZE(4,1)
    #define PROGRESS_POS BTN_POS(9,2), BTN_SIZE(4,1)
  #endif

  /*if (what & BACKGROUND) {
    tag(0);
    font(font_medium);
    fgcolor(progress);button(TIME_POS, (const char*)F(""), OPT_FLAT);
    button(PROGRESS_POS, (const char*)F(""), OPT_FLAT);
  }*/

  {
    label_time = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label_time, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(9,1));
    lv_obj_set_width(label_time, BTN_W(4));
    //lv_label_set_text(label_time, time_str);

    label_progress = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label_progress, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(9,2));
    lv_obj_set_width(label_progress, BTN_W(4));
    //lv_label_set_text(label2, progress_str);
  }
}

#undef GRID_COLS

static bool _has_media;
#define has_media true

//LV_EVENT_CB_DECLARE(media_btn_cb)
static void media_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) GOTO_SCREEN(FilesScreen);
}
static void menu_btn_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED){
      if (isPrinting()) {
        GOTO_SCREEN(TuneMenu);
      } else {
        GOTO_SCREEN(MainMenu);
      }
    }
}

void StatusScreen::draw_interaction_buttons() {
  #define GRID_COLS 4
  {
    using namespace ExtUI;

  #if ENABLED(TOUCH_UI_PORTRAIT)
    #define MEDIA_BTN_POS  BTN_POS(1,8), BTN_SIZE(2,1)
    #define MENU_BTN_POS   BTN_POS(3,8), BTN_SIZE(2,1)
  #else
    #define MEDIA_BTN_POS  BTN_POS(1,7), BTN_SIZE(2,2)
    #define MENU_BTN_POS   BTN_POS(3,7), BTN_SIZE(2,2)
  #endif

    //const bool has_media = isMediaInserted() && !isPrintingFromMedia();
    if (_has_media != has_media)
    {
      _has_media=has_media;

      lv_obj_t * label;
      lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
      lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(1,7));
      label = lv_label_create(btn1, NULL);
      lv_label_set_text(label, isPrintingFromMedia() ? (const char*)GET_TEXT_F(MSG_PRINTING) : (const char*)GET_TEXT_F(MSG_BUTTON_MEDIA));
      lv_obj_set_width(btn1,BTN_W(2));
      lv_obj_set_height(btn1,BTN_H(2));
      lv_obj_set_event_cb(btn1, media_btn_cb);

      lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
      lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(3,7));
      label = lv_label_create(btn2, NULL);
      lv_label_set_text(label, (const char*)GET_TEXT_F(MSG_BUTTON_MENU));
      lv_obj_set_width(btn2,BTN_W(2));
      lv_obj_set_height(btn2,BTN_H(2));
      lv_obj_set_event_cb(btn2, menu_btn_cb);

      /*colors(normal_btn);
      font(Theme::font_medium);
      enabled(has_media);
      colors(has_media ? action_btn : normal_btn);

      tag(3);
      button(MEDIA_BTN_POS, isPrintingFromMedia() ? (const char*)GET_TEXT_F(MSG_PRINTING) : (const char*)GET_TEXT_F(MSG_BUTTON_MEDIA));
      colors(!has_media ? action_btn : normal_btn);

      tag(4);
      button( MENU_BTN_POS, (const char*)GET_TEXT_F(MSG_BUTTON_MENU));*/

    }
  }
  #undef  GRID_COLS
}

char buff[32];
void StatusScreen::draw_status_message() {
  #define GRID_COLS 1

  #if ENABLED(TOUCH_UI_PORTRAIT)
    #define STATUS_POS  BTN_POS(1,4), BTN_SIZE(1,1)
  #else
    #define STATUS_POS  BTN_POS(1,3), BTN_SIZE(1,2)
  #endif

  /*if (what & BACKGROUND) {
    fgcolor(Theme::status_msg);
    tag(0);
    button( STATUS_POS, (const char*)F(""), OPT_FLAT);

    draw_text_box(STATUS_POS, message, OPT_CENTER, font_large);
  }*/
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_48);
  
  status_message = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(status_message, LV_LABEL_PART_MAIN, &style);
  lv_obj_set_height(status_message, BTN_H(1));
  lv_obj_set_width(status_message, BTN_W(2));
  lv_obj_align(status_message, NULL, LV_ALIGN_IN_TOP_LEFT, BTN_POS(1,3));
  lv_label_set_text(status_message, buff);
  #undef  GRID_COLS
}

void StatusScreen::setStatusMessage(progmem_str message) {
  //char buff[strlen_P((const char * const)message)+1];
  strcpy_P(buff, (const char * const) message);
  setStatusMessage((const char *) buff);
}

void StatusScreen::setStatusMessage(const char* message) {

  /*cmd_dlstart();
  clear_color_rgb(Theme::bg_color);
  clear(true,true,true);

  draw_temperature(BACKGROUND);
  draw_status_message(BACKGROUND, message);
  draw_interaction_buttons(BACKGROUND);
  draw_progress(BACKGROUND);
  draw_axis_position(BACKGROUND);
*/
  //storeBackground();
  
  #if ENABLED(TOUCH_UI_DEBUG)
    SERIAL_ECHO_START();
    SERIAL_ECHOLNPAIR("New status message: ", message);
  #endif

  if (AT_SCREEN(StatusScreen)) {
    current_screen.onRefresh();
    lv_label_set_text(status_message, message);
  }
}

void StatusScreen::onStartup() {
  //UIFlashStorage::initialize();
}

void StatusScreen::onRedraw(draw_mode_t what) {
  char e0_str[20];
  char e1_str[20];
  char bed_str[20];
  char fan_str[20];

  sprintf_P(
    fan_str,
    PSTR("%-3d %%"),
    int8_t(getActualFan_percent(FAN0))
  );

  if (isHeaterIdle(BED))
    format_temp_and_idle(bed_str, getActualTemp_celsius(BED));
  else
    format_temp_and_temp(bed_str, getActualTemp_celsius(BED), getTargetTemp_celsius(BED));

  if (isHeaterIdle(H0))
    format_temp_and_idle(e0_str, getActualTemp_celsius(H0));
  else
    format_temp_and_temp(e0_str, getActualTemp_celsius(H0), getTargetTemp_celsius(H0));


  #if EXTRUDERS == 2
    if (isHeaterIdle(H1))
      format_temp_and_idle(e1_str, getActualTemp_celsius(H1));
    else
      format_temp_and_temp(e1_str, getActualTemp_celsius(H1), getTargetTemp_celsius(H1));
  #else
    strcpy_P(
      e1_str,
      PSTR("-")
    );
  #endif

  lv_label_set_text(label_e0, e0_str);
  lv_label_set_text(label_e1, e1_str);
  lv_label_set_text(label_bed, bed_str);
  lv_label_set_text(label_fan, fan_str);

  strncpy(_e0_str,e0_str,20);
  strncpy(_e1_str,e1_str,20);
  strncpy(_bed_str,bed_str,20);
  strncpy(_fan_str,fan_str,20);

  const uint32_t elapsed = getProgress_seconds_elapsed();
  const uint8_t hrs = elapsed/3600;
  const uint8_t min = (elapsed/60)%60;

  char time_str[10];
  char progress_str[10];

  sprintf_P(time_str,     PSTR(" %02d : %02d"), hrs, min);
  sprintf_P(progress_str, PSTR("%-3d %%"),      getProgress_percent() );

  lv_label_set_text(label_time, time_str);
  lv_label_set_text(label_progress, progress_str);

  strncpy(_time_str,time_str,10);
  strncpy(_progress_str,progress_str,10);
}

void StatusScreen::onEntry() {
  lv_obj_clean(lv_scr_act());

  ZERO(_e0_str);
  ZERO(_e1_str);
  ZERO(_bed_str);
  ZERO(_fan_str);

  ZERO(_time_str);
  ZERO(_progress_str);

  ZERO(_x_str);
  ZERO(_y_str);
  ZERO(_z_str);
  _has_media=false;

  draw_temperature();
  draw_progress();
  draw_axis_position();
  draw_status_message();
  draw_interaction_buttons();
  onRefresh();
}

void StatusScreen::onIdle() {
  if (refresh_timer.elapsed(STATUS_UPDATE_INTERVAL)) {
    onRefresh();
    refresh_timer.start();
  }
  BaseScreen::onIdle();
}

bool StatusScreen::onTouchEnd(uint8_t tag) {
  using namespace ExtUI;

  switch (tag) {
    case 3: GOTO_SCREEN(FilesScreen); break;
    case 4:
      if (isPrinting()) {
        GOTO_SCREEN(TuneMenu);
      } else {
        GOTO_SCREEN(MainMenu);
      }
      break;
    case 5:  GOTO_SCREEN(TemperatureScreen); break;
    case 6:
      if (!isPrinting()) {
        GOTO_SCREEN(MoveAxisScreen);
      }
      break;
    default:
      return true;
  }
  // If a passcode is enabled, the LockScreen will prevent the
  // user from proceeding.
  LockScreen::check_passcode();
  return true;
}

#endif // TOUCH_UI_FTDI_EVE
