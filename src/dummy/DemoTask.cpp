#include "DemoTask.hpp"
#include "Arduino.h"
#include "lwip_netconn_api.h"
#include "lwip/api.h"

const static char some_data[] = "This is a cool UDP Test!\n";

void DemoTask::activity()
{
    vTaskDelay(2000); //wait for ip stack!

    struct netconn *conn_1;
	int conn_check;

    printf("Starting UDP Sender!");

	/* create a connection structure */
	conn_1 = netconn_new(NETCONN_UDP);
	/* bind the connection to port on any IP address */
	conn_check = netconn_bind(conn_1, IP4_ADDR_ANY, 1000);
	while (conn_check != ERR_OK) {
		LWIP_DEBUGF(LWIP_DBG_ON, ("Bind error=%d\n", conn_check));
        netconn_delete(conn_1);
        vTaskSuspend(this->getTaskHandle());
	}

    ip4_addr_t dst;
    IP4_ADDR(&dst, 192,168,1,1 );
    netconn_connect(conn_1,&dst,1001);

    while(true){
        netbuf* buf = netbuf_new();
        netbuf_alloc(buf,sizeof(some_data));
        void* data_ptr;
        u16_t data_size;
        netbuf_data(buf,&data_ptr,&data_size);
        memcpy(data_ptr,some_data,sizeof(some_data));
        netconn_send(conn_1,buf);
        netbuf_delete(buf);
        vTaskDelay(1000);
    }

}