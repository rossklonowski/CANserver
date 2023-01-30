//
//  oled.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <Adafruit_SSD1325.h>
#include "oled.h"

// OLED
// If using software SPI, define CLK and MOSI
#define OLED_CLK 18
#define OLED_MOSI 23

#define OLED_CS 21
#define OLED_RESET 17
#define OLED_DC 16

Adafruit_SSD1325 oled(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void oled_clear() {
	oled.clearDisplay();   // clears the splash screen
}

void oled_update() {
	oled.display();
}

void setupOLED() {
    oled.begin();
	oled.clearDisplay();   // clears the splash screen
	oled.setRotation(2);
	oled.setTextSize(1);
	oled.setTextColor(WHITE);
	oled.display();
    delay(2000);
}

void send_to_oled_buffer(int row, String line) {
    oled.setTextSize(1);
    oled.setCursor(0, row * 8);
    oled.println(line);
}

void write_oled(int row, int textSize, String line) {
	oled.clearDisplay();   // clears the splash screen
	oled.setTextSize(textSize);
    oled.setCursor(0, row * 8);
    oled.println(line);
	oled.display();
}
