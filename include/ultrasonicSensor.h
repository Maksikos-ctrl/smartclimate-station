#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

// HC-SR04 пины
#define TRIG_PIN 15
#define ECHO_PIN 2

class UltrasonicSensor {
private:
    float lastDistance;
    unsigned long lastMeasurement;
    float presenceThreshold;     // Убрали static const
    bool personPresent;
    unsigned long presenceStartTime;
    unsigned long absenceStartTime;
    
    static const unsigned long MEASUREMENT_INTERVAL = 200; // Измерение каждые 200ms
    static const unsigned long PRESENCE_DELAY = 1000;  // 1 сек для активации
    static const unsigned long ABSENCE_DELAY = 5000;   // 5 сек для деактивации

public:
    UltrasonicSensor();
    void begin();
    float measureDistance();
    bool isPersonPresent();
    bool checkPresence(); // Возвращает true если статус изменился
    float getLastDistance();
    void printDistance();
    void setPresenceThreshold(float threshold);
};

#endif