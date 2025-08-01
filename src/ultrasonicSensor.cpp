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
    Serial.print("Using pins - TRIG: ");
    Serial.print(TRIG_PIN);
    Serial.print(", ECHO: ");
    Serial.println(ECHO_PIN);
}

float UltrasonicSensor::measureDistance() {
    unsigned long currentTime = millis();
    
    // Минимальный интервал между измерениями
    if (currentTime - lastMeasurement < MEASUREMENT_INTERVAL) {
        return lastDistance;
    }
    
    // Генерируем ультразвуковой импульс
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Измеряем время отклика
    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    
    if (duration == 0) {
        // Нет отклика - объект слишком далеко или ошибка
        lastDistance = 999.0;
    } else {
        // Вычисляем расстояние в сантиметрах
        lastDistance = (duration * 0.034) / 2;
        
        // Фильтруем аномальные значения
        if (lastDistance > 400) {
            lastDistance = 999.0;
        }
    }
    
    lastMeasurement = currentTime;
    return lastDistance;
}

bool UltrasonicSensor::checkPresence() {
    float distance = measureDistance();
    unsigned long currentTime = millis();
    bool statusChanged = false;
    
    if (distance <= presenceThreshold && distance > 0) {
        // Объект в зоне обнаружения
        absenceStartTime = 0;  // Сбрасываем таймер отсутствия
        
        if (!personPresent) {
            if (presenceStartTime == 0) {
                presenceStartTime = currentTime;
                Serial.print("Object detected at ");
                Serial.print(distance);
                Serial.println(" cm - starting timer...");
            } else if (currentTime - presenceStartTime >= PRESENCE_DELAY) {
                personPresent = true;
                presenceStartTime = 0;
                statusChanged = true;
                Serial.println("✓ Person DETECTED - Display ON");
            }
        }
    } else {
        // Объект вне зоны обнаружения
        presenceStartTime = 0;  // Сбрасываем таймер присутствия
        
        if (personPresent) {
            if (absenceStartTime == 0) {
                absenceStartTime = currentTime;
                Serial.print("Object left (distance: ");
                Serial.print(distance);
                Serial.println(" cm) - starting timer...");
            } else if (currentTime - absenceStartTime >= ABSENCE_DELAY) {
                personPresent = false;
                absenceStartTime = 0;
                statusChanged = true;
                Serial.println("✓ Person LEFT - Display OFF");
            }
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
    if (lastDistance >= 999.0) {
        Serial.print(">400");
    } else {
        Serial.print(lastDistance, 1);
    }
    Serial.print(" cm, Person present: ");
    Serial.println(personPresent ? "YES" : "NO");
}

void UltrasonicSensor::setPresenceThreshold(float threshold) {
    presenceThreshold = threshold;
    Serial.print("Presence threshold updated to: ");
    Serial.print(threshold);
    Serial.println(" cm");
}