# SmartClimate Station

An intelligent weather monitoring system with proximity-based display activation using ESP32 microcontroller.

## 🌟 Features

- **Real-time environmental monitoring** - Temperature and humidity sensing
- **Proximity-based activation** - Display automatically turns on/off based on user presence
- **Color-coded TFT display** - Visual representation of sensor data
- **Energy efficient** - Smart sleep mode when no one is around
- **Modular architecture** - Easy to extend with additional sensors

## 📋 Hardware Requirements

### Components
- **ESP32-WROOM-32** - Main microcontroller
- **DHT11** - Temperature and humidity sensor
- **HC-SR04** - Ultrasonic distance sensor
- **2.4" TFT ILI9341/ST7789** - Color display (240x320)
- **Breadboard** - For prototyping connections
- **Jumper wires** - For connections

### Pin Configuration
```
DHT11:
├── VCC → 3.3V
├── GND → GND
└── DATA → GPIO4

TFT Display:
├── VCC → 3.3V
├── GND → GND
├── CS → GPIO5
├── RST → GPIO22
├── DC → GPIO21
├── MOSI → GPIO23
├── SCK → GPIO18
├── MISO → GPIO19
└── LED → 3.3V (backlight)

HC-SR04:
├── VCC → 3.3V/5V
├── GND → GND
├── Trig → GPIO15
└── Echo → GPIO2
```

## 🛠️ Software Setup

### Prerequisites
- [PlatformIO](https://platformio.org/) IDE
- [Visual Studio Code](https://code.visualstudio.com/) (recommended)

### Installation

1. **Clone the repository**
```bash
git clone https://github.com/Maksikos-ctrl/smartclimate-station.git
cd smartclimate-station
```

2. **Open in PlatformIO**
```bash
code .
```

3. **Install dependencies**
Dependencies are automatically installed via `platformio.ini`:
- DHT sensor library
- Adafruit Unified Sensor
- TFT_eSPI

4. **Configure display driver**
Update `User_Setup.h` in TFT_eSPI library with your display type:
```cpp
#define ST7789_DRIVER  // or ILI9341_DRIVER depending on your display
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
```

5. **Build and upload**
```bash
platformio run --target upload
```

## 📁 Project Structure

```
├── include/
│   ├── displayTFT.h          # TFT display management
│   ├── sensorDHT.h           # DHT11 sensor interface
│   └── ultrasonicSensor.h    # HC-SR04 proximity sensor
├── src/
│   ├── main.cpp              # Main application logic
│   ├── displayTFT.cpp        # Display implementation
│   ├── sensorDHT.cpp         # DHT11 implementation
│   └── ultrasonicSensor.cpp  # Ultrasonic sensor implementation
├── platformio.ini            # Project configuration
└── README.md
```

## 🔧 System Architecture

### Hardware Layer
```
┌─────────────────────────────────────────────────────┐
│                  ESP32-WROOM-32                     │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────┐ │
│  │   FreeRTOS  │ │ Arduino     │ │ System Tasks    │ │  
│  │   Kernel    │ │ Framework   │ │ WiFi/Bluetooth  │ │
│  └─────────────┘ └─────────────┘ └─────────────────┘ │
└─────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────┐
│                Communication Protocols              │
│    SPI    │    1-Wire    │    GPIO    │    I2C      │
└─────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────┐
│                    Sensors Layer                    │
│  TFT Display │   DHT11    │  HC-SR04  │   Future    │
└─────────────────────────────────────────────────────┘
```

### Communication Protocols Used

#### **SPI (Serial Peripheral Interface)**
- **Used by**: TFT Display (and potentially RFID modules)
- **Pins**: SCK (GPIO18), MOSI (GPIO23), MISO (GPIO19), CS (GPIO5)
- **Type**: Synchronous, full-duplex, master-slave architecture
- **Speed**: 27MHz
- **Benefits**: High-speed data transfer, multiple devices on same bus

#### **1-Wire Protocol**
- **Used by**: DHT11 Temperature/Humidity Sensor
- **Pins**: Single data line (GPIO4) with built-in pull-up
- **Type**: Asynchronous, half-duplex, single-wire digital protocol
- **Features**: Power and data on same line, timing-critical communication
- **Implementation**: Custom DHT11 library handling timing sequences

#### **GPIO (General Purpose Input/Output)**
- **Used by**: HC-SR04 Ultrasonic Sensor
- **Pins**: TRIG (GPIO15 - OUTPUT), ECHO (GPIO2 - INPUT)
- **Type**: Digital pulse-based measurement
- **Method**: Pulse width modulation for distance calculation

### Real-Time Operating System

The project runs on **FreeRTOS** (Free Real-Time Operating System) which is integrated into the ESP32 Arduino framework:

#### **FreeRTOS Features Used:**
- **Task Scheduling**: Automatic time-slicing between operations
- **Memory Management**: Dynamic heap allocation for sensor data
- **Timer Services**: Built-in delay and timing functions
- **Interrupt Handling**: Hardware timer interrupts for sensor readings
- **Watchdog Timer**: System stability monitoring

#### **RTOS Benefits in This Project:**
- **Concurrent Operations**: Simultaneous sensor reading, display updates, and proximity detection
- **Real-time Response**: Guaranteed response times for critical operations
- **Memory Safety**: Protected memory allocation for sensor buffers
- **System Stability**: Automatic recovery from sensor failures

#### **Task Distribution:**
```cpp
// Implicit FreeRTOS tasks in Arduino framework:
loop()           // Main application task (1ms tick)
WiFi handling    // Background network task  
SPI operations   // Hardware abstraction task
Timer callbacks  // Sensor reading interrupts
```

You can access FreeRTOS directly if needed:
```cpp
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Check FreeRTOS version and memory
Serial.println(tskKERNEL_VERSION_NUMBER);
Serial.println(ESP.getFreeHeap());
```

## 🔧 System Behavior
### Proximity Detection
- **Activation delay**: 1 second
- **Deactivation delay**: 5 seconds
- **Measurement interval**: 200ms

### Display States

#### Sleep Mode (No presence detected)
- Black screen with "Подойдите ближе..." message
- Minimal power consumption
- Continuous proximity monitoring

#### Active Mode (Presence detected)
- **Header**: "Weather Station"
- **Humidity**: Green text with % symbol
- **Temperature**: Yellow text with °C symbol  
- **Distance**: White text with cm measurement
- **Update interval**: 2 seconds

### Data Output
Real-time sensor data is output to Serial Monitor:
```
Temperature: 28.5°C, Humidity: 45.2%
Distance: 67 cm, Person present: YES
```

## 🎨 Display Layout

```
┌─────────────────────────────────┐
│         Weather Station         │
│                          Active │
│                                 │
│  Humidity:              45.2 %  │
│                                 │
│  Temperature:           28.5 C  │
│                                 │
│  Distance:                67 cm │
│                                 │
└─────────────────────────────────┘
```

## ⚙️ Configuration

### Sensor Thresholds
Modify values in `ultrasonicSensor.cpp`:
```cpp
static const float PRESENCE_THRESHOLD = 100.0; // Detection range in cm
static const unsigned long PRESENCE_DELAY = 1000;  // Activation delay
static const unsigned long ABSENCE_DELAY = 5000;   // Deactivation delay
```

### Display Settings
Adjust positioning in `displayTFT.h`:
```cpp
static const int HUM_LABEL_X = 20;
static const int HUM_LABEL_Y = 80;
// ... other coordinates
```

## 🔍 Troubleshooting

### Display Issues
- Verify TFT driver type in `User_Setup.h`
- Check SPI pin connections
- Ensure backlight (LED) pin is connected to power

### Sensor Issues
- DHT11 requires pull-up resistor (built-in `INPUT_PULLUP` used)
- HC-SR04 needs stable power supply
- Check serial output for sensor readings

### Common Problems
- **White/blank display**: Wrong driver or missing backlight
- **No sensor readings**: Check pin connections and power
- **Erratic proximity detection**: Adjust `PRESENCE_THRESHOLD`

## 🚀 Future Enhancements

- [ ] WiFi connectivity for remote monitoring
- [ ] RFID access control
- [ ] Additional sensors (pressure, light, air quality)
- [ ] Web dashboard
- [ ] Mobile app integration
- [ ] AI-powered object classification

## 📊 Technical Specifications

- **Microcontroller**: ESP32-WROOM-32 (Dual-core Xtensa LX6, 240MHz)
- **Operating System**: FreeRTOS v10.x
- **Operating Voltage**: 3.3V
- **Power Consumption**: ~150mA (active), ~50mA (sleep)
- **Temperature Range**: 0-50°C (DHT11 limitation)
- **Humidity Range**: 20-90% RH (±5% accuracy)
- **Distance Range**: 2-400cm (±3mm accuracy)
- **Display Resolution**: 240x320 pixels, 16-bit color
- **Communication Protocols**: SPI, 1-Wire, GPIO
- **Update Rate**: 2 seconds (display), 200ms (proximity)
- **Memory**: 520KB SRAM, 4MB Flash

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [PlatformIO](https://platformio.org/) for the development environment
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library for display management
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) by Adafruit

## 📧 Contact

Your Name - maksikos973@gmail.com

Project Link: [https://github.com/Maksikos-ctrl/smartclimate-station](https://github.com/Maksikos-ctr/smartclimate-station)