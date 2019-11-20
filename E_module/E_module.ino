#include "FactoryClient.h"

const char* modulName = "E";

int rayPinCheckCup = D6;
int rayPinCheckProduct = D7;


FactoryClient connectHelper;

WiFiClient client;

const char* SSID = "smartFactory";
const char* PASSWORD =  "smart1234";

const uint16_t PORT = 8090;
const char * HOST = "192.168.0.2";

void setup() {
  Serial.begin(115200);
  connectHelper.connectWiFi(SSID,PASSWORD);
  pinMode(rayPinCheckCup,INPUT);
  pinMode(rayPinCheckProduct,INPUT);
}

void loop() {
  if (!connectHelper.connectedServer(PORT,HOST,modulName,&client)) {
    Serial.println("!!!!!!!!!!!!!!!!");
        return;
    }
    
  while(client.connected()){
    int remaincup=digitalRead(rayPinCheckCup);
    int remainproduct=digitalRead(rayPinCheckProduct);
    if(remaincup+remainproduct==2){
      client.print("A,B");
      }
    else 
    {
      if( digitalRead(rayPinCheckCup)==1){
        Serial.println("A err");
        client.print("A");
        }
      if (digitalRead(rayPinCheckProduct)==1){
         Serial.println("B err");
         client.print("B");
         }
    }
      delay(3000);
  }
}
