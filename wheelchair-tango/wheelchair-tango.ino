#include <Smartcar.h>

Odometer encoderLeft(570), encoderRight(570);
unsigned short LEFT_MOTOR[] = {8, 9, 6};
unsigned short RIGHT_MOTOR[] = {11, 10, 5};

Car wheelchair(useShieldMotors(LEFT_MOTOR, RIGHT_MOTOR));

const unsigned long PID_FREQUENCY = 20; //update the pid controllers at this frequency
unsigned long lastAngleUpdate = 0; // the last time we updated the angle
long frontDistance = 0;
long backDistance = 0;
float previousError = 0.0, integratedError = 0.0, correction = 0.0, previousControlledAngle = 0.0;
int angleSetPoint = 0; //current error in angle
/* PID values for ANGLE */
float angleP = 6.0;
float angleI = 0.0;
float angleD = 20.0;
/* PID values for SPEED */
float speedP = 1.2;
float speedI = 0.0;
float speedD = 0.0;

void setup() {
  Serial.begin(9600); //for debug
  Serial1.begin(9600);
  Serial1.setTimeout(200);
  encoderRight.attach(2, 12, HIGH); // Pulse pin, direction pin, the state of the direction pin when going forward
  encoderLeft.attach(3, 13, LOW);
  wheelchair.begin(encoderLeft, encoderRight);
  wheelchair.enableCruiseControl(speedP, speedI, speedD, PID_FREQUENCY);
}

void loop() {
  wheelchair.updateMotors();
  updateAngle(); //PID controller for the angle
  getTangoInput(); // input from the bluetooth module
  handleInput(); // debug input from serial
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

void getTangoInput() {
  if (Serial1.available()) {
    String input = Serial1.readStringUntil('*');
    Serial.print(input);
    if (input.startsWith("t")) { //angle
      angleSetPoint = - input.substring(1).toInt();
      Serial.println();
    } else if (input.startsWith("m")) { //speed
      float throttle = input.substring(1).toFloat();
      wheelchair.setSpeed(throttle);
      Serial.print("\t");
    }
  }
}

void updateAngle() {
  unsigned long currentTime = millis();
  if (currentTime >= lastAngleUpdate + PID_FREQUENCY) {
    float controlledAngle = anglePIDcontrol(previousControlledAngle, (float) angleSetPoint);
    wheelchair.setAngle(lroundf(controlledAngle));
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
