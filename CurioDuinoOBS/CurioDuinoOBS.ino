/* David Keck
 * CurioDuinoOBS.ino
 * This software controls the on-board
 * operations of the CurioDuino Autonomous
 * robot project.
 * More information can be found here:
 * davidskeck.wordpress.com
 */

#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
// The next 2 libraries are already included in
// other custom library source code. However,
// Arduino requires that all libraries that are
// used anywhere within the program are included
// in the main sketch. Otherwise, functions are undefined
#include <CurioDuinoReflectanceSensorArray.h>
#include <QTRSensors.h>
#include <Wire.h>
#include <LSM303.h>
#include <CurioDuinoData.h>

// Motor speeds
#define REVERSE_SPEED     75 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     75

// Almost makes 90 degree turn
// todo: change to compass nav
#define REVERSE_DURATION  400 // ms
#define TURN_DURATION     400 // ms

// Compass
#define CALIBRATION_SAMPLES 70  // Number of compass readings to take when calibrating
#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M value for magnetometer +/-2.5 gauss full scale
#define CRA_REG_M_220HZ    0x1C // CRA_REG_M value for magnetometer 220 Hz update rate

// Allowed deviation (in degrees) relative to target angle that must be achieved before driving straight
#define DEVIATION_THRESHOLD 5

LSM303 compass;
ZumoBuzzer buzzer;
ZumoMotors motors;
CurioDuinoData data;

// Start/stop signal
boolean isStarted = false;

void waitForSignalAndCountDown()
{
  // Check if signaled to start
  while(isStarted != true)
  {
    // Read and send data
    data.update();
    data.send();
    
    if(Serial.available() > 0)
    {
      // Signal was received
      isStarted = Serial.read();
    }
  }
  
  buzzer.playNote(NOTE_G(4), 500, 15);
}

void turnLeft()
{
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  delay(TURN_DURATION);
}

void turnRight()
{
  motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  delay(TURN_DURATION);
}

void goForward()
{
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}

void goReverse()
{
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
}

void stopMoving()
{
  motors.setSpeeds(0, 0);
}

void calibrateCompass()
{
  // The highest and lowest possible magnetic value to read in any direction is 2047 or -2047
  LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32767, -32767, -32767};
  
  unsigned char index;
  
  // To calibrate the magnetometer, the Zumo spins to find the max/min
  // magnetic vectors. This information is used to correct for offsets
  // in the magnetometer data.
  motors.setLeftSpeed(FORWARD_SPEED);
  motors.setRightSpeed(-FORWARD_SPEED);

  for(index = 0; index < CALIBRATION_SAMPLES; index ++)
  {
    // Take a reading of the magnetic vector and store it in compass.m
    compass.read();

    running_min.x = min(running_min.x, compass.m.x);
    running_min.y = min(running_min.y, compass.m.y);

    running_max.x = max(running_max.x, compass.m.x);
    running_max.y = max(running_max.y, compass.m.y);
    
    delay(50);
  }
  
  stopMoving();
  
  // Set calibrated values to compass.m_max and compass.m_min
  compass.m_max.x = running_max.x;
  compass.m_max.y = running_max.y;
  compass.m_min.x = running_min.x;
  compass.m_min.y = running_min.y;
}

void setup()
{  
  // Open serial port at 9600 baud
  Serial.begin(9600);
  
  // Initiate the Wire library and join the I2C bus as a master
  Wire.begin();

  // Initiate LSM303
  compass.init();

  // Enables accelerometer and magnetometer
  compass.enableDefault();

  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // +/- 2.5 gauss sensitivity to hopefully avoid overflow problems
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 220 Hz compass update rate

  waitForSignalAndCountDown();
  //calibrateCompass();
}

void loop()
{
  if (Serial.available() > 0)
  {
    isStarted = Serial.read();
    if(!isStarted)
    {
      // If signaled to stop, stop and wait
      stopMoving();
      // Continue sending data until new signal arrives
      while(Serial.available() == 0)
      {
        data.update();
        data.send();
      }
    }
  }
  
  data.update();
  data.send();
  
  if (data.leftEdge)
  {
    // Leftmost reflectance sensor detected an edge
    stopMoving();
    goReverse();
    turnRight();
    goForward();
  }
  
  else if (data.rightEdge)
  {
    // Rightmost reflectance sensor detected an edge
    stopMoving();
    goReverse();
    turnLeft();
    goForward();
  }

  if (data.middleObstacle || data.rightObstacle || data.leftObstacle)
  {
    // Obstacle detected
    stopMoving();
    
    // Get a random int from 1 to 2
    int rand = random (1, 3);
    
    if (rand == 1)
    {
      turnLeft();
    }
    else
    {
      turnRight();
    }
    
    goForward();
  }

  //else
  {
    goForward();
  }
  //*/
}

