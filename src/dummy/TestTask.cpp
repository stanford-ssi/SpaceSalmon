#include "main.hpp"
#include "SSIEth.hpp"

void TestTask::activity()
{
    pinMode(1, OUTPUT);
    Serial.begin(9600);

    SSIEth::init();

    Serial.println("INIT Done!");
    
    while(true){
        SSIEth::test();
        Serial.println("Sent!");
        vTaskDelay(1000);
    }
};