#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

#include <Arduino.h>
#include "DHT.h"


#define DHTPIN 4
#define DHTTYPE DHT11

class SensorDHT {
private:
    DHT dht;
    float lastTemperature;
    float lastHumidity;
    unsigned long lastReadTime;
    static const unsigned long READ_INTERVAL = 2000; 

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