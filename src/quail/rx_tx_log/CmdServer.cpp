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

    cmdConn = netconn_new(NETCONN_UDP);
    err_t err = netconn_bind(cmdConn, IP4_ADDR_ANY, MY_CMD_PORT);

    while (true)
    {
        netbuf *buf;
        pb_byte_t *data;
        uint16_t len;
        quail_telemetry_Message msg;

        netconn_recv(cmdConn, &buf);

        ip_addr_t *addr = netbuf_fromaddr(buf);
        uint16_t port = netbuf_fromport(buf);

        netbuf_data(buf, (void **)&data, &len);
        pb_istream_t rxd_msg = pb_istream_from_buffer(data, len);
        pb_decode(&rxd_msg, quail_telemetry_Message_fields, &msg);

        msg_handler(msg, addr, port);
    }
}

err_t CmdServer::msg_handler(quail_telemetry_Message &msg, ip_addr_t *addr, uint16_t port)
{
    bool reboot = false;
    bool respond = false;

    switch (msg.which_message)
    {
    case quail_telemetry_Message_reboot_tag:
        printf("Rebooting");
        msg.which_message = quail_telemetry_Message_ack_tag;
        respond = true;
        reboot = true;
        break;

    case quail_telemetry_Message_start_udp_tag:
        printf("start udp\n");
        sys.slateServer.connect({msg.message.start_udp.addr}, msg.message.start_udp.port);
        msg.which_message = quail_telemetry_Message_ack_tag;
        respond = true;
        break;

    case quail_telemetry_Message_request_metaslate_tag:
        printf("request meta\n");
        msg.which_message = quail_telemetry_Message_response_metaslate_tag;
        msg.message.response_metaslate.hash = msg.message.request_metaslate.hash;
        memcpy(msg.message.response_metaslate.metaslate.bytes, telemetry_t::metaslate_blob, sizeof(telemetry_t::metaslate_blob));
        msg.message.response_metaslate.metaslate.size = sizeof(telemetry_t::metaslate_blob);
        respond = true;
        break;

    case quail_telemetry_Message_set_field_tag:
        printf("set field\n");
        // TODO: acutally set the field
        msg.which_message = quail_telemetry_Message_ack_tag;
        respond = true;
        break;

    default:
        printf("oops\n");
        break;
    }

    if (respond)
    {

        pb_ostream_t substream = PB_OSTREAM_SIZING;
        pb_encode(&substream, quail_telemetry_Message_fields, &msg); // this is very waseful. We're encoding like 4 times here.

        netbuf *buf = netbuf_new();
        netbuf_alloc(buf, substream.bytes_written); // enough?

        pb_byte_t *dataptr;
        u16_t size;
        netbuf_data(buf, (void **)&dataptr, &size);
        if (size != substream.bytes_written)
        {
            printf("AHHH!\n");
        }

        substream = pb_ostream_from_buffer(dataptr, substream.bytes_written);
        pb_encode(&substream, quail_telemetry_Message_fields, &msg);
        netconn_sendto(cmdConn, buf, addr, port);
        printf("Resonded with %u bytes.\n", substream.bytes_written);
    }

    if (reboot)
    {
        vTaskDelay(100); // flush?
        NVIC_SystemReset();
    }

    return RET::OK;
}
