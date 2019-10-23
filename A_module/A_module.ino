//a 공정 기초 테스트

#include <Servo.h>

Servo myservo; 
int pos = 0;
void setup() {
  
  myservo.attach(9);
  Serial.begin(9600); // Computer Serial(시리얼통신을 통해 컴퓨터로 모니터링함)
  myservo.write(pos); //servo의 초기값 입력
}

void loop() {
  if(Serial.available())
  {
    char in_data;
    in_data = Serial.read();
    if(in_data = 's') // s 입력시  회전값 + 100
    {
      pos +=100;
    }
    else
      pos =0; // 아닐경우 0 고정
    myservo.write(pos); //회전값 입력
    delay(500); //0.5초 뒤
    if(pos ==100){  //0 으로 초기화 ( 재위치)
      pos=0;
      myservo.write(pos); 
    }
  }
}
