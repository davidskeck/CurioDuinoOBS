/* 
 * David Keck
 * filename: CurioDuinoData.h
 * Library header file for CurioDuinoData implementation
 * For use with the CurioDuinoOBS software.
 * More information here: http://davidskeck.wordpress.com
 * or here http://github.com/davidskeck/CurioDuinoOBS
 */

// Macro guard
#ifndef CurioDuinoData_h
#define CurioDuinoData_h

// Include arduino specific function
#include <Arduino.h>

// Forward declaration of class
class CurioDuinoReflectanceSensorArray;

// Reflectance array right and left sensors only
#define NUM_SENSORS 2

// Higher value here means less sensitive edge detection
#define QTR_THRESHOLD  1400 // microseconds

 // Battery readout pin
#define BATTERY_SENSOR A1

// Obstacle sensors and their pins
#define MIDDLE_OBST_SENSOR 11
#define RIGHT_OBST_SENSOR  17
#define LEFT_OBST_SENSOR 14

class CurioDuinoData
{
public:

	// Update all sensor data
	void update();

	// Send string containing formatted data
	void send();

	// Prepare string to be sent
	void prepareData();

	// Public to adhere to NASA's MSL rules of
	// only void return functions

	// Array to hold sensor pin for edge detections
	unsigned int sensor_values[NUM_SENSORS];
    // Automatically initialized to no emitter pin
    // Side header lights on both sides always turned on
    CurioDuinoReflectanceSensorArray sensors;
    int battery;
    boolean leftEdge, rightEdge, leftObstacle,
    rightObstacle, middleObstacle;
    String dataFormatted;
};
#endif