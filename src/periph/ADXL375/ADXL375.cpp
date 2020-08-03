/*
ADXL375 - High Range Accelerometer

Status: Functional

This is one of the more put together sensor libraries. There are a few TODOs that need to be cleaned up and tested, and porting to a better SPI interface would be cool.

*/
#include "ADXL375.hpp"

ADXL375::ADXL375(RTOSPI *SPI, uint8_t CS_PIN, const char* id) : Sensor(id) //TODO: port to C++ SPI implementation.
{
  this->SPI = SPI;
  this->CS_PIN = CS_PIN;
}

int ADXL375::init()
{
  setDataRate(BW_12_5HZ);
  writeRegister(ADXL375_REG_DATA_FORMAT, 0x0B);
  return 0;
}

void ADXL375::startMeasuring()
{
  writeRegister(ADXL375_REG_POWER_CTL, 0x08);
}

void ADXL375::setDataRate(ADXL375_Bandwidth bw)
{
  writeRegister(ADXL375_REG_BW_RATE, (uint8_t)bw);
}

ADXL375::Data ADXL375::readSensor()
{
  uint8_t data[6];
  _multiReadRegister(ADXL375_REG_DATAX0, data, 6);

  Data xyz;
  //convert to mg, then g, then m/s2
  xyz.x = (float)((int16_t)(data[0] | data[1] << 8)) * ADXL375_XYZ_READ_SCALE_FACTOR / 1000.0 * 9.80665;
  xyz.y = (float)((int16_t)(data[2] | data[3] << 8)) * ADXL375_XYZ_READ_SCALE_FACTOR / 1000.0 * 9.80665;
  xyz.z = (float)((int16_t)(data[4] | data[5] << 8)) * ADXL375_XYZ_READ_SCALE_FACTOR / 1000.0 * 9.80665;

  return xyz;
}

uint8_t ADXL375::readRegister(uint8_t regAddress)
{
  uint8_t data[1];
  _multiReadRegister(regAddress, data, 1);
  return data[0];
}

// input is in g
void ADXL375::setShockThreshold(uint8_t shockThreshold)
{
  uint8_t scaledValue = shockThreshold * 1000 / ADXL375_THRESH_SHOCK_SCALE;
  writeRegister(ADXL375_REG_THRESH_SHOCK, scaledValue);
}

// returns the number of items in the FIFO buffer
uint8_t ADXL375::getFIFOBufferSize()
{
  return readRegister(ADXL375_REG_FIFO_STATUS) & 0b00111111;
}

void ADXL375::startShockDetection()
{
  setShockAxes(true, true, true);

  // shock duration to test for 625 μs/LSB
  writeRegister(ADXL375_REG_DUR, 2);

  // reset then enable FIFO trigger mode
  setFIFOMode(ADXL375_FIFO_MODE_BYPASS);
  setFIFOMode(ADXL375_FIFO_MODE_TRIGGER, ADXL375_TRIGGER_INT1_PIN, 16);

  // set the interrupt bit for shock detection
  uint8_t intEnable = readRegister(ADXL375_REG_INT_ENABLE);
  writeRegister(ADXL375_REG_INT_ENABLE, intEnable | 0b01000000);

  startMeasuring();
}

void ADXL375::setShockAxes(bool x, bool y, bool z)
{
  uint8_t shockAxesData = 0b00000000;
  if (x)
    shockAxesData |= 0b100;
  if (y)
    shockAxesData |= 0b010;
  if (z)
    shockAxesData |= 0b001;

  // the axes we want to detect shocks on
  writeRegister(ADXL375_REG_SHOCK_AXES, shockAxesData);
}

void ADXL375::setFIFOMode(uint8_t mode, uint8_t pin, uint8_t samples)
{
  writeRegister(ADXL375_REG_FIFO_CTL, mode << 6 | pin << 5 | samples);
}

void ADXL375::_multiReadRegister(uint8_t regAddress, uint8_t values[], uint8_t numberOfBytes)
{

  regAddress |= 0x80;

  // set the multi-read byte if required
  if (numberOfBytes > 1)
  {
    regAddress |= 0x40;
  }

  uint8_t txbuf[numberOfBytes + 1];
  uint8_t rxbuf[numberOfBytes + 1];

  memset(txbuf, 0x00, numberOfBytes + 1);

  txbuf[0] = regAddress;

  digitalWrite(CS_PIN,LOW);
  SPI->dmaTransfer(txbuf, rxbuf, numberOfBytes + 1);
  digitalWrite(CS_PIN,HIGH);

  memcpy(values, rxbuf + 1, numberOfBytes);
}

void ADXL375::writeRegister(uint8_t regAddress, uint8_t value)
{
  uint8_t txbuf[] = {regAddress, value};
  uint8_t rxbuf[2] = {0};

  digitalWrite(CS_PIN,LOW);
  SPI->dmaTransfer(txbuf, rxbuf, 2);
  digitalWrite(CS_PIN,HIGH);
}
