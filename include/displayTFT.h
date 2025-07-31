#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

class DisplayTFT {
private:
    TFT_eSPI tft;
    float lastTemperature;
    float lastHumidity;
    bool displayInitialized;

   
    static const int TEMP_LABEL_X = 20;
    static const int TEMP_LABEL_Y = 80;
    static const int TEMP_VALUE_X = 140;
    static const int TEMP_VALUE_Y = 80;
    
    static const int HUM_LABEL_X = 20;
    static const int HUM_LABEL_Y = 140;
    static const int HUM_VALUE_X = 140;
    static const int HUM_VALUE_Y = 140;

    void clearValueArea(int x, int y, int width = 80, int height = 25);

public:
    DisplayTFT();
    void begin();
    void drawStaticElements();
    void updateTemperature(float temperature);
    void updateHumidity(float humidity);
    void updateValues(float temperature, float humidity);
    void showError(const char* errorMsg);
    void clearScreen();
};

#endif