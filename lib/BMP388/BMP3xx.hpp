/*!
 * @file BMP3xx.h
 *
 * Adafruit BMP3XX temperature & barometric pressure sensor driver
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
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#pragma once

extern "C"
{
#include "bmp3.h"
#include <hal_spi_m_sync.h>
#include <hal_gpio.h>
#include <math.h>
#include <string.h>
#include <hal_delay.h>
}


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define BMP3XX_DEFAULT_ADDRESS       (0x77)     ///< The default I2C address
/*=========================================================================*/
#define BMP3XX_DEFAULT_SPIFREQ       (1000000)  ///< The default SPI Clock speed



/** BMP3xx Class for both I2C and SPI usage.
 *  Wraps the Bosch library for Arduino usage
 */

class BMP3xx
{
  public:
    typedef bmp3_data Data;

    BMP3xx(struct spi_m_sync_descriptor *spi, int8_t cspin);

    bool  begin();

    bool setTemperatureOversampling(uint8_t os);
    bool setPressureOversampling(uint8_t os);
    bool setIIRFilterCoeff(uint8_t fs);
    bool setOutputDataRate(uint8_t odr);

    /// Perform a reading in blocking mode
    Data readSensor(void);

    void write();

  private:
    int8_t _cs;
    bool _filterEnabled, _tempOSEnabled, _presOSEnabled, _ODREnabled;

    unsigned long _meas_end;

    uint8_t spixfer(uint8_t x);

    struct bmp3_dev the_sensor;
};
