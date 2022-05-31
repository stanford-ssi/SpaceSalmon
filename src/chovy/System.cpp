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

	// LEDs
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);

	// COTS arming
	digitalWrite(32, LOW);
	digitalWrite(33, LOW);
	pinMode(32, OUTPUT);
	pinMode(33, OUTPUT);

	pinMode(25,OUTPUT);
	pinMode(26,OUTPUT);
	pinMode(27,INPUT);
	pinMode(28,OUTPUT);
	pinMode(29,INPUT);
	

	//start all RTOS tasks (this never returns)
	vTaskStartScheduler();
}