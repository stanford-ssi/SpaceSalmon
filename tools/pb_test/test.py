import zlib
import cmd_pb2
import sys
import socket
import zlib, msgpack

from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32



IP = '192.168.2.2'
CMD_PORT = 1002

cmd_sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
cmd_sock.bind(("0.0.0.0", 5423))

seq = 1

print("Requesting telemetry stream")
# Start UDP stream:
msg = cmd_pb2.Message()
msg.sequence = seq
seq += 1
msg.start_udp.SetInParent()
msg.start_udp.hash = 0
msg.start_udp.addr = 0x0102A8C0
msg.start_udp.port = 8000
cmd_sock.sendto(msg.SerializeToString(),(IP, CMD_PORT))

print("Waiting for ack")
# decode metaslate
data = cmd_sock.recv(1024)
read_msg = cmd_pb2.Message()
read_msg.ParseFromString(data)
if (read_msg.sequence == seq):
    print("got ack!")


print("Requesting metaslate")
# Ask for metaslate
msg = cmd_pb2.Message()
msg.sequence = seq
seq += 1
msg.request_metaslate.SetInParent()
msg.request_metaslate.hash = 0
cmd_sock.sendto(msg.SerializeToString(),(IP, CMD_PORT))


print("Waiting for metaslate")
# decode metaslate
data = cmd_sock.recv(1024)
read_msg = cmd_pb2.Message()
read_msg.ParseFromString(data)
if (read_msg.sequence == seq):
    print("got metaslate!")
data = zlib.decompress(read_msg.response_metaslate.metaslate)
data = msgpack.unpackb(data)

print(data)
cmd_sock.close()

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