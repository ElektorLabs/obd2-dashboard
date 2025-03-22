# OBD2 Dashboard 🚗📟

## Overview

This repository hosts the code and documentation for an advanced automotive dashboard built using an ESP32-S3 microcontroller and an ELM327 OBD-II module. The dashboard displays real-time vehicle sensor data, including RPM, speed, coolant temperature, engine load, oxygen sensor voltage, battery voltage, fuel economy, and more, providing an intuitive graphical interface for monitoring vehicle performance.

### Features
- Real-Time Vehicle Diagnostics: Instantly fetch and display OBD-II data.
- Graphical Interface: Rich UI built using Arduino_GFX.
- Easy Setup: Simple integration with ELM327 modules.
- Derived Calculations: Displays calculated parameters like instantaneous fuel economy.

### Hardware Requirements
- ELM327 OBD-II Module (UART compatible)
- JC3248W535 Developement Board (ESP32S3 Based) [Purchase Link](https://s.click.aliexpress.com/e/_ol1A4SB).

### Libraries Used
Arduino_GFX

## Getting Started

### Hardware Connections
1. Connect ESP32-S3 UART pins to ELM327 (default GPIO 17 RX, GPIO 18 TX).
2. Connect the JC3248W535 display according to its documentation.

### Setup & Installation

1. Clone the repository:
```git clone https://github.com/[YourGithub]/OBD2-Dashboard.git```

2.	Install Arduino_GFX: Go to Arduino IDE > Sketch > Include Library > Manage Libraries and search for Arduino_GFX.
3.	Configure ESP32 board in Arduino IDE.
4.	Upload the code via Arduino IDE.

### How It Works

The ESP32-S3 sends OBD-II PID requests to the vehicle ECU via the ELM327 module over UART. Responses are parsed using specific formulas and displayed visually on the connected screen. Here’s a simplified example of how data is parsed:

```cpp 
// Requesting Coolant Temperature
sendOBDCommand("0105");

// Parsing OBD-II response
String hexData = response.substring(4, 6); // Extracting temperature hex data
int temperature = strtol(hexData.c_str(), NULL, 16) - 40; // Converting to Celsius
Serial.printf("Coolant Temp: %d°C\\n", temperature);
```


