#include "ADXL375.hpp"

ADXL375::ADXL375(struct spi_m_sync_descriptor *SPI, uint8_t CS_PIN)
{
  this->SPI = SPI;
  this->CS_PIN = CS_PIN;
}

void ADXL375::init()
{

  setDataRate(BW_12_5HZ);
}

void ADXL375::startMeasuring()
{
  writeRegister(ADXL375_REG_POWER_CTL, 0x08);
}

void ADXL375::setDataRate(ADXL375_Bandwidth bw)
{
  writeRegister(ADXL375_REG_BW_RATE, (uint8_t)bw);
}

ADXL375::ADXL375_Data ADXL375::getXYZ()
{
  uint8_t data[6];
  _multiReadRegister(ADXL375_REG_DATAX0, data, 6);

  ADXL375_Data xyz;
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

  uint8_t send[numberOfBytes + 1];
  uint8_t recv[numberOfBytes + 1];

  memset(send, 0x00, numberOfBytes + 1);
  memset(recv, 0x00, numberOfBytes + 1);

  send[0] = regAddress;

  struct spi_xfer data;

  data.size = numberOfBytes + 1;
  data.txbuf = send;
  data.rxbuf = recv;

  //TODO: this could be removed...
  spi_m_sync_disable(SPI);
  spi_m_sync_set_mode(SPI, SPI_MODE_3);
  spi_m_sync_enable(SPI);

  gpio_set_pin_level(CS_PIN, false);
  spi_m_sync_transfer(SPI, &data);
  gpio_set_pin_level(CS_PIN, true);

  memcpy(values, recv + 1, numberOfBytes);
}

void ADXL375::writeRegister(uint8_t regAddress, uint8_t value)
{
  uint8_t send[] = {regAddress, value};
  uint8_t recv[] = {0x00, 0x00};

  struct spi_xfer data;

  data.size = 2;
  data.txbuf = send;
  data.rxbuf = recv;

  //TODO: this could be removed...
  spi_m_sync_disable(SPI);
  spi_m_sync_set_mode(SPI, SPI_MODE_3);
  spi_m_sync_enable(SPI);

  gpio_set_pin_level(CS_PIN, false);
  spi_m_sync_transfer(SPI, &data);
  gpio_set_pin_level(CS_PIN, true);
}
