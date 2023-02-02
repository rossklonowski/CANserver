//
//  graph.cpp
//
//

// #include <Adafruit_Sensor.h> // allows serial to work??
// #include "oled.h"

// int pixels_from_side = 10;

// void draw_axis() {
//     oled.drawLine(OLED_WIDTH - pixels_from_side, pixels_from_side, OLED_WIDTH - pixels_from_side, OLED_HEIGHT - pixels_from_side, WHITE); // y axis
//     oled.drawLine(pixels_from_side, pixels_from_side, OLED_WIDTH - pixels_from_side, pixels_from_side, WHITE); // x axis
// }

// void update_graph(int values_array[], int bars) {
    
//     oled.clearDisplay();
    
//     draw_axis(oled);

//     for (int bar = 0; bar < bars; bar++) {
//         int bar_x_val = OLED_WIDTH - pixels_from_side - bar - 2;
//         oled.drawLine(bar_x_val, pixels_from_side, bar_x_val, values_array[bar], WHITE); // test data 
//     }

//     oled.oled_update();
// }

// void simulate_graph() {
//     int bars = OLED_WIDTH - (pixels_from_side * 2) - 2;
//     Serial.println("Data Bars: " + String(bars));
//     int values_array[bars] = { 0 };

//     for (int bar_val = pixels_from_side; bar_val < OLED_HEIGHT - (pixels_from_side * 2) - 10; bar_val++) {
        
//         // fill up values_array
//         for (int bar = 0; bar < bars; bar++) {
            
//             // int temp = (bar_val*0.5) + (bar*0.5);
//             // if (temp <= pixels_from_side) {
//             //     temp = temp + pixels_from_side;
//             // }
//             // if (temp >= (OLED_HEIGHT - (pixels_from_side))) {
//             //     temp = OLED_HEIGHT - (pixels_from_side);
//             // }
//             int temp = 4;
//             values_array[bar] = temp;
//         }

//         // update each bar with the new val
//         update_graph(oled, values_array, bars);
//         // delay(100);
//     }
// }
