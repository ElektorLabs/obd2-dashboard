# define SCREEN_WIDTH 480
# define SCREEN_HEIGHT 320

# define ROUND_EDGES 8



void drawRPMIndicator(int rpm) {
  int ledCount = 8;                                // Total LEDs across the screen
  int ledSpacing = SCREEN_WIDTH / (ledCount + 1);  // Space them evenly
  uint16_t color;

  for (int i = 0; i < ledCount; i++) {
    int ledRPM = (i + 1) * 1000;  // Each LED represents 1000 RPM increments

    // Determine color based on RPM level
    if (rpm >= ledRPM) {
      if (ledRPM < 2000) {
        color = RGB565_YELLOW;
      } else if (ledRPM < 3500) {
        color = RGB565_ORANGE;
      } else if (ledRPM < 5000) {
        color = RGB565_RED;
      } else {
        color = (millis() % 500 < 250) ? RGB565_PURPLE : RGB565_BLACK;  // Flashing at 5000+
      }
    } else {
      color = RGB565_DARKGREY;  // Turn off unused LEDs
    }

    // Draw LED
    gfx->fillCircle(ledSpacing * (i + 1), 20, 10, color);
  }
}

void drawRPMBar(int rpm) {
  int maxRPM = 7000;                                    // Define max RPM for full bar
  int barWidth = map(rpm, 0, maxRPM, 0, SCREEN_WIDTH);  // Scale width dynamically
  int barHeight = 30;                                   // Thickness of the bar
  uint16_t color;

  // Draw the gradient effect
  for (int x = 0; x < barWidth; x++) {
    int colorZone = map(x, 0, SCREEN_WIDTH, 0, 4);  // Divide into 4 color zones

    if (rpm >= 5000 && millis() % 500 < 250) {
      color = RGB565_PURPLE;  // Flash purple at high RPM
    } else {
      if (colorZone == 0) color = RGB565_YELLOW;
      else if (colorZone == 1) color = RGB565_ORANGE;
      else if (colorZone == 2) color = RGB565_RED;
      else color = RGB565_PURPLE;
    }

    gfx->drawFastVLine(x, 0, barHeight, color);  // Draw colored segments
  }
}

void drawSmoothRPMBar(int rpm) {
  int maxRPM = 7000;                       // Max RPM
  int barHeight = 30;                      // Thickness of the bar
  static bool flashState = false;          // Flash toggle state
  static unsigned long lastFlashTime = 0;  // Timer for flashing

  gfx->fillRect(0, 0, SCREEN_WIDTH, barHeight, RGB565_BLACK);  // Clear previous bar

  if (rpm > 5000) {
    // Flash entire bar in solid purple
    if (millis() - lastFlashTime > 200) {
      flashState = !flashState;
      lastFlashTime = millis();
    }

    if (flashState) {
      gfx->fillRect(0, 0, SCREEN_WIDTH, barHeight, gfx->color565(128, 0, 128));  // Full Purple Bar
    }
  } else {
    // Normal smooth gradient effect
    int barWidth = map(rpm, 0, maxRPM, 0, SCREEN_WIDTH);  // Scale width
    for (int x = 0; x < barWidth; x++) {
      float ratio = (float)x / SCREEN_WIDTH;  // Normalize position (0 to 1)
      uint8_t r, g, b;

      if (ratio < 0.25) {
        // Yellow (255,255,0) → Orange (255,165,0)
        float t = ratio / 0.25;
        r = 255;
        g = 255 - (90 * t);
        b = 0;
      } else if (ratio < 0.6) {
        // Orange (255,165,0) → Red (255,0,0)
        float t = (ratio - 0.25) / 0.35;
        r = 255;
        g = 165 - (165 * t);
        b = 0;
      } else {
        // Red (255,0,0) → Purple (128,0,128)
        float t = (ratio - 0.6) / 0.4;
        r = 255 - (127 * t);
        g = 0;
        b = 128 * t;
      }

      uint16_t color = gfx->color565(r, g, b);     // Convert to 16-bit color
      gfx->drawFastVLine(x, 0, barHeight, color);  // Draw smooth gradient
    }
  }
}

void drawCenterCard(int gear, int rpm) {
  int cardWidth = 120;
  int cardHeight = 150;
  int centerX = 180;
  int centerY = 40;

  // Draw dark grey card background
  //gfx->fillRoundRect(centerX, centerY, cardWidth, cardHeight, ROUND_EDGES, RGB565_DARKGREY);
  if (rpm > 5000) {
    gfx->fillRoundRect(centerX, centerY, cardWidth, cardHeight, ROUND_EDGES, RGB565_RED);
    //gfx->drawRoundRect(centerX, centerY, cardWidth, cardHeight, ROUND_EDGES, RGB565_RED);
  } else {
    gfx->fillRoundRect(centerX, centerY, cardWidth, cardHeight, ROUND_EDGES, RGB565_DARKGREY);
    //gfx->drawRoundRect(centerX, centerY, cardWidth, cardHeight, ROUND_EDGES, RGB565_DARKGREY);
  }


  // Draw gear in large font
  gfx->setFont(&FreeSansBold54pt7b);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setCursor(centerX + 32, centerY + 95);
  gfx->print(gear);

  // Draw RPM below gear
  gfx->setFont(&FreeSansBold12pt7b);

  if (rpm < 10) {
    gfx->setCursor(centerX + 55, centerY + 140);
    gfx->print(rpm);
  } else if (rpm > 10 & rpm < 1000) {
    gfx->setCursor(centerX + 40, centerY + 140);
    gfx->print(rpm);
  } else if (rpm > 1000) {
    gfx->setCursor(centerX + 32, centerY + 140);
    gfx->print(rpm);
  }
}
void drawSpeed(int speed) {
  int cardWidth = 120;
  int cardHeight = 40;
  int centerX = 180;
  int centerY = 205;

  // Draw white card background
  gfx->fillRoundRect(centerX, centerY, cardWidth, cardHeight, ROUND_EDGES, RGB565_WHITE);

  // Draw speed font
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->setTextColor(RGB565_BLACK);

  if (speed < 10) {
    gfx->setCursor(centerX + 55, centerY + 30);
    gfx->print(speed);
  } else if (speed > 10 & speed < 100) {
    gfx->setCursor(centerX + 40, centerY + 30);
    gfx->print(speed);
  } else if (speed > 100) {
    gfx->setCursor(centerX + 32, centerY + 30);
    gfx->print(speed);
  }
}

void drawStatusBar(int status) {
  if (status == 1) {
    // drawing a purple line separator
    gfx->fillRoundRect(180, 192, 120, 10, ROUND_EDGES, gfx->color565(128, 0, 128));
  } else if (status == 0) {
    // drawing a red line separator
    gfx->fillRoundRect(180, 192, 120, 10, ROUND_EDGES, gfx->color565(128, 0, 0));
  }
}

void drawBoarders() {
  // Define rectangle properties
  int leftX = 10, rightX = 310;  // X positions for left and right rectangles
  int rectY = 40, rectWidth = 460, rectHeight = 205;
  // Draw Big Sensor Data Rectangle
  gfx->drawRoundRect(leftX, rectY, rectWidth, rectHeight, ROUND_EDGES, RGB565_WHITE);
  // Draw Right Sensor Data Rectangle
  //gfx->drawRoundRect(rightX, rectY, rectWidth, rectHeight, ROUND_EDGES, RGB565_WHITE);
  // Draw a Bottom Rectangle
  gfx->drawRoundRect(10, 255, 460, 60, ROUND_EDGES, RGB565_WHITE);
}

void displayUptime() {
  unsigned long uptimeMillis = millis();  // Get system uptime
  int seconds = (uptimeMillis / 1000) % 60;
  int minutes = (uptimeMillis / 60000) % 60;
  int hours = (uptimeMillis / 3600000);

  // Format and display the uptime
  //             x , y
  gfx->setCursor(15, 305);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->setTextColor(RGB565_YELLOW);
  gfx->printf("%02d:%02d:%02d", hours, minutes, seconds);

  gfx->setCursor(15, 273);
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(125, 123, 123));
  gfx->print("UP TIME");
}


void drawError() {
  // Format and display the uptime
  //             x , y
  gfx->setCursor(190, 290);
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(RGB565_RED);
  gfx->print("ELM327 NOT CONNECTED");
}

// Left Side Cards
void drawCoolantTemp(int coolantTemp) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 17;
  int y = 45;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(0, 51, 102));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(0, 128, 255));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("Coolant (  C)");
  gfx->drawCircle(gfx->getCursorX() - 24, gfx->getCursorY() - 8, 2, gfx->color565(0, 128, 255));  // Draw degree symbol manually

  // Draw value
  gfx->setCursor(x + 50, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(coolantTemp);
}
void drawFuelRate(float fuelEfficiency) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 17;
  int y = 111;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(102, 51, 0));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(255, 153, 51));
  gfx->setCursor(x + 10, y + 18);
  gfx->print("Fuel Rate(L/h)");

  // Draw value
  gfx->setCursor(x + 30, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(fuelEfficiency);
}
void drawEngineLoad(int engineLoad) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 17;
  int y = 178;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(102, 0, 0));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(255, 51, 51));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("Engine Load (%)");

  // Draw value
  gfx->setCursor(x + 50, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(engineLoad);
}


// Center Cards
void drawOxygen(float oxygen) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 167;
  int y = 45;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(0, 51, 0));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("Oxygen (V)");
  // Draw value
  gfx->setCursor(x + 50, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(oxygen);
}
void drawIntakeTemp(float temperature) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 167;
  int y = 111;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(51, 0, 102));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(153, 51, 255));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("IMT (C)");

  // Draw value
  gfx->setCursor(x + 30, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(temperature);
}

void  drawFuelEconomy(float rate) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 167;
  int y = 178;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(102, 102, 0));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(255, 255, 51));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("Fuel Eco. (km/l)");

  // Draw value
  gfx->setCursor(x + 30, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(rate);
}

// Right Side Cards
void drawBattery(float batteryVoltage) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 317;
  int y = 45;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(0, 51, 0));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(0, 255, 0));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("Battery (V)");
  // Draw value
  gfx->setCursor(x + 30, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(batteryVoltage);
}
void drawAirflow(float airflow) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 317;
  int y = 111;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(51, 0, 102));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(153, 51, 255));
  gfx->setCursor(x + 7, y + 18);
  gfx->print("Air Flow (g/sec)");

  // Draw value
  gfx->setCursor(x + 30, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(airflow);
}

void  drawIntakeManifold(int value) {
  int cardWidth = 147;
  int cardHeight = 63;
  int x = 317;
  int y = 178;
  // Draw card background
  gfx->fillRoundRect(x, y, cardWidth, cardHeight, ROUND_EDGES, gfx->color565(102, 102, 0));

  // Draw icon (for now, just text placeholders)
  gfx->setFont(&FreeSans9pt7b);
  gfx->setTextColor(gfx->color565(255, 255, 51));
  gfx->setCursor(x + 10, y + 18);
  gfx->print("IMP (PSI)");

  // Draw value
  gfx->setCursor(x + 50, y + 55);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setFont(&FreeSansBold18pt7b);
  gfx->print(value);
}
