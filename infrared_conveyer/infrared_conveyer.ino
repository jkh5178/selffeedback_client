/*
 * 컨베이어 벨트
 * 작업 : 컨베이어 벨트에 연결된 적외선 센서를 통해 
 *       컵의 유무를 판단하여 컨베이어 정지 및 메시지 전달
*/
#include "FactoryClient.h"
#include <Servo.h>
Servo beltServo;
FactoryClient connectHelper;

const char* modulName="D";
//센서 값을 저장할 변수
int  RAY_VALUE_PRODUCT_DISPENSOR=0;
int RAY_VALUE_WEIGHT_SENSOR=0;
//센서의 값을 받을 핀 번호
int RAY_PIN_PRODUCT_DISPENSOR = D6;
int RAY_PIN_WEIGHT_SENSOR = D7;
//받은 메시지의 갯수 확인
//B,C모듈 모두 메시지를 받았는지 확인을 위함
int KEY=0;
//전체 공정 시작 유무 확인
bool PALY_KEY = false;

WiFiClient client;

//서버 및 wifi연결 정보
const char* SSID = "smartFactory";
const char* PASSWORD =  "smart1234";
 
const uint16_t PORT = 8090;
const char * HOST = "192.168.0.2";

//B,C공정중 하나에 메시지를 전송 했을 때, 응답이 올때 까지 기다림
//wait_message보낼곳, 받을 메시지, client정보
  void waitMessage(String send_to){
    //stop
        beltServo.write(90);
        client.print(send_to+";stop");
        bool key=true;
        while(key){
        String message=client.readStringUntil('\n');//5초
        if(message=="go"){
            return ;
          }
       }
    }
//초기화 작업
void setup()
{
  Serial.begin(115200);
  connectHelper.connectWiFi(SSID,PASSWORD);
  pinMode(RAY_PIN_PRODUCT_DISPENSOR, INPUT);
  pinMode(RAY_PIN_WEIGHT_SENSOR, INPUT);//컨베이어 벨트 마지막
  beltServo.attach(D8);
}
String message="";
void loop()
{
    
    if (!connectHelper.connectedServer(PORT,HOST,modulName,&client)) {
        return;
    }
    Serial.println("con!!!!!!!!!!!!!!!!!!");
    //공정 초기화 모터 정지, 공정 시작, go메시지 받은 유무 확인 변수 초기화
    beltServo.write(90);
    PALY_KEY=false;
    //서버랑 연결되어 있을시
    while(client.connected()){
      //서버에서 start명령 기다리기
      Serial.print(PALY_KEY);
      Serial.println("1!");
      while(!connectHelper.readStart(client,&PALY_KEY,&message)){delay(1000);}
      beltServo.write(180);
      //B공정 아래 적외선의 센서 값 받기(1:물체가 없음, 0:물체가 있음)
      RAY_VALUE_PRODUCT_DISPENSOR = digitalRead(RAY_PIN_PRODUCT_DISPENSOR);
      //C공정 앞의 적외선의 센서 값 받기
      RAY_VALUE_WEIGHT_SENSOR = digitalRead(RAY_PIN_WEIGHT_SENSOR);
      
      Serial.print(RAY_VALUE_PRODUCT_DISPENSOR);
      Serial.print(", ");
      Serial.println(RAY_VALUE_WEIGHT_SENSOR);

      //두개의 값의 합이 2보다 작을 경우 센싱이 된것으로 인식
      if(RAY_VALUE_WEIGHT_SENSOR+RAY_VALUE_PRODUCT_DISPENSOR<2){
        //두개의 센서에 모두 걸렸을 경우
        if(RAY_VALUE_WEIGHT_SENSOR+RAY_VALUE_PRODUCT_DISPENSOR == 0){
          beltServo.write(90);
          //C,B 클라이언트에 모두 전송
          client.print("C;stop");
          client.print("B;stop");
          KEY=0;//B,C 모두 메시지를 받았는지 확인 하기 위한 변수 초기화
          while(KEY<2){//두개 모두 받을때 까지
            message=client.readStringUntil('\n');//5초
            if(message=="go"){
              KEY++;//키 값 증가
            }
          beltServo.write(180);//모터 가동
          break;
            }
          }
         else if(RAY_VALUE_WEIGHT_SENSOR==0){//C에서만 센싱 된경우
           waitMessage("C");
           delay(500);
           beltServo.write(180);
          }else if(RAY_VALUE_PRODUCT_DISPENSOR==0){//B에서만 센싱된 경우
            waitMessage("B");          
           beltServo.write(180);
           delay(500);
          }          
        }
      delay(100);    
     }
}
