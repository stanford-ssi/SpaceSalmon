#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "printf.h"
#include "atmel_start_pins.h"
#include "testing/SPITest.hpp"
#include "driver_init.h"

#define TASK_EXAMPLE_STACK_SIZE (128 / sizeof(portSTACK_TYPE))
#define TASK_EXAMPLE_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

class SquibTask
{
private:
    TaskHandle_t taskHandle;
    static void activity(void *p);
public:
    SquibTask();
    TaskHandle_t getTaskHandle();
};

