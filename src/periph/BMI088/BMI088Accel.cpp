#include "BMI088Accel.hpp"

#define GET_FIELD(regname,value) ((value & regname##_MASK) >> regname##_POS)
#define	SET_FIELD(regval,regname,value) ((regval & ~regname##_MASK) | ((value << regname##_POS) & regname##_MASK))

/* BMI088 object, input the SPI bus and chip select pin */
BMI088Accel::BMI088Accel(SPIClass *bus, uint8_t csPin)
{
  _spi = bus; // SPI bus
  _csPin = csPin; // chip select pin
}

/* begins communication with the BMI088 accel */
int BMI088Accel::begin()
{
  /* check device id */
  if (!isCorrectId()) {
    return -1;
  }
  /* soft reset */
  softReset();
  /* enable the accel */
  if (!setPower(true)) {
    return -2;
  }
  /* enter active mode */
  if (!setMode(true)) {
    return -3;
  } 
  /* self test */
  if (!selfTest()) {
    return -4;
  }
  /* soft reset */
  softReset();
  /* enable the accel */
  if (!setPower(true)) {
    return -5;
  }
  /* enter active mode */
  if (!setMode(true)) {
    return -6;
  } 
  delay(50);
  /* set default range */
  if (!setRange(RANGE_24G)) {
    return -7;
  }
  /* set default ODR */
  if (!setOdr(ODR_1600HZ_BW_280HZ)) {
    return -8;
  }
  /* check config errors */
  if (isConfigErr()) {
    return -9;
  }
  /* check fatal errors */
  if (isFatalErr()) {
    return -10;
  }
  return 1;
}

/* sets the BMI088 output data rate */
bool BMI088Accel::setOdr(Odr odr)
{
  uint8_t writeReg = 0, readReg = 0, value;
  switch (odr) {
    case ODR_1600HZ_BW_280HZ: {
      value = (0x0A << 4) | 0x0C;
      break;
    }
    case ODR_1600HZ_BW_234HZ: {
      value = (0x09 << 4) | 0x0C;
      break;      
    }
    case ODR_1600HZ_BW_145HZ: {
      value = (0x08 << 4) | 0x0C;
      break;       
    }
    case ODR_800HZ_BW_230HZ: {
      value = (0x0A << 4) | 0x0B;
      break;            
    }
    case ODR_800HZ_BW_140HZ: {
      value = (0x09 << 4) | 0x0B;
      break;      
    }
    case ODR_800HZ_BW_80HZ: {
      value = (0x08 << 4) | 0x0B;
      break;           
    }
    case ODR_400HZ_BW_145HZ: {
      value = (0x0A << 4) | 0x0A;
      break;      
    }
    case ODR_400HZ_BW_75HZ: {
      value = (0x09 << 4) | 0x0A;
      break;          
    }
    case ODR_400HZ_BW_40HZ: {
      value = (0x08 << 4) | 0x0A;
      break;    
    }
    case ODR_200HZ_BW_80HZ: {
      value = (0x0A << 4) | 0x09;
      break;       
    }
    case ODR_200HZ_BW_38HZ: {
      value = (0x09 << 4) | 0x09;
      break;      
    }
    case ODR_200HZ_BW_20HZ: {
      value = (0x08 << 4) | 0x09;
      break;      
    }
    case ODR_100HZ_BW_40HZ: {
      value = (0x0A << 4) | 0x08;
      break;      
    }
    case ODR_100HZ_BW_19HZ: {
      value = (0x09 << 4) | 0x08;
      break;      
    }
    case ODR_100HZ_BW_10HZ: {
      value = (0x08 << 4) | 0x08;
      break;      
    }
    case ODR_50HZ_BW_20HZ: {
      value = (0x0A << 4) | 0x07;
      break;      
    }
    case ODR_50HZ_BW_9HZ: {
      value = (0x09 << 4) | 0x07;
      break;      
    }
    case ODR_50HZ_BW_5HZ: {
      value = (0x08 << 4) | 0x07;
      break;            
    }
    case ODR_25HZ_BW_10HZ: {
      value = (0x0A << 4) | 0x06;
      break;            
    }
    case ODR_25HZ_BW_5HZ: {
      value = (0x09 << 4) | 0x06;
      break;         
    }
    case ODR_25HZ_BW_3HZ: {
      value = (0x08 << 4) | 0x06;
      break;         
    }
    case ODR_12_5HZ_BW_5HZ: {
      value = (0x0A << 4) | 0x05;
      break;         
    }
    case ODR_12_5HZ_BW_2HZ: {
      value = (0x09 << 4) | 0x05;
      break;         
    }
    case ODR_12_5HZ_BW_1HZ: {
      value = (0x08 << 4) | 0x05;
      break;         
    }
    default: {
      value = (0x0A << 4) | 0x0C;
      break;
    }
  }
  writeReg = SET_FIELD(writeReg,ACC_ODR,value);
  writeRegister(ACC_ODR_ADDR,writeReg);
  delay(1);
  readRegisters(ACC_ODR_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;
}

/* sets the BMI088 range */
bool BMI088Accel::setRange(Range range)
{
  uint8_t writeReg = 0, readReg = 0;
  readRegisters(ACC_RANGE_ADDR,1,&readReg);
  writeReg = SET_FIELD(readReg,ACC_RANGE,range);
  writeRegister(ACC_RANGE_ADDR,writeReg);
  delay(1);
  readRegisters(ACC_RANGE_ADDR,1,&readReg);
  if (readReg == writeReg) {
    switch (range) {
      case RANGE_3G: {
        accel_range_mss = 3.0f * G;
        break;
      }
      case RANGE_6G: {
        accel_range_mss = 6.0f * G;
        break;
      }
      case RANGE_12G: {
        accel_range_mss = 12.0f * G;
        break;
      }
      case RANGE_24G: {
        accel_range_mss = 24.0f * G;
        break;
      }      
    }
    return true;
  } else {
    return false;
  }
}

/* sets the Int1 pin configuration */
bool BMI088Accel::pinModeInt1(PinMode mode, PinLevel level)
{
  return pinModeInt1(PIN_OUTPUT,mode,level);
}

/* sets the Int2 pin configuration */
bool BMI088Accel::pinModeInt2(PinMode mode, PinLevel level)
{
  return pinModeInt2(PIN_OUTPUT,mode,level);
}

/* maps the data ready signal to the Int1 pin */
bool BMI088Accel::mapDrdyInt1(bool enable)
{
  uint8_t writeReg = 0, readReg = 0;
  readRegisters(ACC_INT1_DRDY_ADDR,1,&readReg);
  writeReg = SET_FIELD(readReg,ACC_INT1_DRDY,enable);
  writeRegister(ACC_INT1_DRDY_ADDR,writeReg);
  delay(1);
  readRegisters(ACC_INT1_DRDY_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;  
}

/* maps the data ready signal to the Int2 pin */
bool BMI088Accel::mapDrdyInt2(bool enable)
{
  uint8_t writeReg = 0, readReg = 0;
  readRegisters(ACC_INT2_DRDY_ADDR,1,&readReg);
  writeReg = SET_FIELD(readReg,ACC_INT2_DRDY,enable);
  writeRegister(ACC_INT2_DRDY_ADDR,writeReg);
  delay(1);
  readRegisters(ACC_INT2_DRDY_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;
}

/* returns whether data is ready or not */
bool BMI088Accel::getDrdyStatus()
{
  uint8_t readReg = 0;
  readRegisters(ACC_DRDY_ADDR,1,&readReg);
  return (GET_FIELD(ACC_DRDY,readReg)) ? true : false;
}

/* reads the BMI088 accel */
BMI088Accel::Data BMI088Accel::readSensor()
{
  //struct to hold data:
  Data data;

  /* accel data */
  uint16_t temp_uint11;
  int16_t accel[3], temp_int11;
  readRegisters(ACC_ACCEL_DATA_ADDR,9,_buffer);

  accel[0] = (_buffer[1] << 8) | _buffer[0];
  accel[1] = (_buffer[3] << 8) | _buffer[2];
  accel[2] = (_buffer[5] << 8) | _buffer[4];
  
  accel_mss[0] = (float) accel[0] / 32768.0f * accel_range_mss;
  accel_mss[1] = (float) accel[1] / 32768.0f * accel_range_mss;
  accel_mss[2] = (float) accel[2] / 32768.0f * accel_range_mss;

  data.x = accel_mss[0];
  data.y = accel_mss[1];
  data.z = accel_mss[2];
  
  /* time data */ //but what is it for?
  current_time_counter = (_buffer[8] << 16) | (_buffer[7] << 8) | _buffer[6];
  time_counter = current_time_counter - prev_time_counter;
  prev_time_counter = current_time_counter;
  data.time = current_time_counter;

  /* temperature data */
  readRegisters(ACC_TEMP_DATA_ADDR,2,_buffer);
  temp_uint11 = (_buffer[0] * 8) + (_buffer[1] / 32);
  if (temp_uint11 > 1023) {
    temp_int11 = temp_uint11 - 2048;
  } else {
    temp_int11 = temp_uint11;
  }
  temp_c = (float) temp_int11 * 0.125f + 23.0f;
  data.temp = temp_c;
  
  return data;
}


/* sets the Int1 pin configuration */
bool BMI088Accel::pinModeInt1(PinIO io, PinMode mode, PinLevel level)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t pin_io, pin_mode, active_lvl;
  readRegisters(ACC_INT1_IO_CTRL_ADDR,1,&readReg);
  switch (io) {
    case PIN_INPUT: {
      pin_io = ACC_INT_INPUT;
      break;
    }
    case PIN_OUTPUT: {
      pin_io = ACC_INT_OUTPUT;
      break;
    }
    default: {
      pin_io = ACC_INT_OUTPUT;
      break;      
    }
  }
  switch (mode) {
    case PUSH_PULL: {
      pin_mode = ACC_INT_PUSHPULL;
      break;
    }
    case OPEN_DRAIN: {
      pin_mode = ACC_INT_OPENDRAIN;
      break;
    }
    default: {
      pin_mode = ACC_INT_PUSHPULL;
      break;      
    }
  }
  switch (level) {
    case ACTIVE_HIGH: {
      active_lvl = ACC_INT_LVL_HIGH;
      break;
    }
    case ACTIVE_LOW: {
      active_lvl = ACC_INT_LVL_LOW;
      break;
    }
    default: {
      active_lvl = ACC_INT_LVL_HIGH;
      break;      
    }
  }
  writeReg = SET_FIELD(readReg,ACC_INT1_IO_CTRL,(pin_io | pin_mode | active_lvl));
  writeRegister(ACC_INT1_IO_CTRL_ADDR,writeReg);
  delay(1);
  readRegisters(ACC_INT1_IO_CTRL_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;  
}

/* sets the Int2 pin configuration */
bool BMI088Accel::pinModeInt2(PinIO io, PinMode mode, PinLevel level)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t pin_io, pin_mode, active_lvl;
  readRegisters(ACC_INT2_IO_CTRL_ADDR,1,&readReg);
  switch (io) {
    case PIN_INPUT: {
      pin_io = ACC_INT_INPUT;
      break;
    }
    case PIN_OUTPUT: {
      pin_io = ACC_INT_OUTPUT;
      break;
    }
    default: {
      pin_io = ACC_INT_OUTPUT;
      break;      
    }
  }
  switch (mode) {
    case PUSH_PULL: {
      pin_mode = ACC_INT_PUSHPULL;
      break;
    }
    case OPEN_DRAIN: {
      pin_mode = ACC_INT_OPENDRAIN;
      break;
    }
    default: {
      pin_mode = ACC_INT_PUSHPULL;
      break;      
    }
  }
  switch (level) {
    case ACTIVE_HIGH: {
      active_lvl = ACC_INT_LVL_HIGH;
      break;
    }
    case ACTIVE_LOW: {
      active_lvl = ACC_INT_LVL_LOW;
      break;
    }
    default: {
      active_lvl = ACC_INT_LVL_HIGH;
      break;      
    }
  }
  writeReg = SET_FIELD(readReg,ACC_INT2_IO_CTRL,(pin_io | pin_mode | active_lvl));
  writeRegister(ACC_INT2_IO_CTRL_ADDR,writeReg);
  delay(1);
  readRegisters(ACC_INT2_IO_CTRL_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;  
}

/* performs BMI088 accel self test */
bool BMI088Accel::selfTest()
{
  uint8_t writeReg = 0;
  float accel_pos_mg[3], accel_neg_mg[3];
  /* set 24G range */
  setRange(RANGE_24G);
  /* set 1.6 kHz ODR, 4x oversampling */
  setOdr(ODR_1600HZ_BW_145HZ);
  /* wait >2 ms */
  delay(3);
  /* enable self test, positive polarity */
  writeReg = SET_FIELD(writeReg,ACC_SELF_TEST,ACC_POS_SELF_TEST);
  writeRegister(ACC_SELF_TEST_ADDR,writeReg);
  /* wait >50 ms */
  delay(51);
  /* read self test values */
  readSensor();
  for (uint8_t i = 0; i < 3; i++) {
    accel_pos_mg[i] = accel_mss[i] / G * 1000.0f;
  }
  /* enable self test, negative polarity */
  writeReg = SET_FIELD(writeReg,ACC_SELF_TEST,ACC_NEG_SELF_TEST);
  writeRegister(ACC_SELF_TEST_ADDR,writeReg);
  /* wait >50 ms */
  delay(51);
  /* read self test values */
  readSensor();
  for (uint8_t i = 0; i < 3; i++) {
    accel_neg_mg[i] = accel_mss[i] / G * 1000.0f;
  }
  /* disable self test */
  writeReg = SET_FIELD(writeReg,ACC_SELF_TEST,ACC_DIS_SELF_TEST);
  writeRegister(ACC_SELF_TEST_ADDR,writeReg);
  /* wait >50 ms */
  delay(51); 
  /* check self test results */
  if ((fabs(accel_pos_mg[0] - accel_neg_mg[0]) >= 1000) && (fabs(accel_pos_mg[1] - accel_neg_mg[1]) >= 1000) && (fabs(accel_pos_mg[2] - accel_neg_mg[2]) >= 500)) {
    return true;
  } else {
    return false;
  }
}

/* sets whether the sensor is in active or suspend mode */
bool BMI088Accel::setMode(bool active)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t value = (active) ? ACC_ACTIVE_MODE_CMD : ACC_SUSPEND_MODE_CMD;
  writeReg = SET_FIELD(writeReg,ACC_PWR_CONF,value);
  writeRegister(ACC_PWR_CONF_ADDR,writeReg);
  delay(5); // 5 ms wait after power mode changes
  readRegisters(ACC_PWR_CONF_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;
}

/* sets whether the sensor is enabled or disabled */
bool BMI088Accel::setPower(bool enable)
{
  uint8_t writeReg = 0, readReg = 0;
  uint8_t value = (enable) ? ACC_ENABLE_CMD : ACC_DISABLE_CMD;
  writeReg = SET_FIELD(writeReg,ACC_PWR_CNTRL,value);
  writeRegister(ACC_PWR_CNTRL_ADDR,writeReg);
  delay(5); // 5 ms wait after power mode changes
  readRegisters(ACC_PWR_CNTRL_ADDR,1,&readReg);
  return (readReg == writeReg) ? true : false;
}

/* performs a soft reset */
void BMI088Accel::softReset()
{
  uint8_t reg = 0;
  reg = SET_FIELD(reg,ACC_SOFT_RESET,ACC_RESET_CMD);
  writeRegister(ACC_SOFT_RESET_ADDR,reg);
  delay(50);
  digitalWrite(_csPin,LOW);
  digitalWrite(_csPin,HIGH);
  
}

/* checks the BMI088 for configuration errors */
bool BMI088Accel::isConfigErr()
{
  uint8_t readReg = 0;
  readRegisters(ACC_ERR_CODE_ADDR,1,&readReg);
  return (GET_FIELD(ACC_ERR_CODE,readReg)) ? true : false;
}

/* checks the BMI088 for fatal errors */
bool BMI088Accel::isFatalErr()
{
  uint8_t readReg = 0;
  readRegisters(ACC_FATAL_ERR_ADDR,1,&readReg);
  return (GET_FIELD(ACC_FATAL_ERR,readReg)) ? true : false;
}

/* checks the BMI088 accelerometer ID */
bool BMI088Accel::isCorrectId()
{
  uint8_t readReg = 0;
  readRegisters(ACC_CHIP_ID_ADDR,1,&readReg);
  return (GET_FIELD(ACC_CHIP_ID,readReg) == ACC_CHIP_ID) ? true : false;
}

/* writes a byte to BMI088 register given a register address and data */
void BMI088Accel::writeRegister(uint8_t subAddress, uint8_t data)
{
  subAddress &= ~SPI_READ;
  uint8_t buf[] = {subAddress, data};

  digitalWrite(_csPin,LOW);
	_spi->transfer(buf, 2);
	digitalWrite(_csPin,HIGH);
}

/* writes multiple bytes to BMI088 register given a register address and data */
void BMI088Accel::writeRegisters(uint8_t subAddress, uint8_t count, const uint8_t *data)
{
  uint8_t buf[count + 1];

  buf[0] = subAddress | SPI_READ;

  memcpy(buf + 1, data, count);

  digitalWrite(_csPin,LOW);
	_spi->transfer(buf, count + 1);
	digitalWrite(_csPin,HIGH);
}

/* reads registers from BMI088 given a starting register address, number of bytes, and a pointer to store data */
void BMI088Accel::readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest)
{
  uint8_t buf[count + 2];

  memset(buf, 0x00, count + 2);

  buf[0] = subAddress | SPI_READ;

  digitalWrite(_csPin,LOW);
	_spi->transfer(buf, count + 2);
	digitalWrite(_csPin,HIGH);

  memcpy(dest, buf + 2, count);
}