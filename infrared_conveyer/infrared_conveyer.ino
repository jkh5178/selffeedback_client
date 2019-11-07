#include <ESP8266WiFi.h>
#include <Servo.h>

Servo belt_servo;

int ray_Value=0;
int ray_Value2=0;
int ray_Pin = D6;
int ray_Pin2 = D7;


int key=0;
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

  void wait_message(String send_to, String rcv_data,WiFiClient client){
    //stop
        belt_servo.write(90);
        client.print(send_to+";stop");
        bool key=true;
        while(key){
        String m=client.readStringUntil('\n');//5초
        if(m==rcv_data){
          belt_servo.write(180);
          key=false;
          delay(500);
          }
       }
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
      ray_Value = digitalRead(ray_Pin);
      //시리얼에 센서값 전송
      //컨베이어 벨트 마지막
      ray_Value2 = digitalRead(ray_Pin2);
      Serial.print(ray_Value);
      Serial.print(", ");
      Serial.println(ray_Value2);
      if(ray_Value2+ray_Value<2){
        if(ray_Value2+ray_Value==0){
          belt_servo.write(90);
          client.print("C;stop");
          client.print("B;stop");
          key=0;
          while(key<2){
            String m=client.readStringUntil('\n');//5초
            if(m=="go1" || m=="go2"){
              key++;
            }
          belt_servo.write(180);
          break;
            }
          }
         else if(ray_Value2==0){
           wait_message("C","go2",client);
          }else if(ray_Value==0){
            wait_message("B","go",client);
          }          
        }
      delay(100);    
     }
}
