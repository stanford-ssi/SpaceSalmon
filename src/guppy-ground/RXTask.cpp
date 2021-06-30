#include "RXTask.hpp"
#include "main.hpp"
#include <RadioLib.h>

TaskHandle_t RXTask::taskHandle = NULL;
StaticTask_t RXTask::xTaskBuffer;
StackType_t RXTask::xStack[stackSize];

RXTask::RXTask(uint8_t priority)
{
    RXTask::taskHandle = xTaskCreateStatic(activity_wrapper,              //static function to run
                                           "rx",                //task name
                                           stackSize,             //stack depth (words!)
                                           NULL,                  //parameters
                                           priority,              //priority
                                           RXTask::xStack,        //stack object
                                           &RXTask::xTaskBuffer); //TCB object
}

TaskHandle_t RXTask::getTaskHandle()
{
    return taskHandle;
}

void RXTask::activity_wrapper(void *ptr){
    sys.tasks.rx.activity();
}


void RXTask::activity()
{
    uint32_t i = 0;
    while (true)
    {
        packet_t packet;
        sys.tasks.radio.waitForPacket(packet);
        // i++;
        // char str[50];
        // snprintf(str, 50, "Got %lu of %s", i, (char *)packet.data);
        // log(info,str);
    }
}

void RXTask::log(log_type t, const char *msg)
{
    if (t & log_mask)
    {
        StaticJsonDocument<1000> doc;
        doc["id"] = pcTaskGetName(taskHandle);
        doc["msg"] = msg;
        doc["level"] = (uint8_t)t;
        doc["tick"] = xTaskGetTickCount();
        sys.tasks.logger.log(doc);
    }
}