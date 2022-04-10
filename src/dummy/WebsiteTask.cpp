#include "WebsiteTask.hpp"
#include "Arduino.h"
#include "main.hpp"

/* This is the data for the actual web page. */
const static char quail_webpage[] = "<html> \
<head><title>Basic webpage</title></head> \
<body> \
<h1> Welcome to your basic webpage hosted from Quail over Ethernet!</h1> \
<img src=\"https://s3-us-west-1.amazonaws.com/habmc/assets/Logo_BlackSubText.png\"  style=\"width: 542px; height: 542px;\"> \
<form action=\"i_can_haz_data\"> \
  <input type=\"submit\" value=\"moar data\"> \
</form> \
<form action=\"too_much_data_stomp\"> \
  <input type=\"submit\" value=\"stomp data\"> \
</form> \
</body> \
</html>";
const static char http_html_hdr[]   = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";

void WebsiteTask::activity()
{
	vTaskDelay(2000);
	struct netbuf *inbuf;
	char *         rq;
	unsigned portSHORT len;
	int                conn_check;

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
				netbuf_data(inbuf, (void **)&rq, &len);
				
				/* Check if the request was an HTTP "GET /\r\n". */
				if ((NULL != rq) && (!strncmp(rq, "GET", 3))) {
					
					/* Check if the start/stop buttons have been clicked */
					if ((strlen(rq) > 19) && (!strncmp(rq, "GET /i_can_haz_data?", 19)) && sys.tasks.sensor_demo.get_state() == eSuspended) {
						printf("starting sensor data UDP stream\n");
						sys.tasks.sensor_demo.resume();
					} else if ((strlen(rq) > 24) && (!strncmp(rq, "GET /too_much_data_stomp?", 24))) {
						printf("stopping sensor data UDP stream\n");
						sys.tasks.sensor_demo.suspend();
					}
					
					/* Send the header. */
					conn_check = netconn_write(newconn_1, http_html_hdr, sizeof(http_html_hdr), NETCONN_NOCOPY);
					if (conn_check != ERR_OK) {
						LWIP_DEBUGF(LWIP_DBG_ON, ("Write error=%d\n", conn_check));
						goto netbuf_free;
					}

					/* Send the actual web page. */
					conn_check = netconn_write(newconn_1, quail_webpage, sizeof(quail_webpage), NETCONN_NOCOPY);
					if (conn_check != ERR_OK) {
						LWIP_DEBUGF(LWIP_DBG_ON, ("Write error=%d\n", conn_check));
						goto netbuf_free;
					}
				}
				netbuf_free:
					netbuf_delete(inbuf);
			}

		

		conn_close: /* Close the connection. */
			netconn_close(newconn_1);
			netconn_delete(newconn_1);
		}
	}
}