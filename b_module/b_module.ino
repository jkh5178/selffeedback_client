#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo; 
int pos = 0;
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

void setup() {
  // put your setup code here, to run once:
  myservo.attach(D8);
  Serial.begin(115200);
  connectWiFi(ssid,password);
  myservo.write(pos);
}

void loop() {
  // put your main code here, to run repeatedly:
    WiFiClient client;
 
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    Serial.println("Connected to server successful!");
    client.print("B");
    Serial.println("B");
   
    while(client.connected()){
      while(!play_key){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        break;
        }
      }
      String m=client.readStringUntil('\n');
      if(m=="stop"){
        myservo.write(15);
        delay(2000);
        myservo.write(0);
        delay(500);
        client.print("go");
        }
      delay(1000);   
      }
    //Serial.println("Disconnecting...");
    //client.stop();
    delay(10000);
}  
