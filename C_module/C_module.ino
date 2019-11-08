#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo; 

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
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-2875);   //스케일 지정 
  myservo.attach(D8);
  myservo.write(90);
  scale.tare();  //스케일 설정
  connectWiFi(ssid,password);
  Serial.println("HX711 scale TEST");  

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
       String m = client.readStringUntil('\n');
       Serial.print(m);
       if(m=="stop"){
           delay(1000);
           float value=abs(scale.get_units(10));
           delay(1000);
           Serial.println(value, 1);
           dtostrf(value,6,3,sendtemp);
           client.print(sendtemp);
           /*무게 판별과 서보모터 움직*/
           Serial.print(weight-weight*range);
           Serial.print(", ");
           Serial.println(weight+weight*range);
           if( value>=(weight-weight*range) && value<=(weight+weight*range)){
                  for (int i=90;i<=180;i++){
            myservo.write(i);
            delay(3);
                }
            }
            else{
                    for (int i=90;i>=0;i--){
            myservo.write(i);
            delay(3);
        }
              }
        delay(500);//0.5초간의 딜레이
        myservo.write(90);
        client.print("go2");//go메시지 전달
        }
      
      //무게 측정 해서 전송

      delay(1500);
      }
}
