//
//  button.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <esp_now.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>
#include "program_settings.h"

int lastSwitchValue1 = 0;
int lastSwitchValue2 = 0;

void setup_buttons() {
    // analogSetCycles(64);
    pinMode(page_button, INPUT);
    delay(100);
    // analogSetCycles(64);
    pinMode(reset_data_button, INPUT);
}

bool check_page_button(const int pin) {
    bool pressed_and_released_status = true;
    int currentSwitchVal = digitalRead(pin);
    // Serial.println("Pin: " + String(page_button) + " Status: " + String(currentSwitchVal));

    if ((lastSwitchValue1 == 1) && (currentSwitchVal == 0)) {
        pressed_and_released_status = true;
        Serial.println(String(pin) + "XXX");
    } else {
        pressed_and_released_status = false;
    }

    lastSwitchValue1 = currentSwitchVal;

    return pressed_and_released_status;
}

bool check_reset_button(const int pin) {
    bool pressed_and_released_status = true;
    int currentSwitchVal = digitalRead(pin);

    if ((lastSwitchValue2 == 1) && (currentSwitchVal == 0)) {
        pressed_and_released_status = true;
        Serial.println(String(pin) + "---");
    } else {
        pressed_and_released_status = false;
    }

    lastSwitchValue2 = currentSwitchVal;

    return pressed_and_released_status;
}
