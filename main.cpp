#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "Arduino.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "SoftwareSerial.h"
#include "inc/MarlinConfig.h"
#include "sd/cardreader.h"
#include "feature/powerloss.h"
#include "gcode/queue.h"

#if CONFIG_AUTOSTART_ARDUINO
#include "USB.h"
#include "USBCDC.h"
#include "driver/uart.h"

#define FIL_RUNOUT_PIN                         130
#define POWER_LOSS_PIN                         145

bool loopTaskWDTEnabled;
volatile uint8_t pwr_ok=100;
volatile int32_t e_pos,sensor_pos,filament_cnt;
extern uint8_t pause_print;
extern void write_to_lcd_P(PGM_P const message);
extern void malyan_Startup();
extern void malyan_Idle();

USBCDC USBSerial;

uint8_t virtualpin[127];

TaskHandle_t loopTaskHandle = NULL;
void loopTask(void *pvParameters)
{
    for(;;) {
        if(loopTaskWDTEnabled){
            esp_task_wdt_reset();
        }
        for (uint8_t i=0;i<10;i++) 
        loop();
        yield();

        if (serialEventRun) serialEventRun();
    }
}

TaskHandle_t usrTaskHandle = NULL;
void usrTask(void *pvParameters)
{
    uint8_t t=0;
    malyan_Startup();

    for (;;)
    {
        if (t++ == 50) 
		{
			malyan_Idle();
			t=0;
		}
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void gpio_init()
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_MTDI_GPIO41);
    io_conf.pin_bit_mask = (uint64_t)1<<41;
    gpio_config(&io_conf);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_MTMS_GPIO42);
    io_conf.pin_bit_mask = (uint64_t)1<<42;
    gpio_config(&io_conf);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_DAC_1_U, FUNC_DAC_1_U1TXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_DAC_2_U, FUNC_DAC_2_U1RXD);
}

extern "C" void app_main()
{
    gpio_init();
    Serial.begin(115200);
    Serial1.begin(128000);

    USB.begin();
    USBSerial.begin();

    loopTaskWDTEnabled = false;
    initArduino();
    setup();

    xTaskCreateUniversal(usrTask, "usrTask", 8192, NULL, 4, &usrTaskHandle, NULL);
    xTaskCreateUniversal(loopTask, "loopTask", 8192, NULL, 3, &loopTaskHandle, NULL);
}

#endif
