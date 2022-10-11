#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "clock_gpio.h"

#define HOLD_TIME 1

int SMG_DEF[] = {SMG_0, SMG_1, SMG_2, SMG_3, SMG_4, SMG_5, SMG_6, SMG_7, SMG_8, SMG_9};
int SMG_POS[] = {SMG_BIT0, SMG_BIT1, SMG_BIT2, SMG_BIT3, SMG_BIT4, SMG_BIT5};

QueueHandle_t mode_queue;
QueueHandle_t level_queue;

void show() {
    gpio_set_level(PIN_NUM_ST, 1);
    for(int i = HOLD_TIME; i > 0; i--){}
    gpio_set_level(PIN_NUM_ST, 0);
}

void put_bit(int bit) {
    if(bit == 0) {
        gpio_set_level(PIN_NUM_DS, 1);
        gpio_set_level(PIN_NUM_SH, 1);
    }
    else {
        gpio_set_level(PIN_NUM_DS, 0);
        gpio_set_level(PIN_NUM_SH, 1);
    }
    for(int i = HOLD_TIME; i > 0; i--){}
    gpio_set_level(PIN_NUM_SH, 0);
    for(int i = HOLD_TIME; i > 0; i--){}
}

void put_byte(int byte) {
    for(int i=7; i>=0 ; i--) {
        put_bit((byte & (1 << i)) >> i);
    }
    // show();
}

void show_num(int smg_num, int smg_bit) {
    put_byte(SMG_POS[smg_bit]);
    put_byte(SMG_DEF[smg_num]);
    show();
}

void show_none() {
    put_byte(0xff);
    put_byte(0);
    show();
}

void clear_smg() {
    put_byte(0);
    put_byte(0);
    show();
}

void gpio_init() {
    gpio_config_t config_gpio_out = {
        .pin_bit_mask = (1ULL << PIN_NUM_SH) | (1ULL << PIN_NUM_ST) | (1ULL << PIN_NUM_DS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&config_gpio_out));

    gpio_config_t config_gpio_in = {
        .pin_bit_mask = (1ULL << PIN_NUM_KEY0) | (1ULL << PIN_NUM_KEY1) | (1ULL << PIN_NUM_KEY2),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&config_gpio_in));

    // gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
    // gpio_isr_handler_add(PIN_NUM_KEY0, gpio_isr_handler, NULL);

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, PIN_NUM_TX, PIN_NUM_RX, -1, -1));
    int uart_buffer_size = (1024 * 4);
    QueueHandle_t uart_queue;
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
}

/// @brief !!! It won't show the number consistantly.
/// @param num0 
/// @param num1 
/// @param num2 
void show_time(int num0, int num1, int num2, int bright_level) {
    switch (bright_level)
    {
    case 0:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();
        break;
    case 1:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 1000; i>0; i--);
        break;
    case 2:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 2000; i>0; i--);
        break;
    case 3:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 4000; i>0; i--);
        break;
    case 4:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 8000; i>0; i--);
        break;
    case 5:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 16000; i>0; i--);
        break;
    case 6:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 32000; i>0; i--);
        break;
    case 7:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 48000; i>0; i--);
        break;
    case 8:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 64000; i>0; i--);
        break;
    case 9:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();for(int i = 96000; i>0; i--);
        break;
    default:
        show_num((num0 % 100)/10, 0);
        show_num((num0 % 10), 1);
        show_num((num1 % 100)/10, 2);
        show_num((num1 % 10), 3);
        show_num((num2 % 100)/10, 4);
        show_num((num2 % 10), 5);
        show_none();
        break;
    }
    
}

void gps_restart() {
    gpio_set_level(PIN_NUM_GPSOFF, 0);
    vTaskDelay(1000);
    gpio_set_level(PIN_NUM_GPSOFF, 1);
}