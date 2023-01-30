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

void write_oled(int, int, String);

#endif /* oled_h */