//
//  oled.h
//

#include "Adafruit_LEDBackpack.h"

#ifndef oled_h
#define oled_h

void oled_clear();

void oled_update();

void setupOLED();

void send_to_oled_buffer(int, String);

void send_to_oled_buffer(int, int, String);

void send_to_oled_buffer(int, int, int, String);


void write_oled(int, int, String);

bool oled_invert_color();

void oled_image(int);

#endif /* oled_h */