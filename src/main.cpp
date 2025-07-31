#include <Arduino.h>
#include <TFT_eSPI.h>
#include "sensorDHT.h"
#include "ultrasonicSensor.h"

TFT_eSPI tft = TFT_eSPI();
SensorDHT sensor;
UltrasonicSensor proximity;

bool displayActive = false;

void showSleepScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_DARKGREY);
    tft.setTextSize(2);
    tft.setCursor(50, 100);
    tft.print("Hand over the device");
    tft.setCursor(70, 130);
    tft.print("closer...");
}

void showWeatherData() {
    tft.fillScreen(TFT_BLACK);
    
   
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(30, 20);
    tft.print("Weather Station");
    
    
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(1);
    tft.setCursor(200, 50);
    tft.print("Active");
    

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
       
        tft.fillRect(180, 80, 120, 25, TFT_BLACK);
        tft.fillRect(180, 120, 120, 25, TFT_BLACK);
        tft.fillRect(180, 160, 120, 25, TFT_BLACK);
        
       
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(2);
        tft.setCursor(180, 80);
        tft.print(sensor.getHumidity(), 1);
        tft.print(" %");
        
      
        tft.setTextColor(TFT_YELLOW);
        tft.setCursor(180, 120);
        tft.print(sensor.getTemperature(), 1);
        tft.print(" C");
        
      
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(180, 160);
        tft.print(proximity.getLastDistance(), 0);
        tft.print(" cm");
        
       
        sensor.printToSerial();
        proximity.printDistance();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("=== Smart Weather Station ===");
    
   
    sensor.begin();
    proximity.begin();
    
  
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
    delay(100);
    digitalWrite(22, HIGH);
    delay(100);
    
    tft.init();
    tft.setRotation(1);
    
   
    showSleepScreen();
    
    Serial.println("System ready! Approach to activate display.");
}

void loop() {
    static unsigned long lastWeatherUpdate = 0;
    

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
    
  
    if (displayActive && (millis() - lastWeatherUpdate >= 2000)) {
        updateDisplay();
        lastWeatherUpdate = millis();
    }
    
    delay(100);
}