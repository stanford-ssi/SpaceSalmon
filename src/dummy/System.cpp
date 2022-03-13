#include "System.hpp"

#include "lwip_netconn_api.h"
#include "ethernet_phy_main.h"
#include "SSIEth.hpp"


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

	pinMode(1,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);

	digitalWrite(1,1);
	digitalWrite(4,1);

	delay(2000);

	Serial.begin(9600);

	SSIEth::init();

	for(int i = 0; i<10;i++){
		SSIEth::test();
	}

	ethernet_phys_init();

	//start all RTOS tasks (this never returns)
	basic_netconn();
	
}


