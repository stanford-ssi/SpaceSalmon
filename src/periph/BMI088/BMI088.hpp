/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2018 Bolder Flight Systems
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
* and associated documentation files (the "Software"), to deal in the Software without restriction, 
* including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or 
* substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

extern "C"
{
#include <hal_spi_m_os.h>
#include <hal_gpio.h>
#include <math.h>
#include <string.h>
#include <hal_delay.h>
#include "../Sensor.hpp"
}

#include "BMI088Accel.hpp"
#include "BMI088Gyro.hpp"

class BMI088 : public Sensor
{
public:
  enum AccelRange
  {
    ACCEL_RANGE_3G,
    ACCEL_RANGE_6G,
    ACCEL_RANGE_12G,
    ACCEL_RANGE_24G
  };
  enum GyroRange
  {
    GYRO_RANGE_2000DPS,
    GYRO_RANGE_1000DPS,
    GYRO_RANGE_500DPS,
    GYRO_RANGE_250DPS,
    GYRO_RANGE_125DPS
  };
  enum Odr
  {
    ODR_2000HZ,
    ODR_1000HZ,
    ODR_400HZ
  };
  enum DrdyPin
  {
    PIN_1,
    PIN_2
  };
  enum SyncPin
  {
    PIN_3,
    PIN_4
  };
  enum PinMode
  {
    PUSH_PULL,
    OPEN_DRAIN
  };
  enum PinLevel
  {
    ACTIVE_HIGH,
    ACTIVE_LOW
  };
  BMI088(struct spi_m_os_descriptor *bus, uint8_t accel_cs, uint8_t gyro_cs, char* id);
  int init();
  bool setOdr(Odr odr);
  bool setRange(AccelRange accel_range, GyroRange gyro_range);
  bool mapDrdy(DrdyPin pin);
  bool mapSync(SyncPin pin);
  bool pinModeDrdy(PinMode mode, PinLevel level);

  BMI088Accel *accel;
  BMI088Gyro *gyro;

private:

  uint8_t drdy_pin;
  // constants
  static const uint8_t ACC_DISABLE = 0;
  static const uint8_t ACC_ENABLE = 1;
  static const uint8_t ACC_DATA_SYNC_LEN = 1;
  static const uint16_t ACC_DATA_SYNC_MODE_MASK = 0x0003;
  static const uint16_t ACC_DATA_SYNC_MODE_OFF = 0x00;
  static const uint16_t ACC_DATA_SYNC_MODE_400HZ = 0x01;
  static const uint16_t ACC_DATA_SYNC_MODE_1000HZ = 0x02;
  static const uint16_t ACC_DATA_SYNC_MODE_2000HZ = 0x03;
  static const uint8_t ACC_INTA_DISABLE = 0x00;
  static const uint8_t ACC_INTA_ENABLE = 0x01;
  static const uint8_t ACC_INTB_DISABLE = 0x00;
  static const uint8_t ACC_INTB_ENABLE = 0x02;
  // registers
  static const uint8_t ACC_INIT_CTRL_ADDR = 0x59;
  static const uint8_t ACC_FEATURE_LSB_ADDR = 0x5B;
  static const uint8_t ACC_FEATURE_MSB_ADDR = 0x5C;
  static const uint8_t ACC_FEATURE_CFG_ADDR = 0x5E;
  static const uint8_t ACC_INTERNAL_STATUS_ADDR = 0x2A;
  static const uint8_t ACC_DATA_SYNC_ADDR = 0x02;
  static const uint8_t ACC_INT1_MAP_ADDR = 0x56;
  static const uint8_t ACC_INT2_MAP_ADDR = 0x57;
  bool writeFeatureConfig();
  void updateFeatureConfig(uint8_t addr, uint8_t count, const uint16_t *data);
};
