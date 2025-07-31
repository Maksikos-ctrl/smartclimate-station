#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

#include <Arduino.h>
#include "DHT.h"

// Определение пинов и типа датчика
#define DHTPIN 4
#define DHTTYPE DHT11

class SensorDHT {
private:
    DHT dht;
    float lastTemperature;
    float lastHumidity;
    unsigned long lastReadTime;
    static const unsigned long READ_INTERVAL = 2000; // Минимальный интервал между чтениями (мс)

public:
    SensorDHT();
    void begin();
    bool readSensor();
    float getTemperature();
    float getHumidity();
    bool isDataValid();
    void printToSerial();
};

#endif