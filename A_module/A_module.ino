
#include <Servo.h>
#include "FactoryClient.h"

/*
  A_module
  공정 기능 : 서보모터를 제어하여 컵디스펜서에서 컵을 떨어뜨린다.
*/


Servo pushDispensorServo; //서보모터 제어의 개체

//int rayValue=0; //적외선 센서의 값을 받는 변수
//int rayPinCheckCup = D7; // 적외선 센서의 핀번호

bool playKey = false; //공정 작동을 제어하기 위한 bool 변수

const char* ssid = "smartFactory"; // 사용할 wifi ID
const char* password =  "smart1234"; // 사용할 wifi PW
 
const uint16_t port = 8090; //서버 접속 포트번호
const char * host = "192.168.0.2"; // 서버의 IP주소

FactoryClient connectHelper;

const char* moduleName ="A";

void servoMove(){
        delay(500);
        pushDispensorServo.write(120);
        delay(1000);
        pushDispensorServo.write(0);
        delay(1000);
}

void setup()
{
  Serial.begin(115200); // Serial 모니터 값 115200 설정
  connectHelper.connectWiFi(ssid,password);
  //pinMode(rayPinCheckCup, INPUT);
  pushDispensorServo.attach(D8); //서보 모터 연결 pin번호
  pushDispensorServo.write(0); // 서보 모터의 값 0으로 초기화
}


String message="";
void loop()
{
    WiFiClient client;
    //서버 연결 안될 때
    //1초마다 '연결안됨' 메세지 전송
    if(!connectHelper.connectedServer(port,host,moduleName,&client)){
      return;
    }
    
    Serial.print("connect");
    bool playKey = false;
    while(client.connected()){
      while(!connectHelper.readStart(client,&playKey,&message)){
      }
      
      //rayValue = analogRead(rayPinCheckCup);
      //Serial.println(rayValue);

      //적외선 값이 100보다 클때(즉 물체인식을 하지 못할 때) 서보모터 제어
      if(digitalRead(rayPinCheckCup)==1){
        //start servo
        servoMove();
     }
     delay(6000);

        delay(500);
        pushDispensorServo.write(120);
        delay(1000);
        pushDispensorServo.write(0);
        delay(1000);
     }
     delay(4000);

      }
    //Serial.println("Disconnecting...");
    //client.stop();
}
