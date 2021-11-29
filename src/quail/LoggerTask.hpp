// #pragma once

// #include <FreeRTOS.h>
// #include <task.h>
// #include <message_buffer.h>
// #include <semphr.h>
// #include <hal_rtos.h>
// #include <string.h>
// #include "ArduinoJson.h"
// #include "SSISD.hpp"
// #include "StrBuffer.hpp"
// #include "Task.hpp"

// #define DISK_LED 3

// typedef enum
// {
//   fatal = 1,
//   error = 2,
//   warning = 4,
//   stats = 8,
//   data = 16,
//   info = 32
// } log_type;

// class LoggerTask : public Task<1000>
// {
// private:
//   static StrBuffer<10000> strBuffer;

//   static char lineBuffer[10000];

//   static char inputLineBuffer[1000];

//   static FATFS fs;
//   static FIL file_object;
//   static FIL shitl_file_object;

//   static bool loggingEnabled;
//   static bool shitlEnabled;

//   void activity();
//   static void readSHITL();
//   static void writeUSB(char *buf);
//   static void writeSD(char *buf);
//   static void format();

// public:
//   LoggerTask(uint8_t priority);
//   void log(const char *message);
//   void log(JsonDocument &jsonDoc);
//   void logJSON(JsonDocument &jsonDoc, const char *id);
//   bool isLoggingEnabled() { return loggingEnabled; };
// };