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
#include "hal_gpio.h"

#define DISK_LED LED3

class LoggerTask
{
private:
  static const size_t stackSize = 1000;

  static TaskHandle_t taskHandle;
  static StaticTask_t xTaskBuffer;
  static StackType_t xStack[stackSize];

  static const size_t bufferSize = 1000;

  static MessageBufferHandle_t bufferHandle;
  static StaticMessageBuffer_t messageBufferStruct;
  static uint8_t ucStorageBuffer[bufferSize];

  static char lineBuffer[bufferSize];

  static FATFS fs;
  static FIL file_object;

  static bool loggingEnabled;

  static void activity(void *p);
  static void format();

public:
  LoggerTask();
  TaskHandle_t getTaskHandle();
  void log(char *message);
  bool isLoggingEnabled() { return loggingEnabled; };
};