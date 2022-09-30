import zlib
import cmd_pb2
import sys
import socket
import zlib, msgpack

from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32

msg = cmd_pb2.Message()
msg.request_metaslate.SetInParent()
msg.request_metaslate.hash = 0
size = msg.ByteSize()

TCP_IP = '192.168.2.2'
TCP_PORT = 1002

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(_VarintBytes(size) + msg.SerializeToString())
data = s.recv(1024)

msg_len, n = _DecodeVarint32(data,0)
data = data[n:n+msg_len]
read_msg = cmd_pb2.Message()
read_msg.ParseFromString(data)

data = zlib.decompress(read_msg.response_metaslate.metaslate)
data = msgpack.unpackb(data)
print(data)
s.close()