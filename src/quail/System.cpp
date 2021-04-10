#include "System.hpp"

System sys;

extern "C" void __libc_init_array(void);

// #include "SPI.h"
// #include "periph/ADXL375/ADXL375.hpp"
// #include "periph/BMP388/BMP388.hpp"
// #include "periph/BMI088/BMI088.hpp"
// #include "periph/MC33797/Squib.hpp"

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

	//start all RTOS tasks (this never returns)
	vTaskStartScheduler();
}