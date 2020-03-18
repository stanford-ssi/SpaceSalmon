#include "System.hpp"

System sys;

extern "C" void __libc_init_array(void);

#include "SPI.h"
#include "periph/ADXL375/ADXL375.hpp"
#include "periph/BMP388/BMP388.hpp"
#include "periph/BMI088/BMI088.hpp"
#include "periph/MC33797/Squib.hpp"

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

	Serial.begin(9600);

	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
	pinMode(14, OUTPUT);

	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	digitalWrite(14, HIGH);

	sys.sensors.spi.begin();
	sys.sensors.spi.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));

	for(int i = 0; i < 10; i++){
		Serial.println("Bootup Timer...");
		for(int j = 0; j<1000000; j++){
			digitalWrite(4, HIGH);
		}
	}
	Serial.println("Starting!");

	vTaskStartScheduler();

}