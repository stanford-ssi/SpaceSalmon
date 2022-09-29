import cmd_pb2
import sys
import socket

from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32

msg = cmd_pb2.Message()
msg.start_udp.SetInParent()
size = msg.ByteSize()

TCP_IP = '192.168.2.2'
TCP_PORT = 1002

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(_VarintBytes(size))
s.send(msg.SerializeToString())
s.close()