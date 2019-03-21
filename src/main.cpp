#include <atmel_start.h>
#include "ADXL375.hpp"
#include "BMI088.hpp"

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

	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	ADXL375 adxl375(&SPI_SENSOR,ADXL_CS_1);
	adxl375.init();
	adxl375.setDataRate(ADXL375::BW_12_5HZ);
	adxl375.startMeasuring();

	testBMIGyroSpi();
	testBMIAccelSpi();

	

	BMI088Accel bmi088accel(&SPI_SENSOR,ACCEL_CS_1);
	printf("begin %d\n", bmi088accel.begin());

	BMI088Gyro bmi088gyro(&SPI_SENSOR,GYRO_CS_1);
	printf("begin %d\n", bmi088gyro.begin());

	//printf("test: %f\n", 193.435F);
	//printf("str %d",5);

	for(int k=0;k<10000;k++){
		for(int i=0;i<1000;i++){}
			//gpio_toggle_pin_level(BUZZER);
	}


	while (true)
	{
		/*for(int j=0;j<20;j++){
			
			gpio_toggle_pin_level(LED4);
			
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){}
				//gpio_toggle_pin_level(BUZZER);
			}
			printf("Blink!\n");
			
		}
		*/

		//printf("SD Test!");
		//sdtester();
		
		for(int j=0;j<10;j++){
			delay_ms(50);
		}

		
		BMI088Accel::Data accel = bmi088accel.readSensor();
		printf("AX: %5d  ", (int)(accel.x*100));
		printf("AY: %5d  ", (int)(accel.y*100));
		printf("AZ: %5d  ", (int)(accel.z*100));
		printf("AT: %5d  ", (int)(accel.temp*100));
		printf("AT: %5d  ", accel.time);
		
		BMI088Gyro::Data gyro = bmi088gyro.readSensor();
		printf("GX: %5d  ", (int)(gyro.x*100));
		printf("GY: %5d  ", (int)(gyro.y*100));
		printf("GZ: %5d  ", (int)(gyro.z*100));		

		ADXL375::ADXL375_Data lol = adxl375.getXYZ();
		printf("HX: %5d  ", (int)(lol.x*100));
		printf("HY: %5d  ", (int)(lol.y*100));
		printf("HZ: %5d  \n", (int)(lol.z*100));


		for(int i=0;i<100000;i++){}
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
	uint8_t send[] = {0x80, 0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00, 0x00};
	spi_xfer data = {send, recv, 3};

	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	gpio_set_pin_level(BMP_CS_1, false);
	spi_m_sync_transfer(&SPI_SENSOR, &data);
	gpio_set_pin_level(BMP_CS_1, true);
	
	printf("BMP388 Response: 0x%x 0x%x 0x%x\n", data.rxbuf[0], data.rxbuf[1], data.rxbuf[2]);
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
	
	printf("Squib Response: 0x%x\n", data.rxbuf[0]);
	return data.rxbuf[0] == 0x69;
}