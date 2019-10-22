/* 
  IR sensor - WPS-SMVN 
  Detector distance : 1.5[m]
  Output : NPN Current
*/ 
#include <Servo.h>
// #1. 초기 변수(Parameter) 설정
int sensorPin = A0;   // IR Sensor PIN Number
int sensorValue = 0;  // Sensor Value Parameter
Servo myservo;

// #2. Setup
void setup() {
  Serial.begin(9600);        // Computer Serial(시리얼통신을 통해 컴퓨터로 모니터링함)
  pinMode(sensorPin, INPUT); // 적외선 센서의 출력 값을 계속 읽겠다.(입력모드)
  myservo.attach(7);
}
int k=1023;
// #3. Loop문
void loop() {
  
  // IR Sensor READ
  sensorValue = analogRead(sensorPin); // 적외선 빔 센서가 물체를 계속 감지합니다. 
  
   Serial.print(sensorValue);
   Serial.print(", ");
   Serial.print(k);
   Serial.print("\n");

    if(sensorValue <= 100 ){
      myservo.write(90);
      
    }
    else{
      k=sensorValue;
      myservo.write(180);
    }
  
  // IR Sensor READ AD -> Print(Serial monitor) 모니터에 아날로그 값을 보내줍니다. 


  // Dark ON(Object ON) : about 0[V] // Dark Off(Object Off) : about 5[V] // So, 0~5[V] == 0~1023(ADC Value) 
  // if Dark Off(Object Off) 5[V] => ADC Value : 1023, So about (sensorValue <= 800)
  
  // 1023보다 작을 시에 물체가 있다고 감지합니다. 적절하게 800보다 작을 경우에 물체로 인지하도록 합니다. 
//물체가 감지됬을 경우 0에 가까운 값이 나옵니다. 

  delay(120); // System delay
}
