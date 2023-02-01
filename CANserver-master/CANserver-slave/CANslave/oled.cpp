//
//  oled.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

// #include <Adafruit_SSD1325.h>
#include "oled.h"
#include "bitmaps.h"

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

int oled_set_rotation(int rotation) {
	oled.setRotation(rotation);
}


void oled_image(int image) {
	oled.clearDisplay();
    oled.drawBitmap(1, 1, bmp_array[image], 128, 64, 1);
    oled.display();   
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

int pixels_from_side = 10;

void draw_axis() {
    oled.drawLine(OLED_WIDTH - pixels_from_side, pixels_from_side, OLED_WIDTH - pixels_from_side, OLED_HEIGHT - pixels_from_side, WHITE); // y axis
    oled.drawLine(pixels_from_side, pixels_from_side, OLED_WIDTH - pixels_from_side, pixels_from_side, WHITE); // x axis
}

void update_graph(int values_array[], int bars) {
    
    oled.clearDisplay();
    
    draw_axis();

    for (int bar = 0; bar < bars; bar++) {
        int bar_x_val = OLED_WIDTH - pixels_from_side - bar - 2;
        oled.drawLine(bar_x_val, pixels_from_side, bar_x_val, values_array[bar], WHITE); // test data 
    }

    oled.display();
}

void simulate_graph() {
    int bars = OLED_WIDTH - (pixels_from_side * 2) - 2;
    Serial.println("Data Bars: " + String(bars));
    int values_array[bars] = { 0 };

    for (int bar_val = pixels_from_side; bar_val < OLED_HEIGHT - (pixels_from_side * 2) - 10; bar_val++) {
        
        // fill up values_array
        for (int bar = 0; bar < bars; bar++) {
            
            int temp = (bar_val*0.5) + (bar*0.5);
            if (temp <= pixels_from_side) {
                temp = temp + pixels_from_side;
            }
            if (temp >= (OLED_HEIGHT - (pixels_from_side))) {
                temp = OLED_HEIGHT - (pixels_from_side);
            }
            values_array[bar] = temp;
        }

        // update each bar with the new val
        update_graph(values_array, bars);
        // delay(100);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Adafruit_SSD1325 OLED::OLED() {
// 	oled_1(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
// 	// Serial.println("here333333");
// 	return oled_1;
// }

// void OLED::sample(Adafruit_SSD1325 copy) {
// 	Serial.println("here333333");
// }

// void OLED::clearDisplay() {
// 	oled_1.clearDisplay();   // clears the splash screen
// }

// void OLED::oled_update() {
// 	oled_1.display();
// }

// void OLED::setupOLED() {
//     oled_1.begin();
// 	oled_1.clearDisplay();   // clears the splash screen
// 	oled_1.invertDisplay(inverted);
// 	oled_1.setRotation(2);
// 	oled_1.setTextSize(1);
// 	oled_1.setTextColor(WHITE);
// 	oled_1.display();
//     delay(2000);
// }

// void OLED::send_to_oled_buffer(int row, String line) {
//     oled_1.setTextSize(1);
//     oled_1.setCursor(1, (row * 8) + 1);
//     oled_1.println(line);
// }

// void OLED::send_to_oled_buffer(int row, int text_size, String line) {
//     oled_1.setTextSize(text_size);
// 	int line_spacing = 8;
// 	if (text_size == 1) {
// 		line_spacing = 14;
// 	}
// 	if (text_size == 2) {
// 		line_spacing = 18;
// 	}
// 	if (text_size == 3) {
// 		line_spacing = 22;
// 	}
//     oled_1.setCursor(1, (row * line_spacing) + 1);
//     oled_1.println(line);
// }

// void OLED::send_to_oled_buffer(int row, int text_size, int ypos, String line) {
//     oled_1.setTextSize(text_size);
//     oled_1.setCursor(1, ypos);
//     oled_1.println(line);
// }

// void OLED::write_oled(int row, int textSize, String line) {
// 	oled_1.clearDisplay();   // clears the splash screen
// 	oled_1.setTextSize(textSize);
//     oled_1.setCursor(1, (row * 8) + 1);
//     oled_1.println(line);
// 	oled_1.display();
// }

// bool OLED::oled_invert_color() {
// 	if (inverted) {
// 		oled_1.invertDisplay(false);
// 		inverted = false;
// 	} else {
// 		oled_1.invertDisplay(true);
// 		inverted = true;
// 	}
// }

// void OLED::oled_image(int image) {
// 	oled_1.clearDisplay();
//     oled_1.drawBitmap(1, 1, bmp_array[image], 128, 64, 1);
//     oled_1.display();   
// }
