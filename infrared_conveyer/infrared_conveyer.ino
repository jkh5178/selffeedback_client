#include <ESP8266WiFi.h>
#include <Servo.h>

Servo belt_servo;

int ray_Value=0;
int ray_Pin = A0;

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

    //서버 연결시 공정 이름 전송
    Serial.println("Connected to server successful!");
    client.print("D");
    Serial.println("D");
    //서버랑 연결되어 있을시
    while(client.connected()){
      //서버에서 start명령 기다리기
      while(!play_key){
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
      
      if(ray_Value<=100){//측정 센서 값이 100이하이면 물체가 있다고 판별
        belt_servo.write(90);//stop
        client.print("stop");//서버에게 벨트 정지 전송
        }
      String m=client.readStringUntil('\n');// 메시지 수신
      
      if(m=="go"){//서버에서 go 라는 명령이 올시 
          belt_servo.write(180);//벨트를 움직이게하고
          delay(500);//0.5초의 딜레이를 갖는다.
          }
      if(m=="end"){//서버에서 end 라는 명령이 올시
        belt_servo.write(90);//stop
        play_key=false;//start명령이 기다릴수 있도록 조건 변경
        }
      delay(100);    
      }
    delay(10000);
}
