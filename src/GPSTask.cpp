#include "GPSTask.hpp"
#include "TinyGPS++.h"

TaskHandle_t GPSTask::taskHandle = NULL;
StaticTask_t GPSTask::xTaskBuffer;
StackType_t GPSTask::xStack[stackSize];

GPSTask::GPSTask(uint8_t priority)
{
    GPSTask::taskHandle = xTaskCreateStatic(activity,               //static function to run
                                            "GPSTask",              //task name
                                            stackSize,              //stack depth (words!)
                                            NULL,                   //parameters
                                            priority,               //priority
                                            GPSTask::xStack,        //stack object
                                            &GPSTask::xTaskBuffer); //TCB object
}

TaskHandle_t GPSTask::getTaskHandle()
{
    return taskHandle;
}

void GPSTask::activity(void *ptr)
{
    GPSSerial.begin(9600);
    TinyGPSPlus parser;
    TickType_t updateTimer = xTaskGetTickCount();
    while (true)
    {
        vTaskDelay(100);

        while (GPSSerial.available() > 0)
        {
            parser.encode(GPSSerial.read());
        }

        if (parser.location.isUpdated())
        {
            StaticJsonDocument<1024> gps_json;
            gps_json["updated"] = true;
            gps_json["valid"] = parser.location.isValid();
            gps_json["time"] = parser.time.value();
            gps_json["lat"] = parser.location.lat();
            gps_json["lng"] = parser.location.lng();
            gps_json["alt"] = parser.altitude.meters();
            gps_json["sat"] = parser.satellites.value();
            gps_json["failed"] = parser.failedChecksum();
            sys.tasks.logger.logJSON(gps_json, "gps");
            updateTimer = xTaskGetTickCount();
        }
        else if ((xTaskGetTickCount() - updateTimer) > 1000)
        {
            StaticJsonDocument<1024> gps_json;
            gps_json["updated"] = false;
            sys.tasks.logger.logJSON(gps_json, "gps");
            updateTimer = xTaskGetTickCount();
        }
    }
}