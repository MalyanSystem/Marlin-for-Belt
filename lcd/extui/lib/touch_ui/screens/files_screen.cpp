/********************
 * files_screen.cpp *
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
#include "screen_data.h"
#include "../compat.h"

using namespace ExtUI;
using namespace Theme;

void FilesScreen::onEntry() {
  screen_data.FilesScreen.cur_page        = 0;
  screen_data.FilesScreen.selected_tag    = 0xFF;
  #if ENABLED(SCROLL_LONG_FILENAMES) && (FTDI_API_LEVEL >= 810)
    CLCD::mem_write_32(CLCD::REG::MACRO_0,DL::NOP);
  #endif
  gotoPage(0);
  BaseScreen::onEntry();
}

const char *FilesScreen::getSelectedFilename(bool longName) {
  FileList files;
  files.seek(getSelectedFileIndex(), true);
  return longName ? files.longFilename() : files.shortFilename();
}

void FilesScreen::drawSelectedFile() {
  FileList files;
  files.seek(getSelectedFileIndex(), true);
  screen_data.FilesScreen.flags.is_dir = files.isDir();
  drawFileButton(
    files.filename(),
    screen_data.FilesScreen.selected_tag,
    screen_data.FilesScreen.flags.is_dir,
    true
  );
}

uint16_t FilesScreen::getSelectedFileIndex() {
  return getFileForTag(screen_data.FilesScreen.selected_tag);
}

uint16_t FilesScreen::getFileForTag(uint8_t tag) {
  return screen_data.FilesScreen.cur_page * files_per_page + tag - 2;
}

#ifdef TOUCH_UI_PORTRAIT
  #define GRID_COLS  6
  #define GRID_ROWS (files_per_page + header_h + footer_h)
#else
  #define GRID_COLS  6
  #define GRID_ROWS (files_per_page + header_h + footer_h)
#endif

void FilesScreen::drawFileButton(const char* filename, uint8_t t, bool is_dir, bool is_highlighted) {
  const uint8_t line = getLineForTag(t)+1;

  tag(t);
  color_rgb(is_highlighted ? fg_action : bg_color);
  font(font_medium);
  rectangle( 0, BTN_Y(header_h+line), display_width, BTN_H(1));
  color_rgb(is_highlighted ? normal_btn.rgb : bg_text_enabled);

  #if ENABLED(SCROLL_LONG_FILENAMES)
    if (is_highlighted) {
      cmd.cmd(SAVE_CONTEXT());
      cmd.cmd(MACRO(0));
    }
  #endif

  text(BTN_POS(1,header_h+line), BTN_SIZE(6,1), filename, OPT_CENTERY
    #if ENABLED(SCROLL_LONG_FILENAMES)
      | OPT_NOFIT
    #endif
  );
  if (is_dir) {
    text(BTN_POS(1,header_h+line), BTN_SIZE(6,1), (const char*)F("> "),  OPT_CENTERY | OPT_RIGHTX);
  }
  #if ENABLED(SCROLL_LONG_FILENAMES)
    if (is_highlighted) {
      cmd.cmd(RESTORE_CONTEXT());
    }
  #endif
}

void FilesScreen::drawFileList() {
  FileList files;
  screen_data.FilesScreen.num_page = max(1,(ceil)(float(files.count()) / files_per_page));
  screen_data.FilesScreen.cur_page = min(screen_data.FilesScreen.cur_page, screen_data.FilesScreen.num_page-1);
  screen_data.FilesScreen.flags.is_root  = files.isAtRootDir();

  #undef MARGIN_T
  #undef MARGIN_B
  #define MARGIN_T 0
  #define MARGIN_B 0
  uint16_t fileIndex = screen_data.FilesScreen.cur_page * files_per_page;
  for(uint8_t i = 0; i < files_per_page; i++, fileIndex++) {
    if (files.seek(fileIndex)) {
      drawFileButton(files.filename(), getTagForLine(i), files.isDir(), false);
    } else {
      break;
    }
  }
}

void FilesScreen::drawHeader() {
  const bool prev_enabled = screen_data.FilesScreen.cur_page > 0;
  const bool next_enabled = screen_data.FilesScreen.cur_page < (screen_data.FilesScreen.num_page - 1);

  #undef MARGIN_T
  #undef MARGIN_B
  #define MARGIN_T 0
  #define MARGIN_B 2

  char str[16];
  sprintf_P(str, PSTR("Page %d of %d"),
    screen_data.FilesScreen.cur_page + 1, screen_data.FilesScreen.num_page);

  colors(normal_btn);
  font(font_small);
  tag(0);
  button( BTN_POS(2,1), BTN_SIZE(4,header_h), str, OPT_CENTER | OPT_FLAT);
  font(font_medium);
  colors(action_btn);
  tag(241);enabled(prev_enabled);button( BTN_POS(1,1), BTN_SIZE(1,header_h), (const char*)F("<"));
  tag(242);enabled(next_enabled);button( BTN_POS(6,1), BTN_SIZE(1,header_h), (const char*)F(">"));
}

void FilesScreen::drawFooter() {
  #undef MARGIN_T
  #undef MARGIN_B
  #ifdef TOUCH_UI_PORTRAIT
  #define MARGIN_T 15
  #define MARGIN_B 5
  #else
  #define MARGIN_T 5
  #define MARGIN_B 5
  #endif
  const bool    has_selection = screen_data.FilesScreen.selected_tag != 0xFF;
  const uint8_t back_tag      = screen_data.FilesScreen.flags.is_root ? 240 : 245;
  const uint8_t y             = GRID_ROWS - footer_h + 1;
  const uint8_t h             = footer_h;

  colors(normal_btn);
  font(font_medium);
  colors(has_selection ? normal_btn : action_btn);
  tag(back_tag);button( BTN_POS(4,y), BTN_SIZE(3,h), (const char*)GET_TEXT_F(MSG_BACK));
  enabled(has_selection);
  colors(has_selection ? action_btn : normal_btn);

  if (screen_data.FilesScreen.flags.is_dir) {
    tag(244);
    button( BTN_POS(1, y), BTN_SIZE(3,h), (const char*)GET_TEXT_F(MSG_BUTTON_OPEN));
  } else {
    tag(243);
    button( BTN_POS(1, y), BTN_SIZE(3,h), (const char*)GET_TEXT_F(MSG_BUTTON_PRINT));
  }
}

void FilesScreen::onRedraw(draw_mode_t what) {
  if (what & FOREGROUND) {
    drawHeader();
    drawSelectedFile();
    drawFooter();
  }
}

void FilesScreen::gotoPage(uint8_t page) {
  screen_data.FilesScreen.selected_tag = 0xFF;
  screen_data.FilesScreen.cur_page     = page;

  cmd_dlstart();
  clear_color_rgb(bg_color);
  clear(true,true,true);
  colors(normal_btn);

  drawFileList();
  storeBackground();
}

bool FilesScreen::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 240: GOTO_PREVIOUS();                  return true;
    case 241:
      if (screen_data.FilesScreen.cur_page > 0) {
        gotoPage(screen_data.FilesScreen.cur_page-1);
      }
      break;
    case 242:
      if (screen_data.FilesScreen.cur_page < (screen_data.FilesScreen.num_page-1)) {
        gotoPage(screen_data.FilesScreen.cur_page+1);
      }
      break;
    case 243:
      ConfirmStartPrintDialogBox::show(getSelectedFileIndex());
      return true;
    case 244:
      {
        FileList files;
        files.changeDir(getSelectedShortFilename());
        gotoPage(0);
      }
      break;
    case 245:
      {
        FileList files;
        files.upDir();
        gotoPage(0);
      }
      break;
    default:
      if (tag < 240) {
        screen_data.FilesScreen.selected_tag = tag;
        #if ENABLED(SCROLL_LONG_FILENAMES) && (FTDI_API_LEVEL >= 810)
          if (FTDI::ftdi_chip >= 810) {
            const char *longFilename = getSelectedLongFilename();
            if (longFilename[0]) {
              font(font_medium);
              uint16_t text_width = text_width(longFilename);
              screen_data.FilesScreen.scroll_pos = 0;
              if (text_width > display_width)
                screen_data.FilesScreen.scroll_max = text_width - display_width + MARGIN_L + MARGIN_R;
              else
                screen_data.FilesScreen.scroll_max = 0;
            }
          }
        #endif
      }
      break;
  }
  return true;
}

void FilesScreen::onIdle() {
  #if ENABLED(SCROLL_LONG_FILENAMES) && (FTDI_API_LEVEL >= 810)
    if (FTDI::ftdi_chip >= 810) {
      CLCD::mem_write_32(CLCD::REG::MACRO_0,
        VERTEX_TRANSLATE_X(-int32_t(screen_data.FilesScreen.scroll_pos)));
      if (screen_data.FilesScreen.scroll_pos < screen_data.FilesScreen.scroll_max * 16)
        screen_data.FilesScreen.scroll_pos++;
    }
  #endif
}

#endif // TOUCH_UI
