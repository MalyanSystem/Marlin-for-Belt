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
#pragma once

/**
 * Espressif ESP32 (Tensilica Xtensa LX6) pin assignments
 */

#ifndef ARDUINO_ARCH_ESP32
  "Oops! Select an ESP32 board in 'Tools > Board.'"
#endif

#define BOARD_INFO_NAME "Malyan ESP32"

#if 1
#define GPIO0 0
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO6 6
#define GPIO7 7
#define GPIO8 8
#define GPIO9 9
#define GPIO10 10
#define GPIO11 11
#define GPIO12 12
#define GPIO13 13
#define GPIO14 14
#define GPIO15 15
#define GPIO16 16
#define GPIO17 17
#define GPIO18 18
#define GPIO19 19
#define GPIO20 20
#define GPIO21 21
#define GPIO22 22
#define GPIO23 23
#define GPIO24 24
#define GPIO25 25
#define GPIO26 26
#define GPIO27 27
#define GPIO28 28
#define GPIO29 29
#define GPIO30 30
#define GPIO31 31
#define GPIO32 32
#define GPIO33 33
#define GPIO34 34
#define GPIO35 35
#define GPIO36 36
#define GPIO37 37
#define GPIO38 38
#define GPIO39 39
#define GPIO40 40
#define GPIO41 41
#define GPIO42 42
#define GPIO43 43
#define GPIO44 44
#define GPIO45 45
#define GPIO46 46

#define PIN5 GPIO0
#define PIN6 GPIO1
#define PIN7 GPIO2
#define PIN8 GPIO3
#define PIN9 GPIO4
#define PIN10 GPIO5
#define PIN11 GPIO6
#define PIN12 GPIO7
#define PIN13 GPIO8
#define PIN14 GPIO9
#define PIN15 GPIO10
#define PIN16 GPIO11
#define PIN17 GPIO12
#define PIN18 GPIO13
#define PIN19 GPIO14
#define PIN21 GPIO15
#define PIN22 GPIO16
#define PIN23 GPIO17
#define PIN24 GPIO18
#define PIN25 GPIO19
#define PIN26 GPIO20
#define PIN28 GPIO21
#define PIN29 GPIO26
#define PIN31 GPIO27
#define PIN32 GPIO28
#define PIN33 GPIO29
#define PIN34 GPIO30
#define PIN35 GPIO31
#define PIN36 GPIO32
#define PIN37 GPIO33
#define PIN38 GPIO34
#define PIN39 GPIO35
#define PIN40 GPIO36
#define PIN41 GPIO37
#define PIN42 GPIO38
#define PIN43 GPIO39
#define PIN44 GPIO40
#define PIN46 GPIO41
#define PIN47 GPIO42
#define PIN48 GPIO43
#define PIN49 GPIO44
#define PIN50 GPIO45
#define PIN55 GPIO46
#endif

//
// I2S (steppers & other output-only pins)
//
/*#define I2S_STEPPER_STREAM
#define I2S_WS                                25
#define I2S_BCK                               26
#define I2S_DATA                              27
*/
#undef I2S_STEPPER_STREAM
//
// Limit Switches
//
#define X_MIN_PIN                            PIN42
#define Y_MIN_PIN                            PIN43
#define Z_MIN_PIN                            -1 //PIN44

//
// Steppers
//
#define X_STEP_PIN                           PIN6
#define X_DIR_PIN                            PIN7
#define X_ENABLE_PIN                         PIN8
#define X_SERIAL_RX_PIN                      X_ENABLE_PIN
#define X_SERIAL_TX_PIN                      X_ENABLE_PIN
//#define X_CS_PIN                             0

#define Y_STEP_PIN                           PIN9
#define Y_DIR_PIN                            PIN10
#define Y_ENABLE_PIN                         PIN11
#define Y_SERIAL_RX_PIN                      Y_ENABLE_PIN
#define Y_SERIAL_TX_PIN                      Y_ENABLE_PIN
//#define Y_CS_PIN                            13

#define Z_STEP_PIN                           PIN12
#define Z_DIR_PIN                            PIN19
#define Z_ENABLE_PIN                         PIN21
#define Z_SERIAL_RX_PIN                      Z_ENABLE_PIN
#define Z_SERIAL_TX_PIN                      Z_ENABLE_PIN
//#define Z_CS_PIN                             5  // SS_PIN

#define E0_STEP_PIN                          PIN22
#define E0_DIR_PIN                           PIN28
#define E0_ENABLE_PIN                        PIN29
#define E0_SERIAL_RX_PIN                     E0_ENABLE_PIN
#define E0_SERIAL_TX_PIN                     E0_ENABLE_PIN
//#define E0_CS_PIN                           21

//
// Temperature Sensors
//
#define TEMP_0_PIN                            7  // Analog Input
#define TEMP_BED_PIN                          8  // Analog Input

//
// Heaters / Fans
//
#define HEATER_0_PIN                           PIN46
#define EXFAN_PIN                              PIN50
#define FAN_PIN                                PIN37
#define HEATER_BED_PIN                         PIN47

// SPI
#define SDSS                                   34
#define FIL_RUNOUT_PIN                         130
//#define POWER_LOSS_PIN                         145
