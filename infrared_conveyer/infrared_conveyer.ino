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
    
    while(client.connected()){
      while(!play_key){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        break;
        }
      }
      ray_Value = analogRead(ray_Pin);
      belt_servo.write(180);
      Serial.println(ray_Value);
      
      if(ray_Value<=100){
        //stop
        belt_servo.write(90);
        client.print("stop");
        key=true;
        while(key){
        String m=client.readStringUntil('\n');
        if(m=="go"){
          belt_servo.write(180);
          delay(500);
          key=false;
          
          }
        }}
      delay(80);    
      }
    //Serial.println("Disconnecting...");
    //client.stop();
    delay(10000);
}
