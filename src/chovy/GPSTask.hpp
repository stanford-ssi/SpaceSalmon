#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Poster.hpp"
#include "Task.hpp"

struct
{
    bool valid = false;
    uint32_t time = 0;
    double lat = 0.0;
    double lon = 0.0;
    double alt = 0.0;
    uint8_t sats = 0;
    double hdop = 0.0;
} typedef gps_data_t;

class GPSTask : Task<2000>
{
public:
    GPSTask(uint8_t priority);
    Poster<gps_data_t> locationData;

private:
    void activity();
};

#include "main.hpp"
#include "ArduinoJson.h"
