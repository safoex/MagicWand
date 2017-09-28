import serial
import time

ser = serial.Serial('COM3',9600)
cmd = '1'
ser.write('12')
