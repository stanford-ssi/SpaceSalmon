
import socket
 

localIP     = "192.168.1.1"
localPort   = 2000

bufferSize  = 10_000
 

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPServerSocket.bind((localIP, localPort))
 
print("UDP server up and listening")

while(True):

    message, address = UDPServerSocket.recvfrom(bufferSize)
    
    print(b"Message from Client: " + message)
    print()