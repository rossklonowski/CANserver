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

int lastSwitchValue = 0;

bool checkButton() {

    analogSetCycles(64);

    const int pin = 36;
    const int led1pin = 9;

    pinMode(pin, INPUT);
    pinMode(led1pin, OUTPUT);

    int currentSwitchVal = digitalRead(pin);
    if (currentSwitchVal == 1) {
        digitalWrite(led1pin, HIGH);
        //Serial.print("High");
    } else {
        digitalWrite(led1pin, LOW);
    }

    if ((lastSwitchValue == 1) && (currentSwitchVal == 0)) {
        Serial.print("Toggled");
        return true;
    } else {
        return false;
    }

    //Serial.println("Value:" + String(currentSwitchVal));
    lastSwitchValue = currentSwitchVal;
}
