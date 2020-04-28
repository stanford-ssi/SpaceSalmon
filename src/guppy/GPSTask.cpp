#include "GPSTask.hpp"
#include "TinyGPS++.h"

GPSTask::GPSTask(uint8_t priority) : Task(priority, "GPSTask") {}

void GPSTask::activity()
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
            gps_data_t data;
            data.lat = parser.location.lat();
            data.lon = parser.location.lng();
            data.alt = parser.altitude.meters();
            data.time = parser.time.value();
            data.sats = parser.satellites.value();
            data.hdop = parser.hdop.hdop();
            data.valid = parser.location.isValid();
            locationData.post(data);

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