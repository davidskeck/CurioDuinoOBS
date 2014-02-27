#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
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

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // Button on pin 12

// No emitter pin means edge detection LEDs are on at all times
CurioDuinoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

// todo: convert this for gui
void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);

  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(400);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  
  buzzer.playNote(NOTE_G(4), 500, 15);  
  delay(1000);
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

void setup()
{
  pinMode(LED, HIGH);
  Serial.begin(9600);
  //waitForButtonAndCountDown();
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

void loop()
{
  if (button.isPressed())
  {
    // If button is pressed, stop and wait for another press to go again
    stopMoving();
    button.waitForRelease();
    button.waitForButton();
  }
  
  // Read edge detection sensors
  sensors.read(sensor_values);
  
  sendData();
  /*
  if (sensor_values[0] > QTR_THRESHOLD)
  {
    // Leftmost reflectance sensor detected an edge
    stopMoving();
    goReverse();
    turnRight();
    goForward();
  }
  
  else if (sensor_values[1] > QTR_THRESHOLD)
  {
    // Rightmost reflectance sensor detected an edge
    stopMoving();
    goReverse();
    turnLeft();
    goForward();
  }

  if (!(digitalRead(MIDDLE_OBST_SENSOR) && digitalRead(RIGHT_OBST_SENSOR) && digitalRead(LEFT_OBST_SENSOR)))
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
      turnRight();
      
    goForward();
  }

  else
  {
    goForward();
  }
  */
}

