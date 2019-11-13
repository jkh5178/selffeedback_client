/*
 * C 모듈 : 무게 측정 센서
 * 작업 : 무게를 측정하여 서버로 무게를 전송
*/
#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#include "FactoryClient.h"
#include <Servo.h>
//서버 모터, 무게센서  클래스 생성
Servo myservo; 
HX711 scale;
WiFiClient client;
FactoryClient connectHelper;
const char*  modulName="C";
// float를 string으로 바꾸기 위한 배열

//wifi, server로 연결하기 위한 정보
const char* SSID = "smartFactory";
const char* PASSWORD =  "smart1234";
 
const uint16_t PORT = 8090;
const char * HOST = "192.168.0.2";
//무게 센서의 핀 번호
const int LOADCELL_DOUT_PIN = D7;
const int LOADCELL_SCK_PIN = D6;

//목표무게와 판별 범위를 위한 변수
int weight=0;
float range=0.0;


//시스템 시작점
void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-2875);   //스케일 지정  무게측정의 값 조정
  myservo.attach(D8);//D8번 핀으로 서보모터 조작
  myservo.write(90);
  scale.tare();  //스케일 설정 0점 조절
  connectHelper.connectWiFi(SSID,PASSWORD);
  Serial.println("HX711 scale TEST");  

}

String message="";
void loop() {
  //서버 연결을 위한 클래스

   //서버와 연결 확인
    if (!connectHelper.connectedServer(PORT,HOST,modulName,&client)) {
        return;
    }
    //서버와 연결후의 작업
    //B라는 공정 명 서버로 전송
    //공정값 초기화
    Serial.println("con!!!!!!!!!!!!!!!!!!");
    bool play_key = false;
    int getvaluecheck=0;
    //서버 연결 확인 
    //서버와 연결이 되어있을 때만 작동
    while(client.connected()){
      //서버에서 start메시지 대기
      
      while(!connectHelper.readStart(client,&play_key,&message)){
      //서버에서 초기값 가져오기
      
      Serial.println(message);
      switch(getvaluecheck){
        //무게값 받아오기
          case 0:
            weight=message.toInt();
            break;
        //판별 범위 받아오기
          case 1:
            range=0.01*message.toInt();
            break;
          }
          getvaluecheck++;
      }
      //서버에서 오는 메시지 대기
       message = client.readStringUntil('\n');
       Serial.print(message);
       //컨베이어에서 오는 stop메시지
       if(message=="stop"){
            //컵이 무게센서에 대기후
           delay(1000);
           //무게 측정 10개의 측정후 평균 계산
           float value=abs(scale.get_units(10));
           delay(1000);
           
           Serial.println(value, 1);
           //float의 값을 string으로 변환
           char sendtemp[10];
           dtostrf(value,6,3,sendtemp);
           //server로 value 전달
           client.print(sendtemp);

           //범위 안에 센서값이 있는지 측정
           if( value>=(weight-weight*range) && value<=(weight+weight*range)){
            //있을 경우 왼쪽으로 회전/양품 
            //myservo.write(180);
              /**/for (int i=90;i<=180;i++){
                  myservo.write(i);
                  delay(1);
                }
                /**/
            }
            else{
              //myservo.write(0);
              //범위 밖일 경우 오른쪽으로 회전/불량품
               /**/for (int i=90;i>=0;i--){
                myservo.write(i);
                delay(1);
                }/**/
            }
        delay(500);//0.5초간의 딜레이
        //분류를 한후 원위치로 조정
        myservo.write(90);
        //컨베이어로 끝났다는 메시지 전송
        client.print("go");//go메시지 전달
        }
      delay(1500);
      }
}
