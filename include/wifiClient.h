#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config.h"

class WiFiClientManager {
private:
    bool wifiConnected;
    unsigned long lastConnectionCheck;
    unsigned long lastDataSend;
    static const unsigned long CONNECTION_CHECK_INTERVAL = 30000;
    
public:
    WiFiClientManager();
    void begin();
    bool isConnected();
    void checkConnection();
    bool sendToFirebase(float temperature, float humidity, float distance, bool personPresent);
    String createSensorJSON(float temperature, float humidity, float distance, bool personPresent);
    void printStatus();
    String getLocalIP();
    bool shouldSendData();
    void updateLastSendTime();
};

#endif