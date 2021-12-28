#include "main.hpp"

void TestTask::activity()
{
    vTaskDelay(6000);
    
    pinMode(LED_BUILTIN,OUTPUT);
	digitalWrite(LED_BUILTIN,HIGH);

	Serial.begin(9600);
	Serial.println("Test!");

    // slate.solenoids[1] = true;
    // float f = slate.adc[0];
    // slate.squib.arm = true;
    // slate.squib.fire[0] = true;

    // for(auto i : slate.adc.list){
    //     float a = i;
    // }

	Serial.println("OK");

    // StaticJsonDocument<1000> doc;
    // JsonVariant variant = doc.to<JsonVariant>();
    // slate.dump(variant);
    // serializeJson(doc,Serial);
};