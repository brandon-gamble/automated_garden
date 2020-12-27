import Adafruit_DHT     # for hum/temp
import smbus            # for luminance
import RPi.GPIO as GPIO # for moisture
import time

class TempHumSensor(object):
# DHT sensor class
# returns temperatureF,C and humidity

    def __init__(self, dataPin):
        # self.pin=pin
        self.sensor = Adafruit_DHT.DHT11
        self.dataPin = dataPin
        self.hi = "this dht object exists!"

    def getDataOnce(self):
        # humidity, temperature = Adafruit_DHT.read_retry(self.sensor, self.pin)
        humidity, temperatureC = Adafruit_DHT.read_retry(self.sensor, self.dataPin)
        temperatureF = (temperatureC * 9/5) + 32
        # print('Temp: {0:0.1f} C  Humidity: {1:0.1f}'.format(temperature, humidity))
        # print('Temp : %0.1f C' % temperatureC)
        # print('Temp : %0.1f F' % temperatureF)
        # print('Humidity : %0.1f%%' % humidity)
        return temperatureF, temperatureC, humidity


class LumSensor(object):
# max44009 sensor class
# returns luminance (lux)
# adapted from https://github.com/ControlEverythingCommunity/MAX44009/blob/master/Python/MAX44009.py
    def __init__(self, SDApin):
        self.SDApin = SDApin
        self.bus = smbus.SMBus(1)
        self.hi = "this lum sensor exists!"

    def getDataOnce(self):
        # MAX44009 address, 0x4A(74)
        # Select configuration register, 0x02(02)
        #		0x40(64)	Continuous mode, Integration time = 800 ms
        self.bus.write_byte_data(0x4A, 0x02, 0x40)

        time.sleep(0.5)

        # MAX44009 address, 0x4A(74)
        # Read data back from 0x03(03), 2 bytes
        # luminance MSB, luminance LSB
        data = self.bus.read_i2c_block_data(0x4A, 0x03, 2)
        # data = self.bus.read_i2c_block_data(0x4A, 0x03, self.SDApin)

        # Convert the data to lux
        exponent = (data[0] & 0xF0) >> 4
        mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F)
        luminance = ((2 ** exponent) * mantissa) * 0.045

        # Output data to screen
        # print( "luminance : %.2f lux" % luminance)
        return luminance


class MoistSensor(object):
# moisture sensor class
# returns dry, a boolean
#   0 means moist
#   1 means dry

	def __init__(self, digiPin):
		self.digiPin = digiPin
		self.hi = "a moisture sensor exists! :)"

		GPIO.setmode(GPIO.BCM)
		GPIO.setup(self.digiPin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

	def getDataOnce(self):

		if GPIO.input(self.digiPin): # GPIO = 1 means dry and led off
#			print("GPIO is %d" % GPIO.input(self.digiPin))
#			print("led off")
			# print("Moisture : we dry baby")
			dry = 1.0

		else: # GPIO = 0 means wet and led on
#			print("GPIO is %d" % GPIO.input(self.digiPin))
#			print("led on")
			# print("Moisture : drip drip")
			dry = 0.0

		return dry
