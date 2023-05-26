import sys
from serial import *
from tkinter import *
import json
import base64


import tkinter as tk


import logging
logging.basicConfig(filename="gui.log",
                            filemode='a',
                            format='%(asctime)s | %(name)s | %(levelname)s | %(message)s',
                            # datefmt='%H:%M:%S',
                            level=logging.DEBUG)

serialPort = sys.argv[1]
baudRate = 9600
ser = Serial(serialPort, baudRate, timeout=0,
             writeTimeout=0)  # ensure non-blocking

# make a TkInter Window
root = Tk()
root.wm_title("S6M Chat")

Grid.columnconfigure(root, 1, weight=1)

Grid.rowconfigure(root, 1, weight=1)
Grid.rowconfigure(root, 2, weight=1)

console = Text(root, height=12, width=60, takefocus=0)
console.grid(pady=5, row=1, column=1, sticky=N+S+W+E)

log = Text(root, height=12, width=60, takefocus=0, wrap=NONE)
log.grid(pady=5, row=2, column=1, sticky=N+S+W+E)

frame = Frame(root, borderwidth=1)
frame.grid(row=3, column=1)

e = Entry(frame, width=50)
e.pack(side=RIGHT)

Label(frame, text=": ").pack(side=RIGHT)

prefix = Entry(frame, width=10)
prefix.pack(side=RIGHT)

# frame for widgets
dataFrame = Frame(root, borderwidth=5)
dataFrame.grid(column=2, row=1, rowspan=2)

Label(dataFrame, text="Data").grid(column=0, columnspan=2)

vars = [["freq", "Frequency", "MHz"], ["bw", "Bandwidth", "kHz"],
        ["sf", "Spreading", ""], ["pwr", "Power", "dBm"],
        ["cr", "Coding Rate", ""], ["rx_success", "RX Good", ""],
        ["rx_failure", "RX Fail", ""], ["tx_success", "TX Good", ""],
        ["tx_failure", "TX Fail", ""], ["rssi", "Last RSSI:", "dBm"],
        ["snr", "Last SNR:", "dB"],
        ["filter_vel", "Last Velocity (m/s):", ""], # WARNING: NOT CHECKED
        ]

# Build Data Window Dynamicaly
data_fields = {}
data_row = 1
for entry in vars:
    data_fields[entry[0]] = (StringVar(), entry[2])
    data_fields[entry[0]][0].set("???"+entry[2])
    Label(dataFrame, text=entry[1]).grid(column=0, row=data_row)
    Label(dataFrame, textvariable=data_fields[entry[0]][0]).grid(
        column=1, row=data_row)
    data_row += 1

# label = tk.Label(dataFrame, text="Initial Velocity Text")
# label.pack()


# radiobuttons
msgtype = StringVar()
msgtype.set("base64")
Radiobutton(dataFrame, text="Base64", variable=msgtype,
            value="base64").grid(column=0, row=data_row)
data_row += 1

telem_count = 1
telem_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)),"telem")

for filename in os.listdir(telem_dir):
    f = os.path.join(telem_dir,filename)
    # checking if it is a file
    if os.path.isfile(f) and filename.endswith(".py"):
        Radiobutton(dataFrame, text="Decoder: "+filename,
                variable=msgtype, value=filename).grid(column=0, row=data_row)
        data_row += 1

# latitude and longitude correction feilds
pos = (IntVar(None,35), IntVar(None,-117))

Label(dataFrame, text="Latitude").grid(column=0, row=data_row)
lat_entry = Entry(dataFrame, textvariable=pos[0])
lat_entry.grid(column=1, row=data_row)
data_row += 1

Label(dataFrame, text="Longitude").grid(column=0, row=data_row)
lon_entry = Entry(dataFrame, textvariable=pos[1])
lon_entry.grid(column=1, row=data_row)
data_row += 1

# make our own buffer
# useful for parsing commands
# Serial.readline seems unreliable at times too
serBuffer = ""
print(data_fields.keys())
# Check velocity not over capacity. WARNING: NOT CHECKED

velocityWarningText = StringVar()
velocityWarningText.set("Velocity Data Not Received")
tk.Label(dataFrame, textvariable=velocityWarningText, fg='black').grid(column=0, row=data_row)

# Update data
def process(line):
    logging.info("Got Msg: " + line.strip())
    data = json.loads(line)
    if data["id"] == "Radio":
        # if 'data' in data:
        #     print(data['data'])
        for key in data:
            if key in data_fields:
                data_fields[key][0].set(str(data[key]) + data_fields[key][1])

            # if key=="filter_vel":
            #     print(data[key])


        if (data["msg"] == "RX" or data["msg"] == "TX"):
            if msgtype.get() == "base64":
                try:
                    msg = base64.b64decode(data["data"]).decode("utf-8")
                except UnicodeDecodeError as e:
                    msg = "<invalid base64>"

                logging.info("Decoded Text: " + msg)
                msg = data["msg"] + ">> " + msg + "\n"
                console.insert(END, msg)

            else:
                telem_py_path = os.path.join(telem_dir,msgtype.get())
                
                # dynamically import telemetry parser
                spec = importlib.util.spec_from_file_location("module.name", telem_py_path)
                telem = importlib.util.module_from_spec(spec)
                sys.modules["module.name"] = telem
                spec.loader.exec_module(telem)

                # parse telemetry packet
                msg, msgJSON = telem.decodeTelem(data["data"], pos)
                print("------")
                print(msg)
                print(msgJSON['filter_vel'])
                console.insert(END, msg + "\n")
                logging.info("Decoded Telem: " + msg)
                if ((data_fields["filter_vel"][1] != "") and (int(data_fields["filter_vel"][1]) <= -100)):
                    velocityWarningText.set("descent velocity is too fast")
                    tk.Label(dataFrame, textvariable=velocityWarningText, fg='red').grid(column=0, row=data_row)
                    # new_text = "Descent Velocity is too fast"
                    # label.config(text = new_text)
                else:
                    velocityWarningText.set("Velocity nominal")
                    tk.Label(dataFrame, textvariable=velocityWarningText, fg='green').grid(column=0, row=data_row)

                # else: label = tk.Label(dataFrame, text="Descent velocity is appropriate.").grid(column=0, row=data_row)


            console.see(END)


def readSerial():
    while True:
        c = ser.read()  # attempt to read a character from Serial

        # was anything read?
        if len(c) == 0:
            break

        # get the buffer from outside of this function
        global serBuffer

        # check if character is a delimeter
        if c == b'\r':
            c = b''  # don't want returns. chuck it

        if c == b'\n':
            serBuffer += "\n"  # add the newline to the buffer
            # add the line to the TOP of the log
            log.insert(END, serBuffer)
            log.see(END)
            process(serBuffer)
            serBuffer = ""  # empty the buffer
        else:
            try:
                serBuffer += c.decode("utf-8")  # add to the buffer
            except UnicodeDecodeError as a:
                print(c)

    root.after(10, readSerial)  # check serial again soon


def callback(event):
    msg = e.get()
    if(len(prefix.get()) > 0):
        msg = prefix.get() + ": " + msg
    
    data = base64.b64encode(msg.encode("utf-8")).decode("utf-8")
    data = json.dumps({"id": "tx", "data": data}, separators=(',', ':'))
    logging.info("Sent Msg: " + data)
    data += '\n'
    ser.write(data.encode('utf-8'))
    e.delete(0, END)


logging.info("GUI started, opening serial port " + serialPort)

# after initializing serial, an arduino may need a bit of time to reset
root.after(100, readSerial)

root.bind('<Return>', callback)

root.mainloop()
