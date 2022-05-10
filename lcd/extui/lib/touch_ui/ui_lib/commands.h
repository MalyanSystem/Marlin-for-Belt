/****************
 * commands.cpp *
 ****************/

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
#include <stdio.h>
#include "core/types.h"
#include "../event_loop.h"
#include "../unicode/unicode.h"
#include "bitmap_info.h"
#include "esp_debug_helpers.h"


#pragma once

#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lvgl_spi_conf.h"

namespace ExtUI {
  void init();
  void default_touch_transform();
  void turn_on_backlight();
  void enable();
  void disable();
  void set_brightness (uint8_t brightness);
  uint8_t get_brightness();
  static uint8_t lcd_brightness=0;

  void get_font_metrics (uint8_t font, struct FontMetrics &fm);
  uint16_t get_text_width(const uint8_t font, const char *str);
  uint16_t get_text_width_P(const uint8_t font, const char *str);

  extern "C" esp_err_t esp_backtrace_print(int depth);
  static uint8_t get_tag ()     {return 0;printf("get_tag()\n");esp_backtrace_print(6);}
  static bool is_touching ()    {return 0;}

  static uint8_t get_tracker (uint16_t &value) {
    printf("get_tracker\n");
    return 0;
    /*uint32_t tracker = mem_read_32(REG::TRACKER);
    value            = tracker >> 16;
    return tracker & 0xFF;*/
  }

  void tag(uint8_t tag);
  extern void draw_btn(int16_t x, int16_t y, int16_t w, int16_t h, const char* text, lv_event_cb_t event_cb);

  /*************************** FT800/810 Font Metrics ****************************/

  class FontMetrics {
    public:
      uint8_t   char_widths[128];
      uint32_t  format;
      uint32_t  stride;
      uint32_t  width;
      uint32_t  height;
      uint32_t  ptr;

      FontMetrics() {}
      FontMetrics(uint8_t font) {load(font);}

      void load(uint8_t font);

      // Returns width of string, up to a maximum of n characters.
      uint16_t get_text_width(const char *str, size_t n = SIZE_MAX) const;
      uint16_t get_text_width(progmem_str str, size_t n = SIZE_MAX) const;

      inline uint16_t get_char_width(utf8_char_t c) const {
      #ifdef TOUCH_UI_USE_UTF8
        return get_utf8_char_width(c, fs);
      #else
        return char_widths[(uint8_t)c];
      #endif
    }

    inline uint8_t get_height() const {
      #ifdef TOUCH_UI_USE_UTF8
        return fs.get_height();
      #else
        return height;
      #endif
    }

  };

  /******************* FT800/810 Graphic Commands *********************************/
  void cmd_start();
  void cmd_reset();
  void cmd_execute();
  void execute();

  void dl_display();
  void cmd_dlstart();
  void cmd_swap();
  void execute();
  void cmd_calibrate();
  void cmd_stop();

  typedef struct {
    uint32_t bg;
    uint32_t grad;
    uint32_t fg;
    uint32_t rgb;
  } btn_colors;

  void fgcolor     (uint32_t rgb);
  void bgcolor     (uint32_t rgb);
  void gradcolor   (uint32_t rgb);
  void color_rgb(uint32_t rgb);
  void colors(btn_colors btn);
  void color_mask(bool r, bool g, bool b, bool a);

  void enabled  (bool enabled=false);

  void track       (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t tag);
  void number      (int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n);
  void spinner     (int16_t x, int16_t y, uint16_t style, uint16_t scale);
  //void sketch      (int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format);
  void gradient    (int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
  void snapshot    (uint32_t ptr);
  void loadimage   (uint32_t ptr, uint32_t options);

  void scale       (int32_t sx, int32_t sy);
  void rotate      (int32_t a);
  void translate   (int32_t tx, int32_t ty);

  // All the following must be followed by str()
  void text      (int16_t x, int16_t y, int16_t font,  uint16_t options);
  //void button    (int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t option);
  void toggle    (int16_t x, int16_t y, int16_t w,            int16_t font, uint16_t options, bool state);

  void text (int16_t x, int16_t y, int16_t w, int16_t h, const char* text, uint16_t options = OPT_CENTER);
  void update_text (int16_t x, int16_t y, int16_t w, int16_t h, const char* text, const char* orig, uint16_t options = OPT_CENTER);
  void button(int16_t x, int16_t y, int16_t w, int16_t h, const char* text, uint16_t options = OPT_3D);
  void icon(int16_t x, int16_t y, int16_t w, int16_t h, const bitmap_info_t& info, const float scale = 1);
  void keys(int16_t x, int16_t y, int16_t w, int16_t h, char* keys, uint16_t options = OPT_3D);

  void rectangle(int16_t x, int16_t y, int16_t w, int16_t h);
  void slider(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t val, uint16_t range, uint16_t options = OPT_3D);
  void toggle2(int16_t x, int16_t y, int16_t w, int16_t h, const char * no, const char * yes, bool state, uint16_t options = OPT_3D);
  void track_linear(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag);
  uint8_t track_tag (uint16_t &value);

  // Sends the string portion of text, button, toggle and keys.
  void str (const char * data);
  void str (progmem_str data);

  void append   (uint32_t ptr, uint32_t size);

  void clear_color_rgb(uint32_t rgb);
  void clear(bool c,bool s,bool t);

  void font(int16_t font);

  // Disable TOUCH_UI_FIT_TEXT on a case-by-case basis
  constexpr uint16_t OPT_NOFIT = OPT_NOTICKS;

  /**************************** Enhanced Command Processor **************************/

  /* The CommandProcessor class wraps the CommandFifo with several features to make
  * defining user interfaces much easier.
  *
  *   - Implements chaining on all methods
  *   - Automatically adds text to button, toggle, text and keys.
  *   - Constrains all widgets to fit inside a box for ease of layout.
  *   - Font size is specified using a chained modifier.
  *   - Option argument is given the default OPT_3D value.
  */

  static constexpr uint8_t STYLE_DISABLED = 0x80;

  static bool default_button_style_func(uint8_t tag, uint8_t &style, uint16_t &options, bool post) {
    if (tag != 0 && EventLoop::get_pressed_tag() == tag) {
      options = OPT_FLAT;
    }
    return false;
  }

  typedef bool btn_style_func_t(uint8_t tag, uint8_t &style, uint16_t &options, bool post);

  static btn_style_func_t  *_btn_style_callback = default_button_style_func;
  static bool is_tracking = false;
  static int8_t  _font = 26, _tag = 0;
  static uint8_t _style = 0;

  static uint32_t _fgcolor,_bgcolor,_gradcolor,_color;
  static uint32_t _clear_color_rgb;
  static uint16_t _options;
  static int16_t _x,_y;

  void set_button_style_callback(const btn_style_func_t *func);
}
