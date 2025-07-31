#include "ultrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor() {
    lastDistance = 0;
    lastMeasurement = 0;
    personPresent = false;
    presenceStartTime = 0;
    absenceStartTime = 0;
    presenceThreshold = 100.0; 
}

void UltrasonicSensor::begin() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    Serial.println("HC-SR04 Ultrasonic sensor initialized");
    Serial.print("Presence threshold: ");
    Serial.print(presenceThreshold);
    Serial.println(" cm");
}

float UltrasonicSensor::measureDistance() {
    unsigned long currentTime = millis();
    
   
    if (currentTime - lastMeasurement < MEASUREMENT_INTERVAL) {
        return lastDistance;
    }
    
   
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
   
    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
    
    if (duration == 0) {
       
        lastDistance = 999.0;
    } else {
       
        lastDistance = (duration * 0.034) / 2;
    }
    
    lastMeasurement = currentTime;
    return lastDistance;
}

bool UltrasonicSensor::checkPresence() {
    float distance = measureDistance();
    unsigned long currentTime = millis();
    bool statusChanged = false;
    
    if (distance <= presenceThreshold && distance > 0) {
        
        if (!personPresent) {
            if (presenceStartTime == 0) {
                presenceStartTime = currentTime;
            } else if (currentTime - presenceStartTime >= PRESENCE_DELAY) {
                personPresent = true;
                presenceStartTime = 0;
                absenceStartTime = 0;
                statusChanged = true;
                Serial.println("Person DETECTED - Display ON");
            }
        }
    } else {
        
        if (personPresent) {
            if (absenceStartTime == 0) {
                absenceStartTime = currentTime;
            } else if (currentTime - absenceStartTime >= ABSENCE_DELAY) {
                personPresent = false;
                absenceStartTime = 0;
                presenceStartTime = 0;
                statusChanged = true;
                Serial.println("Person LEFT - Display OFF");
            }
        } else {
            presenceStartTime = 0; 
        }
    }
    
    return statusChanged;
}

bool UltrasonicSensor::isPersonPresent() {
    return personPresent;
}

float UltrasonicSensor::getLastDistance() {
    return lastDistance;
}

void UltrasonicSensor::printDistance() {
    Serial.print("Distance: ");
    Serial.print(lastDistance);
    Serial.print(" cm, Person present: ");
    Serial.println(personPresent ? "YES" : "NO");
}

void UltrasonicSensor::setPresenceThreshold(float threshold) {
    presenceThreshold = threshold;
    Serial.print("Presence threshold updated to: ");
    Serial.print(threshold);
    Serial.println(" cm");
}