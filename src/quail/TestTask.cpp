#include "TestTask.hpp"
#include "RXTask.hpp"
#include "main.hpp"

void TestTask::activity()
{
    vTaskDelay(9000);
    digitalWrite(2, HIGH); // write LED 2 low when TestTask gets started
    for(uint8_t i = 0; i < 8; i++){
        sys.statedata.setSolenoidState(i, OPEN);
        vTaskDelay(1000);
    };
    vTaskDelay(3000);
    for(uint8_t i = 0; i < 8; i++){
        sys.statedata.setSolenoidState(i, CLOSED);
        vTaskDelay(1000);
    };
    sys.tasks.rxtask.sendcmd("{\"pulseSV\":1, \"pulseTime\":5000}"); // attempt to pulse a solenoid
    digitalWrite(3,HIGH);
    vTaskDelay(5000);
    digitalWrite(4, HIGH);
};