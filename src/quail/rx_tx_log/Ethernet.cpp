#include "Ethernet.hpp"
#include "../config.h"

Ethernet::Ethernet() {
    err_t err;

    // bind to a host port
    conn = netconn_new(NETCONN_UDP);
    err = netconn_bind(conn, IP4_ADDR_ANY, MY_PORT);

    while (err != ERR_OK) {
        netconn_delete(conn);
        //honestly if we get here fuck all
    }

    // connect to user
    ip4_addr_t dst;
    IP4_ADDR(&dst, 192, 168, 1, 1);
    err = netconn_connect(conn, &dst, 2001);

    while(err != ERR_OK) {

    }
}

void Ethernet::send(JsonDocument& jsonDoc) {
    char str[DATA_PCKT_LEN];
    serializeJson(jsonDoc, str, DATA_PCKT_LEN);
    send(str);
}

void Ethernet::send(const char* message) {
    // want data as void * (still const i promise)
    void *data_ptr;
    memcpy(data_ptr, message, DATA_PCKT_LEN);
    
    // create a packet
    netbuf *buf = netbuf_new();
    uint16_t size;
    netbuf_alloc(buf, DATA_PCKT_LEN);
    netbuf_data(buf, &data_ptr, &size);

    //send packet
    netconn_send(conn, buf);

    //clean up
    netbuf_delete(buf);
}