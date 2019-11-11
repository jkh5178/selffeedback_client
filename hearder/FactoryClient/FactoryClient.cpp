#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "FactoryClient.h"


void FactoryClient::connectWiFi(const char* ssid , const char* password){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("...");
    }
 
    Serial.print("WiFi connected with IP: ");
    Serial.println(WiFi.localIP());
}



bool FactoryClient::connectedServer(const uint16_t port, const char * host, const char* moduleName, WiFiClient* client){
    if (!(*client).connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return false;
    }
    else{Serial.println("Connected to server successful!");
    (*client).print(moduleName);
    Serial.println(moduleName);
    return true;}
    
}



bool FactoryClient::readStart(WiFiClient client, bool * playKey, String* message){

    if (!(*playKey) && client.connected()){
    //서버로 부터의 start 메세지 수신 대기
    (*message) = client.readStringUntil('\n');
        if(*message=="start"){
            *playKey = true;
            return *playKey;
        }
    }
    return *playKey;
}



