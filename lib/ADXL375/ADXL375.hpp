#pragma once

extern "C"
{
#include <hal_spi_m_sync.h>
#include <hal_gpio.h>
#include <stdint.h>
#include <string.h>
}

#define ADXL375_REG_DEVID 0x00            // Device ID
#define ADXL375_REG_THRESH_SHOCK 0x1D     // Shock threshold
#define ADXL375_REG_OFSX 0x1E             // X-axis offset
#define ADXL375_REG_OFSY 0x1F             // Y-axis offset
#define ADXL375_REG_OFSZ 0x20             // Z-axis offset
#define ADXL375_REG_DUR 0x21              // Shock duration
#define ADXL375_REG_LATENT 0x22           // Shock latency
#define ADXL375_REG_WINDOW 0x23           // Tap window
#define ADXL375_REG_THRESH_ACT 0x24       // Activity threshold
#define ADXL375_REG_THRESH_INACT 0x25     // Inactivity threshold
#define ADXL375_REG_TIME_INACT 0x26       // Inactivity time
#define ADXL375_REG_ACT_INACT_CTL 0x27    // Axis enable control for activity and inactivity detection
#define ADXL375_REG_SHOCK_AXES 0x2A       // Axis control for single/double tap
#define ADXL375_REG_ACT_SHOCK_STATUS 0x2B // Source for single/double tap
#define ADXL375_REG_BW_RATE 0x2C          // Data rate and power mode control
#define ADXL375_REG_POWER_CTL 0x2D        // Power-saving features control
#define ADXL375_REG_INT_ENABLE 0x2E       // Interrupt enable control
#define ADXL375_REG_INT_MAP 0x2F          // Interrupt mapping control
#define ADXL375_REG_INT_SOURCE 0x30       // Source of interrupts
#define ADXL375_REG_DATA_FORMAT 0x31      // Data format control
#define ADXL375_REG_DATAX0 0x32           // X-axis data 0
#define ADXL375_REG_DATAX1 0x33           // X-axis data 1
#define ADXL375_REG_DATAY0 0x34           // Y-axis data 0
#define ADXL375_REG_DATAY1 0x35           // Y-axis data 1
#define ADXL375_REG_DATAZ0 0x36           // Z-axis data 0
#define ADXL375_REG_DATAZ1 0x37           // Z-axis data 1
#define ADXL375_REG_FIFO_CTL 0x38         // FIFO control
#define ADXL375_REG_FIFO_STATUS 0x39      // FIFO status

#define ADXL375_XYZ_READ_SCALE_FACTOR 49 // scaling factor when reading xyz data
#define ADXL375_THRESH_SHOCK_SCALE 780   // scaling factor for shock threshold register

#define ADXL375_FIFO_MODE_BYPASS 0b00
#define ADXL375_FIFO_MODE_FIFO 0b01
#define ADXL375_FIFO_MODE_STREAM 0b10
#define ADXL375_FIFO_MODE_TRIGGER 0b11

#define ADXL375_TRIGGER_INT1_PIN 0
#define ADXL375_TRIGGER_INT2_PIN 1

class ADXL375
{
public:
  //Accelerometer data from ADXL375, in meters/sec^2
  struct ADXL375_Data
  {
    float x;
    float y;
    float z;
  };

  enum ADXL375_Bandwidth
  {
    BW_0_05HZ = 0,
    BW_0_1HZ = 1,
    BW_0_2HZ = 2,
    BW_0_39HZ = 3,
    BW_0_78HZ = 4,
    BW_1_56HZ = 5,
    BW_3_13HZ = 6,
    BW_6_25HZ = 7,
    BW_12_5HZ = 8,
    BW_25HZ = 9,
    BW_50HZ = 10,
    BW_100HZ = 11,
    BW_200HZ = 12,
    BW_400HZ = 13,
    BW_800HZ = 14,
    BW_1600HZ = 15
  };

  ADXL375(struct spi_m_sync_descriptor *SPI, uint8_t CS_PIN);
  void init();
  void startMeasuring();
  ADXL375_Data getXYZ();
  uint8_t readRegister(uint8_t regAddress);
  void writeRegister(uint8_t regAddress, uint8_t value);
  void setShockThreshold(uint8_t shockThreshold);
  void setShockAxes(bool x = true, bool y = true, bool z = true);
  void startShockDetection();
  void setDataRate(ADXL375_Bandwidth bw);
  uint8_t getFIFOBufferSize();
  void setFIFOMode(uint8_t mode, uint8_t trigger = 0, uint8_t samples = 0);

private:
  void _multiReadRegister(uint8_t regAddress, uint8_t values[], uint8_t numberOfBytes = 1);
  struct spi_m_sync_descriptor *SPI;
  uint8_t CS_PIN;
};
