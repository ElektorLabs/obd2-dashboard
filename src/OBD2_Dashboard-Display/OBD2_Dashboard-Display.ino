
//=======================================================================================
// OBD2 Dashboard Display with ESP32-S3 and ELM327
// =======================================================================================
//
// Description:
// This project implements a real-time automotive dashboard using an ESP32-S3 microcontroller
// paired with an ELM327 module. It fetches and displays live vehicle data such as Engine RPM,
// Vehicle Speed, Engine Load, Coolant Temperature, Battery Voltage, and calculated parameters
// like Fuel Economy. The data is read from the vehicle's OBD-II port, parsed, and then
// visualized on a colorful graphical interface built using the Arduino_GFX library.
//
// Hardware:
// - ESP32-S3 Microcontroller
// - ELM327 OBD-II Scanner Module (UART)
// - JC3248W535 Display Module (Arduino_GFX compatible)
//
// Communication Flow:
// 1. ESP32-S3 sends UART commands to ELM327 using standard OBD-II PID requests.
// 2. ELM327 communicates with the vehicle's ECU and returns ASCII responses.
// 3. ESP32-S3 parses these responses to extract relevant data using specific formulas.
// 4. Parsed data is displayed dynamically on the dashboard interface.
//
// Dependencies:
// - Arduino_GFX library (for graphical interface)
// - HardwareSerial (ESP32 built-in for UART communication)
//
// Author: Saad Imtiaz
// =========================================================================================

#include <Arduino_GFX_Library.h>

// Fonts
#include "Fonts/FreeSansBold10pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSansBold24pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSansBold30pt7b.h"
#include "Fonts/FreeSansBold72pt7b.h"
#include "Fonts/FreeSansBold60pt7b.h"
#include "Fonts/FreeSansBold54pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans24pt7b.h"

// Arduino_GFX Setting
#define JC3248W535
#define GFX_DEV_DEVICE JC3248W535
#define GFX_BL 1
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  45 /* CS */, 47 /* SCK */, 21 /* D0 */, 48 /* D1 */, 40 /* D2 */, 39 /* D3 */);
Arduino_GFX *g = new Arduino_AXS15231B(bus, GFX_NOT_DEFINED /* RST */, 270 /* rotation */, false /* IPS */, 320 /* width */, 480 /* height */);
#define CANVAS
Arduino_Canvas *gfx = new Arduino_Canvas(320 /* width */, 480 /* height */, g, 0 /* output_x */, 0 /* output_y */, 270 /* rotation */);

// ELM327 Setting
#define ELM327_RX 17  // ESP32-S3 GPIO 17 (Connected to TX of ELM327)
#define ELM327_TX 18  // ESP32-S3 GPIO 18 (Connected to RX of ELM327)

#define SERIAL_BAUD 115200  // Serial Monitor baud rate
#define ELM327_BAUD 38400   // ELM327 default baud rate

HardwareSerial ELMSerial(1);  // Use UART1 for communication

// Finalized List of Working OBD-II PIDs
const char *obdPIDs[] = {
  "010C", "010D", "0104", "0110", "010F", "010B",
  "0134", "010E", "ATRV" , "0111", "0105", "015B",
};
int numPIDs = sizeof(obdPIDs) / sizeof(obdPIDs[0]);

float fuelRate = 0 ; 
/*
==================================================================================
| OBD-II PIDs Used in This Code                                                 |
|-------------------------------------------------------------------------------|
|  PID   | Description                     | Formula                     | Unit |
|--------|---------------------------------|-----------------------------|------|
| 010C   | 1 Engine RPM                    | (A*256 + B) / 4             | RPM  |
| 010D   | 2 Vehicle Speed                 | A                           | km/h |
| 0104   | 3 Engine Load                   | (A * 100) / 255             | %    |
| 0110   | 4 Mass Air Flow (MAF)           | ((A*256) + B) / 100         | g/s  |
| 010F   | 5 Intake Air Temperature        | A - 40                      | °C   |
| 010B   | 6 Intake Manifold Pressure      | A                           | kPa  |
| 0134   | 7 Oxygen Sensor Voltage         | A * 0.005                   | V    |
| 010E   | 8 Timing Advance                | (A - 128) / 2               | °    |
| 0105   | 9 Coolant Temperature           |  A - 40.                    | °C   |
==================================================================================
*/


void setup() {
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(SERIAL_BAUD);
  ELMSerial.begin(ELM327_BAUD, SERIAL_8N1, ELM327_RX, ELM327_TX);
  // Serial.setDebugOutput(true);
  // while(!Serial);

  // Init Display
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->setRotation(1);  // Rotate screen for horizontal layout
  gfx->fillScreen(RGB565_BLACK);



#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
}

void loop() {

  gfx->fillScreen(RGB565_BLACK);

  displayUptime(); 
  for (int i = 0; i < numPIDs; i++) {
    requestOBDData(i);
  }

  //debugAllOBDCommands();

  drawBoarders();

  gfx->flush();
}

// Function to clean & extract OBD-II response
String cleanResponse(String response) {
  response.trim();
  response.replace(" ", "");

  int index = response.lastIndexOf("41");
  if (index == -1) index = response.lastIndexOf("62");

  if (index != -1) {
    response = response.substring(index);
  }

  if (response.startsWith("SEARCHING") || response.startsWith("OK") || response.startsWith(">") || response.startsWith("NODATA")) {
    return "";
  }

  return response;
}
// Function to parse OBD-II response
float parseOBDResponse(String response, String pid) {
  response = cleanResponse(response);
  if (response.length() == 0) {
    Serial.printf("No valid data received for %s.\n", pid.c_str());
    //drawStatusBar(0);
    drawError();
    return -1;
  } else {
   // drawStatusBar(1);
  }

  Serial.printf("Parsed Response for %s: %s\n", pid.c_str(), response.c_str());

  String hexA, hexB;
  int A, B;
  float value = -1;

   if (pid == "010C" && response.length() >= 8) {  // **Engine RPM**
    hexA = response.substring(4, 6);
    hexB = response.substring(6, 8);
    A = strtol(hexA.c_str(), NULL, 16);
    B = strtol(hexB.c_str(), NULL, 16);
    value = ((A * 256) + B) / 4.0;
    
    Serial.printf("Engine RPM: %.2f RPM\n", value);
}
else if (pid == "010D" && response.length() >= 6) {  // **Vehicle Speed**
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = A;

    float fuelEconomy = (fuelRate > 0) ? (value / fuelRate) : 0;
    drawFuelEconomy(fuelEconomy);
    
    Serial.printf("Speed: %.2f km/h\n", value);
    Serial.printf("Fuel Economy: %.2f km/L\n", fuelEconomy);
}
  else if (pid == "0104" && response.length() >= 6) {  // **Engine Load** -- Working 
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = (A * 100) / 255.0;
    drawEngineLoad(value);
    Serial.printf("Engine Load: %.2f%%\n", value);
  } 
  else if (pid == "0110" && response.length() >= 8) {  // **Mass Air Flow (MAF)**
    hexA = response.substring(4, 6);
    hexB = response.substring(6, 8);
    A = strtol(hexA.c_str(), NULL, 16);
    B = strtol(hexB.c_str(), NULL, 16);
    value = ((A * 256) + B) / 100.0;  // Convert to g/s
    // Calculate Fuel Consumption (L/h)
    float fuelConsumption = (value * 3600) / (14.7 * 730); 
    fuelRate = fuelConsumption; 
    drawAirflow(value);
    drawFuelRate(fuelConsumption);
    
    Serial.printf("Mass Air Flow: %.2f g/s\n", value);
    Serial.printf("Instantaneous Fuel Consumption: %.2f L/h\n", fuelConsumption);
}
  else if (pid == "010F" && response.length() >= 6) {  // **Intake Air Temp** -- Working
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = A - 40;
    drawIntakeTemp(value);
    Serial.printf("Intake Air Temp: %.2f°C\n", value);
  } 
  else if (pid == "010B" && response.length() >= 6) {  // **Intake Manifold Pressure** -- Working
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = A;
    drawIntakeManifold(value);
    Serial.printf("Intake Manifold Pressure: %.2f kPa\n", value);
  } 
  else if (pid == "0134" && response.length() >= 6) {  // **Oxygen Sensor Voltage** -- Working 
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = A * 0.005;
    drawOxygen(value);
    Serial.printf("Oxygen Sensor Voltage: %.2f V\n", value);
  } 
  else if (pid == "010E" && response.length() >= 6) {  // **Timing Advance** - Not Used 
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = (A - 128) / 2.0;
    Serial.printf("Timing Advance: %.2f°\n", value);
  } 
  else if (pid == "ATRV") {  // **Battery Voltage (ELM327 specific)** -- WORKING
        int startIndex = response.indexOf("ATRV");
        if (startIndex != -1) {
            String voltageStr = response.substring(startIndex + 4); // Extract part after "ATRV"
            voltageStr.trim(); // Remove extra spaces or line breaks
            voltageStr.replace("V", ""); // Remove "V" character
            value = voltageStr.toFloat(); // Convert to float
            drawBattery(value);
            Serial.printf("Battery Voltage: %.2f V\n", value);
        } else {
            Serial.printf("Invalid Battery Voltage Response: %s\n", response.c_str());
        }
  }

else if (pid == "0111" && response.length() >= 6) {  // **Throttle Position**
    hexA = response.substring(4, 6);  // Extract A byte
    A = strtol(hexA.c_str(), NULL, 16);  // Convert HEX to decimal
    value = A*100/255;  // Correct formula
    Serial.printf("Throttle Position (%): %.2f%%\n", value);
}

else if (pid == "015B" && response.length() >= 8) {  // **Intake Air Flow Rate**
    hexA = response.substring(4, 6);
    hexB = response.substring(6, 8);
    A = strtol(hexA.c_str(), NULL, 16);
    B = strtol(hexB.c_str(), NULL, 16);
    value = ((A * 256) + B) / 100.0;
    drawAirflow(value);
    Serial.printf("Intake Air Flow Rate: %.2f g/s\n", value);
}


  else if (pid == "0105" && response.length() >= 6) {  // **Coolant Temperature** -- WORKING
    hexA = response.substring(4, 6);
    A = strtol(hexA.c_str(), NULL, 16);
    value = A - 40;
    drawCoolantTemp(value);
    Serial.printf("Coolant Temp: %.2f°C\n", value);
  } 
  else {
    Serial.println("Unknown Response Format");
  }

  return value; 
}

// Function to send OBD-II command
void sendOBDCommand(String command) {
  Serial.print("Sending: ");
  Serial.println(command);
  ELMSerial.println(command);
}

// Function to request OBD-II data
float requestOBDData(int pidIndex) {
  if (pidIndex < 0 || pidIndex >= numPIDs) {
    return -1;
  }

  String pid = obdPIDs[pidIndex];
  sendOBDCommand(pid);

  String response = "";
  unsigned long timeout = millis() + 100;
  while (millis() < timeout) {
    if (ELMSerial.available()) {
      char c = ELMSerial.read();
      response += c;
    }
  }

  return parseOBDResponse(response, pid);
}


// Function to manually debug a specific OBD-II PID
void debugOBDCommand(const char* pid) {
    Serial.printf("\n===== DEBUG OBD COMMAND: %s =====\n", pid);
    
    sendOBDCommand(pid);  // Send the command
    Serial.printf("Sending: %s\n", pid);  

    String response = "";
    unsigned long timeout = millis() + 150;  // Wait up to 500ms for a response
    while (millis() < timeout) {
        if (ELMSerial.available()) {
            char c = ELMSerial.read();
            response += c;
        }
    }

    Serial.printf("Raw Response: %s\n", response.c_str());  // Show Raw Response

    // Check for NO DATA or error messages
    if (response.indexOf("NO DATA") != -1 || response.indexOf("NODATA") != -1) {
        Serial.printf("No valid data received for %s\n", pid);
        return;
    }

    // Clean and process the response
    String cleanedResponse = cleanResponse(response);
    Serial.printf("Processed Response: %s\n", cleanedResponse.c_str());

    // Parse and display the final value
    float parsedValue = parseOBDResponse(cleanedResponse, pid);
    Serial.printf("Parsed Value [%s]: %.2f\n", pid, parsedValue);
}


// Function to automatically debug all OBD-II PIDs in the list
void debugAllOBDCommands() {
    Serial.println("\n===== STARTING OBD-II DEBUGGING =====");

    for (int i = 0; i < numPIDs; i++) {
        const char* pid = obdPIDs[i];

        Serial.printf("\n===== DEBUGGING PID: %s =====\n", pid);
        
        sendOBDCommand(pid);
        Serial.printf("Sending: %s\n", pid);

        String response = "";
        unsigned long timeout = millis() + 150;  // Wait up to 500ms for response
        while (millis() < timeout) {
            if (ELMSerial.available()) {
                char c = ELMSerial.read();
                response += c;
            }
        }

        Serial.printf("Raw Response: %s\n", response.c_str());

        // Check for NO DATA or errors
        if (response.indexOf("NO DATA") != -1 || response.indexOf("NODATA") != -1) {
            Serial.printf("No valid data received for %s\n", pid);
            continue;  // Skip to the next PID
        }

        // Clean and process the response
        String cleanedResponse = cleanResponse(response);
        Serial.printf("Processed Response: %s\n", cleanedResponse.c_str());

        // Parse and display the final value
        float parsedValue = parseOBDResponse(cleanedResponse, pid);
        Serial.printf("Parsed Value [%s]: %.2f\n", pid, parsedValue);
    }

    Serial.println("\n===== OBD-II DEBUGGING COMPLETE =====\n");
}