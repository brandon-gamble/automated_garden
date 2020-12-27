import serial
import time
import csv

if __name__ == '__main__':
    // ttyUSB0 connection with baud rate 9600
    ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1) 
    ser.flush()

    while True:
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip()
                print(line)
                fname = "test.csv"
                with open(fname, "a") as f:
                    writer = csv.writer(f, delimiter=",")
                    writer.writerow([time.time(),line])
        except:
            print("uhoh exception made")
            break
