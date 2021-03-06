/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef ARDUINO_ARCH_ESP32

#include "../../inc/MarlinConfig.h"

#if ENABLED(EEPROM_SETTINGS) && DISABLED(FLASH_EEPROM_EMULATION)

#include "../shared/eeprom_api.h"
#include "EEPROM.h"
#include "timers.h"

extern "C" esp_err_t timer_start(timer_group_t group_num, timer_idx_t timer_num);
extern "C" esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num);

#define EEPROM_SIZE 4096

extern uint8_t timer_started;

bool PersistentStore::access_start() {
  if (timer_started)
  {
    timer_pause(TIMER_GROUP_1,TIMER_0);
    timer_pause(TIMER_GROUP_1,TIMER_1);
  }
  return EEPROM.begin(EEPROM_SIZE);
}

bool PersistentStore::access_finish() {
  EEPROM.end();
  if (timer_started)
  {
    timer_start(TIMER_GROUP_1,TIMER_0);
    timer_start(TIMER_GROUP_1,TIMER_1);
  }
  return true;
}

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc) {
  for (size_t i = 0; i < size; i++) {
    EEPROM.write(pos++, value[i]);
    crc16(crc, &value[i], 1);
  }
  return false;
}

bool PersistentStore::read_data(int &pos, uint8_t* value, size_t size, uint16_t *crc, const bool writing/*=true*/) {
  for (size_t i = 0; i < size; i++) {
    uint8_t c = EEPROM.read(pos++);
    if (writing) value[i] = c;
    crc16(crc, &c, 1);
  }
  return false;
}

size_t PersistentStore::capacity() { return EEPROM_SIZE; }

#endif // EEPROM_SETTINGS
#endif // ARDUINO_ARCH_ESP32
