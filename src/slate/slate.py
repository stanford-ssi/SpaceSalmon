import jinja2
import yaml
import msgpack
import hashlib
import zlib
import pathlib
import sys
import os

def generate_slate(yaml_path, header_path):

    codegen_dir = pathlib.Path(__file__).parent.resolve()

    yaml_path = pathlib.Path(yaml_path)
    header_path = pathlib.Path(header_path)

    slate_util_path = os.path.relpath(codegen_dir/"slate_utils.h",header_path.parent)

    with open(yaml_path, 'r') as stream:
        data_loaded = yaml.safe_load(stream)

    type_to_size = {"float":4, "bool":1, "int16_t":1, "uint32_t":4}

    offset = 8 # 8 bytes for flow ID
    for id,channel in data_loaded["channels"].items():
        channel["size"] = type_to_size[channel["type"]]
        channel["offset"] = offset
        offset += channel["size"]

    data_loaded["total_len"] = offset

    metaslate_pack = msgpack.packb(data_loaded)
    metaslate_pack = zlib.compress(metaslate_pack)

    metaslate_hash = int.from_bytes(hashlib.sha256(metaslate_pack).digest()[:8], 'little')

    blob_str = metaslate_pack.hex()
    blob_str = " ".join(["0x"+blob_str[i:i+2]+"," for i in range(0, len(blob_str), 2)])

    data_loaded["metaslate_blob"] = blob_str
    data_loaded["metaslate_hash"] = hex(metaslate_hash)

    environment = jinja2.Environment(loader=jinja2.FileSystemLoader(codegen_dir))
    environment.trim_blocks = True
    environment.lstrip_blocks = True
    template = environment.get_template("slate.j2")
    content = template.render(data_loaded,path=slate_util_path)

    header_path.parent.mkdir(exist_ok=True)
    with open(header_path, mode="w", encoding="utf-8") as message:
            message.write(content)
            print(f"Wrote {header_path}")

if __name__ == "__main__":
    try:
        yaml_path = pathlib.Path(sys.argv[1])
        header_path = pathlib.Path(sys.argv[2])
    except IndexError:
        print("Please specify a YAML input and header output!")

    if yaml_path.exists():
        generate_slate(yaml_path,header_path)
    else:
        raise FileNotFoundError(f"YAML not found: {yaml_path}")
    