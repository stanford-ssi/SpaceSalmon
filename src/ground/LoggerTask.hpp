#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include <string.h>
#include "ArduinoJson.h"
#include "StrBuffer.hpp"
#include "Task.hpp"

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

class LoggerTask : public Task<1000>
{
private:

  StrBuffer<10000> logBuffer;

  char logLineBuffer[10000];
  char inputLineBuffer[3000];

  bool loggingEnabled;
  bool shitlEnabled;

  void activity();
  void readSHITL();
  void writeUSB(char *buf);
  void format();

public:
  StrBuffer<3000> inputBuffer;
  LoggerTask(uint8_t priority);
  void log(const char *message);
  void log(JsonDocument &jsonDoc);
  void logJSON(JsonDocument &jsonDoc, const char *id);
  bool isLoggingEnabled() { return loggingEnabled; };
};