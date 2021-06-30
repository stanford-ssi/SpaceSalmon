#pragma once

#include <hal_rtos.h>
#include <string.h>
#include "ArduinoJson.h"
#include "StrBuffer.hpp"
#include "BinarySemaphore.hpp"
#include "MsgBuffer.hpp"
#include "LoggerTask.hpp"

#define RADIO_LED 1

class TXTask
{
private:

  static const size_t stackSize = 1000;

  static TaskHandle_t taskHandle;
  static StaticTask_t xTaskBuffer;
  static StackType_t xStack[stackSize];

  static void activity_wrapper(void *p);
  void activity();

  void log(log_type t, const char *msg);

  uint8_t log_mask = fatal | error | warning | data | info;

public:
  TXTask(uint8_t priority);
  TaskHandle_t getTaskHandle();
  
};