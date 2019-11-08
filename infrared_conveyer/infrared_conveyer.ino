/*
 * 컨베이어 벨트
 * 작업 : 컨베이어 벨트에 연결된 적외선 센서를 통해 
 *       컵의 유무를 판단하여 컨베이어 정지 및 메시지 전달
*/
#include <ESP8266WiFi.h>
#include <Servo.h>

Servo belt_servo;

//센서 값을 저장할 변수
int ray_Value=0;
int ray_Value2=0;
//센서의 값을 받을 핀 번호
int ray_Pin = D6;
int ray_Pin2 = D7;
//받은 메시지의 갯수 확인
//B,C모듈 모두 메시지를 받았는지 확인을 위함
int key=0;
//전체 공정 시작 유무 확인
bool play_key = false;

//서버 및 wifi연결 정보
const char* ssid = "smartFactory";
const char* password =  "smart1234";
 
const uint16_t port = 8090;
const char * host = "192.168.0.2";

//wifi연결 함수
void connectWiFi(const char* ssid,const char* password){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  }
//B,C공정중 하나에 메시지를 전송 했을 때, 응답이 올때 까지 기다림
//wait_message보낼곳, 받을 메시지, client정보
  void wait_message(String send_to, String rcv_data,WiFiClient client){
    //stop
        belt_servo.write(90);
        client.print(send_to+";stop");
        bool key=true;
        while(key){
        String m=client.readStringUntil('\n');//5초
        if(m==rcv_data){
            return ;
          }
       }
    }

//초기화 작업
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
    //공정 초기화 모터 정지, 공정 시작, go메시지 받은 유무 확인 변수 초기화
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
      //B공정 아래 적외선의 센서 값 받기(1:물체가 없음, 0:물체가 있음)
      ray_Value = digitalRead(ray_Pin);
      //C공정 앞의 적외선의 센서 값 받기
      ray_Value2 = digitalRead(ray_Pin2);
      
      Serial.print(ray_Value);
      Serial.print(", ");
      Serial.println(ray_Value2);

      //두개의 값의 합이 2보다 작을 경우 센싱이 된것으로 인식
      if(ray_Value2+ray_Value<2){
        //두개의 센서에 모두 걸렸을 경우
        if(ray_Value2+ray_Value==0){
          belt_servo.write(90);
          //C,B 클라이언트에 모두 전송
          client.print("C;stop");
          client.print("B;stop");
          key=0;//B,C 모두 메시지를 받았는지 확인 하기 위한 변수 초기화
          while(key<2){//두개 모두 받을때 까지
            String m=client.readStringUntil('\n');//5초
            if(m=="go" || m=="go2"){
              key++;//키 값 증가
            }
          belt_servo.write(180);//모터 가동
          break;
            }
          }
         else if(ray_Value2==0){//C에서만 센싱 된경우
           wait_message("C","go2",client);
           delay(500);
           belt_servo.write(180);
          }else if(ray_Value==0){//B에서만 센싱된 경우
            wait_message("B","go",client);          
           belt_servo.write(180);
           delay(500);
          }          
        }
      delay(100);    
     }
}
