#include <Smartcar.h>

const unsigned short LEFT_MOTOR[] = {8, 9, 6};
const unsigned short RIGHT_MOTOR[] = {11, 10, 5};

Car wheelchair(useShieldMotors(LEFT_MOTOR, RIGHT_MOTOR));

void setup() {
  wheelchair.begin();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime >= 2000 && currentTime <= 3600) wheelchair.setMotorSpeed(2,2);
  if (currentTime >= 3600 && currentTime <= 5600) wheelchair.setMotorSpeed(5, 5);
  if (currentTime >= 5600 && currentTime <= 6600) wheelchair.setMotorSpeed(6, 6);
  if (currentTime >= 6600 && currentTime <= 10600) wheelchair.setMotorSpeed(8, 8);
  if (currentTime >= 10600 && currentTime <= 13000) wheelchair.setMotorSpeed(14, 2);
  if (currentTime >= 13000 && currentTime <= 15000) wheelchair.setMotorSpeed(6, 9);
  if (currentTime >= 15000 && currentTime <= 19000) wheelchair.setMotorSpeed(8, 8);
  if (currentTime >= 19000) wheelchair.setMotorSpeed(0, 0);
}
