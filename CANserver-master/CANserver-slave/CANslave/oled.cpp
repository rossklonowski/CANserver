//
//  oled.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include "oled.h"
#include "bitmaps.h"

bool inverted = false;

OLED::OLED() {
    current_orientation = 2;
}

void OLED::clearDisplay() {
	private_display.clearDisplay();   // clears the splash screen
}

void OLED::oled_update() {
	private_display.display();
}

void OLED::setupOLED() {
    
    private_display.setup(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

    private_display.begin();
	private_display.clearDisplay();   // clears the splash screen
	private_display.invertDisplay(inverted);
	private_display.setRotation(2);
	private_display.setTextSize(1);
	private_display.setTextColor(WHITE);
	private_display.display();
    delay(2000);
}

void OLED::send_to_oled_buffer(int row, String line) {
    private_display.setTextSize(1);
    private_display.setCursor(1, (row * 8) + 1);
    private_display.println(line);
}

void OLED::send_to_oled_buffer(int row, int text_size, String line) {
    private_display.setTextSize(text_size);
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
    private_display.setCursor(1, (row * line_spacing) + 1);
    private_display.println(line);
}

void OLED::send_to_oled_buffer(int row, int text_size, int ypos, String line) {
    private_display.setTextSize(text_size);
    private_display.setCursor(1, ypos);
    private_display.println(line);
}

void OLED::write_oled(int row, int textSize, String line) {
	private_display.clearDisplay();   // clears the splash screen
	private_display.setTextSize(textSize);
    private_display.setCursor(1, (row * 8) + 1);
    private_display.println(line);
	private_display.display();
}

bool OLED::oled_invert_color() {
	if (inverted) {
		private_display.invertDisplay(false);
		inverted = false;
	} else {
		private_display.invertDisplay(true);
		inverted = true;
	}
}

void OLED::oled_image(int image) {
	private_display.clearDisplay();
    private_display.drawBitmap(1, 1, bmp_array[image], 128, 64, 1);
    private_display.display();   
}

void OLED::set_rotation(int rot) {
    private_display.setRotation(rot);
}

void OLED::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    private_display.drawLine(x0, y0, x1, x1, color);
}

int pixels_from_side = 10;

void OLED::draw_axis() {
    private_display.drawLine(OLED_WIDTH - pixels_from_side, pixels_from_side, OLED_WIDTH - pixels_from_side, OLED_HEIGHT - pixels_from_side, WHITE); // y axis
    private_display.drawLine(pixels_from_side, pixels_from_side, OLED_WIDTH - pixels_from_side, pixels_from_side, WHITE); // x axis
}

void OLED::update_graph(my_queue& my_queue) {
    
    int bars = my_queue.get_size();

    private_display.clearDisplay();
    
    // draw_axis();

    // for (int bar = 0; bar < bars; bar++) {
    //     int bar_x_val = OLED_WIDTH - pixels_from_side - bar - 2;
    //     // private_display.drawLine(bar_x_val, pixels_from_side, bar_x_val, my_queue.get_val(bar), WHITE); // test data 
    // }

    private_display.display();
}

void OLED::simulate_graph() {
    int bars = OLED_WIDTH - (pixels_from_side * 2) - 2;
    Serial.println("Data Bars: " + String(bars));
    // int values_array[bars] = { 0 };

    // for (int bar_val = pixels_from_side; bar_val < OLED_HEIGHT - (pixels_from_side * 2) - 10; bar_val++) {
        
    //     // fill up values_array
    //     for (int bar = 0; bar < bars; bar++) {
            
    //         // int temp = (bar_val*0.5) + (bar*0.5);
    //         // if (temp <= pixels_from_side) {
    //         //     temp = temp + pixels_from_side;
    //         // }
    //         // if (temp >= (OLED_HEIGHT - (pixels_from_side))) {
    //         //     temp = OLED_HEIGHT - (pixels_from_side);
    //         // }
    //         int temp = 20;
    //         values_array[bar] = temp;
    //     }

    //     // update each bar with the new val
    //     update_graph(values_array, bars);
    //     // delay(100);
    // }
    // update_graph(bars);

}

int OLED::get_orientation() {
    return current_orientation;
}

void OLED::set_orientation(int rot) {
    current_orientation = rot;
    set_rotation(rot);
}