
import socket
import json
 

localIP     = "0.0.0.0"
localPort   = 8000

bufferSize  = 10_000

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPServerSocket.bind((localIP, localPort))
 
print("UDP server up and listening")

accumulator = []
while(True):

    message, address = UDPServerSocket.recvfrom(bufferSize)

    json_object = json.loads(message)
    print("Message from Client: ", json_object)
    print()
    