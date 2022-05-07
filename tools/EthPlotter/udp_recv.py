
import socket
import json
 

localIP     = "192.168.1.1"
localPort   = 2000

bufferSize  = 10_000

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPServerSocket.bind((localIP, localPort))
 
print("UDP server up and listening")

accumulator = []
while(True):

    message, address = UDPServerSocket.recvfrom(bufferSize)

    if message[0] != ord('\n'):
        accumulator.append(message)
        continue
    
    try:
        json_object = json.loads(b"".join(accumulator))
    except ValueError:
        pass # invalid json
    else:
        print("Message from Client: ", json_object)
        print()
    finally:
        accumulator = []
    