#include "CommandTask.hpp"
#include "Arduino.h"
#include "ArduinoJson.h"

void CommandTask::activity()
{
    vTaskDelay(2000); // wait for ip stack!

    struct netconn *conn, *newconn;
    err_t err;

    /* Create a new connection identifier. */
    /* Bind connection to well known port number 7. */

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 2002);

    LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

    /* Tell connection to go into listening mode. */
    netconn_listen(conn);

    while (1)
    {

        /* Accept new connection. */
        err = netconn_accept(conn, &newconn);

        /* Process the new connection. */
        if (err == ERR_OK)
        {
            struct netbuf *buf;
            void *data;
            u16_t len;

            while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
            {
                do
                {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_COPY);
                    if (err != ERR_OK)
                    {
                        printf("tcpecho: netconn_write: error \"%s\"\n", lwip_strerr(err));
                    }

                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }

            /* Close connection and discard connection identifier. */
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}