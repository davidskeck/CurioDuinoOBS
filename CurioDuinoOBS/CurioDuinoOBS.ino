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

// Pre-packaged libraries
#include <NilRTOS.h>
#include <NilSerial.h>

// Custom libraries
#include <CurioDuinoData.h>
#include <CurioDuinoNav.h>

// Macro to redefine Serial as NilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
#define Serial NilSerial

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
  
  // Check if speed has changed
  if (nav.forwardSpeed != data.newSetSpeed)
  {
    nav.forwardSpeed = data.newSetSpeed;
  }
}

//------------------------------------------------------------------------------ 
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
NIL_WORKING_AREA(waThread1, 64);

// Declare thread function for thread 1.
NIL_THREAD(Thread1, arg) {

  while (TRUE) 
  {
    data.update();
    data.receive();
    checkDataAndWait();
    data.send();
    
    // Sleep so lower priority threads can execute.
    nilThdSleep(10);
  }
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
NIL_WORKING_AREA(waThread2, 64);

// Declare thread function for thread 2.
NIL_THREAD(Thread2, arg) {

  while (TRUE) 
  {
    if (data.leftEdge || data.rightEdge)
    {
      nav.stopMoving();
      
      if (data.leftEdge)
      {
        nav.goReverse();
        nav.turnRight(EDGE_TURN_DURATION);
      }
      
      else
      {
        nav.goReverse();
        nav.turnLeft(EDGE_TURN_DURATION);
      }
      
      nav.goForward();
    }
    
    // Sleep so lower priority threads can execute.
    nilThdSleep(20);
  }
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
NIL_WORKING_AREA(waThread3, 64);

// Declare thread function for thread 3.
NIL_THREAD(Thread3, arg) {

  while (TRUE) 
  {
    if (data.middleObstacle || data.rightObstacle || data.leftObstacle)
    {
      // Obstacle detected
      nav.stopMoving();
      
      // Get a random int from 1 to 2
      int rand = random (1, 3);
      
      if (rand == 1)
      {
        nav.turnLeft(OBSTACLE_TURN_DURATION);
      }
      else
      {
        nav.turnRight(OBSTACLE_TURN_DURATION);
      }
    }
    
    else
    {
      nav.goForward();
    }
  }
}
//------------------------------------------------------------------------------
/*
 * Threads static table, one entry per thread.  A thread's priority is
 * determined by its position in the table with highest priority first.
 *
 * These threads start with a null argument.  A thread's name is also
 * null to save RAM since the name is currently not used.
 */
NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY(NULL, Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY(NULL, Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_ENTRY(NULL, Thread3, NULL, waThread3, sizeof(waThread3))
NIL_THREADS_TABLE_END()
//------------------------------------------------------------------------------
void setup()
{  
  // Open serial port at 9600 baud
  Serial.begin(9600);
  
  // Start data class
  data.begin();
  
  // Start navigation class
  nav.begin();
  
  // Set starting speed
  nav.forwardSpeed = 75;
  
  // Wait for GUI to signal start
  checkDataAndWait();
  
  // Start the kernel
  nilSysBegin();
}

// Idle thread
void loop()
{}

