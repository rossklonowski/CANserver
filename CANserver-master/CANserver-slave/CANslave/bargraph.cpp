//
//  button.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>
#include "bargraph.h"
#include <esp_now.h> // for ESP32 to ESP32 wifi communication

Adafruit_24bargraph BarGraph1 = Adafruit_24bargraph();
Adafruit_24bargraph BarGraph2 = Adafruit_24bargraph();

Adafruit_24bargraph BarGraphsArray[] = { BarGraph1, BarGraph2 };
static int BarGraphsArraySize = 2;

bool isFlipped = true;

void sendToBarGraphPower(String frontOrRear, int power_temp, int dischargeLimit, int regenLimit) {

    String scaling = "logarithmic";

    double power = power_temp + 0.001;

    int bar = -1;
    if (frontOrRear == "front") {
        bar = 0;
    } else if (frontOrRear == "rear") {
        bar = 1;
    }

    int barsToLightUp = 0;
    int powerLimit = 0;                     // regen or discharge
    int powerBars = 24;                     // number of bars on bargraph

    bool isPositive = true;
    if (power >= 0.0) {                    // if power is being dicharged
        isPositive = true;
        powerLimit = dischargeLimit;        // use dicharge limit
    } else {                                // power is being regen'd
        isPositive = false;
        powerLimit = regenLimit;            // use regen limit
    }

    power = abs(power);  
    
    double percentageOfBar = 0;
    if (scaling == "linear") {                                             // get absoulute value to determine the amount of bars to light up
        percentageOfBar = (double)power  / (double)powerLimit; // get the ratio of regen/discharge power to the regen/discharge limit
        barsToLightUp = round((double)powerBars * percentageOfBar);  // get number of bars to light up
    } else if (scaling == "logarithmic") {
        percentageOfBar = log10((double)power) / log10((double)powerLimit); // get the ratio of regen/discharge power to the regen/discharge limit
        barsToLightUp = round((double)powerBars * percentageOfBar);  // get number of bars to light up
    }

    // Serial.println("bar: " + String(bar));
    // Serial.println("regenLimit: " + String(regenLimit));
    // Serial.println("dischargeLimit: " + String(dischargeLimit));
    // Serial.println("power: " + String(power));
    // Serial.println("percentageOfBar: " + String(percentageOfBar));
    // Serial.println("barsToLightUp: " + String(barsToLightUp));

    if (isPositive) { 
        setBarGraphValue(bar, barsToLightUp, LED_RED);
    } else {
        setBarGraphValue(bar, barsToLightUp, LED_GREEN); 
    }
}

void setBarGraphValue(int display, int value) {

    if (!isFlipped) {
        // write the populated bars
        for (int i = 0; i < value; i++) {
            BarGraphsArray[display].setBar(i, LED_RED);
        }

        // write the unpopulated bars
        for (int i = value; i < 24; i++) {
            BarGraphsArray[display].setBar(i, LED_OFF);
        }

        writeBarGraphs(); // update
    } else {
        // write the populated bars
        for (int i = 23; i > 23 - value; i--) {
            BarGraphsArray[display].setBar(i, LED_RED);
        }

        // write the unpopulated bars
        for (int i = 23 - value; i >= 0; i--) {
            BarGraphsArray[display].setBar(i, LED_OFF);
        }

        writeBarGraphs(); // update 
    }
}

void setBarGraphValue(int display, int value, uint8_t color) {

    // this is to protect against crashing the displays
    if (value <= 0) {
        value = 0;
    }

    // Serial.println("Display: " + String(display) + "Value: " + String(value) + "Color: " + String(color));

    if (!isFlipped) {
        // write the populated bars
        for (int i = 0; i < value; i++) {
            BarGraphsArray[display].setBar(i, color);
        }

        // write the unpopulated bars
        for (int i = value; i < 24; i++) {
            BarGraphsArray[display].setBar(i, LED_OFF);
        }
        writeBarGraphs(); // update
    } else {
        // write the populated bars
        for (int i = 23; i > 23 - value; i--) {
            BarGraphsArray[display].setBar(i, color);
        }

        // write the unpopulated bars
        for (int i = 23 - value; i >= 0; i--) {
            BarGraphsArray[display].setBar(i, LED_OFF);
        }
        writeBarGraphs(); // update 
    }
}

void writeBarGraphs() {
    // write
    for (int i = 0; i < BarGraphsArraySize; i++) {
        BarGraphsArray[i].writeDisplay(); 
    }
}

void clearBarGraphs() {  
    // clear
    for (int i = 0; i < BarGraphsArraySize; i++) {
        BarGraphsArray[i].clear(); 
    }

    // write
    writeBarGraphs();
}

void setupBarGraphs() {
    BarGraphsArray[0].begin(0x77);
    BarGraphsArray[0].blinkRate(0);
    BarGraphsArray[0].setBrightness(15);
    
    BarGraphsArray[1].begin(0x76);
    BarGraphsArray[1].blinkRate(0);
    BarGraphsArray[1].setBrightness(15);
}

void displayLoadingAnimationBarGraph() {   
    
    int animationDelay = 20;
    
    for (int i = 0; i < 24; i++) {
        setBarGraphValue(0, i, LED_RED);
        setBarGraphValue(1, i, LED_RED);
        writeBarGraphs();
        delay(animationDelay);
    }

    for (int i = 23; i >= 0; i--) {
        setBarGraphValue(0, i, LED_YELLOW);
        setBarGraphValue(1, i, LED_YELLOW);
        writeBarGraphs();
        delay(animationDelay);
    }

    for (int i = 0; i < 24; i++) {
        setBarGraphValue(0, i, LED_GREEN);
        setBarGraphValue(1, i, LED_GREEN);
        writeBarGraphs();
        delay(animationDelay);
    }
    clearBarGraphs();

    // for (int i = 0; i < 12; i++) {
    //     if (i % 2 == 0) {           // if even
    //         setBarGraphValue(0, i);
    //         setBarGraphValue(1, i);
    //     } else {                    // if odd
    //         setBarGraphValue(0, i);
    //         setBarGraphValue(1, i);
    //     }
    //     writeBarGraphs();
    //     delay(animationDelay);


    //     if (i % 2 == 0) {           // if even
    //         setBarGraphValue(0, 23 - i);
    //         setBarGraphValue(1, 23 - i);
    //     } else {                    // if odd
    //         setBarGraphValue(0, 23 - i);
    //         setBarGraphValue(1, 23 - i);
    //     }
    //     writeBarGraphs();
    //     delay(animationDelay);
    // }
    // clearBarGraphs();
}
