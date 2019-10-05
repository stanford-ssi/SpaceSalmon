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
                                               3,                         //priority
                                               AltimeterTask::xStack,        //stack object
                                               &AltimeterTask::xTaskBuffer); //TCB object
}

TaskHandle_t AltimeterTask::getTaskHandle()
{
    return taskHandle;
}

void AltimeterTask::activity(void *ptr)
{
    char str[100];

    snprintf(str,sizeof(str),"Altimeter Started\nBuild Version: %s", PIO_BUILD);
    sys.tasks.logger.log(str);
    
    TickType_t lastStatusTime = xTaskGetTickCount();

    OneBattery battery(&ADC_0);

    while(true){

        vTaskDelayUntil(&lastStatusTime, 1000);

        gpio_set_pin_level(SENSOR_LED,true);

        StaticJsonDocument<1000> status_json;

        status_json["tick"] = xTaskGetTickCount();

        uint32_t runtime;
        TaskStatus_t tasks[6];
        uint8_t count = uxTaskGetSystemState(tasks, 6, &runtime);

        JsonObject tasks_json = status_json.createNestedObject("tasks");

        for(uint8_t i = 0; i < count; i++){
            float percent = ((float)tasks[i].ulRunTimeCounter)/((float)runtime)*100.0;
            tasks_json[tasks[i].pcTaskName] = percent;
        }

        OneBattery::cell_voltage_t voltage = battery.readVoltage();

        JsonObject bat_json = status_json.createNestedObject("bat");
        bat_json["cell"] = voltage.cellMain;

        status_json["log"] = sys.tasks.logger.isLoggingEnabled();

        sys.tasks.logger.logJSON(status_json,"status");

        gpio_set_pin_level(SENSOR_LED,false);
    }
}