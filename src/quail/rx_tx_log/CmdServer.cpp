#include "CmdServer.hpp"
#include "main.hpp"
#include "Task.hpp"

#include "pb_decode.h"
#include "pb_encode.h"
#include "cmd.pb.h"

CmdServer::CmdServer(uint8_t priority) : Task(priority, "Ethernet") {}

void CmdServer::activity()
{
    vTaskDelay(4000);
    sys.slateServer.bind(MY_SLATE_PORT);

    netconn *conn = netconn_new(NETCONN_TCP);
    err_t err = netconn_bind(conn, IP4_ADDR_ANY, MY_CMD_PORT);

    while (err != ERR_OK)
    {
        netconn_delete(conn);
        vTaskDelay(NETWORKING_DELAY);
        conn = netconn_new(NETCONN_TCP);
        err = netconn_bind(conn, IP4_ADDR_ANY, MY_CMD_PORT);
    }

    netconn_listen(conn);
    isSetup = true;

    netconn *newconn;
    while (true)
    {
        // accept a new TCP connection for commands
        err = netconn_accept(conn, &newconn);

        if (err == ERR_OK)
        {
            netbuf *buf;
            pb_byte_t *data;
            uint16_t len;
            quail_telemetry_Message msg;

            // while connection is not stale
            while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
            {
                do
                {
                    /*
                        Going to take a shortcut here, should revise in the future.
                        Because the TCP stream is not packetized, we don't know when we've gotten a single full PB message.
                        This could result in failures that may cascade into future messages in the stream.
                        Or could result in some message being dropped.
                        Begs the question of whether TCP is the right choice here.
                    */
                    netbuf_data(buf, (void **)&data, &len);
                    pb_istream_t rxd_msg = pb_istream_from_buffer(data, len);
                    pb_decode_ex(&rxd_msg, quail_telemetry_Message_fields, &msg, PB_DECODE_DELIMITED);

                    // process command and send ACK
                    bool respond = false;
                    msg_handler(msg, respond);

                    if (respond)
                    {
                        pb_ostream_t substream = PB_OSTREAM_SIZING;
                        pb_encode_ex(&substream, quail_telemetry_Message_fields, &msg, PB_ENCODE_DELIMITED); // this is very waseful. We're encoding like 4 times here.
                        uint8_t data[substream.bytes_written];
                        substream = pb_ostream_from_buffer(data, substream.bytes_written);
                        pb_encode_ex(&substream, quail_telemetry_Message_fields, &msg, PB_ENCODE_DELIMITED);
                        netconn_write(newconn, data, substream.bytes_written, NETCONN_COPY);
                        printf("Resonded with %u \n", substream.bytes_written);
                    }

                    if (err != ERR_OK)
                    {
                        PRINT("ETHERNET: cmd tcp connection failed to echo");
                    }
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }

            // clean up and wait for new connection
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

err_t CmdServer::msg_handler(quail_telemetry_Message &msg, bool &respond)
{
    switch (msg.which_message)
    {
    case quail_telemetry_Message_reboot_tag:
        printf("Rebooting");
        vTaskDelay(100); // flush
        NVIC_SystemReset();
        break;

    case quail_telemetry_Message_start_udp_tag:
        PRINT("start udp\n");
        sys.slateServer.connect({msg.message.start_udp.addr}, msg.message.start_udp.port);
        break;

    case quail_telemetry_Message_request_metaslate_tag:
        PRINT("request meta\n");
        msg.which_message = quail_telemetry_Message_response_metaslate_tag;
        msg.message.response_metaslate.hash = msg.message.request_metaslate.hash;
        memcpy(msg.message.response_metaslate.metaslate.bytes, telemetry_t::metaslate_blob, sizeof(telemetry_t::metaslate_blob));
        msg.message.response_metaslate.metaslate.size = sizeof(telemetry_t::metaslate_blob);
        respond = true;
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
