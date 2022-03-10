/**
 * \file
 *
 * \brief LwIP netconn api example
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */



#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "webserver_tasks.h"
#include "FreeRTOS.h"
#include "lwip_netconn_api.h"
#include "string.h"

#include "netif/etharp.h"


/* This is the data for the actual web page. */
const static char netconn_webpage[] = "<html> \
<head><title>Basic webpage</title></head> \
<body> \
Welcome to your basic webpage hosted from Quail over Ethernet!. \
</body> \
</html>";
const static char http_html_hdr[]   = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";

void basic_netconn()
{
	/* Create LED task */
	task_led_create();

	/* Create task for Ethernet */
	if (xTaskCreate(netconn_basic_ethernet,
	                "Ethernet_basic",
	                TASK_ETHERNETBASIC_STACK_SIZE,
	                NULL,
	                (TASK_ETHERNETBASIC_STACK_PRIORITY - 1),
	                &xCreatedEthernetBasicTask)
	    != pdPASS) {
		while (1)
			;
	}

	/* Start FreeRTOS scheduler */
	vTaskStartScheduler();
}

void netconn_basic_ethernet(void *p)
{

	struct netbuf *inbuf;
	char *         rq;
	unsigned portSHORT len;
	int                conn_check;

	sys_sem_t sem;
	err_t     err_sem;
	err_sem = sys_sem_new(&sem, 0); /* Create a new semaphore. */
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(&sem); /* Block until the lwIP stack is initialized. */
	sys_sem_free(&sem); /* Free the semaphore. */

	print_ipaddress();

	printf("A!\n");

	//gpio_set_pin_level(LED_1,false);

	//struct eth_addr eth = {{0xa0, 0xce, 0xc8, 0x0e, 0xb4, 0x83}};
	ip_addr_t addr;
	IP4_ADDR(&addr, 192, 168, 1, 1);     

	//etharp_add_static_entry(&addr,&eth);

	struct netconn *test = netconn_new_with_proto_and_callback(NETCONN_RAW,0,0);
	printf("test = %#010x\n", (uint32_t) test);
	netconn_connect(test,&addr,0);

	printf("Connected!\n");

	struct netbuf *buf = netbuf_new();
	netbuf_alloc(buf,100);
	char* ptr;
	uint16_t plen;
	netbuf_data(buf,&ptr,&plen);
	strcpy(ptr,"This Is a Test!");
 
	u16_t port = 100;
	netconn_new(NETCONN_RAW);

	//gpio_set_pin_level(LED_2,false);



	printf("B!\n");
	netconn_send(test,buf);

	printf("C!\n");
	LWIP_DEBUGF(LWIP_DBG_ON, ("DebugPrint?\n"));
	//gpio_set_pin_level(LED_3,false);

	struct netconn *conn_1, *newconn_1;
	/* create a connection structure */
	conn_1 = netconn_new(NETCONN_TCP);
	/* bind the connection to port on any IP address */
	conn_check = netconn_bind(conn_1, NULL, 80);
	while (conn_check != ERR_OK) {
		LWIP_DEBUGF(LWIP_DBG_ON, ("Bind error=%d\n", conn_check));
		goto conn_close;
	}




	/* tell the connection to listen for incoming
	connection requests */
	netconn_listen(conn_1);

	for (;;) {

		conn_check = netconn_accept(conn_1, &newconn_1);
		while (conn_check != ERR_OK) {
			LWIP_DEBUGF(LWIP_DBG_ON, ("Connection accept error=%d\n", conn_check));
			goto conn_close;
		}

		if (newconn_1 != NULL) {
			conn_check = netconn_recv(newconn_1, &inbuf);
			while (conn_check != ERR_OK) {
				LWIP_DEBUGF(LWIP_DBG_ON, ("Receive error=%d\n", conn_check));
				goto conn_close;
			}

			if (inbuf != NULL) {
				/* Get the pointer to the data in the first netbuf
				fragment which we hope contains the request. */
				netbuf_data(inbuf, (void *)&rq, &len);
				/* Check if the request was an HTTP "GET /\r\n". */
				if ((NULL != rq) && (!strncmp(rq, "GET", 3))) {
					/* Send the header. */
					conn_check = netconn_write(newconn_1, http_html_hdr, sizeof(http_html_hdr), NETCONN_NOCOPY);
					if (conn_check != ERR_OK) {
						LWIP_DEBUGF(LWIP_DBG_ON, ("Write error=%d\n", conn_check));
						goto conn_close;
					}

					/* Send the actual web page. */
					conn_check = netconn_write(newconn_1, netconn_webpage, sizeof(netconn_webpage), NETCONN_NOCOPY);
					if (conn_check != ERR_OK) {
						LWIP_DEBUGF(LWIP_DBG_ON, ("Write error=%d\n", conn_check));
						goto conn_close;
					}
				}
				netbuf_delete(inbuf);
			}

		conn_close: /* Close the connection. */
			netconn_close(newconn_1);
			netconn_delete(newconn_1);
		}
	}
}