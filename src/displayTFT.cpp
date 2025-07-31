#include "displayTFT.h"

DisplayTFT::DisplayTFT() : tft(TFT_eSPI()) {
    lastTemperature = NAN;
    lastHumidity = NAN;
    displayInitialized = false;
}

void DisplayTFT::begin() {
    Serial.println("Starting TFT initialization...");
    

    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
    delay(100);
    digitalWrite(22, HIGH);
    delay(100);
    
    tft.init();
    Serial.println("TFT init() called");
    
    tft.setRotation(1); 
    Serial.println("Rotation set to 1");
    
    tft.fillScreen(TFT_RED);
    Serial.println("Screen filled with RED - test");
    delay(1000);
    
    tft.fillScreen(TFT_BLACK);
    Serial.println("Screen filled with BLACK");
    
    delay(500); 
    
    displayInitialized = true;
    Serial.println("Display marked as initialized");
    
    drawStaticElements();
    Serial.println("Static elements drawn");
}

void DisplayTFT::drawStaticElements() {
    if (!displayInitialized) return;
    

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(50, 30);
    tft.print("Weather Station");
    
  
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    
    tft.setCursor(HUM_LABEL_X, HUM_LABEL_Y);
    tft.print("Humidity:");
    
    tft.setCursor(TEMP_LABEL_X, TEMP_LABEL_Y);
    tft.print("Temperature:");
    
    
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(280, HUM_VALUE_Y);
    tft.print("%");
    
    tft.setCursor(280, TEMP_VALUE_Y);
    tft.print("C");
}

void DisplayTFT::clearValueArea(int x, int y, int width, int height) {
    tft.fillRect(x, y, width, height, TFT_BLACK);
}

void DisplayTFT::updateHumidity(float humidity) {
    if (!displayInitialized || humidity == lastHumidity) return;
    
    clearValueArea(HUM_VALUE_X, HUM_VALUE_Y, 90, 25);
    
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(HUM_VALUE_X, HUM_VALUE_Y);
    tft.print(humidity, 1);
    
    lastHumidity = humidity;
}

void DisplayTFT::updateTemperature(float temperature) {
    if (!displayInitialized || temperature == lastTemperature) return;
    
    clearValueArea(TEMP_VALUE_X, TEMP_VALUE_Y, 70, 25);
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(TEMP_VALUE_X, TEMP_VALUE_Y);
    tft.print(temperature, 1);
    
    lastTemperature = temperature;
}

void DisplayTFT::updateValues(float temperature, float humidity) {
    
    updateHumidity(humidity);
    updateTemperature(temperature);
}

void DisplayTFT::showError(const char* errorMsg) {
    if (!displayInitialized) return;
    
    clearValueArea(10, 110, 300, 30);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 110);
    tft.print("ERROR: ");
    tft.print(errorMsg);
}

void DisplayTFT::clearScreen() {
    if (!displayInitialized) return;
    tft.fillScreen(TFT_BLACK);
}