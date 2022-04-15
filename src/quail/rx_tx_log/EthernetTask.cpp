#pragma once

#include "EthernetTask.hpp"
#include "main.hpp"
#include "Task.hpp"

EthernetTask::EthernetTask(uint8_t priority) : Task(priority, "Ethernet") {}

void EthernetTask::setup() {
    vTaskDelay(NETWORKING_DELAY);
    createUDP(slateConn, MY_SLATE_PORT, CLIENT_SLATE_PORT);
    createTCP(metaConn, MY_META_PORT);
    createTCP(cmdConn, MY_CMD_PORT);
    isSetup = true;
}

void EthernetTask::activity() {
    setup();

    netconn *newconn;
    err_t err;
    // while(true) {
    //     // accept a new TCP connection for commands
    //     err = netconn_accept(cmdConn, &newconn);

    //     if (err == ERR_OK) {
    //         netbuf *buf;
    //         void *data;
    //         uint16_t len;

    //         // while connection is not stale
    //         while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
    //             do {
    //                 // get pointers
    //                 netbuf_data(buf, &data, &len);

    //                 // send command to RXTask for processing
    //                 // cmdBuf.send((char *)data, DATA_PCKT_LEN);

    //                 // TCP echo confirms reception
    //                 err = netconn_write(newconn, data, len, NETCONN_COPY);
    //                 if (err != ERR_OK) {
    //                     PRINT("ETHERNET: cmd tcp connection failed to echo");
    //                 }
    //             } while(netbuf_next(buf) >= 0);
    //             netbuf_delete(buf);
    //         }

    //         // clean up and wait for new connection
    //         netconn_close(newconn);
    //         netconn_delete(newconn);
    //     }
    // }
}

void EthernetTask::createUDP(netconn *&conn, uint16_t myport, uint16_t clientport) {
    err_t err;

    // bind to a host UDP Connection
    conn = netconn_new(NETCONN_UDP);
    err = netconn_bind(conn, IP4_ADDR_ANY, myport);

    while (err != ERR_OK) {
        netconn_delete(conn);
        vTaskDelay(NETWORKING_DELAY);
        conn = netconn_new(NETCONN_UDP);
        err = netconn_bind(conn, IP4_ADDR_ANY, myport);
    }

    // connect to user
    ip4_addr_t dst;
    IP4_ADDR(&dst, 192, 168, 1, 1);

    do {
        vTaskDelay(NETWORKING_DELAY);
        err = netconn_connect(conn, &dst, CLIENT_SLATE_PORT);
    } while(err != ERR_OK);
    PRINT("UDP connected");
}

void EthernetTask::createTCP(netconn *&conn, uint16_t myport) {
    conn = netconn_new(NETCONN_TCP);
    err_t err = netconn_bind(conn, IP4_ADDR_ANY, myport);

    while (err != ERR_OK) {
        netconn_delete(conn);
        vTaskDelay(NETWORKING_DELAY);
        conn = netconn_new(NETCONN_TCP);
        err = netconn_bind(conn, IP4_ADDR_ANY, myport);
    }
}

void EthernetTask::sendUDP(netconn *conn, JsonDocument& jsonDoc) {
    if (!isSetup) return;
    char str[DATA_PCKT_LEN];
    serializeJson(jsonDoc, str, DATA_PCKT_LEN);
    sendUDP(conn, str);
}

void EthernetTask::sendUDP(netconn *conn, const char* message) {   
    if (!isSetup) return;
    PRINT("sending");
    
    // create a packet
    netbuf *buf = netbuf_new();
    netbuf_alloc(buf, DATA_PCKT_LEN);

    // fill the packet
    uint16_t size;
    void *data_ptr;
    netbuf_data(buf, &data_ptr, &size);
    memcpy(data_ptr, message, size);

    //send packet
    netconn_send(conn, buf);

    //clean up
    netbuf_delete(buf);
}