#include <atmel_start.h>
#include "ADXL375.hpp"
#include "BMI088.hpp"
#include "ArduinoJson.h"
#include "sdtester.c"
#include "printf.h"

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

	delay_ms(1000);

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
	bmi088accel.begin();

	BMI088Gyro bmi088gyro(&SPI_SENSOR,GYRO_CS_1);
	bmi088gyro.begin();

	//printf_("test: %f\n", 193.435F);
	//printf_("str %d",5);

	delay_ms(1000);

	while (true)
	{
		/*for(int j=0;j<20;j++){
			
			gpio_toggle_pin_level(LED4);
			
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){}
				//gpio_toggle_pin_level(BUZZER);
			}
			printf_("Blink!\n");
			
		}
		*/

		//printf_("SD Test!");	
		//sdtester();

		//printf_("TOPKEK\n");

		//printf_("test: %f\n", 193.435F);
		
		//delay_ms(1000);

		//char strtest[] = "HI famity!";
		DynamicJsonDocument doc(1024);
		doc["key"] = "value";

		char string[100];
		serializeJson(doc,string,sizeof(string));

		//cdcdf_acm_write((uint8_t *)string, 5);

		//printf_("TOPKE2\n");

		//printf_("%s\n",string);

		//printf_("TOPKE3\n");

		BMI088Accel::Data accel = bmi088accel.readSensor();

		printf_("AX: %5f  ", accel.x);
		printf_("AY: %5f  ", accel.y);
		printf_("AZ: %5f  ", accel.z);
		printf_("AT: %5fC ", accel.temp);
		printf_("AT: %5lu  ", accel.time);

		BMI088Gyro::Data gyro = bmi088gyro.readSensor();
		printf_("GX: %5d  ", (int)(gyro.x*100));
		printf_("GY: %5d  ", (int)(gyro.y*100));
		printf_("GZ: %5d  ", (int)(gyro.z*100));		

		ADXL375::ADXL375_Data lol = adxl375.getXYZ();
		printf_("HX: %5d  ", (int)(lol.x*100));
		printf_("HY: %5d  ", (int)(lol.y*100));
		printf_("HZ: %5d  \n", (int)(lol.z*100));
		

		//delay_ms(500);
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