#pragma once
#include "../../slate/slate_utils.h"

#include <array>
#include <functional>
#include "cmd.pb.h"

template <typename... Ts>
class SlateRegistry
{
private:
    const std::array<std::reference_wrapper<Slate>, sizeof...(Ts)> slateList;

public:
    SlateRegistry(Ts &...xs) : slateList({(std::ref<Slate>(xs), ...)})
    {
    }

    void parse_set_field(quail_telemetry_set_field &cmd)
    {
        for (auto wrapper : slateList)
        {
            Slate &slate = wrapper.get();
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

                break;
            }
        }
    }
};