#pragma once

#include "SSIEth.hpp"
#include "ArduinoJson.h"

class Ethernet {
    public:
        Ethernet();
        void send(const char* message);
        void send(JsonDocument& jsonDoc);

    private:
        struct netconn *conn;
};