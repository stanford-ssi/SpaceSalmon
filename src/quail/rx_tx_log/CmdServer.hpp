#pragma once

#include "ArduinoJson.h"
#include "SSIEth.hpp"
#include "StrBuffer.hpp"
#include "../config.h"
#include "cmd.pb.h"

class CmdServer : public Task<2000>
{
public:
    CmdServer(uint8_t priority);

    void activity();

private:
    netconn *cmdConn = NULL; // NULL is important to ensure it doesn't get fake deleted the first time

    err_t msg_handler(quail_telemetry_Message &msg, ip_addr_t *srcAddr, uint16_t srcPort);
};