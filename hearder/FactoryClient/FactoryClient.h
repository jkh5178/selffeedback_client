#ifndef FactoryClient_h
#define FactoryClient_h
    #include "Arduino.h"
    #include <ESP8266WiFi.h>

    class FactoryClient{
        public:
            void connectWiFi(const char* ssid , const char* password);
            bool connectedServer(const uint16_t port, const char * host, const char* moduleName, WiFiClient* client);
            bool readStart(WiFiClient client, bool * palyKey, String* message);
    };
#endif