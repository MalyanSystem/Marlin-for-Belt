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

#include <stdio.h>
#include <esp_types.h>
#include <soc/timer_group_struct.h>
#include <driver/periph_ctrl.h>
//#include "driver/timer.h"

#include "HAL.h"

#include "timers.h"

//yongzong
typedef enum {
    TIMER_ALARM_DIS = 0,  /*!< Disable timer alarm*/
    TIMER_ALARM_EN = 1,   /*!< Enable timer alarm*/
    TIMER_ALARM_MAX
} timer_alarm_t;
typedef enum {
    TIMER_PAUSE = 0, /*!<Pause timer counter*/
    TIMER_START = 1, /*!<Start timer counter*/
} timer_start_t;
typedef enum {
    TIMER_INTR_LEVEL = 0,  /*!< Interrupt mode: level mode*/
    //TIMER_INTR_EDGE = 1, /*!< Interrupt mode: edge mode, Not supported Now*/
    TIMER_INTR_MAX
} timer_intr_mode_t;
typedef enum {
    TIMER_COUNT_DOWN = 0, /*!< Descending Count from cnt.high|cnt.low*/
    TIMER_COUNT_UP = 1,   /*!< Ascending Count from Zero*/
    TIMER_COUNT_MAX
} timer_count_dir_t;
typedef enum {
    TIMER_AUTORELOAD_DIS = 0,  /*!< Disable auto-reload: hardware will not load counter value after an alarm event*/
    TIMER_AUTORELOAD_EN = 1,   /*!< Enable auto-reload: hardware will load counter value after an alarm event*/
    TIMER_AUTORELOAD_MAX,
} timer_autoreload_t;
typedef enum {
    TIMER_SRC_CLK_APB = 0,  /*!< Select APB as the source clock*/
    TIMER_SRC_CLK_XTAL = 1, /*!< Select XTAL as the source clock*/
} timer_src_clk_t;
#define SOC_TIMER_GROUP_SUPPORT_XTAL
typedef struct {
    timer_alarm_t alarm_en;      /*!< Timer alarm enable */
    timer_start_t counter_en;    /*!< Counter enable */
    timer_intr_mode_t intr_type; /*!< Interrupt mode */
    timer_count_dir_t counter_dir; /*!< Counter direction  */
    timer_autoreload_t auto_reload;   /*!< Timer auto-reload */
    uint32_t divider;   /*!< Counter clock divider. The divider's range is from from 2 to 65536. */
#ifdef SOC_TIMER_GROUP_SUPPORT_XTAL
    timer_src_clk_t clk_src;  /*!< Use XTAL as source clock. */
#endif
} timer_config_t;
typedef intr_handle_t timer_isr_handle_t;
extern "C" esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config);
extern "C" esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val);
extern "C" esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value);
extern "C" esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num);
extern "C" esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num,
                             void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle);
extern "C" esp_err_t timer_start(timer_group_t group_num, timer_idx_t timer_num);
extern "C" esp_err_t timer_get_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *alarm_value);
extern "C" esp_err_t timer_get_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *timer_val);

// ------------------------
// Local defines
// ------------------------

#define NUM_HARDWARE_TIMERS 2

// ------------------------
// Private Variables
// ------------------------

static timg_dev_t *TG[2] = {&TIMERG0, &TIMERG1};

DRAM_ATTR const tTimerConfig TimerConfig [NUM_HARDWARE_TIMERS] = {
/*  { TIMER_GROUP_0, TIMER_0, STEPPER_TIMER_PRESCALE, stepTC_Handler }, // 0 - Stepper
  { TIMER_GROUP_0, TIMER_1,    TEMP_TIMER_PRESCALE, tempTC_Handler }, // 1 - Temperature
  { TIMER_GROUP_1, TIMER_0,     PWM_TIMER_PRESCALE, pwmTC_Handler  }, // 2 - PWM
  { TIMER_GROUP_1, TIMER_1,                      1, nullptr }, // 3*/
  { TIMER_GROUP_1, TIMER_0, STEPPER_TIMER_PRESCALE, stepTC_Handler }, // 0 - Stepper
  { TIMER_GROUP_1, TIMER_1,    TEMP_TIMER_PRESCALE, tempTC_Handler }, // 1 - Temperature
  //{ TIMER_GROUP_0, TIMER_0,     PWM_TIMER_PRESCALE, NULL_Handler  }, // 2 - PWM
  //{ TIMER_GROUP_0, TIMER_1,                      1, NULL_Handler }, // 3
};

// ------------------------
// Public functions
// ------------------------
#define timer   TimerConfig[timer_num]
#define itimer   TimerConfig[(int)para]
portMUX_TYPE isrMutex = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR timer_isr(void *para) {
  //tTimerConfig& timer = TimerConfig[(int)para];

  // Retrieve the interrupt status and the counter value
  // from the timer that reported the interrupt
  //yongzong
  //taskENTER_CRITICAL(&isrMutex);

  uint32_t intr_status = TG[itimer.group]->int_st.val;//int_st_timers
  TG[itimer.group]->hw_timer[itimer.idx].update.update = 1;//hw_timer[timer.idx].update

  // Clear the interrupt
  if (intr_status & BIT(itimer.idx)) {
    switch (itimer.idx) {
      case TIMER_0: TG[itimer.group]->int_clr.t0 = 1; break;//int_clr_timers
      case TIMER_1: TG[itimer.group]->int_clr.t1 = 1; break;//int_clr_timers
      case TIMER_MAX: break;
    }
  }

  itimer.fn();

  // After the alarm has been triggered
  // Enable it again so it gets triggered the next time
  TG[itimer.group]->hw_timer[itimer.idx].config.alarm_en = TIMER_ALARM_EN;

  //taskEXIT_CRITICAL(&isrMutex);
}

/**
 * Enable and initialize the timer
 * @param timer_num timer number to initialize
 * @param frequency frequency of the timer
 */
uint8_t timer_started=0;
extern "C" esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num);
void HAL_timer_stop()
{
  timer_pause(TIMER_GROUP_1, TIMER_0);
  timer_pause(TIMER_GROUP_1, TIMER_1);
}
void HAL_timer_start(const uint8_t timer_num, uint32_t frequency) {
  //tTimerConfig timer = TimerConfig[timer_num];

  timer_config_t config;
  config.divider     = timer.divider;
  config.counter_dir = TIMER_COUNT_UP;
  config.counter_en  = TIMER_PAUSE;
  config.alarm_en    = TIMER_ALARM_EN;
  config.intr_type   = TIMER_INTR_LEVEL;
  config.auto_reload = timer_autoreload_t::TIMER_AUTORELOAD_EN;

  // Select and initialize the timer
  timer_init(timer.group, timer.idx, &config);

  // Timer counter initial value and auto reload on alarm
  timer_set_counter_value(timer.group, timer.idx, 0x00000000ULL);

  // Configure the alam value and the interrupt on alarm
  timer_set_alarm_value(timer.group, timer.idx, (HAL_TIMER_RATE) / timer.divider / frequency - 1);

  timer_enable_intr(timer.group, timer.idx);

  //timer_isr_register(timer.group, timer.idx, timer_isr, (void*)(uint32_t)timer_num, 0, nullptr);
  timer_isr_register(timer.group, timer.idx, timer_isr, (void*)(uint32_t)timer_num, ESP_INTR_FLAG_IRAM, nullptr);//ESP_INTR_FLAG_IRAM//ESP_INTR_FLAG_LEVEL3

  timer_start(timer.group, timer.idx);
  timer_started=1;
}

/**
 * Set the upper value of the timer, when the timer reaches this upper value the
 * interrupt should be triggered and the counter reset
 * @param timer_num timer number to set the count to
 * @param count     threshold at which the interrupt is triggered
 */
void IRAM_ATTR HAL_timer_set_compare(const uint8_t timer_num, hal_timer_t count) {
  //tTimerConfig timer = TimerConfig[timer_num];
  timer_set_alarm_value(timer.group, timer.idx, count);
}

/**
 * Get the current upper value of the timer
 * @param  timer_num timer number to get the count from
 * @return           the timer current threshold for the alarm to be triggered
 */
hal_timer_t IRAM_ATTR HAL_timer_get_compare(const uint8_t timer_num) {
  //tTimerConfig timer = TimerConfig[timer_num];

  uint64_t alarm_value;
  timer_get_alarm_value(timer.group, timer.idx, &alarm_value);

  return alarm_value;
}

/**
 * Get the current counter value between 0 and the maximum count (HAL_timer_set_count)
 * @param  timer_num timer number to get the current count
 * @return           the current counter of the alarm
 */
hal_timer_t IRAM_ATTR HAL_timer_get_count(const uint8_t timer_num) {
  //tTimerConfig timer = TimerConfig[timer_num];
  uint64_t counter_value;
  timer_get_counter_value(timer.group, timer.idx, &counter_value);
  return counter_value;
}

/**
 * Enable timer interrupt on the timer
 * @param timer_num timer number to enable interrupts on
 */
void IRAM_ATTR HAL_timer_enable_interrupt(const uint8_t timer_num) {
  //tTimerConfig timer = TimerConfig[timer_num];
  //timer_enable_intr(timer.group, timer.idx);
}

/**
 * Disable timer interrupt on the timer
 * @param timer_num timer number to disable interrupts on
 */
void IRAM_ATTR HAL_timer_disable_interrupt(const uint8_t timer_num) {
  //tTimerConfig timer = TimerConfig[timer_num];
  //timer_disable_intr(timer.group, timer.idx);
}

bool IRAM_ATTR HAL_timer_interrupt_enabled(const uint8_t timer_num) {
  //tTimerConfig timer = TimerConfig[timer_num];
  return TG[timer.group]->int_ena.val | BIT(timer_num);
}

#endif // ARDUINO_ARCH_ESP32
