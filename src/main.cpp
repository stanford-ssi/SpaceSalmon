#include <atmel_start.h>
#include "ADXL375/ADXL375.hpp"
#include "sdtester.c"

static uint8_t example_SPI_0[13] = "Hello World!";

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	spi_m_sync_enable(&SPI_SENSOR);

	struct spi_xfer data;

	spi_m_sync_set_mode(&SPI_SENSOR, SPI_MODE_3);
	
	gpio_set_pin_level(LED1,true);
	gpio_set_pin_level(LED2,true);
	gpio_set_pin_level(LED3,true);
	gpio_set_pin_level(LED4,true);
	
	for(int i=0;i<10000000;i++){}

	gpio_set_pin_level(LED1,false);
	gpio_set_pin_level(LED2,false);
	gpio_set_pin_level(LED3,false);
	gpio_set_pin_level(LED4,false);
	
	while(true){
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
		
		/*ADXL375 kek;
		kek.init();
		kek.startMeasuring();
		AccelData lol = kek.getXYZ();
		printf("X: %lu ",lol.x);
		printf("Y: %lu ",lol.y);
		printf("Z: %lu\n",lol.z);*/

	uint8_t send[] = {0x00, 0x00};
	uint8_t recv[] = {0x00, 0x00};


  	data.size = 1;
	data.txbuf = send;
	data.rxbuf = recv;

	
	for(int i=0;i<100;i++){}

	gpio_set_pin_level(ACCEL_CS_1,false);


	spi_m_sync_transfer(&SPI_SENSOR, &data);
  	//Set the Chip Select pin high to signal the end of an SPI packet.
  	gpio_set_pin_level(ADXL_CS_1,true);

	printf("resp: 0x%x ", data.rxbuf[0]);
	printf("0x%x\n", data.rxbuf[1]);

	}
		
}