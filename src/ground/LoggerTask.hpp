#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include <string.h>
#include "ArduinoJson.h"
#include "StrBuffer.hpp"

#define DISK_LED 3

enum log_type
{
  fatal = 1,
  error = 2,
  warning = 4,
  stats = 8,
  data = 16,
  info = 32
};

class LoggerTask
{
private:
  static const size_t stackSize = 1000;

  static TaskHandle_t taskHandle;
  static StaticTask_t xTaskBuffer;
  static StackType_t xStack[stackSize];

  static StrBuffer<10000> logBuffer;


  static char logLineBuffer[10000];
  static char inputLineBuffer[3000];

  static bool loggingEnabled;
  static bool shitlEnabled;

  static void activity(void *p);
  static void readSHITL();
  static void writeUSB(char *buf);
  static void format();

public:
  static StrBuffer<3000> inputBuffer;
  LoggerTask(uint8_t priority);
  TaskHandle_t getTaskHandle();
  void log(const char *message);
  void log(JsonDocument &jsonDoc);
  void logJSON(JsonDocument &jsonDoc, const char *id);
  bool isLoggingEnabled() { return loggingEnabled; };
};