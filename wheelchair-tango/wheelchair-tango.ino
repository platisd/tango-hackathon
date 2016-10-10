#include <Smartcar.h>

Odometer encoderLeft(570), encoderRight(570);
const unsigned short LEFT_MOTOR[] = {8, 9, 6};
const unsigned short RIGHT_MOTOR[] = {11, 10, 5};

Car wheelchair(useShieldMotors(LEFT_MOTOR, RIGHT_MOTOR));

const unsigned long PID_FREQUENCY = 20; //update the pid controllers at this frequency
unsigned long lastAngleUpdate = 0; // the last time we updated the angle
long frontDistance = 0;
long backDistance = 0;
float previousError = 0.0, integratedError = 0.0, correction = 0.0, previousControlledAngle = 0.0;
float angleSetPoint = 0.0; //current error in angle
/* PID values for ANGLE */
float angleP = 0.5;
float angleI = 0.0;
float angleD = 0.0;
/* PID values for SPEED */
float speedP = 0.5;
float speedI = 0.0;
float speedD = 0.0;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(200);
  encoderRight.attach(2, 12, HIGH);
  encoderLeft.attach(3, 13, LOW);
  wheelchair.begin(encoderLeft, encoderRight);
  wheelchair.enableCruiseControl(speedP, speedI, speedD, PID_FREQUENCY);
}

void loop() {
  wheelchair.updateMotors();
  // updateAngle();
  // getSerialInput();
  handleInput();
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
      int distance = input.substring(1).toInt();
      wheelchair.go(distance);
    }
  }
}

void getSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('*');
    if (input.startsWith("a")) { //angle
      angleSetPoint = input.substring(1).toFloat();
    } else if (input.startsWith("s")) { //speed
      float throttle = input.substring(1).toFloat();
      wheelchair.setSpeed(throttle);
    }
  }
}

void updateAngle() {
  unsigned long currentTime = millis();
  if (currentTime >= lastAngleUpdate + PID_FREQUENCY) {
    float controlledAngle = anglePIDcontrol(previousControlledAngle, angleSetPoint);
    wheelchair.setSpeed(lroundf(controlledAngle));
    previousControlledAngle = controlledAngle;
    lastAngleUpdate = currentTime;
  }
}

float anglePIDcontrol(const float previousAngle, const float targetAngle) { //previous output, set point (current error)
  float correction = 0;
  float error = targetAngle;
  integratedError += error;
  correction = (angleP * error) + (angleI * integratedError) + (angleD * (error - previousError));
  previousError = error;
  return constrain(previousAngle + correction, -90, 90);
}
