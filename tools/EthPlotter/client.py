import socket
import sys
import json
import time
import multiprocessing as mp
from datetime import datetime
from matplotlib import pyplot
from matplotlib.animation import FuncAnimation

def data_to_json(data):
    last_bracket = data.rfind(b'}')
    try:
        string = data[:last_bracket+1].decode('utf-8')
    except UnicodeDecodeError:
        string = ""
        print(data)
    return json.loads(string)

def grapher(data_buf: mp.Queue):
    x_data, y_data, y2_data, y3_data = [], [], [], []

    figure = pyplot.figure()
    line, = pyplot.plot_date(x_data, y_data, '-')
    line2, = pyplot.plot_date(x_data, y2_data, '-')
    line3, = pyplot.plot_date(x_data, y3_data, '-')

    def update(frame):
        while not data_buf.empty():
            packet = data_buf.get(timeout=0.1)
            x_data.append(packet["tick"])
            y_data.append(packet["sensor"])
            y2_data.append(packet["sensor2"])
            y3_data.append(packet["sensor3"])
            if (len(y_data) > 10000):
                x_data.pop(0)
                y_data.pop(0)
                y2_data.pop(0)
                y3_data.pop(0)
        line.set_data(x_data, y_data)
        line2.set_data(x_data, y2_data)
        line3.set_data(x_data, y3_data)
        figure.gca().relim()
        figure.gca().autoscale_view()
        return line, line2, line3,

    animation = FuncAnimation(figure, update, interval=1)
    pyplot.show()

def data_ingestor(data_buf: mp.Queue, ip, port):

    # Create socket for server
    UDP_IP = "0.0.0.0"
    UDP_PORT = 2001
    data_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
    data_socket.bind((ip, port))
    counter = 0

    # recieve data
    while True:
        try:
            data, address = data_socket.recvfrom(4096)
            packet = data_to_json(data)
            data_buf.put(packet)
            counter += 1
        except ConnectionResetError:
            time.sleep(2)
            data_socket.bind((UDP_IP, UDP_PORT))

    # close the socket
    data_socket.close()

def command_sender(ip, port):
    TCP_IP = "192.168.2.2"
    TCP_PORT = 2002
    command_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    command = {
        "valve1": False,
        "valve2": False
    }
    counter = 0
    time.sleep(3)
    print("attempting to connect to TCP command handler")
    command_socket.connect((ip, port))
    print("connected to quail!")

    while True:
        try:
            if (counter % 1000 == 0):
                command["valve1"] = True
                command["valve2"] = False
            elif (counter % 10000 == 1337):
                command["valve1"] = False
                command["valve2"] = True
            command_packet = json.dumps(command)
            command_socket.send(str.encode(command_packet))
            
            data, address = command_socket.recvfrom(4096)
            response_packet = data_to_json(data)

            # assert commands have been recieved
            assert(response_packet == command)
            
            counter += 1
            time.sleep(0.1)
        except ConnectionResetError:
            time.sleep(5)
            command_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print("attempting to connect to TCP command handler")
            command_socket.connect((TCP_IP, TCP_PORT))
            print("connected to quail!")
    
    # close the socket
    command_socket.close()

if __name__ == '__main__':
    if len(sys.argv) == 5:
        # Get "IP address of Server" and also the "port number" from argument 1 and argument 2
        ip_udp = sys.argv[1]
        port_udp = int(sys.argv[2])
        ip_tcp = sys.argv[3]
        port_tcp = int(sys.argv[4])
    else:
        print("Run like : python3 client.py <arg1 server ip 192.168.1.102> <arg2 server port 4444 ><server port #2>")
        exit(1)

    mp.set_start_method('spawn')

    data_buf = mp.Queue()

    one = mp.Process(target=grapher, args=(data_buf, ))
    two = mp.Process(target=data_ingestor, args=(data_buf, ip_udp, port_udp, ))
    three = mp.Process(target=command_sender, args=(ip_tcp, port_tcp, ))
    one.start()
    two.start()
    three.start()
    one.join()
    two.join()
    three.join()
