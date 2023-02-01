//
//  oled.h
//

// #include "Adafruit_LEDBackpack.h"
#include <Adafruit_SSD1325.h>

#ifndef oled_h
#define oled_h

#define OLED_HEIGHT 64
#define OLED_WIDTH 128

#define OLED_CLK 18
#define OLED_MOSI 23

#define OLED_CS 21
#define OLED_RESET 17
#define OLED_DC 16

void oled_clear();

void oled_update();

void setupOLED();

void send_to_oled_buffer(int, String);

void send_to_oled_buffer(int, int, String);

void send_to_oled_buffer(int, int, int, String);

void write_oled(int, int, String);

bool oled_invert_color();

int oled_set_rotation(int);

void oled_image(int);

//////////////////////////////////////////////
void draw_axis();

void update_graph(int values_array[], int bars);

void simulate_graph();

//////////////////////////////////////////////

// class OLED {
//     private:

//         Adafruit_SSD1325 oled_1;

//         int ross;

//     public:
//         OLED();

        // void sample(Adafruit_SSD1325);

        // void clearDisplay();

        // void oled_update();

        // void setupOLED();

        // void send_to_oled_buffer(int, String);

        // void send_to_oled_buffer(int, int, String);

        // void send_to_oled_buffer(int, int, int, String);

        // void write_oled(int, int, String);

        // bool oled_invert_color();

        // void oled_image(int);
// };

#endif /* oled_h */