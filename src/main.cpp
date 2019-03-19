#include <atmel_start.h>
#include "ADXL375/ADXL375.hpp"
#include "sdtester.c"

bool testBMIGyroSpi();
bool testBMIAccelSpi();
bool testADXLSpi();
bool testBMPSpi();
bool testSquibSpi();

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	gpio_set_pin_level(LED1, true);
	gpio_set_pin_level(LED2, true);
	gpio_set_pin_level(LED3, true);
	gpio_set_pin_level(LED4, true);

	for (int i = 0; i < 10000000; i++)
	{
	}

	gpio_set_pin_level(LED1, false);
	gpio_set_pin_level(LED2, false);
	gpio_set_pin_level(LED3, false);
	gpio_set_pin_level(LED4, false);

	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_0);
	spi_m_sync_enable(&SPI_SENSOR);

	//ADXL375 kek;
	//kek.init();
	//kek.setDataRate(BW_12_5HZ);
	//kek.startMeasuring();

	while (true)
	{
		/*for(int j=0;j<20;j++){
			
			gpio_toggle_pin_level(LED4);
			
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){}
				//gpio_toggle_pin_level(BUZZER);
			}
			printf("Blink!\n");
			
		}*/

		//printf("SPI Test!\n");
		/*
		uint8_t send = 0x96;
		uint8_t recv = 0x00;

		data.size = 1;
		data.txbuf = &send;
		data.rxbuf = &recv;

		//printf("sent: 0x%x\n", *(data.txbuf));

		gpio_set_pin_level(SQUIB_CS,false);
		spi_m_sync_enable(&SPI_SQUIB);
		spi_m_sync_transfer(&SPI_SQUIB, &data);
		spi_m_sync_disable(&SPI_SQUIB);
		gpio_set_pin_level(SQUIB_CS,true);
		*/
		//printf("resp: 0x%x\n", *(data.rxbuf));

		//printf("SD Test!");
		//sdtester();
		
		/*
		AccelData lol = kek.getXYZ();
		printf("X: %7d  ",lol.x);
		printf("Y: %7d  ",lol.y);
		printf("Z: %7d  \n",lol.z);
		*/

		for (int i = 0; i < 1000; i++)
		{
		}

		bool bmi;

		//test BMI088-Gyro

		bmi = testBMIGyroSpi();
		bmi &= testBMIAccelSpi();
		gpio_set_pin_level(LED1, bmi);

		for (int i = 0; i < 1000; i++)
		{
		}

		//test ADXL375
		gpio_set_pin_level(LED2, testADXLSpi());

		for (int i = 0; i < 1000; i++)
		{
		}

		//test BMP388
		//gpio_set_pin_level(LED3, testBMPSpi());

		//for (int i = 0; i < 1000; i++){}

		//test Squib Driver
		gpio_set_pin_level(LED4, testSquibSpi());

		for (int i = 0; i < 1000; i++)
		{
		}
	}
}

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
	
	printf("BMI088-Gyro Response: 0x%x 0x%x\n", data.rxbuf[0], data.rxbuf[1]);
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
	
	printf("BMI088-Accel Response: 0x%x 0x%x 0x%x \n", data.rxbuf[0], data.rxbuf[1], data.rxbuf[2]);
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

	printf("ADXL375 Response: 0x%x\n", data.rxbuf[0]);
	return data.rxbuf[0] == 0xE5;
}

bool testBMPSpi()
{
	uint8_t send[] = {0x80, 0x00, 0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00, 0x00, 0x00};
	spi_xfer data = {send, recv, 4};

	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	gpio_set_pin_level(BMP_CS_1, false);
	spi_m_sync_transfer(&SPI_SENSOR, &data);
	gpio_set_pin_level(BMP_CS_1, true);
	
	printf("BMP388 Response: 0x%x 0x%x 0x%x 0x%x\n", data.rxbuf[0], data.rxbuf[1], data.rxbuf[2], data.rxbuf[3]);
	return data.rxbuf[0] == 0x50;
}

bool testSquibSpi()
{
	uint8_t send = 0x96;
	uint8_t recv = 0x00;
	spi_xfer data = {&send, &recv, 1};
	spi_m_sync_disable(&SPI_SQUIB);
	spi_m_sync_set_mode(&SPI_SQUIB, SPI_MODE_0);
	spi_m_sync_enable(&SPI_SQUIB);
	
	gpio_set_pin_level(SQUIB_CS, false);
	spi_m_sync_transfer(&SPI_SQUIB, &data);
	gpio_set_pin_level(SQUIB_CS, true);
	
	printf("Squib Response: 0x%x\n", data.rxbuf[0]);
	return data.rxbuf[0] == 0x69;
}