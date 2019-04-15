#include "main.hpp"

LoggerTask Globals::logger;
SensorTask Globals::sensor;
SquibTask Globals::squib;

int main(void)
{
	//Initializes MCU, drivers and middleware
	atmel_start_init();

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

	//start all tasks
	vTaskStartScheduler();
}


void HardFault_Handler(void){
	assert(false,"fuck",1);
}