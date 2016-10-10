#include <Smartcar.h>

Odometer encoderLeft(570), encoderRight(570);
const unsigned short LEFT_MOTOR[] = {8, 9, 6};
const unsigned short RIGHT_MOTOR[] = {11, 10, 5};

Car wheelchair(useShieldMotors(LEFT_MOTOR, RIGHT_MOTOR));

const unsigned long PID_FREQUENCY = 5; //update the pid controllers at this frequency
unsigned long lastAngleUpdate = 0; // the last time we updated the angle
long frontDistance = 0;
long backDistance = 0;
float previousError = 0.0, integratedError = 0.0, correction = 0.0, previousControlledAngle = 0.0;
float angleSetPoint = 0.0; //current error in angle

/* PID values for SPEED */
float speedP = 0.5;
float speedI = 0.0;
float speedD = 0.0;

enum DemoState {
  firstStraight,
  turningRight,
  secondStraight,
  idle
};

DemoState currentState = idle;
unsigned long startedTurning = 0; //the moment when we start turning right
const unsigned long TURNING_INTERVAL = 5500; //the milliseconds that we turn

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(200);
  encoderRight.attach(2, 12, HIGH); // Pulse pin, direction pin, the state of the direction pin when going forward
  encoderLeft.attach(3, 13, LOW);
  wheelchair.begin(encoderLeft, encoderRight);
  wheelchair.enableCruiseControl(speedP, speedI, speedD, PID_FREQUENCY);
}

void loop() {
  wheelchair.updateMotors();
  handleInput();
  switch (currentState) {
    case idle:
      wheelchair.setSpeed(0);
      break;
    case firstStraight:
      wheelchair.setSpeed(0.3);
      Serial.println("First straight");
      wheelchair.go(150);
      Serial.println("Turning right");
      startedTurning = millis();
      currentState = turningRight;
      break;
    case turningRight:
      wheelchair.setAngle(90);
      if (millis() >= startedTurning + TURNING_INTERVAL) {
        wheelchair.setAngle(0);
        currentState = secondStraight;
      }
      break;
    case secondStraight:
      Serial.println("second straight");
      wheelchair.go(100);
      Serial.println("Idle");
      currentState = idle;
      break;
    default:
      break;
  }
}

void handleInput() { //handle serial input if there is any
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("m")) {
      float throttle = input.substring(1).toFloat();
      wheelchair.setSpeed(throttle);
    } else if (input.startsWith("t")) {
      int deg = input.substring(1).toInt();
      wheelchair.setAngle(deg);
    } else if (input.startsWith("g")) {
      currentState = firstStraight;
    }
  }
}






//  unsigned long currentTime = millis();
//  if (currentTime >= 2000 && currentTime <= 3600) wheelchair.setMotorSpeed(2,2);
//  if (currentTime >= 3600 && currentTime <= 5600) wheelchair.setMotorSpeed(5, 5);
//  if (currentTime >= 5600 && currentTime <= 6600) wheelchair.setMotorSpeed(6, 6);
//  if (currentTime >= 6600 && currentTime <= 10600) wheelchair.setMotorSpeed(8, 8);
//  if (currentTime >= 10600 && currentTime <= 13000) wheelchair.setMotorSpeed(14, 2);
//  if (currentTime >= 13000 && currentTime <= 15000) wheelchair.setMotorSpeed(6, 9);
//  if (currentTime >= 15000 && currentTime <= 19000) wheelchair.setMotorSpeed(8, 8);
//  if (currentTime >= 19000) wheelchair.setMotorSpeed(0, 0);
