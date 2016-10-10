#include <Smartcar.h>

Odometer encoderLeft(570), encoderRight(570);

void setup() {
  Serial.begin(9600);
  encoderRight.attach(2,12, HIGH);
  encoderLeft.attach(3,13, LOW);
}

void loop() {
  Serial.print("Left: ");
  Serial.print(encoderLeft.getRelativeDistance());
  Serial.print("\t");
  Serial.print("Right: ");
  Serial.println(encoderRight.getRelativeDistance());
  delay(100);
}
