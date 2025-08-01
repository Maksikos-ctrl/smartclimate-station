#include "wifiClient.h"

WiFiClientManager::WiFiClientManager() {
    wifiConnected = false;
    lastConnectionCheck = 0;
    lastDataSend = 0;
}

void WiFiClientManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println();
        Serial.println("✓ WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        wifiConnected = false;
        Serial.println();
        Serial.println("✗ WiFi connection failed!");
    }
}

bool WiFiClientManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void WiFiClientManager::checkConnection() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastConnectionCheck >= CONNECTION_CHECK_INTERVAL) {
        if (!isConnected()) {
            Serial.println("WiFi disconnected! Reconnecting...");
            WiFi.reconnect();
            delay(5000);
            
            if (isConnected()) {
                Serial.println("WiFi reconnected!");
                wifiConnected = true;
            } else {
                wifiConnected = false;
                Serial.println("WiFi reconnection failed!");
            }
        }
        lastConnectionCheck = currentTime;
    }
}

String WiFiClientManager::createSensorJSON(float temperature, float humidity, float distance, bool personPresent) {
    DynamicJsonDocument doc(256);
    
  
    doc["timestamp"] = millis() / 1000;
    doc["temperature"] = round(temperature * 10) / 10.0;
    doc["humidity"] = round(humidity * 10) / 10.0;  
    doc["distance"] = round(distance * 10) / 10.0;
    doc["person_present"] = personPresent;
    doc["wifi_rssi"] = WiFi.RSSI();
    
    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

bool WiFiClientManager::sendToFirebase(float temperature, float humidity, float distance, bool personPresent) {
    if (!isConnected()) {
        Serial.println("✗ WiFi not connected - cannot send data");
        return false;
    }
    
    HTTPClient http;
    WiFiClientSecure client; 
    client.setInsecure();
    
 
    String firebaseUrl = "https://" + String(FIREBASE_HOST) + String(FIREBASE_PATH) + ".json";
    
    Serial.println("🔗 Connecting to: " + firebaseUrl);
    
    http.begin(client, firebaseUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(15000); 
    
    String jsonData = createSensorJSON(temperature, humidity, distance, personPresent);
    
    Serial.println("📡 Sending to Firebase:");
    Serial.println("Host: " + String(FIREBASE_HOST));
    Serial.println("Path: " + String(FIREBASE_PATH));
    Serial.println("Full URL: " + firebaseUrl);
    Serial.println("JSON Data: " + jsonData);
    Serial.print("Data size: ");
    Serial.print(jsonData.length());
    Serial.println(" bytes");
    
   
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("🔥 Firebase Response Code: ");
        Serial.println(httpResponseCode);
        
        if (httpResponseCode == 200) {
            Serial.println("✅ Data sent to Firebase successfully!");
            Serial.println("Response: " + response);
        } else {
            Serial.println("⚠️  Firebase responded with code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        }
        
        http.end();
        return (httpResponseCode == 200);
    } else {
        Serial.print("❌ HTTP Error: ");
        Serial.println(httpResponseCode);
        Serial.println("Error: " + http.errorToString(httpResponseCode));
        http.end();
        return false;
    }
}

bool WiFiClientManager::shouldSendData() {
    return (millis() - lastDataSend >= DATA_SEND_INTERVAL);
}

void WiFiClientManager::updateLastSendTime() {
    lastDataSend = millis();
}

void WiFiClientManager::printStatus() {
    Serial.println("=== WiFi Status ===");
    Serial.print("Connected: ");
    Serial.println(isConnected() ? "YES" : "NO");
    
    if (isConnected()) {
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        Serial.print("Gateway: ");
        Serial.println(WiFi.gatewayIP());
    }
    Serial.println("==================");
}

String WiFiClientManager::getLocalIP() {
    return WiFi.localIP().toString();
}