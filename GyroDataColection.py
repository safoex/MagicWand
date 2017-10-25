import serial
import time
import TrackRecognition as tr

ser = serial.Serial('COM4',115200)
# ser_light = serial.Serial("COM3", 9600)

while ser.isOpen():
    input_string = ser.readline().decode('utf-8', 'ignore')
    if input_string[:3] == "AX:":
        print("start track")
        with open("gyro.dat", "w") as f:
            for i in range(70):
                f.write(str(time.time()) + ' ' + ser.readline().decode('utf-8', 'ignore'))

        # cmd = tr.recognize_track("gyro.dat")
        # if cmd == 0:
        #     s_com = '3'
        #     ser_light.write(s_com.encode())
        # elif cmd == 1:
        #     s_com = '2'
        #     ser_light.write(s_com.encode())
        # elif cmd == 2:
        #     s_com = '1'
        #     ser_light.write(s_com.encode())
ser.close()
# ser_light.close()
print("end")
