import serial
import time

ser = serial.Serial("COM3", 9600)
# ser.port = "COM3"
# ser.baudrate = 9600
cmd = '1'
time.sleep(2)
print(ser.write(cmd.encode()))
