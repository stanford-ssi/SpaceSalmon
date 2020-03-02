#pragma once

extern "C"
{
#include <math.h>
#include <string.h>
}

#include "Arduino.h"
#include "SPI.h"

class BMI088Accel
{
  public:
    struct Data
    {
        float x;
        float y;
        float z;
        uint32_t time;
        float temp;
    };

    enum Range
    {
        RANGE_3G = 0x00,
        RANGE_6G = 0x01,
        RANGE_12G = 0x02,
        RANGE_24G = 0x03
    };

    enum Odr
    {
        ODR_1600HZ_BW_280HZ,
        ODR_1600HZ_BW_234HZ,
        ODR_1600HZ_BW_145HZ,
        ODR_800HZ_BW_230HZ,
        ODR_800HZ_BW_140HZ,
        ODR_800HZ_BW_80HZ,
        ODR_400HZ_BW_145HZ,
        ODR_400HZ_BW_75HZ,
        ODR_400HZ_BW_40HZ,
        ODR_200HZ_BW_80HZ,
        ODR_200HZ_BW_38HZ,
        ODR_200HZ_BW_20HZ,
        ODR_100HZ_BW_40HZ,
        ODR_100HZ_BW_19HZ,
        ODR_100HZ_BW_10HZ,
        ODR_50HZ_BW_20HZ,
        ODR_50HZ_BW_9HZ,
        ODR_50HZ_BW_5HZ,
        ODR_25HZ_BW_10HZ,
        ODR_25HZ_BW_5HZ,
        ODR_25HZ_BW_3HZ,
        ODR_12_5HZ_BW_5HZ,
        ODR_12_5HZ_BW_2HZ,
        ODR_12_5HZ_BW_1HZ
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

    BMI088Accel(SPIClass *bus, uint8_t csPin);
    int begin();
    bool setOdr(Odr odr);
    bool setRange(Range range);
    bool pinModeInt1(PinMode mode, PinLevel level);
    bool pinModeInt2(PinMode mode, PinLevel level);
    bool mapDrdyInt1(bool enable);
    bool mapDrdyInt2(bool enable);
    bool getDrdyStatus();
    Data readSensor();
    float getTemperature_C();
    uint64_t getTime_ps();
    void estimateBias();

  private:
    // allow class Bmi088 access to private members
    friend class BMI088;
    // int pin input or output selection
    enum PinIO
    {
        PIN_INPUT,
        PIN_OUTPUT
    };
    // spi
    uint8_t _csPin;
    SPIClass *_spi;
    const uint8_t SPI_READ = 0x80;
    const uint32_t SPI_CLOCK = 10000000; // 10 MHz
    // buffer for reading from sensor
    uint8_t _buffer[9];
    // constants
    static const uint8_t ACC_CHIP_ID = 0x1E;
    static const uint8_t ACC_RESET_CMD = 0xB6;
    static const uint8_t ACC_ENABLE_CMD = 0x04;
    static const uint8_t ACC_DISABLE_CMD = 0x00;
    static const uint8_t ACC_SUSPEND_MODE_CMD = 0x03;
    static const uint8_t ACC_ACTIVE_MODE_CMD = 0x00;
    static const uint8_t ACC_INT_INPUT = 0x11;
    static const uint8_t ACC_INT_OUTPUT = 0x08;
    static const uint8_t ACC_INT_OPENDRAIN = 0x04;
    static const uint8_t ACC_INT_PUSHPULL = 0x00;
    static const uint8_t ACC_INT_LVL_HIGH = 0x02;
    static const uint8_t ACC_INT_LVL_LOW = 0x00;
    static const uint8_t ACC_POS_SELF_TEST = 0x0D;
    static const uint8_t ACC_NEG_SELF_TEST = 0x09;
    static const uint8_t ACC_DIS_SELF_TEST = 0x00;
    // registers
    static const uint8_t ACC_CHIP_ID_ADDR = 0x00;
    static const uint8_t ACC_CHIP_ID_MASK = 0xFF;
    static const uint8_t ACC_CHIP_ID_POS = 0;
    static const uint8_t ACC_FATAL_ERR_ADDR = 0x02;
    static const uint8_t ACC_FATAL_ERR_MASK = 0x01;
    static const uint8_t ACC_FATAL_ERR_POS = 0;
    static const uint8_t ACC_ERR_CODE_ADDR = 0x02;
    static const uint8_t ACC_ERR_CODE_MASK = 0x1C;
    static const uint8_t ACC_ERR_CODE_POS = 2;
    static const uint8_t ACC_DRDY_ADDR = 0x03;
    static const uint8_t ACC_DRDY_MASK = 0x80;
    static const uint8_t ACC_DRDY_POS = 7;
    static const uint8_t ACC_ODR_ADDR = 0x40;
    static const uint8_t ACC_ODR_MASK = 0xFF;
    static const uint8_t ACC_ODR_POS = 0;
    static const uint8_t ACC_RANGE_ADDR = 0x41;
    static const uint8_t ACC_RANGE_MASK = 0x03;
    static const uint8_t ACC_RANGE_POS = 0;
    static const uint8_t ACC_INT1_IO_CTRL_ADDR = 0x53;
    static const uint8_t ACC_INT1_IO_CTRL_MASK = 0x1F;
    static const uint8_t ACC_INT1_IO_CTRL_POS = 0;
    static const uint8_t ACC_INT2_IO_CTRL_ADDR = 0x54;
    static const uint8_t ACC_INT2_IO_CTRL_MASK = 0x1F;
    static const uint8_t ACC_INT2_IO_CTRL_POS = 0;
    static const uint8_t ACC_INT1_DRDY_ADDR = 0x58;
    static const uint8_t ACC_INT1_DRDY_MASK = 0x04;
    static const uint8_t ACC_INT1_DRDY_POS = 2;
    static const uint8_t ACC_INT2_DRDY_ADDR = 0x58;
    static const uint8_t ACC_INT2_DRDY_MASK = 0x40;
    static const uint8_t ACC_INT2_DRDY_POS = 6;
    static const uint8_t ACC_SELF_TEST_ADDR = 0x6D;
    static const uint8_t ACC_SELF_TEST_MASK = 0xFF;
    static const uint8_t ACC_SELF_TEST_POS = 0;
    static const uint8_t ACC_PWR_CONF_ADDR = 0x7C;
    static const uint8_t ACC_PWR_CONF_MASK = 0xFF;
    static const uint8_t ACC_PWR_CONF_POS = 0;
    static const uint8_t ACC_PWR_CNTRL_ADDR = 0x7D;
    static const uint8_t ACC_PWR_CNTRL_MASK = 0xFF;
    static const uint8_t ACC_PWR_CNTRL_POS = 0;
    static const uint8_t ACC_SOFT_RESET_ADDR = 0x7E;
    static const uint8_t ACC_SOFT_RESET_MASK = 0xFF;
    static const uint8_t ACC_SOFT_RESET_POS = 0;
    static const uint8_t ACC_ACCEL_DATA_ADDR = 0x12;
    static const uint8_t ACC_TEMP_DATA_ADDR = 0x22;
    // convert G to m/s/s
    const float G = 9.807f;
    // accel full scale range
    float accel_range_mss;
    // accel data
    float accel_mss[3];
    // temperature data
    float temp_c;
    // sensor time
    uint32_t current_time_counter, prev_time_counter = 0;
    uint64_t time_counter;
    // interrupt pin setup
    bool pinModeInt1(PinIO io, PinMode mode, PinLevel level);
    bool pinModeInt2(PinIO io, PinMode mode, PinLevel level);
    // self test
    bool selfTest();
    // power and mode settings
    bool setMode(bool active);
    bool setPower(bool enable);
    // command soft reset
    void softReset();
    // error checking
    bool isConfigErr();
    bool isFatalErr();
    // check id
    bool isCorrectId();
    // write / read registers
    void writeRegister(uint8_t subAddress, uint8_t data);
    void writeRegisters(uint8_t subAddress, uint8_t count, const uint8_t *data);
    void readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest);
};