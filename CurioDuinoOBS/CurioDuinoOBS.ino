#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <Wire.h>
#include <LSM303.h>
#include <CurioDuinoReflectanceSensorArray.h>

// Status LED on pin 13
#define LED 13

// Higher value here means less sensitive edge detection
#define QTR_THRESHOLD  1400 // microseconds

// Motor speeds
#define REVERSE_SPEED     75 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     75

// Almost makes 90 degree turn
// todo: change to compass nav
#define REVERSE_DURATION  400 // ms
#define TURN_DURATION     400 // ms

// Reflectance array right and left sensors only
#define NUM_SENSORS 2
unsigned int sensor_values[NUM_SENSORS];

// Battery readout pin
#define BATTERY_SENSOR A1

// Obstacle sensors and their pins
#define MIDDLE_OBST_SENSOR 11
#define RIGHT_OBST_SENSOR 17
#define LEFT_OBST_SENSOR 14

// Compass
#define CALIBRATION_SAMPLES 70  // Number of compass readings to take when calibrating
#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M value for magnetometer +/-2.5 gauss full scale
#define CRA_REG_M_220HZ    0x1C // CRA_REG_M value for magnetometer 220 Hz update rate

// Allowed deviation (in degrees) relative to target angle that must be achieved before driving straight
#define DEVIATION_THRESHOLD 5

LSM303 compass;
ZumoBuzzer buzzer;
ZumoMotors motors;

// No emitter pin means edge detection LEDs are on at all times
CurioDuinoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

// Start/stop signal
boolean isStarted = false;

void waitForSignalAndCountDown()
{
  // Check if signalled to start
  while(isStarted != true)
  {
    // Read and send data
    sensors.read(sensor_values);
    sendData();
    
    if(Serial.available() > 0)
    {
      // Signal was received
      isStarted = Serial.read();
    }
  }

  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(200);
    buzzer.playNote(NOTE_G(3), 200, 15);
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

void sendData()
{
    String data;
    
    data = data + (sensor_values[0] > QTR_THRESHOLD);
    data = data + "LE";
    data = data + (sensor_values[1] > QTR_THRESHOLD);
    data = data + "RE";
    data = data + analogRead(BATTERY_SENSOR);
    data = data + "B";
    data = data + (!digitalRead(LEFT_OBST_SENSOR));
    data = data + "LO";
    data = data + (!digitalRead(MIDDLE_OBST_SENSOR));
    data = data + "MO";
    data = data + (!digitalRead(RIGHT_OBST_SENSOR));
    data = data + "RO";
    
    Serial.println(data);
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
  calibrateCompass();
}

void loop()
{
  if (Serial.available() > 0)
  {
    isStarted = Serial.read();
    if(!isStarted)
    {
      // If signalled to stop, stop and wait
      stopMoving();
      // Continue sending data until new signal arrives
      while(Serial.available() == 0)
      {
        // Read edge detection sensors
        sensors.read(sensor_values);
        sendData();
      }
    }
  }
  
  // Read edge detection sensors
  sensors.read(sensor_values);
  sendData();
  
  /*
  if (sensor_values[0] > QTR_THRESHOLD)
  {
    // Leftmost reflectance sensor detected an edge
    stopMoving();
    sendData();
    goReverse();
    turnRight();
    goForward();
  }
  
  else if (sensor_values[1] > QTR_THRESHOLD)
  {
    // Rightmost reflectance sensor detected an edge
    stopMoving();
    sendData();
    goReverse();
    turnLeft();
    goForward();
  }

  if (!(digitalRead(MIDDLE_OBST_SENSOR) && digitalRead(RIGHT_OBST_SENSOR) && digitalRead(LEFT_OBST_SENSOR)))
  {
    // Obstacle detected
    stopMoving();
    
    sendData();
    
    // Get a random int from 1 to 2
    int rand = random (1, 3);
    
    if (rand == 1)
    {
      turnLeft();
    }
    else
      turnRight();
      
    goForward();
  }

  else
  {
    goForward();
  }
  //*/
}

