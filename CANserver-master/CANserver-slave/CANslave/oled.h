//
//  oled.h
//

#include "Custom_Adafruit_SSD1325.h"
#include "queue.h"

#include "ProgressBar.h"

#ifndef oled_h
#define oled_h

#define OLED_HEIGHT 64
#define OLED_WIDTH 128

#define OLED_CLK 18
#define OLED_MOSI 23

#define OLED_CS 21
#define OLED_RESET 17
#define OLED_DC 16

class OLED {
    
    private:
        
        Custom_Adafruit_SSD1325 private_display;

        int current_orientation;

    public:

        OLED();

        void clearDisplay();

        void oled_update();

        void setupOLED();

        void send_to_oled_buffer(int, String);

        void send_to_oled_buffer(int, int, String);

        void send_to_oled_buffer(int , String, String);

        void send_to_oled_buffer(int, int, int, String);

        void write_oled(int, int, String);

        bool oled_invert_color();

        void oled_image(int);

        void set_rotation(int);

        void drawLine(int16_t, int16_t, int16_t, int16_t);

        // graph stuff
        void draw_axis();

        void update_graph(my_queue&);

        void simulate_graph();

        int get_orientation();

        void set_orientation(int);

        // Custom_Adafruit_SSD1325& instance();

        void draw(ProgressBar&);
};

#endif /* oled_h */