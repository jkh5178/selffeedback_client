/*
 * C 모듈 : 무게 측정 센서
 * 작업 : 무게를 측정하여 서버로 무게를 전송
*/
#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#include <ESP8266WiFi.h>
#include <Servo.h>
//서버 모터, 무게센서  클래스 생성
Servo myservo; 
HX711 scale;
bool play_key = false;//서버에서 공정 시작 메시지를 받았나
int getvaluecheck=0;//정지 되어 있을때 받은 메시지의 종류
char sendtemp[10];// float를 string으로 바꾸기 위한 배열

//wifi, server로 연결하기 위한 정보
const char* ssid = "smartFactory";
const char* password =  "smart1234";
 
const uint16_t port = 8090;
const char * host = "192.168.0.2";
//무게 센서의 핀 번호
const int LOADCELL_DOUT_PIN = D7;
const int LOADCELL_SCK_PIN = D6;

//목표무게와 판별 범위를 위한 변수
int weight=0;
float range=0.0;

//wifi연결을 위한 함수
void connectWiFi(const char* ssid,const char* password){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  }

//시스템 시작점
void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-2875);   //스케일 지정  무게측정의 값 조정
  myservo.attach(D8);//D8번 핀으로 서보모터 조작
  myservo.write(90);
  scale.tare();  //스케일 설정 0점 조절
  connectWiFi(ssid,password);
  Serial.println("HX711 scale TEST");  

}


void loop() {
  //서버 연결을 위한 클래스
   WiFiClient client;
   //서버와 연결 확인
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    //서버와 연결후의 작업
    //B라는 공정 명 서버로 전송
    //공정값 초기화
    play_key = false;
    getvaluecheck=0;
    //서버 연결 확인 
    Serial.println("Connected to server successful!");
    //공정의 이름 전송
    client.print("C");
    Serial.println("C");
    //서버와 연결이 되어있을 때만 작동
    while(client.connected()){
      //서버에서 start메시지 대기
      while(!play_key && client.connected()){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        break;
        }
      //서버에서 초기값 가져오기
      switch(getvaluecheck){
        //무게값 받아오기
          case 0:
            weight=m.toInt();
            Serial.print("w : ");
            Serial.println(weight);
            break;
        //판별 범위 받아오기
          case 1:
            range=0.01*m.toInt();
            Serial.print("r : ");
            Serial.println(range);
            break;
          }
          getvaluecheck++;
      }
      //서버에서 오는 메시지 대기
       String m = client.readStringUntil('\n');
       Serial.print(m);
       //컨베이어에서 오는 stop메시지
       if(m=="stop"){
            //컵이 무게센서에 대기후
           delay(1000);
           //무게 측정 10개의 측정후 평균 계산
           float value=abs(scale.get_units(10));
           delay(1000);
           
           Serial.println(value, 1);
           //float의 값을 string으로 변환
           dtostrf(value,6,3,sendtemp);
           //server로 value 전달
           client.print(sendtemp);
           //
           Serial.print(weight-weight*range);
           Serial.print(", ");
           Serial.println(weight+weight*range);
           //범위 안에 센서값이 있는지 측정
           if( value>=(weight-weight*range) && value<=(weight+weight*range)){
            //있을 경우 왼쪽으로 회전/양품 
              for (int i=90;i<=180;i++){
                  myservo.write(i);
                  delay(3);
                }
            }
            else{
              //범위 밖일 경우 오른쪽으로 회전/불량품
               for (int i=90;i>=0;i--){
                myservo.write(i);
                delay(3);
                }
            }
        delay(500);//0.5초간의 딜레이
        //분류를 한후 원위치로 조정
        myservo.write(90);
        //컨베이어로 끝났다는 메시지 전송
        client.print("go2");//go메시지 전달
        }
      delay(1500);
      }
}
