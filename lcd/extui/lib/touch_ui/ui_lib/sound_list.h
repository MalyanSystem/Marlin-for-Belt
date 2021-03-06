/****************
 * sound_list.h *
 ****************/

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

namespace ExtUI {
  class SoundList {
    private:
      static PROGMEM const struct list_t {
        const char* name;
        const SoundPlayer::sound_t* data;
      } list[];
    public:
      static const uint8_t n;
      static inline const char* name(uint8_t val) {
        return (const char* ) pgm_read_ptr_near(&list[val].name);
      }
      static inline SoundPlayer::sound_t* data(uint8_t val) {
        return (SoundPlayer::sound_t*) pgm_read_ptr_near(&list[val].data);
      }
  };
}