#include "SequenceTask.hpp"
#include "RXTask.hpp"
#include "main.hpp"

void SequenceTask::activity()
{
    vTaskDelay(7000);

    while(true){
        Serial.println("Test is opening!");
        sys.tasks.rxtask.sendcmd("{\"openSV\":5}"); // more complex
        vTaskDelay(3000);
        Serial.println("Test is closing!");
        sys.tasks.rxtask.sendcmd("{\"closeSV\":5}");
        vTaskDelay(3000);
        Serial.println("Test is pulsing!");
        sys.tasks.rxtask.sendcmd("{\"pulseSV\":5,\"pulseTime\":3000}"); // more complex
        vTaskDelay(5000);
        Serial.println("Test is ");
    };
};