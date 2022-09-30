import zlib
import cmd_pb2
import sys
import socket
import zlib, msgpack

from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32



TCP_IP = '192.168.2.2'
TCP_PORT = 1002

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

# Start UDP stream:
msg = cmd_pb2.Message()
msg.start_udp.SetInParent()
s.send(_VarintBytes(msg.ByteSize()) + msg.SerializeToString())

# Ask for metaslate
msg = cmd_pb2.Message()
msg.request_metaslate.SetInParent()
msg.request_metaslate.hash = 0
s.send(_VarintBytes(msg.ByteSize()) + msg.SerializeToString())


# decode metaslate
data = s.recv(1024)
msg_len, n = _DecodeVarint32(data,0)
data = data[n:n+msg_len]
read_msg = cmd_pb2.Message()
read_msg.ParseFromString(data)
data = zlib.decompress(read_msg.response_metaslate.metaslate)
data = msgpack.unpackb(data)

print(data)
s.close()

UDP_IP = "0.0.0.0"
UDP_PORT = 8000

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    slate_data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    #print("received message: %s" % slate_data)
    slate = {}
    for name,el in data["channels"].items():
        slate[name] = int.from_bytes(slate_data[el["offset"]:el["offset"]+el["size"]], "little")
    #print(slate)
    print(slate["tick"])