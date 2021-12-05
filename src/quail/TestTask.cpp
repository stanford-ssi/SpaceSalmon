#include "TestTask.hpp"
#include "RXTask.hpp"
#include "main.hpp"

void TestTask::activity()
{
    vTaskDelay(6000);
    digitalWrite(2, HIGH); // write LED 2 low when TestTask gets started
    sys.tasks.rxtask.sendcmd("{\"openSV\":[0,1,2,3,4,5]}");
    vTaskDelay(1000);
    sys.tasks.rxtask.sendcmd("{\"waitThen\":{\"userCmd\":\"abortOx\"}, \"waitTime\":5000}");
    digitalWrite(4, HIGH);
};