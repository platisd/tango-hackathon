#include <Smartcar.h>

Odometer encoderLeft(570), encoderRight(570);
const unsigned short LEFT_MOTOR[] = {8, 9, 6};
const unsigned short RIGHT_MOTOR[] = {11, 10, 5};

Car wheelchair(useShieldMotors(LEFT_MOTOR, RIGHT_MOTOR));

const unsigned long PID_FREQUENCY = 20; //update the pid controllers at this frequency
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

DemoState currentState = firstStraight;
unsigned long startedTurning = 0; //the moment when we start turning right
const unsigned long TURNING_DISTANCE = 150; //distance in cm on how much the left wheel should turn
unsigned long startingDistance = 0;

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
      wheelchair.go(150);
      startedTurning = millis();
      startingDistance = encoderLeft.getDistance();
      currentState = turningRight;
      break;
    case turningRight:
      wheelchair.setAngle(90);
      if ((encoderLeft.getDistance() - startingDistance) >= TURNING_DISTANCE){
        wheelchair.setAngle(0);
        currentState = secondStraight;
      }
      break;
    case secondStraight:
      wheelchair.go(100);
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
