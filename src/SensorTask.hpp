#pragma once

class SensorTask;

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "support/driver_init.h"
#include "printf.h"
#include "support/atmel_start_pins.h"
#include "ADXL375.hpp"
#include "BMP3xx.hpp"
#include "BMI088.hpp"
#include "main.hpp"
#include "ArduinoJson.h"

#define SENSOR_LED LED2

class SensorTask
{
private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activity(void *p);

    static StaticSemaphore_t mutexBuffer[4];

    SemaphoreHandle_t adxl375_mutex;
    SemaphoreHandle_t bmp388_mutex;
    SemaphoreHandle_t bmi088gyro_mutex;
    SemaphoreHandle_t bmi088accel_mutex;

    ADXL375::Data adxl375_data;
    BMP3xx::Data bmp388_data;
    BMI088Gyro::Data bmi088gyro_data;
    BMI088Accel::Data bmi088accel_data;

public:
    SensorTask();
    TaskHandle_t getTaskHandle();
};