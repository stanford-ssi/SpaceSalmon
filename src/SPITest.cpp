#include "SPITest.hpp"

bool testBMIGyroSpi()
{
	uint8_t send[] = {0x80, 0x00};
	uint8_t recv[] = {0x00, 0x00};
	spi_xfer data = {send, recv, 2};
	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);
	gpio_set_pin_level(GYRO_CS_1, false);
	spi_m_sync_transfer(&SPI_SENSOR, &data);
	gpio_set_pin_level(GYRO_CS_1, true);
	
	printf_("BMI088-Gyro Response: 0x%x 0x%x\n", data.rxbuf[0], data.rxbuf[1]);
	return data.rxbuf[1] == 0x0F;
}

bool testBMIAccelSpi()
{
	uint8_t send[] = {0x80, 0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00, 0x00};
	spi_xfer data = {send, recv, 3};

	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	gpio_set_pin_level(ACCEL_CS_1, false);
	spi_m_sync_transfer(&SPI_SENSOR, &data);
	gpio_set_pin_level(ACCEL_CS_1, true);
	
	printf_("BMI088-Accel Response: 0x%x 0x%x 0x%x \n", data.rxbuf[0], data.rxbuf[1], data.rxbuf[2]);
	return data.rxbuf[2] == 0x1E;
}

bool testADXLSpi()
{
	uint8_t send[] = {0x00};
	uint8_t recv[] = {0x00};
	spi_xfer data = {send, recv, 1};
	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	gpio_set_pin_level(ADXL_CS_1, false);
	spi_m_sync_transfer(&SPI_SENSOR, &data);
	gpio_set_pin_level(ADXL_CS_1, true);

	printf_("ADXL375 Response: 0x%x\n", data.rxbuf[0]);
	return data.rxbuf[0] == 0xE5;
}

bool testBMPSpi()
{
	uint8_t send[] = {0x80, 0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00, 0x00};
	spi_xfer data = {send, recv, 3};

	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	gpio_set_pin_level(BMP_CS_1, false);
	spi_m_sync_transfer(&SPI_SENSOR, &data);
	gpio_set_pin_level(BMP_CS_1, true);
	
	printf_("BMP388 Response: 0x%x 0x%x 0x%x\n", data.rxbuf[0], data.rxbuf[1], data.rxbuf[2]);
	return data.rxbuf[2] == 0x50;
}

bool testSquibSpi()
{
	uint8_t send = 0x96;
	uint8_t recv = 0x00;
	spi_xfer data = {&send, &recv, 1};
	spi_m_sync_disable(&SPI_SQUIB);
	spi_m_sync_set_mode(&SPI_SQUIB, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SQUIB);

	gpio_set_pin_level(SQUIB_CS, false);
	spi_m_sync_transfer(&SPI_SQUIB, &data);
	gpio_set_pin_level(SQUIB_CS, true);
	
	printf_("Squib Response: 0x%x\n", data.rxbuf[0]);
	return data.rxbuf[0] == 0x69;
}