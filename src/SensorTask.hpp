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
#include "ArduinoJson.h"
#include "SPITest.hpp"

#define TASK_EXAMPLE_STACK_SIZE (128 / sizeof(portSTACK_TYPE))
#define TASK_EXAMPLE_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

class SensorTask
{
private:
    TaskHandle_t taskHandle;
    static void sensorActivity(void *p);
public:
    SensorTask();
    TaskHandle_t getTaskHandle();
};

