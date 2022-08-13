#include "ethTest.hpp"
#include "string.h"
#include "Arduino.h"

static uint8_t arpdata[] = {0xFF, 0xFF, 0xFF ,0xFF ,0xFF ,0xFF 
                        ,0x0 ,0x0 ,0x0 ,0x0 ,0x20 ,0x76,
 0x8, 0x6, 
 0x0, 0x1,
 0x8, 0x0,
 0x6, 0x4, 
 0x0, 0x1, 
 0x0, 0x0, 0x0, 0x0, 0x20, 0x76,
 0xC0, 0xA8, 0x1, 0x2, 
 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0xC0, 0xA8, 0x1, 0x1};

 static uint8_t msgdata[] = "Hi, This is a test!";

void EthTestTask::activity()
{
    vTaskDelay(6000);

    netconn *conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP4_ADDR_ANY, 1345);
    ip4_addr_t dst;
    IP4_ADDR(&dst, 192, 168, 1, 1);
    netconn_connect(conn, &dst, 2483);

    while (true)
    {
        vTaskDelay(500);
        digitalWrite(1,HIGH);
        
        //printf("send: %s \n",arpdata);
        //SSIEth::global_eth->ethMAC.write(arpdata, sizeof(arpdata));
        
        vTaskDelay(5000);
        netbuf *buf = netbuf_new();
        netbuf_alloc(buf, sizeof(msgdata));

        uint16_t size;
        void *data_ptr;
        netbuf_data(buf, &data_ptr, &size);
        memcpy(data_ptr, msgdata, size);

        printf("size: %lu \n",size);

        //send packet
        netconn_send(conn, buf);

        //clean up
        netbuf_delete(buf);

        }
}