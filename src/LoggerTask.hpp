#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include "support/driver_init.h"
#include "printf.h"
#include <string.h>
#include "ff.h"


#define TASK_EXAMPLE_STACK_SIZE (128 / sizeof(portSTACK_TYPE))
#define TASK_EXAMPLE_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

class LoggerTask
{
  private:
    static const size_t stackSize = 1000;

    static TaskHandle_t taskHandle;
    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[stackSize];

    static void activity(void *p);

    static const size_t bufferSize = 1000;

    static MessageBufferHandle_t bufferHandle;
    static StaticMessageBuffer_t messageBufferStruct;
    static uint8_t ucStorageBuffer[bufferSize];

    static char lineBuffer[bufferSize];

    static FATFS fs;
	  static FIL file_object;

  public:
    LoggerTask();
    TaskHandle_t getTaskHandle();
    void log(char* message);

};