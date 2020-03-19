#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include <string.h>
#include "ff.h"
#include "ArduinoJson.h"
#include "SSISD.hpp"

#define DISK_LED 3

class LoggerTask
{
private:
  static const size_t stackSize = 1000;

  static TaskHandle_t taskHandle;
  static StaticTask_t xTaskBuffer;
  static StackType_t xStack[stackSize];

  static const size_t bufferSize = 10000; //at least 10 lines

  static MessageBufferHandle_t bufferHandle;
  static StaticMessageBuffer_t messageBufferStruct;
  static uint8_t ucStorageBuffer[bufferSize];

  static char lineBuffer[10000];

  static char inputLineBuffer[1000];

  static FATFS fs;
  static FIL file_object;
  static FIL shitl_file_object;

  static bool loggingEnabled;
  static bool shitlEnabled;

  static void activity(void *p);
  static void readSHITL();
  static void writeUSB(char* buf);
  static void writeSD(char* buf);
  static void format();

public:
  LoggerTask();
  TaskHandle_t getTaskHandle();
  void log(const char *message);
  void logJSON(JsonDocument & jsonDoc, const char* id);
  bool isLoggingEnabled() { return loggingEnabled; };
  
};