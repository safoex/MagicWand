import serial
import time
import TrackRecognition as tr

ser = serial.Serial('COM4',115200)
with open("gyro.dat", "w") as f:
    for i in range(100):
        if i == 5:
            print("start")
        f.write(str(time.time()) + ' ' + ser.readline().decode('utf-8', 'ignore'))

tr.recognize_track("gyro.dat")
ser.close()
