#pragma once

#include <hal_rtos.h>
#include <string.h>
#include "ArduinoJson.h"
#include "MsgBuffer.hpp"
#include "event_groups.h"
#include "LoggerTask.hpp"
#include "Mutex.hpp"
#include "Task.hpp"

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
  uint8_t sf = 12;
  uint8_t cr = 8;
  uint8_t syncword = SX126X_SYNC_WORD_PRIVATE;
  int8_t power = 22;
  float currentLimit = 139.0F;
  uint8_t preambleLength = 8;
  float TcxoVoltage = 1.6F;
  uint8_t log_mask = fatal | error | warning | stats | data | info;
} typedef radio_settings_t;

class RadioTask : public Task<1000>
{
private:

  static RadioTask * glob_ptr;

  void activity();

  static void radioISR();

  void applySettings(radio_settings_t &settings);

  MsgBuffer<packet_t, 1050> txbuf;
  MsgBuffer<packet_t, 1050> rxbuf;

  StaticEventGroup_t evbuf;
  EventGroupHandle_t evgroup;

  SPIClass spi = SPIClass(&sercom5, 10, 12, 13, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_2);// TODO: set these hard-coded vals in constructor
  Module mod = Module(11, 14, 16, 15, spi, SPISettings());// TODO: set these hard-coded vals in constructor
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
  bool sendPacket(packet_t &packet);
  void waitForPacket(packet_t &packet);
  void setSettings(radio_settings_t &settings);
  bool isIdle();
};