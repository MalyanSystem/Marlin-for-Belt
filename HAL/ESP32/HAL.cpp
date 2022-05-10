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

#include "HAL.h"
#include "timers.h"
#include <esp32s2/rom/rtc.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <HardwareSerial.h>
#include "soc/adc_periph.h"
#include "driver/adc.h"
#include "../../inc/MarlinConfigPre.h"

#if ENABLED(WIFISUPPORT)
  #include <ESPAsyncWebServer.h>
  #include "wifi.h"
  #if ENABLED(OTASUPPORT)
    #include "ota.h"
  #endif
  #if ENABLED(WEBSUPPORT)
    #include "spiffs.h"
    #include "web.h"
  #endif
#endif

// ------------------------
// Externs
// ------------------------

portMUX_TYPE hal_spinlock = portMUX_INITIALIZER_UNLOCKED;

// ------------------------
// Local defines
// ------------------------

#define V_REF 1100

// ------------------------
// Public Variables
// ------------------------

uint16_t HAL_adc_result;

// ------------------------
// Private Variables
// ------------------------

esp_adc_cal_characteristics_t characteristics[ADC_ATTEN_MAX];
adc_atten_t attenuations[ADC1_CHANNEL_MAX] = {};
uint32_t thresholds[ADC_ATTEN_MAX];
volatile int numPWMUsed = 0,
             pwmPins[MAX_PWM_PINS],
             pwmValues[MAX_PWM_PINS];

uint32_t raw_0 = 0,raw_bed = 0;

// ------------------------
// Public functions
// ------------------------

#if ENABLED(WIFI_CUSTOM_COMMAND)

  bool wifi_custom_command(char * const command_ptr) {
    #if ENABLED(ESP3D_WIFISUPPORT)
      return esp3dlib.parse(command_ptr);
    #else
      UNUSED(command_ptr);
      return false;
    #endif
  }

#endif

void HAL_init() { /*i2s_init();*/ }

void HAL_init_board() {

  #if ENABLED(ESP3D_WIFISUPPORT)
    esp3dlib.init();
  #elif ENABLED(WIFISUPPORT)
    wifi_init();
    #if ENABLED(OTASUPPORT)
      OTA_init();
    #endif
    #if ENABLED(WEBSUPPORT)
      spiffs_init();
      web_init();
    #endif
    server.begin();
  #endif

  // ESP32 uses a GPIO matrix that allows pins to be assigned to hardware serial ports.
  // The following code initializes hardware Serial1 and Serial2 to use user-defined pins
  // if they have been defined.
  #if defined(HARDWARE_SERIAL1_RX) && defined(HARDWARE_SERIAL1_TX)
    HardwareSerial Serial1(1);
    #ifdef TMC_BAUD_RATE  // use TMC_BAUD_RATE for Serial1 if defined
      Serial1.begin(TMC_BAUD_RATE, SERIAL_8N1, HARDWARE_SERIAL1_RX, HARDWARE_SERIAL1_TX);
    #else  // use default BAUDRATE if TMC_BAUD_RATE not defined
      Serial1.begin(BAUDRATE, SERIAL_8N1, HARDWARE_SERIAL1_RX, HARDWARE_SERIAL1_TX);
    #endif
  #endif
  #if defined(HARDWARE_SERIAL2_RX) && defined(HARDWARE_SERIAL2_TX)
    HardwareSerial Serial2(2);
    #ifdef TMC_BAUD_RATE  // use TMC_BAUD_RATE for Serial1 if defined
      Serial2.begin(TMC_BAUD_RATE, SERIAL_8N1, HARDWARE_SERIAL2_RX, HARDWARE_SERIAL2_TX);
    #else  // use default BAUDRATE if TMC_BAUD_RATE not defined
      Serial2.begin(BAUDRATE, SERIAL_8N1, HARDWARE_SERIAL2_RX, HARDWARE_SERIAL2_TX);
    #endif
  #endif

}

void HAL_idletask() {
  #if BOTH(WIFISUPPORT, OTASUPPORT)
    OTA_handle();
  #endif
  #if ENABLED(ESP3D_WIFISUPPORT)
    esp3dlib.idletask();
  #endif
}

void HAL_clear_reset_source() { }

uint8_t HAL_get_reset_source() { return rtc_get_reset_reason(1); }

void _delay_ms(int delay_ms) { delay(delay_ms); }

// return free memory between end of heap (or end bss) and whatever is current
int freeMemory() { return ESP.getFreeHeap(); }
/*
// ------------------------
// ADC
// ------------------------
#define ADC1_CHANNEL(pin) ADC1_GPIO ## pin ## _CHANNEL

adc1_channel_t get_channel(int pin) {
  switch (pin) {
    case 1: return ADC1_CHANNEL(1);
    case 2: return ADC1_CHANNEL(2);
    case 3: return ADC1_CHANNEL(3);
    case 4: return ADC1_CHANNEL(4);
    case 5: return ADC1_CHANNEL(5);
    case 6: return ADC1_CHANNEL(6);
  }
  return ADC1_CHANNEL_MAX;
}

void adc1_set_attenuation(adc1_channel_t chan, adc_atten_t atten) {
  if (attenuations[chan] != atten) {
    adc1_config_channel_atten(chan, atten);
    attenuations[chan] = atten;
  }
}
*/
void HAL_adc_init() {
  adc1_config_width(ADC_WIDTH_BIT_13);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_8, ADC_ATTEN_DB_11);

  //adc_vref_to_gpio(ADC_UNIT_1, GPIO_NUM_39);
  /*
  // Configure ADC
  adc1_config_width(ADC_WIDTH_12Bit);

  // Configure channels only if used as (re-)configuring a pin for ADC that is used elsewhere might have adverse effects
  #if HAS_TEMP_ADC_0
    adc1_set_attenuation(get_channel(TEMP_0_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_1
    adc1_set_attenuation(get_channel(TEMP_1_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_2
    adc1_set_attenuation(get_channel(TEMP_2_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_3
    adc1_set_attenuation(get_channel(TEMP_3_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_4
    adc1_set_attenuation(get_channel(TEMP_4_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_5
    adc1_set_attenuation(get_channel(TEMP_5_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_6
    adc2_set_attenuation(get_channel(TEMP_6_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_ADC_7
    adc3_set_attenuation(get_channel(TEMP_7_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_HEATED_BED
    adc1_set_attenuation(get_channel(TEMP_BED_PIN), ADC_ATTEN_11db);
  #endif
  #if HAS_TEMP_CHAMBER
    adc1_set_attenuation(get_channel(TEMP_CHAMBER_PIN), ADC_ATTEN_11db);
  #endif
  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    adc1_set_attenuation(get_channel(FILWIDTH_PIN), ADC_ATTEN_11db);
  #endif*/

  // Note that adc2 is shared with the WiFi module, which has higher priority, so the conversion may fail.
  // That's why we're not setting it up here.

  esp_adc_cal_value_t val_type;
  //printf("val_type\n");
  // Calculate ADC characteristics (i.e., gain and offset factors for each attenuation level)
  for (int i = 0; i < ADC_ATTEN_MAX; i++) {
    val_type = esp_adc_cal_characterize(ADC_UNIT_1, (adc_atten_t)i, ADC_WIDTH_BIT_13, V_REF, &characteristics[i]);

    /*printf("%d ",i);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("eFuse Vref");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Two Point");
    } else {
        printf("Default");
    }
    printf("\n");*/
    // Change attenuation 100mV below the calibrated threshold
    //thresholds[i] = esp_adc_cal_raw_to_voltage(4095, &characteristics[i]);
  }
  //printf("coeff_a:%d,coeff_b:%d,vref:%d\n",characteristics[ADC_ATTEN_DB_11].coeff_a,characteristics[ADC_ATTEN_DB_11].coeff_b,characteristics[ADC_ATTEN_DB_11].vref);
  //chars->coeff_a + chars->coeff_b
}

void HAL_adc_start_conversion(const uint8_t adc_pin) {
  /*if (adc_pin==TEMP_0_PIN) 
  {
    adc1_get_raw((adc1_channel_t)ADC1_CHANNEL_7)>>3;
    HAL_adc_result=adc1_get_raw((adc1_channel_t)ADC1_CHANNEL_7)>>3;
  }
  else if (adc_pin==TEMP_BED_PIN) 
  {
    adc1_get_raw((adc1_channel_t)ADC1_CHANNEL_8)>>3;
    HAL_adc_result=adc1_get_raw((adc1_channel_t)ADC1_CHANNEL_8)>>3;
  }*/

  
  //const adc1_channel_t chan = get_channel(adc_pin);
  uint32_t mv;
  if (adc_pin==TEMP_0_PIN) 
  {
    esp_adc_cal_get_voltage((adc_channel_t)ADC1_CHANNEL_7, &characteristics[ADC_ATTEN_DB_11], &mv);
    HAL_adc_result = raw_0 * 2 / 3 + mv / 3;
    raw_0 = HAL_adc_result; 
  }
  else if (adc_pin==TEMP_BED_PIN) 
  {
    esp_adc_cal_get_voltage((adc_channel_t)ADC1_CHANNEL_8, &characteristics[ADC_ATTEN_DB_11], &mv);
    HAL_adc_result = raw_bed * 2 / 3 + mv / 3;
    raw_bed = HAL_adc_result;
  }
  //esp_adc_cal_get_voltage((adc_channel_t)ADC1_CHANNEL_8, &characteristics[attenuations[ADC1_CHANNEL_8]], &mv);

  /*
  // Change the attenuation level based on the new reading
  adc_atten_t atten;
  if (mv < thresholds[ADC_ATTEN_DB_0] - 100)
    atten = ADC_ATTEN_DB_0;
  else if (mv > thresholds[ADC_ATTEN_DB_0] - 50 && mv < thresholds[ADC_ATTEN_DB_2_5] - 100)
    atten = ADC_ATTEN_DB_2_5;
  else if (mv > thresholds[ADC_ATTEN_DB_2_5] - 50 && mv < thresholds[ADC_ATTEN_DB_6] - 100)
    atten = ADC_ATTEN_DB_6;
  else if (mv > thresholds[ADC_ATTEN_DB_6] - 50)
    atten = ADC_ATTEN_DB_11;
  else return;

  adc1_set_attenuation(chan, atten);*/
}

void analogWrite(pin_t pin, int value) {
  // Use ledc hardware for internal pins
  if (pin < 34) {
    static int cnt_channel = 1, pin_to_channel[40] = { 0 };
    if (pin_to_channel[pin] == 0) {
      ledcAttachPin(pin, cnt_channel);
      ledcSetup(cnt_channel, 490, 8);
      ledcWrite(cnt_channel, value);
      pin_to_channel[pin] = cnt_channel++;
    }
    ledcWrite(pin_to_channel[pin], value);
    return;
  }
  SERIAL_ECHOLN("Cannot write analog to pin>=34");
/*
  int idx = -1;

  // Search Pin
  for (int i = 0; i < numPWMUsed; ++i)
    if (pwmPins[i] == pin) { idx = i; break; }

  // not found ?
  if (idx < 0) {
    // No slots remaining
    if (numPWMUsed >= MAX_PWM_PINS) return;

    // Take new slot for pin
    idx = numPWMUsed;
    pwmPins[idx] = pin;
    // Start timer on first use
    if (idx == 0) HAL_timer_start(PWM_TIMER_NUM, PWM_TIMER_FREQUENCY);

    ++numPWMUsed;
  }

  // Use 7bit internal value - add 1 to have 100% high at 255
  pwmValues[idx] = (value + 1) / 2;*/
}

// Handle PWM timer interrupt
HAL_PWM_TIMER_ISR() {
/*  HAL_timer_isr_prologue(PWM_TIMER_NUM);

  static uint8_t count = 0;

  for (int i = 0; i < numPWMUsed; ++i) {
    if (count == 0)                   // Start of interval
      WRITE(pwmPins[i], pwmValues[i] ? HIGH : LOW);
    else if (pwmValues[i] == count)   // End of duration
      WRITE(pwmPins[i], LOW);
  }

  // 128 for 7 Bit resolution
  count = (count + 1) & 0x7F;

  HAL_timer_isr_epilogue(PWM_TIMER_NUM);*/
}

extern "C" void NULL_Handler()
{
}
#endif // ARDUINO_ARCH_ESP32
