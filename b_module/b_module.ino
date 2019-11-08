#include <ESP8266WiFi.h>
#include <Servo.h>
/*
  B_module
  공정 기능 : 
*/
Servo myservo; // 서보모터 제어의 개체

bool play_key = false; //공정 작동을 제어하기 위한 bool 변수
bool getvaluecheck=false; // 서버로부터 맞는 값이 들어왔는지 확인하기 위한 bool 변수

const char* ssid = "smartFactory"; // 사용할 wifi ID
const char* password =  "smart1234"; // 사용할 wifi PW
 
const uint16_t port = 8090; // 서버 접속 포트번호
const char * host = "192.168.0.2"; // 서버의 IP주소

int opentime=0; // 서보모터 제어시간 0으로 초기화

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

void setup() {
  myservo.attach(D8); //서보모터 연결 pin번호
  Serial.begin(115200); // Serial 모니터 값 115200 설정
  connectWiFi(ssid,password); // wemos 보드 wifi 접속
  myservo.write(0); // 초기 서보모터 값 0으로 설정
}

void loop() {
    WiFiClient client;
    myservo.write(0); 
    //서버 연결 안될 때
    //1초마다 '연결안됨' 메세지 전송
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    //서버 연결시 공정 이름(B) 전송
    Serial.println("Connected to server successful!");
    client.print("B");
    Serial.println("B");
    
    while(client.connected()){
      //서버로 부터의 start 메세지 수신 대기
      while(!play_key && client.connected()){
      String m = client.readStringUntil('\n');
      if(m=="start"){
        play_key = true;
        break;
        }
        
       else if(!getvaluecheck){
        opentime=m.toInt(); // 서버로부터의 모터 제어시간 int 형으로 받아오기
        Serial.print("opentime : ");
        Serial.println(opentime); // 알맞는 값이 들어왔는지 확인하기 위한 출력
        getvaluecheck=true; //값을 받아왔을 때 bool 변수 true로 변경
        }
      }
      //서버로부터 메세지 수신대기
      String m=client.readStringUntil('\n');
      if(m=="stop"){//stop 메시지가 날라올 경우
        myservo.write(15);//15도로 서보모터를 열기
        delay(opentime);//2초간의 딜레이
        myservo.write(0);//0도로 닫기
        delay(500);//0.5초간의 딜레이
        client.print("go");//go메시지 전달
        }
        
      if(m=="end"){//end메시지가 전송될 경우
        play_key = false; //공정 제어 변수 false로 초기화
       }
      delay(1000);   // 1초간 딜레이
      }
    delay(10000);
}  
