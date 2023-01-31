//
//  button.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <ezButton.h>
#include <esp_now.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>
#include "program_settings.h"

// ezButton page_button(page_button_pin);
// ezButton invert_color_button(invert_color_button_pin);
// ezButton reset_button(reset_data_button_pin);

// void setup_buttons() {
//     invert_color_button.loop();
//     reset_button.loop();
//     page_button.loop();
// }

// bool was_button_pressed(String button) {
//     bool pressed = false;
//     if (button == "reset") {
//         pressed = reset_button.isPressed();
//     } else if (button == "invert") {
//         pressed = invert_color_button.isPressed();
//     } else if (button == "page") {
//         pressed = page_button.isPressed();
//     }
//     return pressed;
// }
