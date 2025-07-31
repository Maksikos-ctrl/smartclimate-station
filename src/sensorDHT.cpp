#include "sensorDHT.h"

SensorDHT::SensorDHT() : dht(DHTPIN, DHTTYPE) {
    lastTemperature = NAN;
    lastHumidity = NAN;
    lastReadTime = 0;
}

void SensorDHT::begin() {
    pinMode(DHTPIN, INPUT_PULLUP); 
    delay(100);
    dht.begin();
    Serial.println("DHT11 sensor initialized");
}

bool SensorDHT::readSensor() {
    unsigned long currentTime = millis();
    
   
    if (currentTime - lastReadTime < READ_INTERVAL) {
        return true; 
    }
    
    float newTemperature = dht.readTemperature();
    float newHumidity = dht.readHumidity();
    
    if (isnan(newTemperature) || isnan(newHumidity)) {
        Serial.println("Error: Failed to read from DHT11 sensor!");
        return false;
    }
    
    lastTemperature = newTemperature;
    lastHumidity = newHumidity;
    lastReadTime = currentTime;
    
    return true;
}

float SensorDHT::getTemperature() {
    return lastTemperature;
}

float SensorDHT::getHumidity() {
    return lastHumidity;
}

bool SensorDHT::isDataValid() {
    return !isnan(lastTemperature) && !isnan(lastHumidity);
}

void SensorDHT::printToSerial() {
    if (isDataValid()) {
        Serial.print("Temperature: ");
        Serial.print(lastTemperature, 1);
        Serial.print("°C, Humidity: ");
        Serial.print(lastHumidity, 1);
        Serial.println("%");
    } else {
        Serial.println("No valid sensor data available");
    }
}