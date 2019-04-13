#include "support/atmel_start.h"
#include "SquibTask.hpp"
#include "SensorTask.hpp"

/*
int main(void)
{
	//Initializes MCU, drivers and middleware
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

	
	
	//SquibTask squib;

	//SensorTask sensor;

	//vTaskStartScheduler();
}*/


void HardFault_Handler(void){
	assert(false,"fuck",1);
}