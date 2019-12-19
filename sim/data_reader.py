import json
import sys
import matplotlib.pyplot as plt
import numpy as np
import pickle
file = open(sys.argv[1],"r")

logs = []
tick_sensor = []
# tick_stat = []
bmp = {"p":[],"t":[] }
bmi = { "ax":[],"ay":[],"az":[], "gx":[],"gy":[],"gz":[]   }
adxl = {"x":[],"y":[],"z":[]}
id = []
stack = []
bat = {"cellA":[],"cellB":[]}
for line in file:
    try:
        data = json.loads(line)
        if (data[u"id"] == u"sensor") and (data[u"tick"] > 9350000) and (data[u"tick"] < 9550000):
            # try:
            bmp["p"].append(data[u"bmp"][u"p"])
            bmp["t"].append(data[u"bmp"][u"t"])
            bmi["ax"].append(data[u"bmi"][u"a"][0])
            bmi["ay"].append(data[u"bmi"][u"a"][1])
            bmi["az"].append(data[u"bmi"][u"a"][2])
            bmi["gx"].append(data[u"bmi"][u"g"][0])
            bmi["gy"].append(data[u"bmi"][u"g"][1])
            bmi["gz"].append(data[u"bmi"][u"g"][2])
            adxl["x"].append(data[u"adxl"]["a"][0])
            adxl["y"].append(data[u"adxl"]["a"][1])
            adxl["z"].append(data[u"adxl"]["a"][2])
            id.append(data[u"id"])
            stack.append(data[u"stack"])
            tick_sensor.append(data[u"tick"] - 9350000)
            # except KeyError:
                #print 'keyError'
                #print line
                # logs.append('keyError')
        # if data[u"id"] == u"status":
            # bat["cellA"].append(data[u'bat'][u"cellA"])
            # bat["cellB"].append(data[u'bat'][u"cellB"])
            # tick_stat.append(data[u"tick"])
            #print "status hi"
    except ValueError:
        logs.append('not JSON')
file.close()

sea_level = 101325.
pressure = np.array(bmp["p"])

time = np.array(tick_sensor) / 1000.
alt = 44330 * (1 - (pressure / sea_level) ** (1/5.255))
adxl_a = np.array(adxl["z"])
bmi_a = np.array(bmi["az"])
with open("launch_data.pkl", "wb") as f:
    pickle.dump([time, alt, adxl_a, bmi_a], f)
