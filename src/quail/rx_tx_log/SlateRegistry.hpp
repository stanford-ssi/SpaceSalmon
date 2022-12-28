#pragma once
#include "../../slate/slate_utils.h"

#include <array>
#include <functional>
#include "cmd.pb.h"
#include "SlateServer.hpp"

template <typename... Ts>
class SlateRegistry
{
private:
    std::tuple<SlateServer<Ts> &...> slateServerList;

public:
    SlateRegistry(SlateServer<Ts> &...xs) : slateServerList(xs...)
    {
    }

    void parse_set_field(quail_telemetry_set_field &cmd)
    {
        std::apply(
            [&](SlateServer<Ts> &...server)
            {
                ([&]
                 {
                    auto &slate = server.slate;
                    if (cmd.hash == slate.metaslate_hash)
                    {
                        switch (cmd.which_adata)
                        {
                        case quail_telemetry_set_field_data_int16_tag:
                            slate.set_int16_t_field(cmd.offset, cmd.adata.data_int16);
                            break;

                        case quail_telemetry_set_field_data_uint32_tag:
                            slate.set_uint32_t_field(cmd.offset, cmd.adata.data_uint32);
                            break;

                        case quail_telemetry_set_field_data_bool_tag:
                            slate.set_bool_field(cmd.offset, cmd.adata.data_bool);
                            break;

                        case quail_telemetry_set_field_data_float_tag:
                            slate.set_float_field(cmd.offset, cmd.adata.data_float);
                            break;
                        }
                    } }(),
                 ...);
            },
            slateServerList);
    }

    void set_server_target(quail_telemetry_start_udp &cmd)
    {
        std::apply(
            [&](SlateServer<Ts> &...server)
            {
                ([&]
                 {
                    if (cmd.hash == server.slate.metaslate_hash)
                    {
                        server.connect({cmd.addr}, cmd.port);
                    } }(),
                 ...);
            },
            slateServerList);
    }

    void find_metaslate(quail_telemetry_response_metaslate &msg)
    {
        msg.metaslate.size = 0; // if none found, size is 0
        std::apply(
            [&](SlateServer<Ts> &...server)
            {
                ([&]
                 {
                    if (msg.hash == server.slate.metaslate_hash)
                    {
                        memcpy(msg.metaslate.bytes, server.slate.metaslate_blob, sizeof(server.slate.metaslate_blob));
                        msg.metaslate.size = sizeof(server.slate.metaslate_blob);
                    } }(),
                 ...);
            },
            slateServerList);
    }

    void fetch_slate_info(quail_telemetry_respond_info &msg)
    {
        uint8_t i = 0;

        std::apply(
            [&](SlateServer<Ts> &...server)
            {
                ([&]
                 {
                     if (i >= (sizeof(msg.slates)/sizeof(msg.slates[0])) )
                     {
                         return;
                     }

                     msg.slates[i].hash = server.slate.metaslate_hash;
                     msg.slates[i].size = server.slate.size;
                     strncpy(msg.slates[i].name, server.slate.name, sizeof(msg.slates[i].name));

                     i++; }(),
                 ...);
            },
            slateServerList);

        msg.slates_count = i;
    }
};