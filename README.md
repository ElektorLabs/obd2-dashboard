# OBD2 Dashboard 🚗📟

![IMG_2541](https://github.com/user-attachments/assets/4f9f7ac7-e158-4273-b354-f286808bec43)


## Overview

This repository hosts the code and documentation for an advanced automotive dashboard built using an ESP32-S3 microcontroller and an ELM327 OBD-II module. The dashboard displays real-time vehicle sensor data, including RPM, speed, coolant temperature, engine load, oxygen sensor voltage, battery voltage, fuel economy, and more, providing an intuitive graphical interface for monitoring vehicle performance.

### Features
- Real-Time Vehicle Diagnostics: Instantly fetch and display OBD-II data.
- Graphical Interface: Rich UI built using Arduino_GFX.
- Easy Setup: Simple integration with ELM327 modules.
- Derived Calculations: Displays calculated parameters like instantaneous fuel economy.

### Hardware Requirements
- ELM327 OBD-II Module (UART compatible) - Choose the PIC Version a.k.a double board pcb. [Purchase Link](https://s.click.aliexpress.com/e/_mKndvpN)
![IMG20250316140646](https://github.com/user-attachments/assets/8d428922-5785-47c6-8741-a1bb8fc46e09)

- JC3248W535 Developement Board (ESP32S3 Based) [Purchase Link](https://s.click.aliexpress.com/e/_ol1A4SB).
![IMG20250316141402](https://github.com/user-attachments/assets/315e4849-b0d4-479b-8952-0a74223649e8)


### Libraries Used
Arduino_GFX

## Getting Started

### Hardware Connections
1. Connect ESP32-S3 UART pins to ELM327 (default GPIO 17 RX, GPIO 18 TX)
   
![IMG20250316140607](https://github.com/user-attachments/assets/d7817a70-121b-4322-bdbb-b3c9f5c34665)
   
2. Connect the JC3248W535 display accordingly.

### Setup & Installation

1. Clone the repository:
```git clone https://github.com/[YourGithub]/OBD2-Dashboard.git```

2.	Install Arduino_GFX: Go to Arduino IDE > Sketch > Include Library > Manage Libraries and search for Arduino_GFX.
3.	Configure ESP32 board in Arduino IDE.
   
     ![image](https://github.com/user-attachments/assets/8dced975-f7de-469e-b0df-42770de577e0)
  
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


### 🖨️ 3D Printing Instructions (OBD2 Dashboard Holder)

To ensure optimal strength, heat resistance, and durability for use inside a car (especially on the dashboard), follow these recommended PETG printing settings:

**General Settings:**

* **Material**: PETG (Black or Dark Gray recommended for UV resistance)
* **Nozzle Temperature**: 235–250°C (depends on filament brand)
* **Bed Temperature**: 70–85°C
* **Cooling**: 20–50% fan after first few layers (avoid full fan to reduce warping and layer splitting)
* **Speed**: 60~70 mm/s (slow speed gives better layers). 

**Mechanical Strength:**

* **Infill Density**: 50–75% (Grid or Gyroid preferred)
* **Perimeters/Walls**: Minimum 3
* **Top Layers**: 5
* **Bottom Layers**: 5
* **Layer Height**: 0.2 mm or 0.3 mm for balanced strength and detail

**Adhesion Tips:**

* Use a clean PEI sheet or apply glue stick/hairspray on glass for better bed adhesion.
* Enable a **brim** (5–8 mm) if corners lift.

**Post-Processing (Optional but Recommended):**

* Allow printed part to **cool fully** on the bed to avoid warping.
* For enhanced UV and heat resistance, consider applying a **matte clear coat** rated for automotive use.


