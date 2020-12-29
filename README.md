# automated_garden
Temperature, humidity, airflow, and light regulation with arduino and raspberry pi. 

Arduino is used for all sensing and actuating. It acts as a **permanent controller**.

Raspberry Pi can be attached via usb for the purpose of data logging. This is as needed, acting as a **temporary data reader** so the user can monitor environmental conditions. Useful when tuning the environment to see how various system parameters affect environmental conditions (e.g. how fan cycle affects air temp and humidity). Once environment is well-tuned, data logging is not necessary and system can be run only with arduino as controller. 

# Arduino
  ## 'timed_switch' class
  The timed_switch class utilizes passive delay logic via the `millis()` function. This allows multiple timers to be made and packaged neatly as objects in the main Arduino code. All the hardwork is hidden in the switch class. The "switch" is a hi/lo signal on a digital pin. This can be wired to a MOSFET or a relay to control external devices.
  ## Lights
  The lights are run with a modified switch class that accepts a PWM value as an argument. The digital pin out from the switch is hooked up to a MOSFET for switching the lights on and off.
  ## Airflow
  The CPU style fans I selected run at 12V, ~0.2A. There are three that are powered in parallel. The digital pin out from the switch toggles a relay. 
  ## Humidity
  ### Homemade humidifier V1 - timer (UNSUCCESSFUL)
  Using an ultrasonic mist module, a CPU fan, and some tubing, a humidifier was constructed. 
  Notably, the software to control it worked great, but the mister was not able to generate enough volume of mist to keep the environment at a sufficient humidity. 
  
  Again, a modified switch class was made with two nested switches. The parent timer toggled the mister, while the child timer toggled the fan on and off within the on period of the mister. The reason for the sub-switch on the fan was to allow the mister to build up mist in the box during a "charge" period and then have the fan distribute the mist during a short "blow" period. This proved to be more effective at distributing mist than having the fan continuously on with the mister.
  
  ### Homemade humidifier V2 - hysteresis (UNSUCCESSFUL)
  The same humidifier set up was used but instead of the charge/blow cycle of distributing mist being on a timed schedule, it was toggled on and off using a DHT22 sensor and hysteresis. This worked better than the strictly timer based humidificaiton, but again the homemade humidifer simply could not output enough water vapor to keep the environment at sufficient humidity.
  
  As such, I broke and bought a cool-mist humidifier and enabled it with hysteresis and a humiditiy sensor. 
  
  ### Cool-mist humidifier

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
  ## continuous ssh session with screen!
  https://raspi.tv/2012/using-screen-with-raspberry-pi-to-avoid-leaving-ssh-sessions-open
  
  Useful commands:
  
  | Command                | Description           |
  | :--------------------- | :-------------------- |
  | `screen -s `_`name`_     | Start a named screen  |
  | <kbd>Ctrl</kbd> + <kbd>A</kbd>, <kbd>D</kbd> | Detach from current screen |
  | `screen -ls`           | List screen sessions  |
  | `screen -r `_`name`_     | Reconnect to _`name`*_|
  | `ECHO $STY`            | Echo current screen name (if in a screen) |
  
  
  
  
  ## Database
  ### credentials and logging in
  ### making a table, granting permissions
  ### writing to table
  ### pulling out of table (cursor usage)
  ### plotting data
  
# Hardware Notes
  ## Fans 
  static pressure vs airflow
