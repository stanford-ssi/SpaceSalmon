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
    sys.telem_server.bind(MY_SLATE_PORT);

    cmdConn = netconn_new(NETCONN_UDP);
    err_t err = netconn_bind(cmdConn, IP4_ADDR_ANY, MY_CMD_PORT);
    if (err != ERR_OK)
    {
        printf("Could not bind to addr/port!\n");
    }

    while (true)
    {
        netbuf *buf;
        pb_byte_t *data;
        uint16_t len;
        quail_telemetry_Message msg;

        if (netconn_recv(cmdConn, &buf) == ERR_OK)
        {
            ip_addr_t *addr = netbuf_fromaddr(buf);
            uint16_t port = netbuf_fromport(buf);

            if (netbuf_data(buf, (void **)&data, &len) == ERR_OK)
            {
                pb_istream_t rxd_msg = pb_istream_from_buffer(data, len);
                if (pb_decode(&rxd_msg, quail_telemetry_Message_fields, &msg))
                {
                    msg_handler(msg, addr, port);
                }
                else
                {
                    printf("failed to decode packet!\n");
                }
            }
            else
            {
                printf("netbuf data error!\n");
            }

            netbuf_delete(buf);
        }
        else
        {
            printf("Failed to recv packet!\n");
        }
    }
}

err_t CmdServer::msg_handler(quail_telemetry_Message &msg, ip_addr_t *addr, uint16_t port)
{
    bool reboot = false;
    bool respond = false;

    switch (msg.which_message)
    {
    case quail_telemetry_Message_reboot_tag:
        msg.which_message = quail_telemetry_Message_ack_tag;
        respond = true;
        reboot = true;
        break;

    case quail_telemetry_Message_start_udp_tag:
        if (msg.message.start_udp.addr == 0)
        { // if target address is 0, we target the source of the packet
            msg.message.start_udp.addr = addr->addr;
        }
        sys.slate_registry.set_server_target(msg.message.start_udp);
        msg.which_message = quail_telemetry_Message_ack_tag;
        respond = true;
        break;

    case quail_telemetry_Message_request_metaslate_tag:
        msg.which_message = quail_telemetry_Message_response_metaslate_tag;
        msg.message.response_metaslate.hash = msg.message.request_metaslate.hash;
        sys.slate_registry.find_metaslate(msg.message.response_metaslate);
        respond = true;
        break;

    case quail_telemetry_Message_set_field_tag:
        sys.slate_registry.parse_set_field(msg.message.set_field);
        msg.which_message = quail_telemetry_Message_ack_tag;
        respond = true;
        break;

    case quail_telemetry_Message_query_info_tag:
        msg.which_message = quail_telemetry_Message_respond_info_tag;
        sys.slate_registry.fetch_slate_info(msg.message.respond_info);
        strncpy(msg.message.respond_info.name, "quail", sizeof(msg.message.respond_info.name));
        strncpy(msg.message.respond_info.version, build_version, sizeof(msg.message.respond_info.version));
        respond = true;
        break;

    default:
        printf("got packet we were not supposed to get!\n");
        break;
    }

    if (respond)
    {
        pb_ostream_t substream = PB_OSTREAM_SIZING;
        pb_encode(&substream, quail_telemetry_Message_fields, &msg); // this is very waseful. We're encoding like 4 times here.

        netbuf *buf = netbuf_new();
        if (buf != NULL)
        {
            if (netbuf_alloc(buf, substream.bytes_written) != NULL)
            {
                pb_byte_t *dataptr;
                u16_t size;
                if (netbuf_data(buf, (void **)&dataptr, &size) == ERR_OK)
                {
                    if (size != substream.bytes_written)
                    {
                        printf("netbuf data size does not match!\n");
                    }
                    else
                    {
                        substream = pb_ostream_from_buffer(dataptr, substream.bytes_written);
                        if (pb_encode(&substream, quail_telemetry_Message_fields, &msg))
                        {
                            if (netconn_sendto(cmdConn, buf, addr, port) != ERR_OK)
                            {
                                printf("failed to send response!\n");
                            }
                        }
                        else
                        {
                            printf("failed to encode pb: %s\n", substream.errmsg);
                        }
                    }
                }
                else
                {
                    printf("netbuf data error!\n");
                }
            }
            netbuf_delete(buf);
        }
        else
        {
            printf("Failed to create new netbuf\n");
        }
    }

    if (reboot)
    {
        vTaskDelay(100); // flush?
        NVIC_SystemReset();
    }

    return RET::OK;
}
