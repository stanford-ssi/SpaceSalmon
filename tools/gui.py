import sys
from serial import *
from tkinter import *
import json
import base64
import telem

import tkinter as tk

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

dataFrame = Frame(root, borderwidth=5)
dataFrame.grid(column=2, row=1, rowspan=2)

Label(dataFrame, text="Data").grid(column=0, columnspan=2)

vars = [["freq", "Frequency", "MHz"], ["bw", "Bandwidth", "kHz"],
        ["sf", "Spreading", ""], ["pwr", "Power", "dBm"],
        ["cr", "Coding Rate", ""], ["rx_success", "RX Good", ""],
        ["rx_failure", "RX Fail", ""], ["tx_success", "TX Good", ""],
        ["tx_failure", "TX Fail", ""], ["rssi", "Last RSSI:", "dBm"],
        ["snr", "Last SNR:", "dB"]]

# Build Data Window Dynamicaly
data_feilds = {}
data_row = 1
for entry in vars:
    data_feilds[entry[0]] = (StringVar(), entry[2])
    data_feilds[entry[0]][0].set("???"+entry[2])
    Label(dataFrame, text=entry[1]).grid(column=0, row=data_row)
    Label(dataFrame, textvariable=data_feilds[entry[0]][0]).grid(
        column=1, row=data_row)
    data_row += 1

# radiobuttons
msgtype = IntVar()
Radiobutton(dataFrame, text="Base64", variable=msgtype,
            value=0).grid(column=0, row=data_row)
data_row += 1
Radiobutton(dataFrame, text="ThunderGuppy Telem",
            variable=msgtype, value=1).grid(column=0, row=data_row)
data_row += 1

# latitude and longitude correction feilds
pos = (IntVar(), IntVar())

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


def process(line):
    data = json.loads(line)
    if data["id"] == "radio":
        for key in data:
            if key in data_feilds:
                data_feilds[key][0].set(str(data[key]) + data_feilds[key][1])

        if (data["msg"] == "RX" or data["msg"] == "TX"):
            if msgtype.get() == 0:
                try:
                    msg = base64.b64decode(data["data"]).decode("utf-8")
                except UnicodeDecodeError as e:
                    msg = "<invalid base64>"

                msg = data["msg"] + ">> " + msg + "\n"
                console.insert(END, msg)

            elif msgtype.get() == 1:
                msg = telem.decodeTelem(data["data"], pos) + "\n"
                console.insert(END, msg)

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
    data += '\n'
    ser.write(data.encode('utf-8'))
    e.delete(0, END)


# after initializing serial, an arduino may need a bit of time to reset
root.after(100, readSerial)

root.bind('<Return>', callback)

root.mainloop()
