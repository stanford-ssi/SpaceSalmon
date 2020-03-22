
#pragma once

extern "C"
{
#include <math.h>
#include <string.h>
}

#include "Arduino.h"
#include "SPI.h"

class BMI088Gyro
{
public:
  struct Data
    {
        float x;
        float y;
        float z;
    };
  enum Range
  {
    RANGE_2000DPS = 0x00,
    RANGE_1000DPS = 0x01,
    RANGE_500DPS = 0x02,
    RANGE_250DPS = 0x03,
    RANGE_125DPS = 0x04
  };
  enum Odr
  {
    ODR_2000HZ_BW_532HZ = 0x80,
    ODR_2000HZ_BW_230HZ = 0x81,
    ODR_1000HZ_BW_116HZ = 0x82,
    ODR_400HZ_BW_47HZ = 0x83,
    ODR_200HZ_BW_23HZ = 0x84,
    ODR_100HZ_BW_12HZ = 0x85,
    ODR_200HZ_BW_64HZ = 0x86,
    ODR_100HZ_BW_32HZ = 0x87
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
  BMI088Gyro(SPIClass *bus, uint8_t csPin);
  int begin();
  bool setOdr(Odr odr);
  bool setRange(Range range);
  bool pinModeInt3(PinMode mode, PinLevel level);
  bool pinModeInt4(PinMode mode, PinLevel level);
  bool mapDrdyInt3(bool enable);
  bool mapDrdyInt4(bool enable);
  bool getDrdyStatus();
  Data readSensor();


private:
  // available power settings
  enum PowerMode
  {
    PWR_NORMAL = 0x00,
    PWR_SUSPEND = 0x80,
    PWR_DEEP_SUSPEND = 0x20
  };
  // spi
  uint8_t _csPin;
  SPIClass *_spi;
  bool _useSPI;
  const uint8_t SPI_READ = 0x80;
  const uint32_t SPI_CLOCK = 10000000; // 10 MHz
  // buffer for reading from sensor
  uint8_t _buffer[9];
  // constants
  static const uint8_t GYRO_CHIP_ID = 0x0F;
  static const uint8_t GYRO_RESET_CMD = 0xB6;
  static const uint8_t GYRO_ENABLE_DRDY_INT = 0x80;
  static const uint8_t GYRO_DIS_DRDY_INT = 0x00;
  static const uint8_t GYRO_INT_OPENDRAIN = 0x02;
  static const uint8_t GYRO_INT_PUSHPULL = 0x00;
  static const uint8_t GYRO_INT_LVL_HIGH = 0x01;
  static const uint8_t GYRO_INT_LVL_LOW = 0x00;
  // registers
  static const uint8_t GYRO_CHIP_ID_ADDR = 0x00;
  static const uint8_t GYRO_CHIP_ID_MASK = 0xFF;
  static const uint8_t GYRO_CHIP_ID_POS = 0;
  static const uint8_t GYRO_DRDY_ADDR = 0x0A;
  static const uint8_t GYRO_DRDY_MASK = 0x80;
  static const uint8_t GYRO_DRDY_POS = 7;
  static const uint8_t GYRO_RANGE_ADDR = 0x0F;
  static const uint8_t GYRO_RANGE_MASK = 0xFF;
  static const uint8_t GYRO_RANGE_POS = 0;
  static const uint8_t GYRO_ODR_ADDR = 0x10;
  static const uint8_t GYRO_ODR_MASK = 0xFF;
  static const uint8_t GYRO_ODR_POS = 0;
  static const uint8_t GYRO_SOFT_RESET_ADDR = 0x14;
  static const uint8_t GYRO_SOFT_RESET_MASK = 0xFF;
  static const uint8_t GYRO_SOFT_RESET_POS = 0;
  static const uint8_t GYRO_INT_CNTRL_ADDR = 0x15;
  static const uint8_t GYRO_INT_CNTRL_MASK = 0xFF;
  static const uint8_t GYRO_INT_CNTRL_POS = 0;
  static const uint8_t GYRO_INT3_IO_CTRL_ADDR = 0x16;
  static const uint8_t GYRO_INT3_IO_CTRL_MASK = 0x03;
  static const uint8_t GYRO_INT3_IO_CTRL_POS = 0;
  static const uint8_t GYRO_INT4_IO_CTRL_ADDR = 0x16;
  static const uint8_t GYRO_INT4_IO_CTRL_MASK = 0x0C;
  static const uint8_t GYRO_INT4_IO_CTRL_POS = 2;
  static const uint8_t GYRO_INT3_DRDY_ADDR = 0x18;
  static const uint8_t GYRO_INT3_DRDY_MASK = 0x01;
  static const uint8_t GYRO_INT3_DRDY_POS = 0;
  static const uint8_t GYRO_INT4_DRDY_ADDR = 0x18;
  static const uint8_t GYRO_INT4_DRDY_MASK = 0x80;
  static const uint8_t GYRO_INT4_DRDY_POS = 7;
  static const uint8_t GYRO_DATA_ADDR = 0x02;
  // transformation from sensor frame to right hand coordinate system
  // convert deg/s to rad/s
  const float D2R = M_PI / 180.0f;
  // gyro full scale range
  float gyro_range_rads;
  // gyro data
  float gyro_rads[3];
  // self test
  bool selfTest();
  // enable data read interrupt
  bool setDrdyInt(bool enable);
  // command soft reset
  void softReset();
  // power settings
  bool setPower(PowerMode mode);
  // check id
  bool isCorrectId();
  // write / read registers
  void writeRegister(uint8_t subAddress, uint8_t data);
  void readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest);
};