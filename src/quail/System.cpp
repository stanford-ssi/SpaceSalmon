#include "System.hpp"

System sys;

extern "C" void __libc_init_array(void);

int main(void)
{
	//Arduino initialization (clocks and such)
	init();

	//libc initialization (do we need it? what's it for?)
	__libc_init_array();

	//magic sauce?
	delay(1);

	USBDevice.init();
	USBDevice.attach();

	//LEDs
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);

	//PWR_FLT pull-up
	pinMode(34,INPUT_PULLUP);
	digitalWrite(34,HIGH);

	//ADC SYNC line (the source of my misery)
	pinMode(9, OUTPUT);
	digitalWrite(9,HIGH);

	digitalWrite(1, HIGH); // turn on power LED

	//start all RTOS tasks (this never returns)
	vTaskStartScheduler();
}