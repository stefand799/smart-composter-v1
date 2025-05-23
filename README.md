# Smart Composter Sensor Hub (Firmware)

This repository contains the ESP32 firmware for the Smart Composter Sensor Hub—v1 prototype of the All-Gardener platform. It reads environmental data from multiple sensors, maps raw values to real-world units, and serves the results via HTTP as JSON.

## Introduction

The Smart Composter Sensor Hub is the edge component of the All-Gardener ecosystem, designed to capture high-resolution environmental data from compost chambers. This firmware runs on an ESP32 and interfaces with temperature, gas, and humidity sensors to produce clean, real-time JSON data for downstream apps and cloud services. It prioritizes reliability, low resource usage, and easy integration.

## Table of Contents

* [Overview](#overview)
* [Hardware Wiring](#hardware-wiring)
* [Software Setup](#software-setup)
* [API Endpoint](#api-endpoint)
* [Calibration Constants](#calibration-constants)
* [Collaborations](#collaborations)
* [Contributing](#contributing)
* [License](#license)

---

## Overview

The Smart Composter Sensor Hub is an edge device that:

1. Reads two DS18B20 temperature sensors.
2. Reads MQ-135 and MQ-4 gas sensors.
3. Reads two analog humidity sensors.
4. Maps raw ADC readings to temperature (°C), gas concentration (%), and moisture (%).
5. Exposes an HTTP GET endpoint (`/data`) returning a JSON payload with all sensor data and a timestamp.

This firmware is designed to be lightweight—no on-board analytics—so all processing and decision-making occur in a companion app or cloud service.

---

## Hardware Wiring

1. **Power Rails**

   * Connect ESP32 **3.3 V** pin to the 3.3 V supply rail.
   * Connect ESP32 **GND** pin to the ground rail.

2. **DS18B20 Temperature Sensors**

   * DATA pin of sensor #1 → **GPIO 4**
   * DATA pin of sensor #2 → **GPIO 16**
   * Each DATA line requires a **4.7 KΩ pull-up resistor** to the 3.3 V rail.
   * VCC → 3.3 V rail; GND → ground rail.

3. **MQ Gas Sensors**

   * MQ-135 signal → **GPIO 32**
   * MQ-4 signal   → **GPIO 33**
   * VCC → 3.3 V rail; GND → ground rail.
   * Add a **10 µF electrolytic capacitor** across VCC and GND on each sensor

4. **Analog Humidity Sensors**

   * Sensor #1 output → **GPIO 34**
   * Sensor #2 output → **GPIO 35**
   * VCC → 3.3 V rail; GND → ground rail.

5. **On-board LED**

   * Driven by **GPIO 2**; blinks for 50 ms each time `/data` is served.

---

## Software Setup

1. **Clone the repo**

   ```bash
   git clone https://github.com/<your-username>/smart-composter-firmware.git
   cd smart-composter-firmware
   ```
2. **Open** the project in Arduino IDE or PlatformIO.
3. **Configure Wi-Fi** in `src/main.cpp`:

   ```cpp
   const char* ssid     = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";
   ```
4. **Flash** the firmware to your ESP32.

---

## API Endpoint

* **GET** `/data`
  Returns JSON:

  ```json
  {
    "temp1_C": 25.3,
    "temp2_C": 25.1,
    "avg_temp_C": 25.2,
    "mq135_raw": 450,
    "mq135_percent": 10.2,
    "mq4_raw": 120,
    "mq4_percent": 14.1,
    "hum1_raw": 2000,
    "hum1_percent": 51.1,
    "hum2_raw": 2050,
    "hum2_percent": 50.0,
    "avg_humidity": 50.55,
    "timestamp_ms": 12345678
  }
  ```

---

## Calibration Constants

Adjust these in `src/main.cpp` to match your sensors:

```cpp
const int MQ135_RAW_MIN  = 100;
const int MQ135_RAW_MAX  = 3500;
const int MQ4_RAW_MIN    = 50;
const int MQ4_RAW_MAX    = 800;
const int HUMIDITY_RAW_DRY = 4095;
const int HUMIDITY_RAW_WET = 0;
```

---

## Collaborations

This firmware pairs with a companion application that provides visualization, configuration, and cloud integration:

* **Firmware Hub**: this repository (ESP32 code)
* **Smart Composter App**: client-side code for mobile/web
  [[https://github.com/](https://github.com/)<your-username>/smart-composter-app](https://github.com/Stefannn444/Smart-Composter)

---


---

## License

MIT License — see [LICENSE](LICENSE) for details.
