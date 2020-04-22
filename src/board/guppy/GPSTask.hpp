#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Poster.hpp"

struct
{
    bool valid;
    uint32_t time;
    double lat;
    double lon;
    double alt;
    uint8_t sats;
    double hdop;
} typedef gps_data_t;

class GPSTask
{
public:
    GPSTask(uint8_t priority);
    TaskHandle_t getTaskHandle();
    Poster<gps_data_t> data_post;

private:
    static const size_t stackSize = 2000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activityWrapper(void *p);
    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
