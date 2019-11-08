#include <ESP8266WiFi.h>
#include <Servo.h>
/*
  A_module
  공정 기능 : 서보모터를 제어하여 컵디스펜서에서 컵을 떨어뜨린다.
*/


Servo open_servo; //서보모터 제어의 개체

int ray_Value=0; //적외선 센서의 값을 받는 변수
int ray_Pin = A0; // 적외선 센서의 핀번호


bool play_key = false; //공정 작동을 제어하기 위한 bool 변수


const char* ssid = "smartFactory"; // 사용할 wifi ID
const char* password =  "smart1234"; // 사용할 wifi PW
 
const uint16_t port = 8090; //서버 접속 포트번호
const char * host = "192.168.0.2"; // 서버의 IP주소

//wemos 보드 wifi 접속 함수
void connectWiFi(const char* ssid,const char* password){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  }

void setup()
{
  Serial.begin(115200); // Serial 모니터 값 115200 설정
  connectWiFi(ssid,password); //wemos 보드 wifi 접속 
  pinMode(ray_Pin, INPUT);
  open_servo.attach(D8); //서보 모터 연결 pin번호
  open_servo.write(0); // 서보 모터의 값 0으로 초기화
}
 
void loop()
{
    WiFiClient client;
    //서버 연결 안될 때
    //1초마다 '연결안됨' 메세지 전송
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }

    //서버 연결시 공정 이름(A) 전송 
    Serial.println("Connected to server successful!");
    client.print("A");
    Serial.println("A");
    play_key = false;
    while(client.connected()){
      
      while(!play_key && client.connected()){
        //서버로 부터의 start 메세지 수신 대기
        String m = client.readStringUntil('\n');
        if(m=="start"){
          play_key = true;
          break;
        }
      }
      
      //ray_Value = analogRead(ray_Pin);
      //Serial.println(ray_Value);
      
      //적외선 값이 100보다 클때(즉 물체인식을 하지 못할 때) 서보모터 제어
      if(analogRead(ray_Pin)>100){
        //start servo
        open_servo.write(120);
        delay(500);
        open_servo.write(0);
        delay(500);
     }
     delay(800);
      }
    //Serial.println("Disconnecting...");
    //client.stop();
}
