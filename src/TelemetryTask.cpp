#include "TelemetryTask.hpp"

TaskHandle_t TelemetryTask::taskHandle = NULL;
StaticTask_t TelemetryTask::xTaskBuffer;
StackType_t TelemetryTask::xStack[stackSize];

TelemetryTask::TelemetryTask(uint8_t priority)
{
    TelemetryTask::taskHandle = xTaskCreateStatic(activityWrapper,               //static function to run
                                            "TelemetryTask",              //task name
                                            stackSize,              //stack depth (words!)
                                            NULL,                   //parameters
                                            priority,               //priority
                                            TelemetryTask::xStack,        //stack object
                                            &TelemetryTask::xTaskBuffer); //TCB object
}

TaskHandle_t TelemetryTask::getTaskHandle()
{
    return taskHandle;
}

void TelemetryTask::activityWrapper(void *ptr)
{
    sys.tasks.telem.activity();
}

void TelemetryTask::activity(){
    while(true){
        vTaskDelay(1000);
    }
}