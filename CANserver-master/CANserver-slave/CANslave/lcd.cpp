//
//  button.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <LiquidCrystal.h>
#include "lcd.h"


// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(19, 18, 5, 17, 16, 4);

void lcdPrint(String content) {
    int whiteSpaceCount = 16 - content.length();

    String whiteSpaceText = "";
    for (int i = 0; i < whiteSpaceCount; i++) {
        whiteSpaceText += " ";
    }

    lcd.print(content + whiteSpaceText);
}

void sendToLCD(uint8_t row, int value, String field, String units) {
    lcd.setCursor(0, row); // columns rows
    lcdPrint(field + " " + String(value) + units);
}

void sendToLCD(uint8_t row, double value, String field, String units) {
    lcd.setCursor(0, row); // columns rows
    lcdPrint(field + " " + String(value) + units);
}

void sendToLCD(uint8_t row, int value1, String field1, String units1, int value2, String field2, String units2) {
    lcd.setCursor(0, row);
    lcdPrint(field1 + " " + String(value1) + units1 + " " + field2 + String(value2) + " " + units2);
}

void sendToLCD(uint8_t row, int value1, String field1, String units1, int value2, String field2, String units2, int value3, String field3, String units3) {
    lcd.setCursor(0, row);
    lcdPrint(field1 + " " + String(value1) + units1 + " " + field2 + " " + String(value2) + units2 + " " + field3 + " " + String(value3) + String(units3));
}

void sendToLCD(uint8_t row, String string) {
    lcd.setCursor(0, row);
    lcdPrint(string);
}

void sendToLCD(uint8_t row1, String string1, uint8_t row2, String string2) {
    lcd.setCursor(0, row1);
    lcdPrint(string1);

    lcd.setCursor(0, row2);
    lcdPrint(string2);
}


void setupLCD() {
    // set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
}

void displayLoadingAnimationLCD() {

    int animationDelay = 20;
    String startupText = "Starting...";

    lcd.setCursor(0, 0);
    for (int i = 0; i < startupText.length(); i++) {
            lcd.print(startupText[i]);
            delay(animationDelay);
    }
    delay(500);
 
    // String temp = "";
    // int textLength = startupText.length(); 
    // for (int i = 0; i < textLength; i++) {
    //     for (int i = 0; i < startupText.length(); i++) {
    //             temp = temp + startupText[i];
    //     }
    //         lcd.print(startupText[i]);
    //         delay(animationDelay);
    //         //lcd.clear();
    // }
    lcd.clear();
}
