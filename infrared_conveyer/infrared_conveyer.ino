#include <ESP8266WiFi.h>
#include <Servo.h>

Servo belt_servo;

int ray_Value=0;
String ray_Value2="";
int ray_Pin = A0;
int ray_Pin2 = D2;


bool key=false;
bool play_key = false;
const char* ssid = "smartFactory";
const char* password =  "smart1234";
 
const uint16_t port = 8090;
const char * host = "192.168.0.2";
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
  Serial.begin(115200);
  connectWiFi(ssid,password);
  pinMode(ray_Pin, INPUT);
  pinMode(ray_Pin2, INPUT);//컨베이어 벨트 마지막
  belt_servo.attach(D8);
}
 
void loop()
{
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    belt_servo.write(90);
    key=false;
    play_key=false;
    //서버 연결시 공정 이름 전송
    Serial.println("Connected to server successful!");
    client.print("D");
    Serial.println("D");
    //서버랑 연결되어 있을시
    while(client.connected()){
      
      //서버에서 start명령 기다리기
      while(!play_key && client.connected()){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        belt_servo.write(180);
        break;
        } 
      }
      //센서값 받기
      ray_Value = analogRead(ray_Pin);
      Serial.println(ray_Value);//시리얼에 센서값 전송
      
      //컨베이어 벨트 마지막
      //ray_Value2 = 
      
      if(ray_Value<=100){
        //stop
        belt_servo.write(90);
        client.print("stop");
        key=true;
        while(key){
        String m=client.readStringUntil('\n');//5초
        if(m=="go"){
          belt_servo.write(180);
          key=false;
          delay(500);
          }
       }
      }
      delay(100);    
     }
}
