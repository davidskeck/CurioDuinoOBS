// Code originally from Pololu
// https://github.com/pololu/zumo-shield
// Edited for CurioDuino Project
// by David Keck

#include <../QTRSensors/QTRSensors.h>
#include <Arduino.h>

#if defined(__AVR_ATmega32U4__)
  // Arduino Leonardo
  #define CURIODUINO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN  A4
#else
  // Arduino UNO and other ATmega328P/168 Arduinos
  #define CURIODUINO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN  2
#endif

class CurioDuinoReflectanceSensorArray : public QTRSensorsRC
{
  public:

  // if this constructor is used, the user must call init() before using
  // the methods in this class
  CurioDuinoReflectanceSensorArray()
  {
    // Initialize to have red header lights on at all times
    init(255);
  }

  // this constructor calls init with the given emitter pin and default values for other settings
  CurioDuinoReflectanceSensorArray(unsigned char emitterPin)
  {
    init(emitterPin);
  }

  // this constructor calls init with all settings as given
  CurioDuinoReflectanceSensorArray(unsigned char * pins, unsigned char numSensors, unsigned int timeout = 2000,
    unsigned char emitterPin = CURIODUINO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN)
  {
    QTRSensorsRC::init(pins, numSensors, timeout, emitterPin);
  }


  /* To disable the emitter control feature, specify QTR_NO_EMITTER_PIN for the first argument. */
  void init(unsigned char emitterPin = CURIODUINO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN)
  {
	  // THIS HAS BEEN MODIFIED FROM unsigned char sensorPins[] = { 4, A3, 11, A0, A2, 5 };
	  // TO ONLY READ TWO SENSORS FOR THE PURPOSE OF CURIODUINO
	  // THIS WILL INCREASE EFFICIENCY IN THE RTOS
    unsigned char sensorPins[] = { 4, 5 };
    QTRSensorsRC::init(sensorPins, sizeof(sensorPins), 2000, emitterPin);
  }

  void init(unsigned char * pins, unsigned char numSensors, unsigned int timeout = 2000,
    unsigned char emitterPin = CURIODUINO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN)
  {
    QTRSensorsRC::init(pins, numSensors, timeout, emitterPin);
  }
};

/*Copyright (c) 2013 Pololu Corporation.  For more information, see

http://www.pololu.com/
http://forum.pololu.com/

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.*/
