#pragma once

#include "SSIEth.hpp"
#include "ArduinoJson.h"
#include "StrBuffer.hpp"
#include "../config.h"

class EthernetTask : public Task<2000>{
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

        netconn *slateConn = NULL; //NULL is important to ensure it doesn't get fake deleted the first time
        netconn *cmdConn;

        StrBuffer<10*MAX_CMD_LENGTH> cmdBuf;
        StaticJsonDocument<META_PCKT_LEN> metaJSON;
        char metaStr[META_PCKT_LEN];

        void setup();

        void createUDP(netconn *&conn, uint16_t myport, uint16_t client, ip4_addr_t *target = NULL);
        void createTCP(netconn *&conn, uint16_t myport);

        err_t requestHandler(netconn *&conn, char *rcv, uint16_t len);
};