#ifndef _DATAFILE_H_
#define _DATAFILE_H_

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define portTICK 10

// #define USE_MOTORB

/* MOTORA 引脚  */
#define EA1 23
#define EA2 21
#define AIN1 26
#define AIN2 27
#define AINPWM 14
/* MOTORB 引脚 与其他引脚复用了 */
#define EB1 22
#define EB2 19

#ifdef USE_MOTORB
#define BIN1 25
#define BIN2 15
#define BINPWM 4
#else
/* LCD 引脚与电机引脚复用  */
#define LCD_BLK 15
#define LCD_CS 25
#define WS2812 4
#endif

#define LCD_SCL 18
#define LCD_SDA 5
#define LCD_RES 17
#define LCD_DC 16
/* MPU6050 引脚 */
#define MPU_SDA 32
#define MPU_SCL 33

/* 舵机引脚 */
#define SERVER 13
/* 蜂鸣器引脚 */
#define BEEP 12

#endif // ! _DATAFILE_H_