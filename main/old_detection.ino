// Import the motor driver lib
#include "CytronMotorDriver.h"


// Configure the motor driver
CytronMD motor1(PWM_DIR, 3, 4);
CytronMD motor2(PWM_DIR, 9, 10);


// Initialize the sensors
int sensor1 = 2;
int sensor2 = 3;


void setup() {
  // Set up pins for sensor inputs
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
}


void loop() {
  //Read sensors 1 & 2
  int s1 = digitalRead(sensor1);
  int s2 = digitalRead(sensor2);


/* If/else loop goes through the different options of sensor detection and decides which action statement to use depending on the condition */
  if (s1 == HIGH && s2 == HIGH)
  {
    // If s1 and s2 detect HIGH, motors 1 & 2 stop
    motor1.setSpeed(0);
    motor2.setSpeed(0);
  }
  else if (s1 == LOW && s2 == LOW)
  {
    // If s1 and s2 detect LOW, motors 1 & 2 spin in opposite directions
    motor1.setSpeed(-255);
    motor2.setSpeed(255);
  }
  else if (s1 == LOW && s2 == HIGH)
  {
    // If s1 detects LOW and s2 detects HIGH, motor 1 spins and motor 2 stops
    motor1.setSpeed(255);
    motor2.setSpeed(0);
  }
  else if (s1 == HIGH && s2 == LOW)
  {
    // If s1 detects HIGH and s2 detects LOW, motor 1 stops and motor 2 spins
    motor1.setSpeed(0);
    motor2.setSpeed(255);
  }
}
