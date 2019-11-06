#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo; 
int pos = 0;
bool play_key = false;

const char* ssid = "smartFactory";
const char* password =  "smart1234";

const uint16_t port = 8090;
const char * host = "192.168.0.2";


int opentime=0

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
    WiFiClient client;
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    //B라는 공정 명 서버로 전송
    Serial.println("Connected to server successful!");
    client.print("B");
    Serial.println("B");
    
    while(client.connected()){
      //서버에서 start메시지 기다리기
      while(!play_key){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        break;
        }
       else{
        opentime=m.toInt();
        Serial.print("opentime : ")
        Serial.print(opentime)
        }
      }
      //서버에서 메시지 기다리기
      String m=client.readStringUntil('\n');
      
      if(m=="stop"){//stop 메시지가 날라올 경우
        myservo.write(15);//15도로 서보모터를 열기
        delay(opentime);//2초간의 딜레이
        myservo.write(0);//0도로 닫기
        delay(500);//0.5초간의 딜레이
        client.print("go");//go메시지 전달
        }
        
      if(m=="end"){//end메시지가 전송될 경우
        play_key = false;
       }
      delay(1000);   
      }
    delay(10000);
}  
