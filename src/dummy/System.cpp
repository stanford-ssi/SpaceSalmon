#include "System.hpp"
#include "slate/Slate.hpp"

System sys;

extern "C" void __libc_init_array(void);

void tests();

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
	//vTaskStartScheduler();
	tests();
}

Slate slate("quail");

void tests(){

	Serial.begin(9600);
	delay(2000);

    slate.solenoids[1] = true;
    float f = slate.adc[0];
    slate.squib.arm = true;
    slate.squib.fire[0] = true;

    for(auto i : slate.adc.list){
        float a = i;
    }

    StaticJsonDocument<1000> doc;
    JsonVariant variant = doc.to<JsonVariant>();
    slate.dump(variant);
    serializeJson(doc,Serial);
}