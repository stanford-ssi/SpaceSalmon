/*!
 * @file BMP3xx.cpp
 *
 * @mainpage Adafruit BMP3XX temperature & barometric pressure sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's BMP3XX driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit BMP388 breakout: https://www.adafruit.com/products/3966
 *
 * These sensors use I2C or SPI to communicate
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "BMP3xx.hpp"

static int8_t spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
static int8_t spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
static void delay_msec(uint32_t ms);

spi_m_os_descriptor *BMP3xx_SPI;

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Instantiates sensor with Hardware SPI or I2C.
    @param  cspin SPI chip select. If not passed in, I2C will be used
*/
/**************************************************************************/
BMP3xx::BMP3xx(struct spi_m_os_descriptor *spi ,int8_t cspin)
{
  BMP3xx_SPI = spi;
  _cs = cspin;
  _filterEnabled = _tempOSEnabled = _presOSEnabled = false;
}

/**************************************************************************/
/*!
    @brief Initializes the sensor

    Hardware ss initialized, verifies it is in the I2C or SPI bus, then reads
    calibration data in preparation for sensor reads.

    @param  addr Optional parameter for the I2C address of BMP3. Default is 0x77
    @param  theWire Optional parameter for the I2C device we will use. Default is "Wire"
    @return True on sensor initialization success. False on failure.
*/
/**************************************************************************/
bool BMP3xx::begin() {

  the_sensor.dev_id = _cs;
  the_sensor.intf = BMP3_SPI_INTF;
  the_sensor.read = &spi_read;
  the_sensor.write = &spi_write;
  the_sensor.delay_ms = delay_msec;

  int8_t rslt = BMP3_OK;
  rslt = bmp3_init(&the_sensor);

#ifdef BMP3XX_DEBUG
  Serial.print("Result: "); Serial.println(rslt);
#endif

  if (rslt != BMP3_OK)
    return false;

#ifdef BMP3XX_DEBUG
  Serial.print("T1 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_t1);
  Serial.print("T2 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_t2);
  Serial.print("T3 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_t3);
  Serial.print("P1 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p1);
  Serial.print("P2 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p2);
  Serial.print("P3 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p3);
  Serial.print("P4 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p4);
  Serial.print("P5 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p5);
  Serial.print("P6 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p6);
  Serial.print("P7 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p7);
  Serial.print("P8 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p8);
  Serial.print("P9 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p9);
  Serial.print("P10 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p10);
  Serial.print("P11 = "); Serial.println(the_sensor.calib_data.reg_calib_data.par_p11);
  //Serial.print("T lin = "); Serial.println(the_sensor.calib_data.reg_calib_data.t_lin);
#endif

  setTemperatureOversampling(BMP3_NO_OVERSAMPLING);
  setPressureOversampling(BMP3_NO_OVERSAMPLING);
  setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);

  // don't do anything till we request a reading
  the_sensor.settings.op_mode = BMP3_FORCED_MODE;

  return true;
}

/**************************************************************************/
/*!
    @brief Performs a full reading of all sensors in the BMP3XX.

    Assigns the internal BMP3xx#temperature & BMP3xx#pressure member variables

    @return True on success, False on failure
*/
/**************************************************************************/
BMP3xx::Data BMP3xx::readSensor(void) {
  int8_t rslt;
  /* Used to select the settings user needs to change */
  uint16_t settings_sel = 0;
  /* Variable used to select the sensor component */
  uint8_t sensor_comp = 0;

  /* Variable used to store the compensated data */
  struct bmp3_data data = {0,0};

  /* Select the pressure and temperature sensor to be enabled */
  the_sensor.settings.temp_en = BMP3_ENABLE;
  settings_sel |= BMP3_TEMP_EN_SEL;
  sensor_comp |= BMP3_TEMP;
  if (_tempOSEnabled) {
     settings_sel |= BMP3_TEMP_OS_SEL;
  }

  the_sensor.settings.press_en = BMP3_ENABLE;
  settings_sel |= BMP3_PRESS_EN_SEL;
  sensor_comp |= BMP3_PRESS;
  if (_presOSEnabled) {
    settings_sel |= BMP3_PRESS_OS_SEL ;
  }

  if (_filterEnabled) {
    settings_sel |= BMP3_IIR_FILTER_SEL;
  }

  if (_ODREnabled) {
    settings_sel |= BMP3_ODR_SEL;
  }

  // set interrupt to data ready
  //settings_sel |= BMP3_DRDY_EN_SEL | BMP3_LEVEL_SEL | BMP3_LATCH_SEL;

  /* Set the desired sensor configuration */
#ifdef BMP3XX_DEBUG
  Serial.println("Setting sensor settings");
#endif
  rslt = bmp3_set_sensor_settings(settings_sel, &the_sensor);
  if (rslt != BMP3_OK)
    return data;

  /* Set the power mode */
  the_sensor.settings.op_mode = BMP3_FORCED_MODE;
#ifdef BMP3XX_DEBUG
  Serial.println("Setting power mode");
#endif
  rslt = bmp3_set_op_mode(&the_sensor);
  if (rslt != BMP3_OK)
    return data;

  /* Temperature and Pressure data are read and stored in the bmp3_data instance */
  rslt = bmp3_get_sensor_data(sensor_comp, &data, &the_sensor);

  /* Save the temperature and pressure data */

  if (rslt != BMP3_OK)
    return data;

  return data;
}


/**************************************************************************/
/*!
    @brief  Setter for Temperature oversampling
    @param  oversample Oversampling setting, can be BMP3_NO_OVERSAMPLING, BMP3_OVERSAMPLING_2X, BMP3_OVERSAMPLING_4X, BMP3_OVERSAMPLING_8X, BMP3_OVERSAMPLING_16X, BMP3_OVERSAMPLING_32X
    @return True on success, False on failure
*/
/**************************************************************************/

bool BMP3xx::setTemperatureOversampling(uint8_t oversample) {
  if (oversample > BMP3_OVERSAMPLING_32X) return false;

  the_sensor.settings.odr_filter.temp_os = oversample;

  if (oversample == BMP3_NO_OVERSAMPLING)
    _tempOSEnabled = false;
  else
    _tempOSEnabled = true;

  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Pressure oversampling
    @param  oversample Oversampling setting, can be BMP3_NO_OVERSAMPLING, BMP3_OVERSAMPLING_2X, BMP3_OVERSAMPLING_4X, BMP3_OVERSAMPLING_8X, BMP3_OVERSAMPLING_16X, BMP3_OVERSAMPLING_32X
    @return True on success, False on failure
*/
/**************************************************************************/
bool BMP3xx::setPressureOversampling(uint8_t oversample) {
  if (oversample > BMP3_OVERSAMPLING_32X) return false;

  the_sensor.settings.odr_filter.press_os = oversample;

  if (oversample == BMP3_NO_OVERSAMPLING)
    _presOSEnabled = false;
  else
    _presOSEnabled = true;

  return true;
}

/**************************************************************************/
/*!
    @brief  Setter for IIR filter coefficient
    @param filtercoeff Coefficient of the filter (in samples). Can be BMP3_IIR_FILTER_DISABLE (no filtering), BMP3_IIR_FILTER_COEFF_1, BMP3_IIR_FILTER_COEFF_3, BMP3_IIR_FILTER_COEFF_7, BMP3_IIR_FILTER_COEFF_15, BMP3_IIR_FILTER_COEFF_31, BMP3_IIR_FILTER_COEFF_63, BMP3_IIR_FILTER_COEFF_127
    @return True on success, False on failure

*/
/**************************************************************************/
bool BMP3xx::setIIRFilterCoeff(uint8_t filtercoeff) {
  if (filtercoeff > BMP3_IIR_FILTER_COEFF_127) return false;

  the_sensor.settings.odr_filter.iir_filter = filtercoeff;

  if (filtercoeff == BMP3_IIR_FILTER_DISABLE)
    _filterEnabled = false;
  else
    _filterEnabled = true;

  return true;
}

/**************************************************************************/
/*!
    @brief  Setter for output data rate (ODR)
    @param odr Sample rate in Hz. Can be BMP3_ODR_200_HZ, BMP3_ODR_100_HZ, BMP3_ODR_50_HZ, BMP3_ODR_25_HZ, BMP3_ODR_12_5_HZ, BMP3_ODR_6_25_HZ, BMP3_ODR_3_1_HZ, BMP3_ODR_1_5_HZ, BMP3_ODR_0_78_HZ, BMP3_ODR_0_39_HZ, BMP3_ODR_0_2_HZ, BMP3_ODR_0_1_HZ, BMP3_ODR_0_05_HZ, BMP3_ODR_0_02_HZ, BMP3_ODR_0_01_HZ, BMP3_ODR_0_006_HZ, BMP3_ODR_0_003_HZ, or BMP3_ODR_0_001_HZ 
    @return True on success, False on failure

*/
/**************************************************************************/
bool BMP3xx::setOutputDataRate(uint8_t odr) {
  if (odr > BMP3_ODR_0_001_HZ) return false;

  the_sensor.settings.odr_filter.odr = odr;

  _ODREnabled = true;

  return true;
}

/**************************************************************************/
/*!
    @brief  Reads 8 bit values over SPI
*/
/**************************************************************************/
static int8_t spi_read(uint8_t cspin, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

  uint8_t send[len + 1];
  uint8_t recv[len + 1];

  memset(send, 0x00, len + 1);
  memset(recv, 0x00, len + 1);

  send[0] = reg_addr | 0x80;

  gpio_set_pin_level(cspin, false);
  spi_m_os_transfer(BMP3xx_SPI, send, recv, len+1);
  gpio_set_pin_level(cspin, true);

  memcpy(reg_data, recv + 1, len);

  return 0;
}

/**************************************************************************/
/*!
    @brief  Writes 8 bit values over SPI
*/
/**************************************************************************/
static int8_t spi_write(uint8_t cspin, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

  uint8_t send[len + 1];
  uint8_t recv[len + 1];

  memset(send, 0x00, len + 1);  
  memcpy(send + 1, reg_data, len);

  send[0] = reg_addr;

  gpio_set_pin_level(cspin, false);
  spi_m_os_transfer(BMP3xx_SPI, send, recv, len+1);
  gpio_set_pin_level(cspin, true);

  return 0;
}


static void delay_msec(uint32_t ms){
  vTaskDelay(ms);
}