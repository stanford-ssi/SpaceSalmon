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

	//start all RTOS tasks (this never returns)
	vTaskStartScheduler();
}