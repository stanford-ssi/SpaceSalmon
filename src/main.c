#include <atmel_start.h>

static uint8_t example_SPI_0[12] = "Hello World!";

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_SQUIB, &io);

	struct spi_xfer data;
	
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
		for(int j=0;j<20;j++){
			
			gpio_toggle_pin_level(LED4);
			
			for(int k=0;k<100;k++){
				for(int i=0;i<10000;i++){}
				//gpio_toggle_pin_level(BUZZER);
			}
			printf("Blink!\n");
			
		}

		printf("SPI Test!\n");
		
		uint8_t send = 0x96;
		uint8_t recv = 0x00;

		data.size = 1;
		data.txbuf = &send;
		data.rxbuf = &recv;

		printf("sent: 0x%x\n", *(data.txbuf));

		gpio_set_pin_level(SQUIB_CS,false);
		spi_m_sync_enable(&SPI_SQUIB);
		spi_m_sync_transfer(&SPI_SQUIB, &data);
		gpio_set_pin_level(SQUIB_CS,true);
		
		printf("resp: 0x%x\n", *(data.rxbuf));
		
		printf("SD Test!");
		sdtester();
	}
		
}