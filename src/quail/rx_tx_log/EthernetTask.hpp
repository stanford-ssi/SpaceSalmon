#pragma once

#include "SSIEth.hpp"
#include "ArduinoJson.h"
#include "StrBuffer.hpp"
#include "../config.h"

class EthernetTask : public Task<4000>{
    public:
        EthernetTask(uint8_t priority);

        void activity();

        void send(const char* message, uint16_t len) {sendUDP(slateConn, message, len);}
        void send(JsonDocument& jsonDoc) {sendUDP(slateConn, jsonDoc);}

        void sendUDP(netconn *conn, const char* message, uint16_t len);
        void sendUDP(netconn *conn, JsonDocument& jsonDoc);

        void waitForCmd(char *cmd) {cmdBuf.receive(cmd, DATA_PCKT_LEN, true);};
        bool cmdAvailable() {return !cmdBuf.empty();}

    private:
        bool isSetup = false;

        netconn *slateConn;
        netconn *metaConn;
        netconn *cmdConn;

        StrBuffer<10*DATA_PCKT_LEN> cmdBuf;

        void setup();

        void createUDP(netconn *&conn, uint16_t myport, uint16_t client);
        void createTCP(netconn *&conn, uint16_t myport);

        err_t requestHandler(netconn *&conn, char *rcv, uint16_t len);
};