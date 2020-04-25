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
        
        gps_data_t data;
        sys.tasks.gps.data_post.get(data);
        OneBattery::cell_voltage_t batt;
        sys.tasks.alt.batt_poster.get(batt);

        packet_t pkt;
        StaticJsonDocument<1000> doc;
        doc["lat"] = data.lat;
        doc["lon"] = data.lon;
        doc["alt"] = data.alt;
        doc["bat"] = batt.cellMain;
        char str[200];
        serializeJson(doc, str, 200);
        sys.tasks.logger.log(str);
        strcpy((char*)pkt.data, str);
        pkt.len = strlen(str);
        sys.tasks.radio.sendPacket(pkt);
    }
}