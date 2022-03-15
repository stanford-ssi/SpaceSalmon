#include "SensorTask.hpp"
#include "Arduino.h"
#include "ArduinoJson.h"

// const static char some_data[] = "This is a cool UDP Test!\n";
// const static char some_data[] = "{tick:1,sensor:10}\n";

void SensorTask::activity()
{
    vTaskDelay(2000); // wait for ip stack!

    struct netconn *conn_1;
    int conn_check;

    printf("Starting UDP Sender!");

    /* create a connection structure */
    conn_1 = netconn_new(NETCONN_UDP);
    printf("created netconn \n");
    /* bind the connection to port on any IP address */
    conn_check = netconn_bind(conn_1, IP4_ADDR_ANY, 1000);
    while (conn_check != ERR_OK)
    {
        LWIP_DEBUGF(LWIP_DBG_ON, ("Bind error=%d\n", conn_check));
        netconn_delete(conn_1);
        vTaskSuspend(this->getTaskHandle());
    }

    ip4_addr_t dst;
    IP4_ADDR(&dst, 192, 168, 1, 1);
    netconn_connect(conn_1, &dst, 2001);

    int tick = 1;
    StaticJsonDocument<500> jsonDoc;
    static int sensor_data = 837799;
    static float y1 = 0;
    static float y2 = 0;
    static float y3 = 0;

    while (true)
    {
        // generate fake data
        if (sensor_data % 2 == 1)
        {
            sensor_data = 3 * sensor_data + 1;
        }
        else
        {
            sensor_data = sensor_data / 2;
        }

        

        y1 += 2 * (rand() % 2) - 1;
        y2 += 2 * (rand() % 2) - 1;
        y3 += 2 * (rand() % 2) - 1;
        
        // printf("tick: %d\n", tick);
        // printf("y1: %f, y2: %f, y3: %f\n", y1, y2, y3);

        // serialize to json
        jsonDoc["sensor"] = y1;
        jsonDoc["sensor2"] = y2;
        jsonDoc["sensor3"] = y3;
        jsonDoc["tick"] = tick;
        size_t len = measureJson(jsonDoc);
        char data_str[len + 5]; // plenty of room!
        serializeJson(jsonDoc, data_str, sizeof(data_str));

        // create packet
        netbuf *buf = netbuf_new();
        netbuf_alloc(buf, sizeof(data_str));
        void *data_ptr;
        u16_t data_size;
        netbuf_data(buf, &data_ptr, &data_size);
        memcpy(data_ptr, data_str, sizeof(data_str));

        // send packet
        netconn_send(conn_1, buf);

        // clean up
        netbuf_delete(buf);
        //vTaskDelay(1);
        ++tick;
    }
}