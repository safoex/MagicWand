import serial
import time
import TrackRecognition as tr
import sys

ser = serial.Serial('/dev/ttyUSB0',115200)
#ser_light = serial.Serial("COM3", 9600)
file_name = 'gyro'
if len(sys.argv) >= 2:
    file_name = sys.argv[1]
print('ready')
j = 0
if len(sys.argv) >= 3:
    j = int(sys.argv[2])

while ser.isOpen():
    input_string = ser.readline().decode('utf-8', 'ignore')
    if input_string[:3] == "AX:":
        print("start track")
        with open("data_for_recognition/accelerometer/"+file_name+"_"+str(j)+".dat", "w") as f:
            for i in range(100):
                line = ser.readline().decode('utf-8', 'ignore')
                if line[:3] == 'AX:':
                    f.write(str(time.time()) + ' ' + line)
                else:
                    break
                    
        print('wrote into ' + file_name+"_"+str(j)+".dat")
        j += 1
        #cmd = tr.recognize_track("gyro.dat")
        #if cmd == 0:
        #    s_com = '3'
        #    ser_light.write(s_com.encode())
        #elif cmd == 1:
        #    s_com = '2'
        #    ser_light.write(s_com.encode())
        #elif cmd == 2:
        #    s_com = '1'
        #    ser_light.write(s_com.encode())
        #print(cmd)
ser.close()
# ser_light.close()
print("end")
