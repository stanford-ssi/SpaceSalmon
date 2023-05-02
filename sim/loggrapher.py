import json
import numpy as np
from matplotlib import pyplot as plt 
import math

class AltFilter():
	def __init__(self, use_pres=True):
		self.X = np.zeros([3])
		self.Z = np.zeros([2])
		self.F = np.array([[1, 0.01, 0], [0, 1, 0.01], [0, 0, 1]])
		self.H = np.array([[1, 0, 0], [0, 0, 1]])
		self.Q = np.array([[0.027, 0.0014, 0.], [0.0014, 0.05, 0.006], [0., 0.006, 1.]]) 
		self.P = np.zeros([3,3])
		self.R = np.array([[100, 0], [0, 1]])
		self.use_pres = use_pres

	def init_filter(self, pres, accel):
	  self.prefilter(pres, accel)
	  self.X[0] = self.Z[0]

	def p2alt(self, p):
		return (1.0-(math.pow((p/101350.0),0.190284)))*145366.45*0.3048

	def prefilter(self, pres, accel):
		if pres < 0:
			pres = 0.0

		self.Z[0] = self.p2alt(pres)
		self.Z[1] = accel*(-1) - 9.807


	def kalmanUpdate(self):
		self.K = np.matmul(np.matmul(self.P, self.H.transpose()), np.linalg.inv(np.matmul(self.H, np.matmul(self.P, self.H.transpose())) + self.R))
		self.X = self.X + np.matmul(self.K, (self.Z - np.matmul(self.H, self.X)))
		self.P = np.matmul((np.identity(3) - np.matmul(self.K, self.H)), self.P)

	def kalmanPredict(self):
		self.X = np.matmul(self.F, self.X)
		self.P = np.matmul(self.F, np.matmul(self.P, self.F.transpose())) + self.Q

	def update(self, pres, accel):
		self.kalmanPredict()
		self.prefilter(pres, accel)

		if self.X[1] > 343*0.7:
			self.R[0, 0] = 100000
		else:
			self.R[0, 0] = 10 + math.pow(abs(self.X[1]/20),2) + abs(self.X[2])*0.5
			if self.R[0, 0] > 500:
				self.R[0, 0] = 500

		if self.use_pres == False:
			self.R[0, 0] = 100000
			# self.R[1, 1] = 0

		self.kalmanUpdate()

		# alt, vel
		return self.X[0], self.X[1]



# writing to file
file1 = open('log3.txt', 'r')

Lines = file1.readlines()

count = 0
# Strips the newline character
print(len(Lines))
tick_arr = np.zeros(len(Lines))
pres1_arr = np.zeros(len(Lines), dtype=np.float32)
adxl2_x = np.zeros(len(Lines), dtype=np.float32)
adxl2_y = np.zeros(len(Lines), dtype=np.float32)
barom_height = np.zeros(len(Lines), dtype=np.float32)

kalman_pred_height = np.zeros(len(Lines), dtype=np.float32)
kalman_pred_vel = np.zeros(len(Lines), dtype=np.float32)

int_pred = np.zeros(len(Lines), dtype=np.float32)
accel_arr = np.zeros(len(Lines), dtype=np.float32)
pred_vel = 0

int_pred2 = np.zeros(len(Lines), dtype=np.float32)
pred_vel2 = 0

log_vel = np.zeros(len(Lines), dtype=np.float32)
log_count = 0

pad_alt = 0

filtercount = 0
predFilter = AltFilter(use_pres=True)
# predFilterNoPres = AltFilter(use_pres=False)
# predFilterNoPresADX = AltFilter(use_pres=False)

# Lines = Lines:]
init_pres = None
for line in Lines:
	if count % 10000 == 0:
		print("ok: " + str(count))

	parsedata = False
	try:
		jobj = json.loads(line.strip())
		parsedata = True
	except:
		print("except")

	if parsedata and ('tick' in jobj.keys()) and ('pres2' in jobj.keys()):
		pres = float(jobj['pres2']['p'])
		pres1_arr[count] = pres
		accel  = float(jobj['adxl2']['a'][0])
		accel2  = float(jobj['adxl2']['a'][1])
		adxl2_x[count] = accel
		adxl2_y[count] = accel2
		if count == 10:
			predFilter.init_filter(pres, accel)
			# predFilterNoPres.init_filter(pres, accel_acc)
			# predFilterNoPresADX.init_filter(pres, accel)
			init_pres = pres
		elif count > 10:
			alt, vel = predFilter.update(pres, accel)
			kalman_pred_vel[filtercount] = vel
			kalman_pred_height[filtercount] = alt
			filtercount += 1

		tick_arr[count] = int(jobj['tick'])		
		barom_height[count] = float(predFilter.p2alt(pres1_arr[count]))

		count += 1
	elif parsedata:
		if 'x' in jobj:
	 		log_vel[log_count] = float(jobj['x'][1])
	 		log_count += 1


# bmp1_arr = bmp1_arr[:count]
barom_height = barom_height[:count]
# bmi2_y = bmi2_y[:count]
adxl2_x = adxl2_x[:count]
adxl2_y = adxl2_y[:count]
# bmi1_y = bmi1_y[:count]
pres1_arr = pres1_arr[:count]
# log_vel = log_vel[:log_count]

kalman_pred_height = kalman_pred_height[:filtercount]
kalman_pred_vel = kalman_pred_vel[:filtercount]

timelen = kalman_pred_height[1:].shape[0]
plt.plot(np.arange(0, timelen), kalman_pred_vel[1:], label="kalman vel") 
plt.plot(np.arange(0, timelen), kalman_pred_height[1:], label="kalman height") 
plt.plot(np.arange(0, barom_height.shape[0]), barom_height, label="barom_height") 
plt.plot(np.arange(0, adxl2_x.shape[0]), adxl2_x, label="adxl2_x")

plt.legend(loc="upper right")

plt.show()
