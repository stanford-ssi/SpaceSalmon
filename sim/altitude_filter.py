import numpy as np
from numpy.linalg import inv
import matplotlib.pyplot as plt
import pickle
import sys


MACH_LOCKOUT_TRIGGER = 343*0.7  # mach 0.7
MACH_LOCKOUT_CLIP_VAL = 100000  # value to clip


class altitude_filter:

    def __init__(self):
        self.X  = np.array([0.,  # state in altitude, velocity, acceleration (SI units)
                            0,
                            0])

        self.Z = np.array([0.,  # measurements
                           0])

        self.F = np.array([1., 0.02, 0,  # state transition model
                           0,  1,    0.02,
                           0,  0,    1]).reshape((3,3))

        self.H = np.array([1., 0, 0,  # measurement model
                           0,  0, 1]).reshape((2,3))

        self.Q = np.array([0.027,  0.0014,  0.,  # process noise (uncertainty in system dynamics)
                           0.0014, 0.05,    0.006,
                           0.,     0.006,   1.]).reshape((3,3))

        self.P = np.array([0., 0, 0,  # covariance matrix (uncertainty in state estimate)
                           0, 0, 0,
                           0, 0, 0]).reshape((3,3))

        self.R = np.array([100., 0,  # measurement noice (uncertainty in measurements)
                           0,    1]).reshape((2,2))

        self.Y = np.array([0,  # residual (how well real measurements match predicted measurements)
                           0])
        
        self.time = 0

    def update(self, time, alt, accel):
        self.update_delta_t(time)
        self.kalmanPredict()
        self.prefilter(alt, accel)
        if (self.X[1] > MACH_LOCKOUT_TRIGGER):
            self.R[0,0] = MACH_LOCKOUT_CLIP_VAL
        else:
            self.R[0,0] = 10 + abs(self.X[1]/20)**2 + abs(self.X[2])*0.5
            if (self.R[0,0] > 500):
                self.R[0,0] = 500
        self.kalmanUpdate()

    def update_delta_t(self, time):
        delta_t = time - self.time
        self.F = np.array([1., delta_t, 0,  # state transition model
                           0,  1,    delta_t,
                           0,  0,    1]).reshape((3,3))
        self.time = time

    def getAltitude(self):
        # return X[0]/0.3048  # altitude in feet
        return self.X[0]

    def getVelocity(self):
        return self.X[1]

    def get_state(self):
        return self.X

    def get_residual(self):
        return self.Y

    def kalmanPredict(self):
        self.X = self.F @ self.X
        self.P = self.F @ self.P @ self.F.T + self.Q

    def kalmanUpdate(self):
        self.Y = self.Z - self.H @ self.X
        self.K = self.P @ self.H.T @ inv(self.H @ self.P @ self.H.T + self.R)
        self.X = self.X + self.K @ self.Y
        # self.P = (np.eye(3) - self.K @ self.H) @ self.P
        self.P = ((np.eye(3) - self.K @ self.H) @ self.P @ (np.eye(3) - self.K @ self.H).T
                  + self.K @ self.R @ self.K.T)

        # skip post-fit residuals

    def prefilter(self, alt, accel):
        self.Z[0] = alt
        self.Z[1] = accel


def main():
    with open(sys.argv[1], 'rb') as f:
        data = pickle.load(f)
    time = data[0]
    alt = data[1]
    adxl_a = data[2] * -1 -10
    bmi_a = data[3] * -1 -10
    n = len(time)

    kf = altitude_filter()
    states = np.zeros((n, 3))
    residuals = np.zeros((n, 2))
    for i in range(n):
        kf.update(time[i], alt[i], adxl_a[i])
        states[i,:] = kf.get_state()
        residuals[i,:] = kf.get_residual()

    kf2 = altitude_filter()
    states2 = np.zeros((n, 3))
    residuals2 = np.zeros((n, 2))
    for i in range(n):
        kf2.update(time[i], alt[i], bmi_a[i])
        states2[i,:] = kf2.get_state()
        residuals2[i,:] = kf2.get_residual()
    
    # plot performance
    fig = plt.figure()
    ax1 = plt.subplot(211)
    plt.plot(time, alt)
    plt.plot(time, states[:,0])
    plt.plot(time, states2[:,0])
    plt.title("Unfiltered vs Kalman filtered altitude and acceleration")
    plt.ylabel("Altitude (m)")
    plt.xlabel("Time (s)")
    plt.legend(["Unfiltered", "Filtered with ADXL", "Filtered with BMI"])

    # ax2 = plt.twinx()
    ax2 = plt.subplot(212, sharex=ax1)
    ax2.plot(time, adxl_a)
    ax2.plot(time, states[:,2])
    ax2.plot(time, bmi_a)
    ax2.plot(time, states2[:,2])
    ax2.set_ylabel("Acceleration (m/s^2)")
    plt.legend(["Unfiltered ADXL", "ADXL", "Unfiltered BMI", "BMI"])

    plt.figure()
    plt.plot(time, states[:,1])
    plt.plot(time, states2[:,1])
    plt.title('Filtered velocity')
    plt.legend(['ADXL', 'BMI'])

    plt.show()

    # plt.figure()
    # plt.plot(time, states[:,1])


if __name__ == '__main__':
    main()
