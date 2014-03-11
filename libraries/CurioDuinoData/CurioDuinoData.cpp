/* 
 * David Keck
 * filename: CurioDuinoData.cpp
 * Implementation of CurioDuinoData
 * For use with the CurioDuinoOBS software.
 * More information here: http://davidskeck.wordpress.com
 * or here http://github.com/davidskeck/CurioDuinoOBS
 */

// Include reflectance array class
#include <../CurioDuinoReflectanceSensorArray/CurioDuinoReflectanceSensorArray.h>

// Include header file
#include "CurioDuinoData.h"

void CurioDuinoData::update()
{
  // update edge sensors
  sensors.read(sensor_values);

  // update data struct
  leftEdge = (sensor_values[0] > QTR_THRESHOLD);
  rightEdge = (sensor_values[1] > QTR_THRESHOLD);
  battery = analogRead(BATTERY_SENSOR);
  leftObstacle = (!digitalRead(LEFT_OBST_SENSOR));
  middleObstacle = (!digitalRead(MIDDLE_OBST_SENSOR));
  rightObstacle = (!digitalRead(RIGHT_OBST_SENSOR));

  prepareData();
}

void CurioDuinoData::prepareData()
{
  // update formatted string for sending data
  dataFormatted = "";
  dataFormatted += leftEdge;
  dataFormatted += "LE";
  dataFormatted += rightEdge;
  dataFormatted += "RE";
  dataFormatted += battery;
  dataFormatted += "B";
  dataFormatted += leftObstacle;
  dataFormatted += "LO";
  dataFormatted += middleObstacle;
  dataFormatted += "MO";
  dataFormatted += rightObstacle;
  dataFormatted += "RO";
}

void CurioDuinoData::send()
{ 
  // send string of sensor data
  Serial.println(dataFormatted);
}