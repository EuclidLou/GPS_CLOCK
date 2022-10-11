#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/time.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/gptimer.h"
#include "clock_gpio.h"
#include "main.h"




void Atask(void *pvParameters) {
    uint16_t mode = 0;
    int bright_level = 0;
    time_t trans_time_sec;
    struct tm date_and_time = {0};

    for(;;) {
        xQueuePeek(mode_queue, &mode, 0);
        xQueuePeek(level_queue, &bright_level, 0);
        time(&trans_time_sec);
        localtime_r(&trans_time_sec, &date_and_time);

        if (mode == 0) {
            show_time(date_and_time.tm_hour, date_and_time.tm_min, date_and_time.tm_sec, bright_level);
        }
        else if (mode == 1) {
            show_time(date_and_time.tm_year, date_and_time.tm_mon + 1, date_and_time.tm_mday, bright_level);
        }
         
    }
}

void Btask(void *pvParameters) {
    TickType_t lastwaketime;
    lastwaketime = xTaskGetTickCount();

    time_t trans_time_sec;
    struct tm date_and_time;
    struct timeval val;
    date_and_time.tm_sec   = 0;
    date_and_time.tm_min   = 0;
    date_and_time.tm_hour  = 0;
    date_and_time.tm_year  = 0;
    date_and_time.tm_mon   = 0;
    date_and_time.tm_mday  = 0;
    date_and_time.tm_isdst= -1;

    int init_done = 0;
    int correct_done = 0;
    uint8_t data[1024];
    uint8_t time_data[6];
    uint8_t date_data[6];
    int length = 1024;

    for(int i=60; i>0; i--) { // initialize the time table at the reboot , set 60s timeout.
        length = uart_read_bytes(UART_NUM_1, data, length, 1000);
        for(int pos=0; pos<1000; pos++) {
            if((data[pos] == '$') && (data[pos+1] == 'G') && (data[pos+2] == 'N') && (data[pos+3] == 'Z') && (data[pos+4] == 'D') && (data[pos+5] == 'A') && (data[pos+6] == ',') && (data[pos+7] < 58) && (data[pos+8] > 47)) {
                // get current time
                time_data[0] = data[pos +  7];
                time_data[1] = data[pos +  8];
                time_data[2] = data[pos +  9];
                time_data[3] = data[pos + 10];
                time_data[4] = data[pos + 11];
                time_data[5] = data[pos + 12];
                // get current date
                date_data[0] = data[pos + 18];
                date_data[1] = data[pos + 19];
                date_data[2] = data[pos + 21];
                date_data[3] = data[pos + 22];
                date_data[4] = data[pos + 26];
                date_data[5] = data[pos + 27];
                date_and_time.tm_year = 2000 + (date_data[5]-48) + (10 * (date_data[4]-48)) - 1900;
                date_and_time.tm_mon = ((date_data[3]-48) + (10 * (date_data[2]-48))-1);
                date_and_time.tm_mday = ((date_data[1]-48) + (10 * (date_data[0]-48)));
                date_and_time.tm_sec = (time_data[5]-48) + (10 * (time_data[4]-48));
                date_and_time.tm_min = (time_data[3]-48) + (10 * (time_data[2]-48));
                date_and_time.tm_hour = (time_data[1]-48) + (10 * (time_data[0]-48));

                trans_time_sec = mktime(&date_and_time);
                val.tv_sec = trans_time_sec + 8*60*60; 
                val.tv_usec = 0;
                settimeofday(&val, NULL);
                init_done = 1;
                break;
            }
        }
        if(init_done == 1) {init_done = 0; break;}
        vTaskDelayUntil(&lastwaketime, 1000);
    }

    
    for(;;) { // main time correct loop: try to correct the time 5 times every day
        for(int try_time = 5; try_time>0; try_time--) {
            length = uart_read_bytes(UART_NUM_1, data, length, 1000);
            for(int pos=0; pos<1000; pos++) {
                if((data[pos] == '$') && (data[pos+1] == 'G') && (data[pos+2] == 'N') && (data[pos+3] == 'Z') && (data[pos+4] == 'D') && (data[pos+5] == 'A') && (data[pos+6] == ',') && (data[pos+7] < 58) && (data[pos+8] > 47)) {
                    // get current time
                    time_data[0] = data[pos +  7];
                    time_data[1] = data[pos +  8];
                    time_data[2] = data[pos +  9];
                    time_data[3] = data[pos + 10];
                    time_data[4] = data[pos + 11];
                    time_data[5] = data[pos + 12];
                    // get current date
                    date_data[0] = data[pos + 18];
                    date_data[1] = data[pos + 19];
                    date_data[2] = data[pos + 21];
                    date_data[3] = data[pos + 22];
                    date_data[4] = data[pos + 26];
                    date_data[5] = data[pos + 27];
                    date_and_time.tm_year = 2000 + (date_data[5]-48) + (10 * (date_data[4]-48)) - 1900;
                    date_and_time.tm_mon = ((date_data[3]-48) + (10 * (date_data[2]-48))-1);
                    date_and_time.tm_mday = ((date_data[1]-48) + (10 * (date_data[0]-48)));
                    date_and_time.tm_sec = (time_data[5]-48) + (10 * (time_data[4]-48));
                    date_and_time.tm_min = (time_data[3]-48) + (10 * (time_data[2]-48));
                    date_and_time.tm_hour = (time_data[1]-48) + (10 * (time_data[0]-48));

                    trans_time_sec = mktime(&date_and_time);
                    val.tv_sec = trans_time_sec + 8*60*60; 
                    val.tv_usec = 0;
                    settimeofday(&val, NULL);
                    correct_done = 1;
                    break;
                }
            }
            if(correct_done == 1){correct_done = 0; break;}
            vTaskDelayUntil(&lastwaketime, 1000);
        }
        vTaskDelayUntil(&lastwaketime, 1000*60*60*24);
    }
}

void Ctask(void *pvParameters) {
    TickType_t lastwaketime;
    lastwaketime = xTaskGetTickCount();
    uint16_t mode = 0;
    int bright_level = 0;
    xQueueSendToBack(mode_queue, &mode, 0);
    xQueueSendToBack(level_queue, &bright_level, 0);
    for(;;) {
        if(gpio_get_level(PIN_NUM_KEY0)==0) {
            xQueueReceive(mode_queue, &mode, 0);
            if(mode == 0) {mode = 1;}
            else {mode = 0;}
            xQueueSendToBack(mode_queue, &mode, 0);
            vTaskDelayUntil(&lastwaketime, 500);
        }
        if(gpio_get_level(PIN_NUM_KEY1)==0) {
            xQueueReceive(level_queue, &bright_level, 0);
            if(bright_level < 9) {bright_level ++;}
            xQueueSendToBack(level_queue, &bright_level, 0);
            vTaskDelayUntil(&lastwaketime, 200);
        }
        if(gpio_get_level(PIN_NUM_KEY2)==0) {
            xQueueReceive(level_queue, &bright_level, 0);
            if(bright_level > 0) {bright_level --;}
            xQueueSendToBack(level_queue, &bright_level, 0);
            vTaskDelayUntil(&lastwaketime, 200);
        }
        vTaskDelayUntil(&lastwaketime, 1);
    }
}

void app_main(void)
{
    gpio_init();
    mode_queue = xQueueCreate(1, sizeof(uint16_t));
    level_queue = xQueueCreate(1, sizeof(int));
    xTaskCreate(Atask, "SHOW THE NUM", 65536, NULL, 1, NULL);
    xTaskCreate(Btask, "GET THE TIME", 65536, NULL, 2, NULL);
    xTaskCreate(Ctask, "GET THE INTER", 65536, NULL, 3, NULL);
}
