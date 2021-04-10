#pragma once

#define SENSOR_LED 2

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "Task.hpp"

class SensorTask : Task<2000>
{
private:
    void activity();

public:
    SensorTask(uint8_t priority);
};

#include "main.hpp"
#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "ArduinoJson.h"
#include "SensorData.h"
// #include "AltFilterTask.h"
// #include "AltFilter.hpp"