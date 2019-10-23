#include <Servo.h>

Servo b_servo;
int pos = 0;
int sensorPin = A0;
int sensorValue = 0;

void setup(){
  b_servo.attach(8);
  pinMode(sensorPin,INPUT);
}

int k = 1023;
void loop(){
  
}
