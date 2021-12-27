#include "Slate.hpp"
#include "ArduinoJson.h"

Slate slate("quail");

void tests(){

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

