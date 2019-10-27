#include "GPSTask.hpp"

TaskHandle_t GPSTask::taskHandle = NULL;
StaticTask_t GPSTask::xTaskBuffer;
StackType_t GPSTask::xStack[stackSize];
TinyGPSPlus GPSTask::parser;

GPSTask::GPSTask()
{
    GPSTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "GPS",                 //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               1,                         //priority
                                               GPSTask::xStack,        //stack object
                                               &GPSTask::xTaskBuffer); //TCB object
}

TaskHandle_t GPSTask::getTaskHandle()
{
    return taskHandle;
}

void GPSTask::activity(void *ptr)
{
    sys.tasks.logger.log("Initializing GPS");

    uint8_t buf[16];

    usart_sync_enable(&USART_ESP);

    while (true)
    {
        if(usart_sync_is_rx_not_empty(&USART_ESP)){
            gpio_set_pin_level(GPS_LED, true);
            while(usart_sync_is_rx_not_empty(&USART_ESP)){
                int end = USART_ESP.io.read(&USART_ESP.io,buf,16);
                for(uint8_t i = 0; i < end; i++){
                    if(parser.encode(buf[i]) && parser.location.isUpdated()){
                        StaticJsonDocument<1024> gps_json;
                        gps_json["time"] = parser.time.value();
                        gps_json["lat"] = parser.location.lat();
                        gps_json["lng"] = parser.location.lng();
                        gps_json["alt"] = parser.altitude.meters();
                        gps_json["sat"] = parser.satellites.value();
                        sys.tasks.logger.logJSON(gps_json, "gps");
                    }
                }
            }
            gpio_set_pin_level(GPS_LED, false);
		}
    }
}