#include <atmel_start.h>
#include "ADXL375.hpp"
#include "BMI088.hpp"
#include "ArduinoJson.h"
#include "sdtester.c"
#include "printf.h"
#include "BMP3xx.hpp"
#include "SPITest.hpp"
#include "Squib.hpp"
#include "Battery.hpp"

uint32_t ms = 0;

uint32_t millis(){//todo: interupts???
	return ms;
}

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


	spi_m_sync_disable(&SPI_SENSOR);
	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SENSOR);

	spi_m_sync_disable(&SPI_SQUIB);
	spi_m_sync_set_mode(&SPI_SQUIB, SPI_MODE_3);
	spi_m_sync_enable(&SPI_SQUIB);

	uint8_t leds[] = {LED1,LED2,LED3,LED4};

	uint8_t led = 0;

	bool fire = false;

	SysTick_Config(48000000/1000);


    ADXL375 adxl375(&SPI_SENSOR, ADXL_CS_1);
    adxl375.init();
	adxl375.startMeasuring();

	BMP3xx bmp388(&SPI_SENSOR, BMP_CS_1);
    bmp388.begin();

    BMI088Accel bmi088accel(&SPI_SENSOR, ACCEL_CS_1);
    bmi088accel.begin();

    BMI088Gyro bmi088gyro(&SPI_SENSOR, GYRO_CS_1);
    bmi088gyro.begin();

    ADXL375::Data accelHigh;
    BMP3xx::Data pressure;
    BMI088Gyro::Data gyro;
    BMI088Accel::Data accel;

    while (true)
    {
		delay_ms(50);
        //accelHigh = adxl375.readSensor();
        pressure = bmp388.readSensor();
        //accel = bmi088accel.readSensor();
        //gyro = bmi088gyro.readSensor();

        printf_(" High:{%f %f %f} ", accelHigh.x, accelHigh.y, accelHigh.z);
        printf_(" Pres:{%f %f} ", pressure.pressure, pressure.temperature);
        printf_(" Gyro:{%f %f %f} ", gyro.x, gyro.y, gyro.z);
        printf_(" Accel:{%f %f %f %lu} \n", accel.x, accel.y, accel.z, accel.time);
    }
}


void SysTick_Handler(void){
	ms++;
}

