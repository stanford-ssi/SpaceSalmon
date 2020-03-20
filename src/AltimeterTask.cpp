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
                                               2,                         //priority
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

    snprintf(str,sizeof(str),"Altimeter Started\nBuild Version: %i", 420);
    sys.tasks.logger.log(str);
    
    TickType_t lastStatusTime = xTaskGetTickCount();

    OneBattery battery(sys.adc0);

    while(true){

        vTaskDelayUntil(&lastStatusTime, 1000);

        digitalWrite(ALT_LED,true);

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
        bat_json["main"] = voltage.cellMain;
        bat_json["bkp"] = voltage.cellBackup;

        status_json["log"] = sys.tasks.logger.isLoggingEnabled();

        JsonArray pyro_json = status_json.createNestedArray("pyro");

        bool pyroA = sys.pyro.getStatus(Pyro::SquibA);
        bool pyroB = sys.pyro.getStatus(Pyro::SquibB);

        pyro_json.add(pyroA);
        pyro_json.add(pyroB);

        sys.tasks.logger.logJSON(status_json,"status");
        
        digitalWrite(ALT_LED,false);

        /*if(!sys.silent){
            if(pyroA && pyroB){
                sys.buzzer.set(5000);
                vTaskDelay(100);
                sys.buzzer.set(0);
            }else{
                sys.buzzer.set(800);
                vTaskDelay(300);
                sys.buzzer.set(0);
            }
        }*/
    }
}