#include "LEDTask.hpp"

LEDTask::LEDTask(uint8_t priority) : Task(priority, "LED"){};

void LEDTask::activity()
{
    while (true)
    {
    digitalWrite(1,HIGH);
    vTaskDelay(500);
    digitalWrite(1,LOW);

    digitalWrite(2,HIGH);
    vTaskDelay(500);
    digitalWrite(2,LOW);

    digitalWrite(3,HIGH);
    vTaskDelay(500);
    digitalWrite(3,LOW);

    digitalWrite(4,HIGH);
    vTaskDelay(500);
    digitalWrite(4,LOW);
    
    }
}