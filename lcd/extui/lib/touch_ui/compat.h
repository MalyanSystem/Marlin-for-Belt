/************
 * compat.h *
 ************/

/****************************************************************************
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

#pragma once

/**
 * This following provides compatibility whether compiling
 * as a part of Marlin or outside it
 */

#ifdef __has_include
  #if __has_include("../../ui_api.h")
    #include "../../ui_api.h"
  #endif
#else
  #include "../../ui_api.h"
#endif

#ifdef __MARLIN_FIRMWARE__
  // __MARLIN_FIRMWARE__ exists when compiled within Marlin.
  #undef max
  #define max(a,b) ((a)>(b)?(a):(b))
  #undef min
  #define min(a,b) ((a)<(b)?(a):(b))
#else
  namespace UI {
    static inline uint32_t safe_millis() {return millis();};
    static inline void     yield()       {};
  };
#endif

class __FlashStringHelper;
typedef const __FlashStringHelper *progmem_str;

#include "../../../../inc/MarlinConfigPre.h"

#include "unicode/font_size_t.h"
#include "unicode/unicode.h"
#include "unicode/standard_char_set.h"
#include "unicode/western_char_set.h"
//#include "unicode/font_bitmaps.h"
#include "ui_lib/rgb_t.h"
#include "ui_lib/bitmap_info.h"
#include "ui_lib/tiny_timer.h"
#include "ui_lib/grid_layout.h"
#include "event_loop.h"
#include "ui_lib/commands.h"
#include "screens/screen_types.h"
#include "ui_lib/sound_player.h"
#include "ui_lib/sound_list.h"
//#include "ui_lib/polygon.h"
#include "ui_lib/text_box.h"
#include "ui_lib/resolutions.h"
