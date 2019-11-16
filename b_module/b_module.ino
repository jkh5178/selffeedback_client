#include "FactoryClient.h"
#include <Servo.h>
/*
  B_module
  공정 기능 : 
*/
Servo openServo; // 서보모터 제어의 개체

bool playKey = false; //공정 작동을 제어하기 위한 bool 변수
bool getValueCheck=false; // 서버로부터 맞는 값이 들어왔는지 확인하기 위한 bool 변수

const char* SSID = "smartFactory"; // 사용할 wifi ID
const char* PASSWORD =  "smart1234"; // 사용할 wifi PW
 
const uint16_t PORT = 8090; // 서버 접속 포트번호
const char * HOST = "192.168.0.2"; // 서버의 IP주소


int openTime=0; // 서보모터 제어시간 0으로 초기화

FactoryClient connectHelper;
const char* moduleName ="B";

void setup() {
  openServo.attach(D8); //서보모터 연결 pin번호
  Serial.begin(115200); // Serial 모니터 값 115200 설정
  connectHelper.connectWiFi(SSID,PASSWORD);
  openServo.write(0); // 초기 서보모터 값 0으로 설정
}


String message="";
void loop() {
    WiFiClient client;
    openServo.write(0); 
    if(!connectHelper.connectedServer(PORT,HOST,moduleName,&client)){
      return;
    }
    bool playKey = false;

    
    while(client.connected()){
      //서버로 부터의 start 메세지 수신 대기
      while(!connectHelper.readStart(client,&playKey,&message)){
        Serial.println(message);
        if(!getValueCheck){
        openTime=message.toInt(); // 서버로부터의 모터 제어시간 int 형으로 받아오기
        Serial.print("opentime : ");
        Serial.println(openTime); // 알맞는 값이 들어왔는지 확인하기 위한 출력
        getValueCheck=true; //값을 받아왔을 때 bool 변수 true로 변경
        }
       }  
      
      
      //서버로부터 메세지 수신대기
      message=client.readStringUntil('\n');
      Serial.println(message);
      if(message=="stop"){//stop 메시지가 날라올 경우
        openServo.write(15);//15도로 서보모터를 열기
        delay(openTime);
        openServo.write(0);//0도로 닫기
        delay(500);//0.5초간의 딜레이
        client.print("go");//go메시지 전달
        }
      else if(message=="end"){//end메시지가 전송될 경우
        playKey = false; //공정 제어 변수 false로 초기화
       }
      else if (message.toInt()>=1){
         openTime=message.toInt();
        }  // 1초간 딜레이
      }
    delay(100);
}  
