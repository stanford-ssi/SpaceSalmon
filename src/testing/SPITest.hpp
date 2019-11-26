#include <atmel_start_pins.h>
#include "stdint.h"
#include "hal_spi_m_os.h"
#include "hal_gpio.h"
#include "printf.h"
#include <driver_init.h>

bool testBMIGyroSpi();
bool testBMIAccelSpi();
bool testADXLSpi();
bool testBMPSpi();
bool testSquibSpi(spi_m_os_descriptor *spi);