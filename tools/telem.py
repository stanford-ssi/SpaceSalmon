import base64
import bitstruct
import math

cf = bitstruct.CompiledFormatDict(">b1 b1 u15 u18 u18 b1 u11 u15 b1 u3 u8 u18 p2 >", [
                                  "pyroB","pyroA","gps_alt", "lon", "lat", "gps_lock", "filter_vel", "filter_alt", "armed", "state", "battery", "time"])


def decodeTelem(data, pos):
    try:
        byte_msg = base64.b64decode(data)
        byte_msg = byte_msg[::-1]
        data = cf.unpack(byte_msg)
    except bitstruct.Error as e:
        return "err"

    data["battery"] = expand_float(data["battery"], 1.0, 6.0, 8)
    data["lat"] = expand_float(
        data["lat"], 0.0, 10.0, 18) + round(pos[0].get(), -1)
    data["lon"] = expand_float(
        data["lon"], 0.0, 10.0, 18) + round(pos[1].get(), -1)

    data["gps_alt"] = expand_float(data["gps_alt"], -2000.0, 40000.0, 15)
    data["filter_alt"] = expand_float(data["filter_alt"], -2000.0, 40000.0, 15)
    data["filter_vel"] = expand_float(data["filter_vel"], -1000.0, 1000.0, 11)

    return str(data)


def expand_float(input, min, max, bits):
    in_max = (2**bits) - 1
    value = (input/in_max)*(max - min) + min
    quant = (max - min) / in_max
    digits = math.floor(math.log10(quant))
    return round(value, -digits)
