#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

#define TRIG_PIN 15
#define ECHO_PIN 2

class UltrasonicSensor {
private:
    float lastDistance;
    unsigned long lastMeasurement;
    float presenceThreshold;     
    bool personPresent;
    unsigned long presenceStartTime;
    unsigned long absenceStartTime;
    
    static const unsigned long MEASUREMENT_INTERVAL = 200; 
    static const unsigned long PRESENCE_DELAY = 1000;
    static const unsigned long ABSENCE_DELAY = 5000;  

public:
    UltrasonicSensor();
    void begin();
    float measureDistance();
    bool isPersonPresent();
    bool checkPresence(); 
    float getLastDistance();
    void printDistance();
    void setPresenceThreshold(float threshold);
};

#endif