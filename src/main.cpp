#include "main.hpp"

LoggerTask Globals::logger;
SensorTask Globals::sensor;
SquibTask Globals::squib;
AltimeterTask Globals::altimeter;

int main(void)
{
	//Initializes MCU, drivers and middleware
	atmel_start_init();

	//hri_cmcc_write_CTRL_reg(CMCC, 1); //this seems to slow things down!

	//Bootup LED Sequence
	gpio_set_pin_level(LED1, true);
	gpio_set_pin_level(LED2, true);
	gpio_set_pin_level(LED3, true);
	gpio_set_pin_level(LED4, true);
	delay_ms(1000);

	gpio_set_pin_level(LED1, false);
	gpio_set_pin_level(LED2, false);
	gpio_set_pin_level(LED3, false);
	gpio_set_pin_level(LED4, false);
	delay_ms(1000);

	printf("\n");
	printf("#####################\n");
	printf("   SpaceSalmon 2.0   \n");
	printf("#####################\n");
	printf("\n");
	printf("Starting Tasks!\n\n\n");
	vTaskStartScheduler();
}


void HardFault_Handler(void){
	//assert(false,"fuck",1);
}