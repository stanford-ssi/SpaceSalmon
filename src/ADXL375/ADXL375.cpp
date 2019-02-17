extern "C" {
#include <atmel_start.h>
}

#include "ADXL375.hpp"

ADXL375::ADXL375()
{
}

void ADXL375::init()
{

  setDataRate(0b00001111);
}

void ADXL375::startMeasuring()
{
  writeRegister(ADXL375_REG_POWER_CTL, 0x08);
}

void ADXL375::setDataRate(uint8_t rate)
{
  writeRegister(ADXL375_REG_BW_RATE, rate);
}

AccelData ADXL375::getXYZ()
{
  uint8_t data[6];
  _multiReadRegister(ADXL375_REG_DATAX0, data, 6);

  AccelData xyz;
  xyz.x = (data[0] | data[1]<<8)*ADXL375_XYZ_READ_SCALE_FACTOR;
  xyz.y = (data[2] | data[3]<<8)*ADXL375_XYZ_READ_SCALE_FACTOR;
  xyz.z = (data[4] | data[5]<<8)*ADXL375_XYZ_READ_SCALE_FACTOR;
  
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
  setShockAxes(true,true,true);

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
  if(x) shockAxesData |= 0b100;
  if(y) shockAxesData |= 0b010;
  if(z) shockAxesData |= 0b001;

  // the axes we want to detect shocks on
  writeRegister(ADXL375_REG_SHOCK_AXES, shockAxesData);
}

void ADXL375::setFIFOMode(uint8_t mode, uint8_t pin, uint8_t samples)
{
  writeRegister(ADXL375_REG_FIFO_CTL, mode<<6 | pin<<5 | samples);
}

void ADXL375::_multiReadRegister(uint8_t regAddress, uint8_t values[], uint8_t numberOfBytes)
{

  regAddress |= 0x80;

  // set the multi-read byte if required
  if (numberOfBytes > 1) {
    regAddress |= 0x40;
  }

  gpio_set_pin_level(ADXL_CS_1,false);

  uint8_t send[numberOfBytes + 1];
	uint8_t recv[numberOfBytes + 1];

  memset(send, 0x00, numberOfBytes+1);
  memset(recv, 0x00, numberOfBytes+1);

  send[0] = regAddress;

  struct spi_xfer data;

  data.size = numberOfBytes+1;
	data.txbuf = send;
	data.rxbuf = recv;

  spi_m_sync_enable(&SPI_SQUIB);
	spi_m_sync_transfer(&SPI_SQUIB, &data);
	spi_m_sync_disable(&SPI_SQUIB);

  memcpy(values,recv+1,numberOfBytes);

  gpio_set_pin_level(ADXL_CS_1,true);
  
}

void ADXL375::writeRegister(uint8_t regAddress, uint8_t value)
{
    //Set Chip Select pin low to signal the beginning of an SPI packet.
  gpio_set_pin_level(ADXL_CS_1,false);
  //Transfer the register address over SPI.
  //SPI.transfer(regAddress);
  //Transfer the desired register value over SPI.
  //SPI.transfer(value);
  uint8_t send[] = {regAddress,value};
	uint8_t recv[] = {0x00, 0x00};

  struct spi_xfer data;

  data.size = 2;
	data.txbuf = send;
	data.rxbuf = recv;

  spi_m_sync_enable(&SPI_SQUIB);
	spi_m_sync_transfer(&SPI_SQUIB, &data);
	spi_m_sync_disable(&SPI_SQUIB);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  gpio_set_pin_level(ADXL_CS_1,true);
}

