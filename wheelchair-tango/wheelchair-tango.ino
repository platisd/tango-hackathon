#include <Smartcar.h>

Odometer encoderRight(100), encoderLeft(100);
Car wheelchair;
const unsigned long PID_FREQUENCY = 20; //update the pid controllers at this frequency
unsigned long lastAngleUpdate = 0; // the last time we updated the angle
long frontDistance = 0;
long backDistance = 0;
float previousError = 0.0, integratedError = 0.0, correction = 0.0, previousControlledAngle = 0.0;
int angleSetPoint = 0; //current error in angle
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
  encoderLeft.attach(2, 10); // pwm pin, direction pin
  encoderRight.attach(3, 11); //pwm pin, direction pin
  wheelchair.begin(encoderLeft, encoderRight);
  wheelchair.enableCruiseControl(speedP, speedI, speedD, PID_FREQUENCY);
}

void loop() {
  wheelchair.updateMotors();
  updateAngle();
  getSerialInput();
}

void getSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('*');
    if (input.startsWith("a")) { //angle
      angleSetPoint = input.substring(1).toInt();
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

float anglePIDcontrol(const float previousAngle, const int targetAngle) { //previous output, set point (current error)
  float correction = 0;
  float error = targetAngle;
  integratedError += error;
  correction = (angleP * error) + (angleI * integratedError) + (angleD * (error - previousError));
  previousError = error;
  return constrain(previousAngle + correction, -90, 90);
}
