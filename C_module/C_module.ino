#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#include <ESP8266WiFi.h>

bool play_key = false;
int getvaluecheck=0;
char sendtemp[10];

const char* ssid = "smartFactory";
const char* password =  "smart1234";
 
const uint16_t port = 8090;
const char * host = "192.168.0.2";

const int LOADCELL_DOUT_PIN = D7;
const int LOADCELL_SCK_PIN = D6;

HX711 scale;

int weight=0;
float range=0.0;

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
  Serial.begin(115200);
  connectWiFi(ssid,password);
  Serial.println("HX711 scale TEST");  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);   //스케일 지정 
  scale.tare();  //스케일 설정
}


void loop() {
   WiFiClient client;
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    //B라는 공정 명 서버로 전송
    play_key = false;
    getvaluecheck=0;
    Serial.println("Connected to server successful!");
    client.print("C");
    Serial.println("C");
    
    while(client.connected()){
     
      while(!play_key && client.connected()){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        break;
        }
        //서버에서 초기값 가져오기
      switch(getvaluecheck){
          case 0:
            weight=m.toInt();
            Serial.print("w : ");
            Serial.println(weight);
            break;
          case 1:
            range=0.01*m.toInt();
            Serial.print("r : ");
            Serial.println(range);
            break;
          }
          getvaluecheck++;
      }

      
      //무게 측정 해서 전송
      float value=abs(scale.get_units(5));
      Serial.println(value, 1);
      dtostrf(value,6,3,sendtemp);
      client.print(sendtemp);
      delay(1500);
      }
}
