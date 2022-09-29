#include "EthernetTask.hpp"
#include "main.hpp"
#include "Task.hpp"

#include "pb_decode.h"
#include "cmd.pb.h"

// void test(){
//     quail_telemetry_Message msg = quail_telemetry_Message_init_zero;

//     msg.which_message = quail_telemetry_Message_reboot_tag;
//     sizeof(quail_telemetry_Message);
// }

constexpr uint8_t telemetry_t::metaslate_blob[];

EthernetTask::EthernetTask(uint8_t priority) : Task(priority, "Ethernet") {}

void EthernetTask::activity() {
    vTaskDelay(NETWORKING_DELAY);
    createUDP(slateConn, MY_SLATE_PORT, CLIENT_SLATE_PORT);
    createTCP(cmdConn, MY_CMD_PORT);
    isSetup = true;

    netconn *newconn;
    err_t err;
    while(true) {
        // accept a new TCP connection for commands
        err = netconn_accept(cmdConn, &newconn);

        if (err == ERR_OK) {
            netbuf *buf;
            pb_byte_t* data;
            uint16_t len;
            quail_telemetry_Message msg;

            // while connection is not stale
            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                do {
                    /*
                        Going to take a shortcut here, should revise in the future. 
                        Because the TCP stream is not packetized, we don't know when we've gotten a single full PB message. 
                        This could result in failures that may cascade into future messages in the stream. 
                        Or could result in some message being dropped. 
                        Begs the question of whether TCP is the right choice here.
                    */
                    netbuf_data(buf, (void**)&data, &len);
                    pb_istream_t rxd_msg = pb_istream_from_buffer(data, len);
                    pb_decode_ex(&rxd_msg,quail_telemetry_Message_fields, &msg, PB_DECODE_DELIMITED);

                    // process command and send ACK
                    err = requestHandler(newconn, (char *)data, len);

                    if (err != ERR_OK) {
                        PRINT("ETHERNET: cmd tcp connection failed to echo");
                    }
                } while(netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }

            // clean up and wait for new connection
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

err_t EthernetTask::msg_handler(quail_telemetry_Message &msg) {
    switch (msg.which_message)
    {
    case quail_telemetry_Message_reboot_tag:
        printf("Rebooting");
        //flush?
        NVIC_SystemReset();
        break;
    
    case quail_telemetry_Message_start_udp_tag:
        
        PRINT("start udp\n");
        break;

    case quail_telemetry_Message_request_metaslate_tag:
        PRINT("request meta\n");
        break;

    case quail_telemetry_Message_response_metaslate_tag:
        PRINT("response meta\n");
        break;

    case quail_telemetry_Message_set_field_tag:
        PRINT("set field\n");
        break;

    default:
        PRINT("oops\n");
        break;
    }
    return RET::OK;
}

err_t EthernetTask::requestHandler(netconn *&conn, char *rcv, uint16_t len) {
    // deserialization changes string, need a copy for echo server
    char rcvCpy[MAX_CMD_LENGTH];
    strncpy(rcvCpy, rcv, len);
    StaticJsonDocument<MAX_CMD_LENGTH> pckt;
    DeserializationError ret = deserializeJson(pckt, rcv);

    if (ret == DeserializationError::Ok) { // TCP requests must be JSONs
        if (pckt.containsKey("reboot")) { // Command echo server
            Serial.println("rebooting");
            NVIC_SystemReset();
        }
        if (pckt.containsKey("cmd")) { // Command echo server
            cmdBuf.send(rcvCpy, MAX_CMD_LENGTH);
            // Serial.println(rcvCpy);
            netconn_write(conn, rcvCpy, len, NETCONN_COPY);
            return netconn_write(conn, "\n", 1, NETCONN_COPY);
        }
        if (pckt.containsKey("meta")) { // Metaslate dump requests

            //get the address from which the request came, send the UDP there
            ip4_addr_t target_ip;
            u16_t port;
            netconn_getaddr(conn, &target_ip, &port, 0);
            createUDP(slateConn, MY_SLATE_PORT, CLIENT_SLATE_PORT, &target_ip);

            netconn_write(conn, telemetry_t::metaslate_blob, sizeof(telemetry_t::metaslate_blob), NETCONN_COPY);
            return netconn_write(conn, "\n", 1, NETCONN_COPY);
        }
    }
    
    // quotes ensure its a valid json string
    char send[] = "\"Misformatted TCP request\"\n";
    return netconn_write(conn, send, sizeof(send), NETCONN_COPY);
}

void EthernetTask::createUDP(netconn *&conn, uint16_t myport, uint16_t clientport, ip4_addr_t *target ) {
    err_t err;

    udpSetup = false;

    if (conn != NULL) {
        netconn_delete(conn);
    }

    // bind to a host UDP Connection
    conn = netconn_new(NETCONN_UDP);
    err = netconn_bind(conn, IP4_ADDR_ANY, myport);

    while (err != ERR_OK) {
        netconn_delete(conn);
        vTaskDelay(NETWORKING_DELAY);
        conn = netconn_new(NETCONN_UDP);
        err = netconn_bind(conn, IP4_ADDR_ANY, myport);
    }

    // if no target specified default to this
    if (target == NULL){
        ip4_addr_t dst;
        IP4_ADDR(&dst, 192, 168, 1, 3);
        target = &dst;
    }

    do {
        vTaskDelay(NETWORKING_DELAY);
        err = netconn_connect(conn, target, CLIENT_SLATE_PORT);
    } while(err != ERR_OK);

    udpSetup = true;
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

    netconn_listen(conn);
}

void EthernetTask::sendUDP(netconn *conn, JsonDocument& jsonDoc) {
    if (!isSetup) return;
    if (!udpSetup) return;

    uint16_t len = measureJson(jsonDoc);
    char str[len];
    serializeJson(jsonDoc, str, sizeof(str));
    sendUDP(conn, (uint8_t*)str, sizeof(str));
}

void EthernetTask::sendUDP(netconn *conn, const uint8_t* message, uint16_t fullLen) {
    if (!isSetup) return;
    if (!udpSetup) return;
    
    // create a packet
    netbuf *buf = netbuf_new();
    netbuf_alloc(buf, fullLen);

    // fill the packet
    // printf("fullLen: %lu \n",fullLen);

    // printf("*******\n\n");
    // printf("%s\n\n",message);
    // printf("*******");

    uint16_t size;
    void *data_ptr;
    netbuf_data(buf, &data_ptr, &size);
    memcpy(data_ptr, message, size);

    // printf("size: %lu \n",size);

    //send packet
    netconn_send(conn, buf);

    //clean up
    netbuf_delete(buf);

    vTaskDelay(5); //?
}