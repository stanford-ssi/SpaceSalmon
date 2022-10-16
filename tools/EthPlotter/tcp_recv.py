import socket
import time
import json

def data_to_json(data):
    last_bracket = data.rfind(b'}')
    try:
        string = data[:last_bracket+1].decode('utf-8')
    except UnicodeDecodeError:
        string = ""
        print(data)
    return json.loads(string)

TCP_IP = "192.168.2.1"
TCP_PORT = 1002

command_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
command = {
    "cmd":{
        "quail":{
            "valves":{
                "S12":{
                    "ptm":1000,
                    "pls":1
                },
                "S11":{
                    "ptm":900,
                    "pls":1
                },
                "S10":{
                    "ptm":800,
                    "pls":1
                },
                "S9":{
                    "ptm":700,
                    "pls":1
                },
                "S8":{
                    "ptm":600,
                    "pls":1
                },
                "S7":{
                    "ptm":500,
                    "pls":1
                },
                "S6":{
                    "ptm":400,
                    "pls":1
                },
                "S5":{
                    "ptm":300,
                    "pls":1
                }
            }
        }
    }
}

meta = {
    "meta" : "gimme"
}

counter = 0

time.sleep(3)
print("attempting to connect to TCP command handler")
command_socket.connect((TCP_IP, TCP_PORT))
print("connected to quail!")

while True:
    try:
        if counter % 2 == 0:
            command_packet = json.dumps(command)
            command_socket.send(str.encode(command_packet))
            
            data, address = command_socket.recvfrom(4096)
            print("Command Echo", data)
            print()
            # # Will not work for large data packets
            # response_packet = data_to_json(data)
            # print("Command Echo:", response_packet)
            # assert commands have been recieved
            # assert(response_packet == command)
            time.sleep(2)
        else:
            meta_rqst = json.dumps(meta)
            command_socket.send(str.encode(meta_rqst))

            # waits for buffer to fully fill up - so likelyhood is higher for whole metaslate
            time.sleep(1)
            
            data, address = command_socket.recvfrom(4096*2)
            print("Metadata", data)
            print()
            # # Below will not work for large data packets
            #response_packet = data_to_json(data)
            #print("MetaDump:", response_packet)
            time.sleep(1)
 
        counter += 1
    except ConnectionResetError:
        time.sleep(5)
        command_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print("attempting to connect to TCP command handler")
        command_socket.connect((TCP_IP, TCP_PORT))
        print("connected to quail!")

# close the socket
command_socket.close()