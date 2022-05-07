#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <message_buffer.h>
#include <semphr.h>
#include <hal_rtos.h>
#include <string.h>
#include "ArduinoJson.h"
#include "SSISD.hpp"
#include "StrBuffer.hpp"
#include "Task.hpp"

#define DISK_LED 4

typedef enum
{
  fatal = 1,
  error = 2,
  warning = 4,
  stats = 8,
  data = 16,
  info = 32
} log_type;

class LoggerTask : public Task<1000>
{
private:
  StrBuffer<10000> strBuffer;
  char lineBuffer[10000];
  char inputLineBuffer[1000];
  FATFS fs;
  FIL file_object;
  FIL shitl_file_object;
  bool loggingEnabled = true;
  bool shitlEnabled = false;

  void activity();
  void readSHITL();
  void writeUSB(char *buf);
  void writeSD(char *buf);
  void format();
  void findFile(char* filename, size_t filesize, int* lognum);

  const char *ErrorNames[3] = {
    "A log file is corrupted",
    "Could Not Mount Disk",
    "Trying to open log file"
  };

public:
  LoggerTask(uint8_t priority);
  void log(const char *message);
  void log(JsonDocument &jsonDoc);
  void logJSON(JsonDocument &jsonDoc, const char *id);
  bool isLoggingEnabled() { return loggingEnabled; };
};