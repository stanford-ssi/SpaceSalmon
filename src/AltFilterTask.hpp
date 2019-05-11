#pragma once

class AltFilterTask;
struct SensorData;

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "support/driver_init.h"
#include "printf.h"
#include "support/atmel_start_pins.h"
#include "main.hpp"
#include "ArduinoJson.h"
#include "AltFilter.hpp"

struct SensorData
{
    ADXL375::Data adxl375_data;
    BMP3xx::Data bmp388_data;
    BMI088Gyro::Data bmi088gyro_data;
    BMI088Accel::Data bmi088accel_data;
};

class AltFilterTask
{
public:
    AltFilterTask();
    TaskHandle_t getTaskHandle();
    void newSensorData(SensorData data);

private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static const size_t bufferSize = 1000;
    static MessageBufferHandle_t bufferHandle;
    static StaticMessageBuffer_t messageBufferStruct;
    static uint8_t ucStorageBuffer[bufferSize];

    static SensorData data;
    static void activity(void *p);

};