CurioDuinoOBS
=============
This is the on board software for the CurioDuino robot. This software is loaded onto the Arduino Uno. This is a very early working prototype. Real-time OS libraries have not yet been added.

Dependencies
=============

IMPORTANT: These belong one directory higher than the CurioDuino folder, in a folder called "libraries". If they are not there, the Arduino IDE will not be able to find and use them.

* Zumo Shield Libraries - https://github.com/pololu/zumo-shield
  * ZumoBuzzer.h
  * ZumoMotors.h
  * QTRSensors.h

* Compass Libraries = https://github.com/pololu/lsm303-arduino
	* Wire.h
	* LSM303.h

* Included Library
  * CurioDuinoReflectanceArray.h

Further reading
=============
More information can be found here: http://davidskeck.wordpress.com
