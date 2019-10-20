#pragma once

class AltFilterTask;
struct SensorData;

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "support/driver_init.h"
#include "printf.h"
#include "support/atmel_start_pins.h"
#include "main.hpp"
#include "ArduinoJson.h"
#include "AltFilter.hpp"
#include "FlightPlan.hpp"
#include "periph/OneBattery/OneBattery.hpp"

struct SensorData
{
    ADXL375::Data adxl1_data;
    ADXL375::Data adxl2_data;
    BMP388::Data bmp1_data;
    BMP388::Data bmp2_data;
    BMI088Gyro::Data bmigyro1_data;
    BMI088Gyro::Data bmigyro2_data;
    BMI088Accel::Data bmiaccel1_data;
    BMI088Accel::Data bmiaccel2_data;
    uint32_t tick;
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
    static AltFilter filter;
    static FlightPlan plan;
    static void activity(void *p);

};