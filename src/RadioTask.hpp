#pragma once

#include <hal_rtos.h>
#include <string.h>
#include "ArduinoJson.h"
#include "MsgBuffer.hpp"
#include "event_groups.h"
#include "LoggerTask.hpp"
#include "Mutex.hpp"

#define RADIOLIB_STATIC_ONLY
#define RADIOLIB_GODMODE
#include "SX1262S.hpp"

struct
{
  uint8_t len = 0;
  uint8_t data[255];
  float rssi = 0.0;
  float snr = 0.0;
} typedef packet_t;

struct
{
  float freq = 433.551F;
  float bw = 125.0F;
  uint8_t sf = 7;
  uint8_t cr = 8;
  uint8_t syncword = SX126X_SYNC_WORD_PRIVATE;
  int8_t power = 0;
  float currentLimit = 139.0F;
  uint8_t preambleLength = 8;
  uint8_t log_mask = fatal | error | warning | stats | data | info;
} typedef radio_settings_t;

class RadioTask
{
private:

  static RadioTask& glob_ptr;
  static const size_t stackSize = 1000;
  static TaskHandle_t taskHandle;
  static StaticTask_t xTaskBuffer;
  static StackType_t xStack[stackSize];

  static void activity_wrapper(void *p);
  void activity();

  static void radioISR();

  void applySettings(radio_settings_t &settings);

  MsgBuffer<packet_t, 1050> txbuf;
  MsgBuffer<packet_t, 1050> rxbuf;

  StaticEventGroup_t evbuf;
  EventGroupHandle_t evgroup;

  SPIClass spi = SPIClass(&sercom4, 21, 23, 22, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_2);
  Module mod = Module(24, 25, 0, 26, spi, SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SX1262S lora = SX1262S(&mod);

  MsgBuffer<radio_settings_t, 1000> settingsBuf;

  uint32_t rx_success_counter = 0;
  uint32_t rx_failure_counter = 0;
  uint32_t tx_success_counter = 0;
  uint32_t tx_failure_counter = 0;

  void log(log_type t, const char *msg);
  void logPacket(const char *msg, packet_t &packet);

  radio_settings_t getSettings();
  radio_settings_t settings;
  Mutex settingsMx;

  void logStats();

public:
  RadioTask(uint8_t priority);
  TaskHandle_t getTaskHandle();
  bool sendPacket(packet_t &packet);
  void waitForPacket(packet_t &packet);
  void setSettings(radio_settings_t &settings);
};