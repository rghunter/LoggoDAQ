***************************
* Loggomatic V2 Firmware Mod
* ADXL345 Accelerometer Support (I2C)
* Ryan Hunter - TR Aeronautics
* <rhunter@traeronautics.com>
****************************


Description:
	This firmware for the loggomatic provides a bitbanged I2C driver that supports the Analog Devices ADXL345 Accelerometer.
	
	
Installation:

	Hardware
	On the IMU Board -> break out the SDA, SCL, GND, and 3.3v lines
	
	Connection diagram:
		
		IMU Board		Loggomatic V2
			3.3v	->	3.3v
			GND		->	GND
			SDA		->	MISO
			SCL		->	SCK
			
	Firmware Installation:
	Simply copy the firmware file (FW.SFE) to a micro sd card formatted with fat16 and power on the board.



References:

	This firmware is based on the Loggomatic Kwan <http://code.google.com/p/logomatic-kwan/> 
	The I2C bitbang algorithim is derived from <http://forum.sparkfun.com/viewtopic.php?f=11&t=22774>