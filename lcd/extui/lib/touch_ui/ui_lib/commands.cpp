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
#include "inc/MarlinConfigPre.h"
#include "commands.h"
#include "bitmap_info.h"
#include <stdio.h>
#include <cstring>

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "spi.h"
#include "esp32-hal-spi.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"

/*
template <class T> const T& min (const T& a, const T& b) {
  return !(b<a)?a:b;     // or: return !comp(b,a)?a:b; for version (2)
}
template <class T> const T& max (const T& a, const T& b) {
  return (a<b)?b:a;     // or: return comp(a,b)?b:a; for version (2)
}
*/
#define MULTIPLE_OF_4(val) ((((val)+3)>>2)<<2)

namespace ExtUI {
  //btn_style_func_t  *_btn_style_callback = default_button_style_func;
  //bool is_tracking = false;

  static lv_disp_buf_t disp_buf;
  static lv_color_t buf1[DISP_BUF_SIZE];
  //static lv_color_t buf2[DISP_BUF_SIZE];
  extern "C" void disp_wait_cb(struct _disp_drv_t *t)
  {
    disp_buf.flushing=0;
    return;
  }

  TaskHandle_t lvglTaskHandle = NULL;
  void lvglTask(void *pvParameters)
  {
    while (1)
    {
      lv_tick_inc(100);
      lv_task_handler();
      vTaskDelay(100);
    }
  }

  extern "C" void tpcal_create(void);
  extern "C" void lcdinit()
  {
    spi_t * lcd_spi;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL<<14)|(1ULL<<10)|(1ULL<<15));
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    lcd_spi=spiStartBus(1, spiFrequencyToClockDiv(60000000), SPI_MODE0, SPI_MSBFIRST);
    //spi_dev_t* spi_dev = (volatile spi_dev_t *)(DR_REG_SPI2_BASE);
    //spi_dev->clock.val = 4097;    //60M

    spiAttachSCK(lcd_spi, 12);
    spiAttachMISO(lcd_spi, 13);
    spiAttachMOSI(lcd_spi, 11);

    lv_init();
	  disp_driver_init();	//lvgl_driver_init();
    touch_driver_init();
	  lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_BUF_SIZE);

	  lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    disp_drv.wait_cb = disp_wait_cb;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = touch_driver_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 3);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_RED);
    //lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);
	
    /*Create a line and apply the new style*/
    static lv_point_t line_points[] = { {0, 0}, {479, 319}, {0, 319}, {479, 0}, {0, 0} };
    lv_obj_t * line1;
    line1 = lv_line_create(lv_scr_act(), NULL);
	  lv_line_set_auto_size(line1, true);
    lv_line_set_points(line1, line_points, 5);     /*Set the points*/
    lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_line);     /*Set the points*/
    lv_obj_align(line1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * label;
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    //lv_obj_set_event_cb(btn1, event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
    //lv_obj_set_event_cb(btn2, event_handler);
    lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_btn_set_checkable(btn2, true);
    lv_btn_toggle(btn2);
    lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Toggled");

    //tpcal_create();
    xTaskCreateUniversal(lvglTask, "lvgl", 8192, NULL, 20, &lvglTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
    //while (1);
  }

  void draw_btn(int16_t x, int16_t y, int16_t w, int16_t h, const char* text, lv_event_cb_t event_cb)
  {
    lv_obj_t * label;
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_LEFT, x,y);
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, text);
    lv_obj_set_width(btn1,w);
    lv_obj_set_height(btn1,h);
    lv_obj_set_event_cb(btn1, event_cb);
  }

  void dl_display(void)
  {
      printf("dl_display\n");
  }
  void cmd_dlstart()
  {
      printf("cmd_dlstart\n");
  }
  void cmd_swap()
  {
      printf("cmd_swap\n");
  }

  void cmd_stop()
  {
      printf("cmd_stop\n");
  }

  void set_brightness(uint8_t brightness) {
    lcd_brightness=brightness;
    printf("CLCD::set_brightness(%d)\n",brightness);
  }

  uint8_t get_brightness()
  {
    printf("Get_brightness(%d)\n",lcd_brightness);
    return lcd_brightness;
  }

  void turn_on_backlight() {
    printf("CLCD::turn_on_backlight\n");
  }

  void FontMetrics::load(const uint8_t font) {
    static_assert(sizeof(FontMetrics) == 148, "Sizeof font metrics is incorrect");
    printf("FontMetrics::load(%d)\n",font);
  }

  uint16_t FontMetrics::get_text_width(const char *str, size_t n) const {
    uint16_t width = 0;
    const uint8_t *p = (const uint8_t *) str;
    for(;;) {
      const uint8_t val = *p++; n--;
      if (!val || n == 0) break;
      width += val < 128 ? char_widths[val] : 0;
    }
    return width;
  }

  uint16_t FontMetrics::get_text_width(progmem_str str, size_t n) const {
    uint16_t width = 0;
    const uint8_t *p = (const uint8_t *) str;
    for(;;) {
      const uint8_t val = *(p++); n--;
      if (!val || n == 0) break;
      width += val < 128 ? char_widths[val] : 0;
    }
    return width;
  }

  // Generic operations for transforming a byte, for use with _mem_write_bulk:
  static inline uint8_t reverse_byte(uint8_t a) {
    return ((a & 0x1)  << 7) | ((a & 0x2)  << 5) |
          ((a & 0x4)  << 3) | ((a & 0x8)  << 1) |
          ((a & 0x10) >> 1) | ((a & 0x20) >> 3) |
          ((a & 0x40) >> 5) | ((a & 0x80) >> 7);
  }

  void colors(btn_colors colors)
  {
    color_rgb(colors.rgb);
    gradcolor(colors.grad);
    fgcolor(colors.fg);
    bgcolor(colors.bg);
  }

  // Helper method for setting all colors at once
  /*void colors(const btn_colors &colors) {
    printf("cmd colors====need to add some code here\n");
    color_rgb(colors.rgb);
    gradcolor(colors.grad);
    fgcolor(colors.fg);
    bgcolor(colors.bg);
  }*/

  void bgcolor(uint32_t rgb) {
    _bgcolor=rgb;
    printf("bgcolor=%d\n",rgb);
    //cmd(CMD_BGCOLOR);
    //cmd(rgb);
  }

  void fgcolor(uint32_t rgb) {
    _fgcolor=rgb;
    //printf("fgcolor=%d\n",rgb);
    //cmd(CMD_FGCOLOR);
    //cmd(rgb);
  }

  void gradcolor(uint32_t rgb) {
    _gradcolor=rgb;
    //printf("gradcolor=%d\n",rgb);
    //cmd(CMD_GRADCOLOR);
    //cmd(rgb);
  }


  void clear_color_rgb(uint32_t rgb)
  {
    printf("===clear_color_rgb\n");
    _clear_color_rgb=rgb;
  }
  void clear(bool c,bool s,bool t)
  {
    printf("===clear\n");
    /*lcd_setWindow(0,0,480,320);
    gpio_set_level(GPIO_NUM_10, 0);
    for (uint32_t i=0;i<480*320;i++)
    {
      
      tx_byte(_clear_color_rgb>>16);
      tx_byte(_clear_color_rgb>>8);
      tx_byte(_clear_color_rgb);
    }
    gpio_set_level(GPIO_NUM_10, 1);*/
  }

  void color_rgb(uint32_t rgb)
  {
    _color=rgb;
    //printf("color_rgb\n");
  }

  void color_mask(bool r, bool g, bool b, bool a)
  {
    printf("color_mask\n");
    //{return DL::COLOR_MASK|((r?1UL:0UL)<<3)|((g?1UL:0UL)<<2)|((b?1UL:0UL)<<1)|(a?1UL:0UL);}
  }
  
  /*void button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font,  uint16_t option) {
    // This sends the a text command to the command preprocessor, must be followed by str()
    printf("CommandFifo::button X=%d Y=%d W=%d H=%d Font=%d Opt=%d\n",x,y,w,h,font,option);
  }*/

  // This sends the a toggle command to the command preprocessor, must be followed by str()
  void toggle (int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, bool state) {
    printf("CommandFifo::toggle\n");
  }

  void gradient (int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1) {
    printf("CommandFifo::gradient\n");
  }

  void number (int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n) {
    printf("CommandFifo::number\n");
  }

  void track(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t tag) {
    printf("CommandFifo::track\n");
  }

  void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale) {
    printf("CommandFifo::Cspinner\n");
  }

  void loadimage(uint32_t ptr, uint32_t options) {
    printf("CommandFifo::loadimage\n");
  }

  void scale(int32_t sx, int32_t sy) {
    printf("CommandFifo::scale\n");
  }

  void rotate(int32_t a) {
    printf("CommandFifo::rotate\n");
  }

  void translate (int32_t tx, int32_t ty) {
    printf("CommandFifo::translate\n");
  }

  void cmd_start() {
    printf("start()\n");
  }

  void cmd_execute() {
    printf("cmd_execute()\n");
  }

  void cmd_reset() {
    printf("reset()\n");
  };

  // CO_PROCESSOR COMMANDS

  void str(const char * data) {
    printf("-Text[%s] Font=%d Pos(%d,%d) Opt=%X\n",data,_font,_x,_y,_options);
  }

  inline void str(progmem_str data) {
    str((const char*)data);
  }

  /******************* LCD INITIALIZATION ************************/

  void init() {
    printf("ExtUI init()\n");
    //spi_init();                                  // Set Up I/O Lines for SPI and FT800/810 Control
    //ftdi_reset();                                // Power down/up the FT8xx with the apropriate delays
    lcdinit();

    /*lcd_setWindow(0,0,240,160);
    gpio_set_level(GPIO_NUM_10, 0);
    for (uint32_t aa=240*160;aa>0;aa--)
    {
      tx_byte(0xFF);
      tx_byte(0x00);
      tx_byte(0x00);
    }
    gpio_set_level(GPIO_NUM_10, 1);*/

    // Initialize the command FIFO
    cmd_reset();

    default_touch_transform();
  }

  void default_touch_transform() {
    printf("default_touch_transform\n");
  }

  // Returns the cannonical thickness of a widget (i.e. the height of a toggle element)
  uint16_t widget_thickness() {
    FontMetrics fm(_font);
    return fm.height * 20.0/16;
  }

  FORCE_INLINE void linear_widget_box(int16_t &x, int16_t &y, int16_t &w, int16_t &h, bool tracker = false) {
    const uint16_t th = widget_thickness() / 2;
    if (w > h) {
      x += tracker ? th * 2.5 : th;
      y += (h - th) / 2;
      w -= tracker ? th * 5.0 : th * 2;
      h  = th;
    } else {
      x += (w - th) / 2;
      y += tracker ? th * 2.5 : th;
      w  = th;
      h -= tracker ? th * 5.0 : th * 2;
    }
  }

  FORCE_INLINE uint16_t circular_widget_box(int16_t &x, int16_t &y, int16_t &w, int16_t &h) {
    const uint16_t r = min(w,h) / 2;
    x += w / 2;
    y += h / 2;
    w  = 1;
    h  = 1;
    return r;
  }

  void bitmap_size(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height) {
    printf("cmd bitmap_size\n");
    //cmd(FTDI::BITMAP_SIZE(filter, wrapx, wrapy, width, height));
  }

  void bitmap_layout(uint8_t format, uint16_t linestride, uint16_t height) {
    printf("cmd bitmap_layout\n");
    //cmd(FTDI::BITMAP_LAYOUT(format, linestride, height));
    //return *this;
  }

  void set_button_style_callback(const btn_style_func_t *func) {
    printf("set_button_style_callback\n");
    _btn_style_callback = func ? func : default_button_style_func;
    //return *this;
  }

  void tag(uint8_t tag)              {_tag = tag;}

  void font(int16_t font)             {_font = font;}

  void enabled  (bool enabled) {
    printf("cmd enabled\n");
    if (enabled)
      _style &= ~STYLE_DISABLED;
    else
      _style |= STYLE_DISABLED;
  }

  inline void style    (uint8_t style) {
    printf("cmd style\n");
    _style = (_style & STYLE_DISABLED) | style;
    //return *this;
  }

  void rectangle(int16_t x, int16_t y, int16_t w, int16_t h) {
    printf("cmd rectangle\n");
    /*using namespace FTDI;
    CLCD::CommandFifo::cmd(BEGIN(RECTS));
    CLCD::CommandFifo::cmd(VERTEX2F(x * 16, y * 16));
    CLCD::CommandFifo::cmd(VERTEX2F((x + w) * 16, (y + h) * 16));
    return *this;*/
  }

  void toggle(int16_t x, int16_t y, int16_t w, int16_t h, char* text, bool state, uint16_t options = OPT_3D) {
    printf("cmd toggle\n");
    /*CLCD::FontMetrics fm(_font);
    const int16_t widget_h = fm.height * 20.0 / 16;
    //const int16_t outer_bar_r = widget_h / 2;
    //const int16_t knob_r      = outer_bar_r - 1.5;
    // The y coordinate of the toggle is the baseline of the text,
    // so we must introduce a fudge factor based on the line height to
    // actually center the control.
    const int16_t fudge_y = fm.height * 5 / 16;
    CLCD::CommandFifo::toggle(x + h / 2, y + (h - widget_h) / 2 + fudge_y, w - h, _font, options, state);
    CLCD::CommandFifo::str(text);
    return *this;*/
  }

  void toggle2(int16_t x, int16_t y, int16_t w, int16_t h, const char * no, const char * yes, bool state, uint16_t options) {
    char text[strlen((const char *)no) + strlen((const char *)yes) + 2];
    strcpy(text, (const char *)no);
    strcat(text, "\xFF");
    strcat(text, (const char *)yes);
    toggle(x, y, w, h, text, state, options);
  }

  // Contrained drawing routines. These constrain the widget inside a box for easier layout.
  // The FORCEDINLINE ensures that the code is inlined so that all the math is done at compile time.

  void track_linear(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag) {
    linear_widget_box(x, y, w, h, true);
    track(x, y, w, h, tag);
    is_tracking = true;
    //return *this;
  }

  void track_circular(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag) {
    circular_widget_box(x,y, w, h);
    track(x, y, w, h, tag);
    is_tracking = true;
  }

  uint8_t track_tag (uint16_t &value) {
    if (is_tracking) {
      if (EventLoop::is_touch_held()) {
        return get_tracker(value);
      } else {
        track(0, 0, 0, 0, 0);
        cmd_execute();
        is_tracking = false;
      }
    }
    return 0;
  }

  void slider(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t val, uint16_t range, uint16_t options) {
    linear_widget_box(x, y, w, h);
    //slider(x, y, w, h, options, val, range);
    printf("CommandFifo::slider\n");
  }

  void progress(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t val, uint16_t range, uint16_t options = OPT_3D) {
    linear_widget_box(x, y, w, h);
    //progress(x, y, w, h, options, val, range);
    printf("CommandFifo::progress\n");
  }

  inline void apply_text_alignment(int16_t &x, int16_t &y, int16_t w, int16_t h, uint16_t options) {
    x += ((options & OPT_CENTERX) ? w/2 : ((options & OPT_RIGHTX) ? w : 0));
    y += ((options & OPT_CENTERY) ? h/2 : h);
  }

  // Reduce font size until text fits the enclosing box.
  int8_t apply_fit_text(int16_t w, int16_t h, char* text) {
    int8_t font = _font;
    #ifdef TOUCH_UI_USE_UTF8
      const bool is_utf8 = has_utf8_chars(text);
    #endif
    for (;font >= 26;) {
      int16_t width, height;
      #ifdef TOUCH_UI_USE_UTF8
        if (is_utf8) {
          width  = get_utf8_text_width(text, font_size_t::from_romfont(font));
          height = font_size_t::from_romfont(font).get_height();
        } else
      #endif
        {
          FontMetrics fm(font);
          width  = fm.get_text_width(text);
          height = fm.height;
        }
      if (width < w && height < h) break;
      font--;
    }
    return font;
  }

  void number(int16_t x, int16_t y, int16_t w, int16_t h, int32_t n, uint16_t options = OPT_CENTER) {
    apply_text_alignment(x, y, w, h, options);
    number(x, y, _font, options, n);
  }

  uint16_t text_width(char* text) {
    #ifdef TOUCH_UI_USE_UTF8
      if (has_utf8_chars(text))
        return get_utf8_text_width(text, font_size_t::from_romfont(_font));
    #endif
    FontMetrics fm(_font);
    return fm.get_text_width(text);
  }

  void icon(int16_t x, int16_t y, int16_t w, int16_t h, const bitmap_info_t& info, const float scale) {
    printf("icon tag=%d Pos(%d,%d) Size(%d,%d) Scale=%f Addr=%X\n",_tag,x,y,w,h,scale,info.RAMG_offset);
    /*using namespace FTDI;
    cmd(BEGIN(BITMAPS));
    if (scale != 1) {
      cmd(BITMAP_TRANSFORM_A(uint32_t(float(256)/scale)));
      cmd(BITMAP_TRANSFORM_E(uint32_t(float(256)/scale)));
    }
    cmd(BITMAP_SIZE(info.filter, info.wrapx, info.wrapy, info.width*scale, info.height*scale));
    cmd(VERTEX2F((x + w/2 - info.width*scale/2)*16, (y + h/2 - info.height*scale/2)*16));
    if (scale != 1) {
      cmd(BITMAP_TRANSFORM_A(256));
      cmd(BITMAP_TRANSFORM_E(256));
    }*/
  }

  void keys(int16_t x, int16_t y, int16_t w, int16_t h, char* keys, uint16_t options) {
    // This sends the a keys command to the command preprocessor, must be followed by str()
    printf("CommandFifo::keys\n");
    str(keys);
  }

  void spinner(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t style = 0, uint16_t scale = 0) {
    circular_widget_box(x, y, w, h);
    spinner(x, y, style, scale);
  }

  void button(int16_t x, int16_t y, int16_t w, int16_t h, const char* text, uint16_t options) {
    bool styleModified = false;
    if (_btn_style_callback) styleModified = _btn_style_callback( _tag, _style, options, false);
    #ifdef TOUCH_UI_FIT_TEXT
      const int8_t font = (options & OPT_NOFIT) ? _font : apply_fit_text(w, h, text);
    #else
      const int8_t font = _font;
    #endif

    printf("+Button Pos(%d,%d) Size(%d,%d) Font=%d Opt=%X\n",x,y,w,h,font,options);
    printf("+tag=%d color=%X fg=%X bg=%X grad=%X\n",_tag,_color,_fgcolor,_bgcolor,_gradcolor);

    /*lcd_setWindow(x,y,x+w,y+h);
    gpio_set_level(GPIO_NUM_10, 0);
    for (uint32_t i=0;i<w*h;i++)
    {
      
      tx_byte(_color>>16);
      tx_byte(_color>>8);
      tx_byte(_color);
    }
    gpio_set_level(GPIO_NUM_10, 1);*/
    //button(x, y, w, h, font, options);
    #ifdef TOUCH_UI_USE_UTF8
      if (has_utf8_chars(text)) {
        CLCD::CommandFifo::str(F(""));
        apply_text_alignment(x, y, w, h, OPT_CENTER);
        if (!(options & OPT_FLAT)) {
          // Reproduce the black "shadow" the FTDI adds to the button label
          CLCD::CommandFifo::cmd(SAVE_CONTEXT());
          CLCD::CommandFifo::cmd(COLOR_RGB(0x00000));
          draw_utf8_text(*this, x-1, y-1, text, font_size_t::from_romfont(font), OPT_CENTER);
          CLCD::CommandFifo::cmd(RESTORE_CONTEXT());
        }
        // Draw the button label
        draw_utf8_text(*this, x, y, text, font_size_t::from_romfont(font), OPT_CENTER);
      }
      else
    #endif
      str(text);
    if (_btn_style_callback && styleModified) _btn_style_callback(_tag, _style, options, true);
  }

  void text(int16_t x, int16_t y, int16_t font,  uint16_t options) {
    _options=options;
    _font=font;
    _x=x;_y=y;
    // This sends the a text command to the command preprocessor, must be followed by str()
    //printf("Text Tag=%d X=%d Y=%d Font=%d Opt=%d\n",_tag,x,y,font,options);
  }

  void text(int16_t x, int16_t y, int16_t w, int16_t h, const char* text, uint16_t options) {
    printf("+Text Tag=%d X=%d Y=%d Font=%d Opt=%d\n",_tag,x,y,font,options);
    apply_text_alignment(x, y, w, h, options);
    #ifdef TOUCH_UI_FIT_TEXT
      const int8_t font = (options & OPT_NOFIT) ? _font : apply_fit_text(w, h, text);
    #else
      const int8_t font = _font;
    #endif
    #ifdef TOUCH_UI_USE_UTF8
      if (has_utf8_chars(text))
        draw_utf8_text(*this, x, y, text, font_size_t::from_romfont(font), options);
      else
    #endif
    {
      ExtUI::text(x, y, font, options);
      str(text);
    }
  }

  void update_text(int16_t x, int16_t y, int16_t w, int16_t h, const char* text,const char* orig, uint16_t options)
  {
    if (strcmp(text,orig)==0) return;
    printf("***");
    //need to clear background here
    ExtUI::text(x, y, w, h, text, options);
  }

  void cmd_calibrate()
  {
    printf("cmd_calibrate\n");
  }
}
