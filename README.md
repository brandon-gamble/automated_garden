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
  
  ## Database
  ### credentials and logging in
  ### making a table, granting permissions
  ### writing to table
  ### pulling out of table (cursor usage)
  ### plotting data
  
# Hardware Notes
  ## Fans 
  static pressure vs airflow
