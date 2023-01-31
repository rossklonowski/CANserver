//
//  oled.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <Adafruit_SSD1325.h>
#include "oled.h"
#include "bitmaps.h"

// OLED
// If using software SPI, define CLK and MOSI
#define OLED_CLK 18
#define OLED_MOSI 23

#define OLED_CS 21
#define OLED_RESET 17
#define OLED_DC 16

bool inverted = false;

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
	oled.invertDisplay(inverted);
	oled.setRotation(2);
	oled.setTextSize(1);
	oled.setTextColor(WHITE);
	oled.display();
    delay(2000);
}

void send_to_oled_buffer(int row, String line) {
    oled.setTextSize(1);
    oled.setCursor(1, (row * 8) + 1);
    oled.println(line);
}

void send_to_oled_buffer(int row, int text_size, String line) {
    oled.setTextSize(text_size);
	int line_spacing = 8;
	if (text_size == 1) {
		line_spacing = 14;
	}
	if (text_size == 2) {
		line_spacing = 18;
	}
	if (text_size == 3) {
		line_spacing = 22;
	}
    oled.setCursor(1, (row * line_spacing) + 1);
    oled.println(line);
}

void send_to_oled_buffer(int row, int text_size, int ypos, String line) {
    oled.setTextSize(text_size);
    oled.setCursor(1, ypos);
    oled.println(line);
}

void write_oled(int row, int textSize, String line) {
	oled.clearDisplay();   // clears the splash screen
	oled.setTextSize(textSize);
    oled.setCursor(1, (row * 8) + 1);
    oled.println(line);
	oled.display();
}

bool oled_invert_color() {
	if (inverted) {
		oled.invertDisplay(false);
		inverted = false;
	} else {
		oled.invertDisplay(true);
		inverted = true;
	}
}

void oled_image(int image) {
	oled.clearDisplay();
    oled.drawBitmap(1, 1, bmp_array[image], 128, 64, 1);
    oled.display();   
}
