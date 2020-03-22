#include "BMI088Gyro.hpp"

#define GET_FIELD(regname, value) ((value & regname##_MASK) >> regname##_POS)
#define SET_FIELD(regval, regname, value) ((regval & ~regname##_MASK) | ((value << regname##_POS) & regname##_MASK))

/* BMI088 object, input the SPI bus and chip select pin */
BMI088Gyro::BMI088Gyro(SPIClass *bus, uint8_t csPin)
{
  _spi = bus;     // SPI bus
  _csPin = csPin; // chip select pin
}

/* begins communication with the BMI088 gyro */
int BMI088Gyro::begin()
{
  /* check device id */
  if (!isCorrectId())
  {
    return -1;
  }
  /* soft reset */
  softReset();
  delay(50);
  /* set default range */
  if (!setRange(RANGE_2000DPS))
  {
    return -2;
  }
  /* enable data ready int */
  if (!setDrdyInt(true))
  {
    return -3;
  }
  /* set default ODR */
  if (!setOdr(ODR_2000HZ_BW_532HZ))
  {
    return -4;
  }
  return 1;
}

/* sets the BMI088 output data rate */
bool BMI088Gyro::setOdr(Odr odr)
{
  uint8_t writeReg = 0, readReg = 0;
  writeReg = SET_FIELD(writeReg, GYRO_ODR, odr);
  writeRegister(GYRO_ODR_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_ODR_ADDR, 1, &readReg);
  return (readReg == writeReg) ? true : false;
}

/* sets the BMI088 range */
bool BMI088Gyro::setRange(Range range)
{
  uint8_t writeReg = 0, readReg = 0;
  writeReg = SET_FIELD(writeReg, GYRO_RANGE, range);
  writeRegister(GYRO_RANGE_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_RANGE_ADDR, 1, &readReg);
  if (readReg == writeReg)
  {
    switch (range)
    {
    case RANGE_125DPS:
    {
      gyro_range_rads = 125.0f * D2R;
      break;
    }
    case RANGE_250DPS:
    {
      gyro_range_rads = 250.0f * D2R;
      break;
    }
    case RANGE_500DPS:
    {
      gyro_range_rads = 500.0f * D2R;
      break;
    }
    case RANGE_1000DPS:
    {
      gyro_range_rads = 1000.0f * D2R;
      break;
    }
    case RANGE_2000DPS:
    {
      gyro_range_rads = 2000.0f * D2R;
      break;
    }
    }
    return true;
  }
  else
  {
    return false;
  }
}

/* sets the Int3 pin configuration */
bool BMI088Gyro::pinModeInt3(PinMode mode, PinLevel level)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t pin_mode, active_lvl;
  readRegisters(GYRO_INT3_IO_CTRL_ADDR, 1, &readReg);
  switch (mode)
  {
  case PUSH_PULL:
  {
    pin_mode = GYRO_INT_PUSHPULL;
    break;
  }
  case OPEN_DRAIN:
  {
    pin_mode = GYRO_INT_OPENDRAIN;
    break;
  }
  default:
  {
    pin_mode = GYRO_INT_PUSHPULL;
    break;
  }
  }
  switch (level)
  {
  case ACTIVE_HIGH:
  {
    active_lvl = GYRO_INT_LVL_HIGH;
    break;
  }
  case ACTIVE_LOW:
  {
    active_lvl = GYRO_INT_LVL_LOW;
    break;
  }
  default:
  {
    active_lvl = GYRO_INT_LVL_HIGH;
    break;
  }
  }
  writeReg = SET_FIELD(readReg, GYRO_INT3_IO_CTRL, (pin_mode | active_lvl));
  writeRegister(GYRO_INT3_IO_CTRL_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_INT3_IO_CTRL_ADDR, 1, &readReg);
  return (readReg == writeReg) ? true : false;
}

/* sets the Int4 pin configuration */
bool BMI088Gyro::pinModeInt4(PinMode mode, PinLevel level)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t pin_mode, active_lvl;
  readRegisters(GYRO_INT4_IO_CTRL_ADDR, 1, &readReg);
  switch (mode)
  {
  case PUSH_PULL:
  {
    pin_mode = GYRO_INT_PUSHPULL;
    break;
  }
  case OPEN_DRAIN:
  {
    pin_mode = GYRO_INT_OPENDRAIN;
    break;
  }
  default:
  {
    pin_mode = GYRO_INT_PUSHPULL;
    break;
  }
  }
  switch (level)
  {
  case ACTIVE_HIGH:
  {
    active_lvl = GYRO_INT_LVL_HIGH;
    break;
  }
  case ACTIVE_LOW:
  {
    active_lvl = GYRO_INT_LVL_LOW;
    break;
  }
  default:
  {
    active_lvl = GYRO_INT_LVL_HIGH;
    break;
  }
  }
  writeReg = SET_FIELD(readReg, GYRO_INT4_IO_CTRL, (pin_mode | active_lvl));
  writeRegister(GYRO_INT4_IO_CTRL_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_INT4_IO_CTRL_ADDR, 1, &readReg);
  return (readReg == writeReg) ? true : false;
}

/* maps the data ready signal to the Int3 pin */
bool BMI088Gyro::mapDrdyInt3(bool enable)
{
  uint8_t writeReg = 0, readReg = 0;
  readRegisters(GYRO_INT3_DRDY_ADDR, 1, &readReg);
  writeReg = SET_FIELD(readReg, GYRO_INT3_DRDY, enable);
  writeRegister(GYRO_INT3_DRDY_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_INT3_DRDY_ADDR, 1, &readReg);
  return (readReg == writeReg) ? true : false;
}

/* maps the data ready signal to the Int4 pin */
bool BMI088Gyro::mapDrdyInt4(bool enable)
{
  uint8_t writeReg = 0, readReg = 0;
  readRegisters(GYRO_INT4_DRDY_ADDR, 1, &readReg);
  writeReg = SET_FIELD(readReg, GYRO_INT4_DRDY, enable);
  writeRegister(GYRO_INT4_DRDY_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_INT4_DRDY_ADDR, 1, &readReg);
  return (readReg == writeReg) ? true : false;
}

/* returns whether data is ready or not */
bool BMI088Gyro::getDrdyStatus()
{
  uint8_t readReg = 0;
  readRegisters(GYRO_DRDY_ADDR, 1, &readReg);
  return (GET_FIELD(GYRO_DRDY, readReg)) ? true : false;
}

/* reads the BMI088 gyro */
BMI088Gyro::Data BMI088Gyro::readSensor()
{
  Data data;
  /* accel data */
  int16_t gyro[3];
  readRegisters(GYRO_DATA_ADDR, 6, _buffer);

  gyro[0] = (_buffer[1] << 8) | _buffer[0];
  gyro[1] = (_buffer[3] << 8) | _buffer[2];
  gyro[2] = (_buffer[5] << 8) | _buffer[4];

  gyro_rads[0] = (float)gyro[0] / 32767.0f * gyro_range_rads;
  gyro_rads[1] = (float)gyro[1] / 32767.0f * gyro_range_rads;
  gyro_rads[2] = (float)gyro[2] / 32767.0f * gyro_range_rads;

  data.x = gyro_rads[0];
  data.y = gyro_rads[1];
  data.z = gyro_rads[2];

  return data;
}

/* enables the new data interrupt */
bool BMI088Gyro::setDrdyInt(bool enable)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t value = (enable) ? GYRO_ENABLE_DRDY_INT : GYRO_DIS_DRDY_INT;
  writeReg = SET_FIELD(writeReg, GYRO_INT_CNTRL, value);
  writeRegister(GYRO_INT_CNTRL_ADDR, writeReg);
  delay(1);
  readRegisters(GYRO_INT_CNTRL_ADDR, 1, &readReg);
  return (readReg == writeReg) ? true : false;
}

/* performs a soft reset */
void BMI088Gyro::softReset()
{
  uint8_t reg = 0;
  reg = SET_FIELD(reg, GYRO_SOFT_RESET, GYRO_RESET_CMD);
  writeRegister(GYRO_SOFT_RESET_ADDR, reg);
  delay(50);
}

/* checks the BMI088 gyro ID */
bool BMI088Gyro::isCorrectId()
{
  uint8_t readReg = 0;
  readRegisters(GYRO_CHIP_ID_ADDR, 1, &readReg);
  return (GET_FIELD(GYRO_CHIP_ID, readReg) == GYRO_CHIP_ID) ? true : false;
}

/* writes a byte to BMI088 register given a register address and data */
void BMI088Gyro::writeRegister(uint8_t subAddress, uint8_t data)
{
  subAddress &= ~SPI_READ;
  uint8_t buf[] = {subAddress, data};

  digitalWrite(_csPin,LOW);
	_spi->transfer(buf, 2);
	digitalWrite(_csPin,HIGH);
}

/* reads registers from BMI088 given a starting register address, number of bytes, and a pointer to store data */
void BMI088Gyro::readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest)
{
  uint8_t buf[count + 1];

  memset(buf, 0x00, count + 1);

  buf[0] = subAddress | SPI_READ;

  digitalWrite(_csPin,LOW);
	_spi->transfer(buf, count + 1);
	digitalWrite(_csPin,HIGH);

  memcpy(dest, buf + 1, count);
}
