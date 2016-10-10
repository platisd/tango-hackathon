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
/* PID values for ANGLE */
float angleP = 0.1;
float angleI = 0.0;
float angleD = 0.1;
/* PID values for SPEED */
float speedP = 1.2;
float speedI = 0.0;
float speedD = 0.0;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(200);
  encoderRight.attach(2, 12, HIGH); // Pulse pin, direction pin, the state of the direction pin when going forward
  encoderLeft.attach(3, 13, LOW);
  wheelchair.begin(encoderLeft, encoderRight);
  wheelchair.enableCruiseControl(speedP, speedI, speedD, PID_FREQUENCY);
}

void loop() {
  long dLeft = encoderLeft.getRelativeDistance();
  long dRight = encoderRight.getRelativeDistance();
  angleSetPoint = dRight - dLeft; //the error we have
  if ((dLeft + dRight) / 2.0 >= 1000) {
    wheelchair.setSpeed(0);
  }
  wheelchair.updateMotors();
  updateAngle();
  handleInput();
}

void updateAngle() {
  unsigned long currentTime = millis();
  if (currentTime >= lastAngleUpdate + PID_FREQUENCY) {
    float controlledAngle = anglePIDcontrol(previousControlledAngle, angleSetPoint);
    wheelchair.setAngle(lroundf(controlledAngle));
    Serial.print(angleSetPoint);
    Serial.print(",");
    Serial.println(controlledAngle);
    previousControlledAngle = controlledAngle;
    lastAngleUpdate = currentTime;
  }
}

float anglePIDcontrol(const float previousAngle, const float targetAngle) { //previous output, set point (current error)
  float correction = 0.0;
  float error = targetAngle;
  integratedError += error;
  correction = (angleP * error) + (angleI * integratedError) + (angleD * (error - previousError));
  previousError = error;
  return constrain(previousAngle + correction, -90, 90);
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






//  unsigned long currentTime = millis();
//  if (currentTime >= 2000 && currentTime <= 3600) wheelchair.setMotorSpeed(2,2);
//  if (currentTime >= 3600 && currentTime <= 5600) wheelchair.setMotorSpeed(5, 5);
//  if (currentTime >= 5600 && currentTime <= 6600) wheelchair.setMotorSpeed(6, 6);
//  if (currentTime >= 6600 && currentTime <= 10600) wheelchair.setMotorSpeed(8, 8);
//  if (currentTime >= 10600 && currentTime <= 13000) wheelchair.setMotorSpeed(14, 2);
//  if (currentTime >= 13000 && currentTime <= 15000) wheelchair.setMotorSpeed(6, 9);
//  if (currentTime >= 15000 && currentTime <= 19000) wheelchair.setMotorSpeed(8, 8);
//  if (currentTime >= 19000) wheelchair.setMotorSpeed(0, 0);
