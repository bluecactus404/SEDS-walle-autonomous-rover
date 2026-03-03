#include <Pixy2.h>
#include <Servo.h>
#include <CytronMotorDriver.h>

// MOTOR SETUP
CytronMD leftMotor(PWM_DIR, 9, 8);
CytronMD rightMotor(PWM_DIR, 10, 7);

// PIXY + SERVO
Pixy2 pixy;
Servo claw;
Servo liftServo;

// IR SENSORS (Boundary Detection)
const int IR_PIN[4] = {A0, A2, A3, A5};
int IR_VAL[4];

// CONTROL CONSTANTS
const int CENTER_X = 150;
const int TARGET_WIDTH = 200;
const int TARGET_HEIGHT = 400;

const float Kp = 1.5;

const int MAX_SPEED = 255;
const int MIN_SPEED = 50;
const int FORWARD_SPEED = 150;
const int SEARCH_SPEED = 80;
const int TURN_THRESHOLD = 5;

// STATE MACHINE
enum Mode {
  SEARCH,
  TRACK,
  GRAB,
  DONE
};

Mode currentMode = SEARCH;

// SETUP
void setup() {

  // IR setup
  for (int i = 0; i < 4; i++) {
    pinMode(IR_PIN[i], INPUT_PULLUP);
  }

  claw.attach(6);
  liftServo.attach(5);

  claw.write(0);        // claw open
  liftServo.write(0);   // arm down

  pixy.init();
}

// MAIN LOOP
void loop() {

  checkBoundary();   // Always check boundary first

  switch(currentMode) {

    case SEARCH:
      searchBehavior();
      break;

    case TRACK:
      trackObject();
      break;

    case GRAB:
      grabObject();
      currentMode = DONE;
      break;

    case DONE:
      stopMotors();
      break;
  }
}

// MOTOR FUNCTIONS
void moveForward(int speed) {
  leftMotor.setSpeed(speed);
  rightMotor.setSpeed(speed);
}

void moveBackward(int speed) {
  leftMotor.setSpeed(-speed);
  rightMotor.setSpeed(-speed);
}

void turnLeft(int speed) {
  leftMotor.setSpeed(speed);
  rightMotor.setSpeed(-speed);
}

void turnRight(int speed) {
  leftMotor.setSpeed(-speed);
  rightMotor.setSpeed(speed);
}

void stopMotors() {
  leftMotor.setSpeed(0);
  rightMotor.setSpeed(0);
}

// SEARCH MODE

void searchBehavior() {

  pixy.ccc.getBlocks();

  if (pixy.ccc.numBlocks > 0) {
    currentMode = TRACK;
    return;
  }

  // Slow rotation search
  turnLeft(SEARCH_SPEED);
}

// TRACK MODE
void trackObject() {

  pixy.ccc.getBlocks();

  if (pixy.ccc.numBlocks == 0) {
    currentMode = SEARCH;
    return;
  }

  // Track largest block
  int largestIndex = 0;
  int largestArea = 0;

  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    int area = pixy.ccc.blocks[i].m_width * pixy.ccc.blocks[i].m_height;
    if (area > largestArea) {
      largestArea = area;
      largestIndex = i;
    }
  }

  int object_x = pixy.ccc.blocks[largestIndex].m_x;
  int object_width = pixy.ccc.blocks[largestIndex].m_width;
  int object_height = pixy.ccc.blocks[largestIndex].m_height;

  int error = object_x - CENTER_X;

  int turn_speed = constrain(abs(error) * Kp, MIN_SPEED, MAX_SPEED);

  // Object to right
  if (error < -TURN_THRESHOLD) {
    turnRight(turn_speed);
  }
  // Object to left
  else if (error > TURN_THRESHOLD) {
    turnLeft(turn_speed);
  }
  // Centered
  else {

    if (object_width < TARGET_WIDTH) {
      moveForward(FORWARD_SPEED);
    }
    else if (object_height > TARGET_HEIGHT) {
      currentMode = GRAB;
    }
    else {
      moveForward(FORWARD_SPEED);
    }
  }
}

// GRAB MODE
void grabObject() {

  stopMotors();
  delay(300);

  claw.write(180);   // close claw
  delay(400);

  liftServo.write(120);  // lift object
  delay(500);
}

// BOUNDARY DETECTION
void checkBoundary() {

  for (int i = 0; i < 4; i++) {
    IR_VAL[i] = digitalRead(IR_PIN[i]);
  }

  // If any outer sensor sees black tape
  if (IR_VAL[0] == HIGH || IR_VAL[3] == HIGH) {

    stopMotors();
    delay(100);

    moveBackward(150);
    delay(300);

    turnLeft(150);
    delay(300);
  }
}