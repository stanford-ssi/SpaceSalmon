#include "SPITest.hpp"

bool testBMIGyroSpi()
{
	uint8_t send[] = {0x80, 0x00};
	uint8_t recv[] = {0x00, 0x00};

	gpio_set_pin_level(GYRO_CS_1, false);
	spi_m_os_transfer(&SPI_SENSOR, send, recv, 2);
	gpio_set_pin_level(GYRO_CS_1, true);
	
	printf_("BMI088-Gyro Response: 0x%x 0x%x\n", recv[0],recv[1]);
	return recv[1] == 0x0F;
}

bool testBMIAccelSpi()
{
	uint8_t send[] = {0x80, 0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00, 0x00};

	gpio_set_pin_level(ACCEL_CS_1, false);
	spi_m_os_transfer(&SPI_SENSOR, send, recv, 3);
	gpio_set_pin_level(ACCEL_CS_1, true);
	
	printf_("BMI088-Accel Response: 0x%x 0x%x 0x%x \n", recv[0], recv[1], recv[2]);
	return recv[2] == 0x1E;
}

bool testADXLSpi()
{
	uint8_t send[] = {0x00};
	uint8_t recv[] = {0x00};

	gpio_set_pin_level(ADXL_CS_1, false);
	spi_m_os_transfer(&SPI_SENSOR, send, recv, 1);
	gpio_set_pin_level(ADXL_CS_1, true);

	printf_("ADXL375 Response: 0x%x\n", recv[0]);
	return recv[0] == 0xE5;
}

bool testBMPSpi()
{
	uint8_t send[] = {0x80, 0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00, 0x00};

	gpio_set_pin_level(BMP_CS_1, false);
	spi_m_os_transfer(&SPI_SENSOR, send, recv, 3);
	gpio_set_pin_level(BMP_CS_1, true);
	
	printf_("BMP388 Response: 0x%x 0x%x 0x%x\n", recv[0], recv[1], recv[2]);
	return recv[2] == 0x50;
}

bool testSquibSpi(spi_m_os_descriptor* spi)
{
	uint8_t send = 0x96;
	uint8_t recv = 0x00;

	gpio_set_pin_level(SQUIB_CS, false);
	spi_m_os_transfer(spi, &send, &recv, 1);
	gpio_set_pin_level(SQUIB_CS, true);
	
	printf_("Squib Response: 0x%x\n", recv);
	
	return recv == 0x69;
}