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
    SlateRegistry(SlateServer<Ts>&... xs) : slateServerList(xs...)
    {
    }

    void parse_set_field(quail_telemetry_set_field &cmd)
    {
        std::apply
        (
            [&](SlateServer<Ts> &... server)
            {
                ([&]
                {
                    auto &slate = server.slate;
                    if (cmd.hash == slate.get_metaslate_hash())
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
                    }
                } (), ...);
            }, slateServerList
        );
    }
};