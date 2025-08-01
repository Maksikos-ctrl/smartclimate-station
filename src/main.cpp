#include <Arduino.h>
#include <TFT_eSPI.h>
#include "sensorDHT.h"
#include "ultrasonicSensor.h"
#include "wifiClient.h"

TFT_eSPI tft = TFT_eSPI();
SensorDHT sensor;
UltrasonicSensor proximity;
WiFiClientManager wifiClient;

bool displayActive = false;

void showSleepScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_DARKGREY);
    tft.setTextSize(2);
    tft.setCursor(50, 100);
    tft.print("Подойдите");
    tft.setCursor(70, 130);
    tft.print("ближе...");
    
    // Показываем статус WiFi
    tft.setTextSize(1);
    tft.setCursor(10, 200);
    if (wifiClient.isConnected()) {
        tft.setTextColor(TFT_GREEN);
        tft.print("WiFi: Connected");
        tft.setCursor(10, 220);
        tft.print(wifiClient.getLocalIP());
    } else {
        tft.setTextColor(TFT_RED);
        tft.print("WiFi: Disconnected");
    }
}

void showWeatherData() {
    tft.fillScreen(TFT_BLACK);
    
    // Заголовок
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(30, 20);
    tft.print("Weather Station");
    
    // Статус WiFi
    tft.setTextSize(1);
    tft.setCursor(200, 50);
    if (wifiClient.isConnected()) {
        tft.setTextColor(TFT_GREEN);
        tft.print("Online");
    } else {
        tft.setTextColor(TFT_RED);
        tft.print("Offline");
    }
    
    // Метки
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(20, 80);
    tft.print("Humidity:");
    
    tft.setCursor(20, 120);
    tft.print("Temperature:");
    
    tft.setCursor(20, 160);
    tft.print("Distance:");
}

void updateDisplay() {
    if (!displayActive) return;
    
    if (sensor.readSensor() && sensor.isDataValid()) {
        // Очищаем области для новых значений
        tft.fillRect(180, 80, 120, 25, TFT_BLACK);
        tft.fillRect(180, 120, 120, 25, TFT_BLACK);
        tft.fillRect(180, 160, 120, 25, TFT_BLACK);
        
        // Влажность (зеленым)
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(2);
        tft.setCursor(180, 80);
        tft.print(sensor.getHumidity(), 1);
        tft.print(" %");
        
        // Температура (желтым)
        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(180, 120);
        tft.print(sensor.getTemperature(), 1);
        tft.print(" C");
        
        // Расстояние (белым)
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(180, 160);
        tft.print(proximity.getLastDistance(), 0);
        tft.print(" cm");
        
        // Вывод в консоль
        sensor.printToSerial();
        proximity.printDistance();
    }
}

void simpleFirebaseTest() {
    Serial.println("\n=== Simple Firebase Test ===");
    
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    // Тест 1: Проверка корневого URL
    String testUrl = "https://smartclimate-station-54618-default-rtdb.europe-west1.firebasedatabase.app/.json";
    
    Serial.println("Test URL: " + testUrl);
    http.begin(client, testUrl);
    
    int code = http.GET();
    Serial.print("GET Response code: ");
    Serial.println(code);
    
    if (code > 0) {
        String response = http.getString();
        Serial.println("Response: " + response.substring(0, 100) + "...");
    }
    http.end();
    
    delay(1000);
    
    // Тест 2: PUT простых данных
    String putUrl = "https://smartclimate-station-54618-default-rtdb.europe-west1.firebasedatabase.app/test.json";
    Serial.println("\nPUT to: " + putUrl);
    
    http.begin(client, putUrl);
    http.addHeader("Content-Type", "application/json");
    
    String data = "{\"message\":\"Hello from ESP32\",\"time\":" + String(millis()) + "}";
    Serial.println("Sending: " + data);
    
    code = http.PUT(data);
    Serial.print("PUT Response code: ");
    Serial.println(code);
    
    if (code > 0) {
        String response = http.getString();
        Serial.println("Response: " + response);
    }
    
    http.end();
    Serial.println("=== Test Complete ===\n");
}

void setup() {
    Serial.println("\n=== Testing Ultrasonic Sensor ===");
    for(int i = 0; i < 5; i++) {
        float dist = proximity.measureDistance();
        Serial.print("Test measurement ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(dist);
        Serial.println(" cm");
        delay(500);
    }
    Serial.println("=== Test Complete ===\n");
    Serial.begin(115200);
    Serial.println("=== Smart Weather Station with WiFi ===");
    
    // Инициализация компонентов
    sensor.begin();
    proximity.begin();
    
    // TFT инициализация
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
    delay(100);
    digitalWrite(22, HIGH);
    delay(100);
    
    tft.init();
    tft.setRotation(1);
    
    // WiFi инициализация
    Serial.println("🌐 Starting WiFi connection...");
    wifiClient.begin();

    simpleFirebaseTest();
    
    // Начинаем с экрана сна
    showSleepScreen();
    
    Serial.println("✅ System ready! Approach to activate display.");
}





void loop() {
    static unsigned long lastWeatherUpdate = 0;
    
    // Проверяем WiFi соединение
    wifiClient.checkConnection();
    
    // Проверяем присутствие
    if (proximity.checkPresence()) {
        if (proximity.isPersonPresent()) {
            displayActive = true;
            showWeatherData();
            Serial.println("Display activated!");
        } else {
            displayActive = false;
            showSleepScreen();
            Serial.println("Display deactivated!");
        }
    }
    
    // Обновляем дисплей
    if (displayActive && (millis() - lastWeatherUpdate >= 2000)) {
        updateDisplay();
        lastWeatherUpdate = millis();
    }
    
    // Отправляем данные каждые 30 секунд
    if (wifiClient.shouldSendData()) {
        if (sensor.readSensor() && sensor.isDataValid()) {
            proximity.measureDistance();
            
            bool success = wifiClient.sendToFirebase(
                sensor.getTemperature(),
                sensor.getHumidity(), 
                proximity.getLastDistance(),
                proximity.isPersonPresent()
            );
            
            if (success) {
                Serial.println("📤 Data prepared for Firebase");
            }
            
            wifiClient.updateLastSendTime();
        }
    }
    
    delay(100);
}