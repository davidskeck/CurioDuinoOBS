/* 
 * David Keck
 * CurioDuinoOBS.ino
 * This software controls the on-board
 * operations of the CurioDuino Autonomous
 * robot project.
 * More information can be found here:
 * davidskeck.wordpress.com
 * or here http://github.com/davidskeck/CurioDuinoOBS
 */

// The next 3 libraries are already included in
// other custom library source code. However,
// Arduino requires that all libraries that are
// used anywhere within the program are included
// in the main sketch. Otherwise, their functions are undefined
#include <CurioDuinoReflectanceSensorArray.h>
#include <QTRSensors.h>
#include <ZumoMotors.h>

// Pre-package libraries
#include <ZumoBuzzer.h>
#include <NilRTOS.h>
#include <NilSerial.h>

// Custom libraries
#include <CurioDuinoData.h>
#include <CurioDuinoNav.h>

// Macro to redefine Serial as NilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
#define Serial NilSerial

ZumoBuzzer buzzer;
CurioDuinoData data;
CurioDuinoNav nav;

void checkDataAndWait()
{
  // Check if signaled to start
  while(data.isStarted != true)
  {
    // Stop moving
    nav.stopMoving();
    
    // Read, send, and receive data
    data.update();
    data.send();
    data.receive();
  }
}

void setup()
{  
  // Open serial port at 9600 baud
  Serial.begin(9600);
  
  // Start data class
  data.begin();
  
  // Start navigation class
  nav.begin();
  
  // Wait for GUI to signal start
  checkDataAndWait();
}

void loop()
{
  data.receive();
  
  checkDataAndWait();
  
  data.update();
  data.send();
  
  nav.setSpeed(100);
  
  if (data.leftEdge)
  {
    // Leftmost reflectance sensor detected an edge
    nav.stopMoving();
    nav.goReverse();
    nav.turnRight();
    nav.goForward();
  }
  
  else if (data.rightEdge)
  {
    // Rightmost reflectance sensor detected an edge
    nav.stopMoving();
    nav.goReverse();
    nav.turnLeft();
    nav.goForward();
  }

  if (data.middleObstacle || data.rightObstacle || data.leftObstacle)
  {
    // Obstacle detected
    nav.stopMoving();
    
    // Get a random int from 1 to 2
    int rand = random (1, 3);
    
    if (rand == 1)
    {
      nav.turnLeft();
    }
    else
    {
      nav.turnRight();
    }
    
    nav.goForward();
  }

  //else
  {
    nav.goForward();
  }
}

