#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "driver_init.h"
#include "printf.h"
#include "atmel_start_pins.h"
#include "ADXL375.hpp"
#include "BMP3xx.hpp"
#include "BMI088.hpp"

#define TASK_EXAMPLE_STACK_SIZE (128 / sizeof(portSTACK_TYPE))
#define TASK_EXAMPLE_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

class SensorTask
{
private:
    static const size_t stackSize = 1000;

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