#include <ESP8266WiFi.h>
#include <Servo.h>

Servo open_servo;

int ray_Value=0;
int ray_Pin = A0;

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
  open_servo.attach(D8);
  open_servo.write(0);
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
    client.print("A");
    Serial.println("A");
    
    while(client.connected()){
      play_key = false;
      while(!play_key && client.connected()){
        String m = client.readStringUntil('\n');
        if(m=="start"){
          play_key = true;
          break;
        }
      }
      
      //ray_Value = analogRead(ray_Pin);
      //Serial.println(ray_Value);
      if(analogRead(ray_Pin)>100){
        //start servo
        open_servo.write(120);
        delay(500);
        open_servo.write(0);
        delay(800);
     }
     delay(800);
      }
    //Serial.println("Disconnecting...");
    //client.stop();
}
