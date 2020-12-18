# automated_garden
Temperature, humidity, airflow, and light regulation with arduino and raspberry pi. 

Arduino is used for all sensing and actuating. It acts as a **permanent controller**.

Raspberry Pi can be attached via usb for the purpose of data logging. This is as needed, acting as a **temporary data reader** so the user can monitor environmental conditions. Useful when tuning the environment to see how various system parameters affect environmental conditions (e.g. how fan cycle affects air temp and humidity). Once environment is well-tuned, data logging is not necessary and system can be run only with arduino as controller. 

# Arduino
  ## 'timed_switch' class
  ## Lights
  ## Airflow
  ## Humidity

# Raspberry Pi
  ## pySerial
  https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial
  https://roboticsbackend.com/raspberry-pi-arduino-serial-communication/
  
  ### Detect the Arduino
  In the Pi terminal:
  
  With Arduino unplugged from Pi, run `ls /dev/tty*` to see serial devices.
  Plug Arduino into Pi and run `ls /dev/tty*` and look for the new device listed. 
  It should be something like `ttyACM0` or `ttyUSB0`.
  
  ### Serial Permissions
  In the Pi terminal:
  
  Run `sudo adduser your_un dialout` to give yourself permissions. Restart Pi.
  
  ### Serial Connection
  First, install the pySerial library from the Pi terminal with `python3 -m pip install pyserial`.
  
  #### Pi Python Code
  This is a simple code that will read from the Pi and log into a csv. **Make sure to set the baud rate the same as you have in your Arduino code**.
  ```python
  import serial
  import time
  import csv
  
  if __name__ == '__main__':
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
  
  ```
  More complicated methods can be found [in this blog post](https://roboticsbackend.com/raspberry-pi-arduino-serial-communication/), including bi-directional communication.
  This [Maker Portal post](https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial) shows how to make a live updating graph with `Matplotlib`.
  
  ## Database
  ### credentials and logging in
  ### making a table, granting permissions
  ### writing to table
  ### pulling out of table (cursor usage)
  ### plotting data
  
# Hardware Notes
  ## Fans 
  static pressure vs airflow
