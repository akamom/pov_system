#include <Servo.h>

Servo myservo;
int pos;

void setup() {
  myservo.attach(9);
  myservo.write(90);
}

void loop() {
  myservo.write(30);
}
