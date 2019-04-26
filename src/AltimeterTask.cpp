#include "AltimeterTask.hpp"

TaskHandle_t AltimeterTask::taskHandle = NULL;
StaticTask_t AltimeterTask::xTaskBuffer;
StackType_t AltimeterTask::xStack[stackSize];

AltimeterTask::AltimeterTask()
{
    AltimeterTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "Altimeter",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               1,                         //priority
                                               AltimeterTask::xStack,        //stack object
                                               &AltimeterTask::xTaskBuffer); //TCB object
}

TaskHandle_t AltimeterTask::getTaskHandle()
{
    return taskHandle;
}

void AltimeterTask::activity(void *ptr)
{
    while(true){
        vTaskDelay(1000);
        gpio_set_pin_level(SENSOR_LED,true);

        StaticJsonDocument<1000> jsonDoc;
        JsonObject status_json = jsonDoc.createNestedObject("status");

        uint32_t runtime;
        TaskStatus_t tasks[6];
        uint8_t count = uxTaskGetSystemState(tasks, 6, &runtime);

        JsonObject tasks_json = status_json.createNestedObject("tasks");

        for(uint8_t i = 0; i < count; i++){
            tasks_json[tasks[i].pcTaskName] = tasks[i].ulRunTimeCounter;
        }

        char str[250];
        serializeJson(jsonDoc, str, sizeof(str));
        Globals::logger.log(str);

        gpio_set_pin_level(SENSOR_LED,false);
    }
        
    
}