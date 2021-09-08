//
//  alphanumeric.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <esp_now.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>

#include "alphanumeric.h"

static bool displayBattPower = true;
static bool displayBattAmps = false;
static bool displayBattVolts = false;
static bool displayGrade = false;
static bool displayBattTemp = false;
static bool displayLimits = false;
static bool displayInstantaneousEfficiency = true;
static bool displayVehicleSpeed = false;
static bool displayZeroToSixty = false;
static bool displaySpeed = false;
static bool displayFrontPower = true;
static bool displayRearPower = true;

Adafruit_AlphaNum4 alpha1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha2 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha3 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha5 = Adafruit_AlphaNum4();

Adafruit_AlphaNum4 displaysArray[] = {alpha1, alpha2, alpha3, alpha4, alpha5};
static int displaysArraySize = 5;

void setupAlphaNum() {
    // begin each display
    alpha1.begin(0x70);  // pass in the address
    alpha2.begin(0x71);  // pass in the address
    alpha3.begin(0x72);  // pass in the address
    alpha4.begin(0x73);  // pass in the address
    alpha5.begin(0x74);  // pass in the address
}

void displayLoadingAnimationAlphaNum() {

    for (int i = 5; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            displaysArray[i].writeDigitRaw(j, 0xFFFF);
        }
        displaysArray[i].writeDisplay();
    }
    
    delay(1000);
    
    displaysArray[5].writeDigitAscii(0, 'S');
    displaysArray[5].writeDigitAscii(1, 'T');
    displaysArray[5].writeDigitAscii(2, 'A');
    displaysArray[5].writeDigitAscii(3, 'R');

    displaysArray[6].writeDigitAscii(0, 'T');
    displaysArray[6].writeDigitAscii(1, 'I');
    displaysArray[6].writeDigitAscii(2, 'N');
    displaysArray[6].writeDigitAscii(3, 'G');

    displaysArray[7].writeDigitAscii(0, 46, true);
    displaysArray[7].writeDigitAscii(1, 46, true);
    displaysArray[7].writeDigitAscii(2, 46, true);
    displaysArray[7].writeDigitAscii(3, 46, true);

    displaysArray[8].writeDigitAscii(0, 46, true);
    displaysArray[8].writeDigitAscii(1, 46, true);
    displaysArray[8].writeDigitAscii(2, 46, true);
    displaysArray[8].writeDigitAscii(3, 46, true);

    displaysArray[9].writeDigitAscii(0, 46, true);
    displaysArray[9].writeDigitAscii(1, 46, true);
    displaysArray[9].writeDigitAscii(2, 46, true);
    displaysArray[9].writeDigitAscii(3, 46, true);

    // displaysArray[5].writeDigitRaw(0, 0x8000);
    // displaysArray[5].writeDigitRaw(1, 0x4000);
    // displaysArray[5].writeDigitRaw(2, 0x2000);
    // displaysArray[5].writeDigitRaw(3, 0x1000);

    // displaysArray[6].writeDigitRaw(0, 0x800);
    // displaysArray[6].writeDigitRaw(1, 0x400);
    // displaysArray[6].writeDigitRaw(2, 0x200);
    // displaysArray[6].writeDigitRaw(3, 0x100);

    // displaysArray[7].writeDigitRaw(0, 0x80);
    // displaysArray[7].writeDigitRaw(1, 0x40);
    // displaysArray[7].writeDigitRaw(2, 0x20);
    // displaysArray[7].writeDigitRaw(3, 0x10);

    // displaysArray[8].writeDigitRaw(0, 0x8);
    // displaysArray[8].writeDigitRaw(1, 0x4);
    // displaysArray[8].writeDigitRaw(2, 0x2);
    // displaysArray[8].writeDigitRaw(3, 0x1);

    // displaysArray[5].writeDigitRaw(0, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x400 | 0x800);    // 0.
    // displaysArray[5].writeDigitRaw(1, 0x4000 | 0x2 | 0x4 );                                             // 1.
    // displaysArray[5].writeDigitRaw(2, 0x4000 | 0x1 | 0x2 | 0x8 | 0x10 | 0x40 | 0x80);                   // 2.
    // displaysArray[5].writeDigitRaw(3, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x80);                           // 3.
    // displaysArray[6].writeDigitRaw(0, 0x4000 | 0x2 | 0x4 | 0x20 | 0x40 | 0x80);                         // 4.
    // displaysArray[6].writeDigitRaw(1, 0x4000 | 0x1 | 0x8 | 0x20 | 0x40 | 0x2000);                       // 5.
    // displaysArray[6].writeDigitRaw(2, 0x4000 | 0x1 | 0x4 | 0x8 | 0x10| 0x20 | 0x40 | 0x80);             // 6.
    // displaysArray[6].writeDigitRaw(3, 0x4000 | 0x1 | 0x2 | 0x4 );                                       // 7.
    // displaysArray[7].writeDigitRaw(0, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80);      // 8.
    // displaysArray[7].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x20 | 0x40 | 0x80);                   // 9.


    for (int i = 5; i < 10; i++) {
        displaysArray[i].writeDisplay();
    }

    delay(2000);

    for (int j = 5; j < 10; j++) {
        for (int k = 0; k < 4; k++) {
            displaysArray[j].writeDigitRaw(k, 0x0);
            displaysArray[j].writeDisplay();
            delay(15);
        }
    }
}

void clearDisplays() {
    for (int i = 5; i < 10; i++) {
        displaysArray[i].clear();
        displaysArray[i].writeDisplay();
    }
}

void writeDisplays() {
    for (int i = 5; i < 10; i++) {
        displaysArray[i].writeDisplay();
    }
}

void initDisplayUnits() {

    // Display 2
    if (displayBattPower) {
        alpha4.writeDigitAscii(0, 'K'); 
        alpha4.writeDigitAscii(1, 'W'); 
    }

    // Display 3
    if (displayGrade) { 
        alpha3.writeDigitAscii(3, '%'); 
    }

    // // Display 5
    // if (displayBattTemp) {
    //     lcd.setCursor(0, 0);
    //     lcd.print("Batt Temp:");
    //     //alpha3.writeDigitAscii(3, 'F');  
    // }

    if (displayInstantaneousEfficiency) {
        alpha1.writeDigitAscii(0, 'w');
        alpha1.writeDigitAscii(1, 'h');
        alpha1.writeDigitAscii(2, '/');
        alpha1.writeDigitAscii(3, 'm');  
    }

    // if (displayInstantaneousEfficiency) {
    //     alpha3.writeDigitAscii(3, 'S'); 
    // }

    // if (displayZeroToSixty) {
    //     lcd.setCursor(0, 1);
    //     lcd.print("0-60:");
    // }

    writeDisplays();
}

void printValueWithSingleUnit(int val, int display) {

    String data = String(abs(val), 10); // Convert absolute value to string
    int absLength = data.length();

    switch(absLength) {
        case 0  :
            // if empty do nothing
            break;
        case 1  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitAscii(1, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
            }

            displaysArray[display].writeDigitAscii(2, data[0]);    
            break;

        case 2  :
            if (val < 0) {
                displaysArray[display].writeDigitAscii(0, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
            }

            for (int i = 0; i < absLength; i++) {
                displaysArray[display].writeDigitAscii(i + 1, data[i]);
            }
            break;
        
        case 3  :
            for (int i = 0; i < absLength; i++) {
                displaysArray[display].writeDigitAscii(i, data[i]);
            }
            break;
    }  
            displaysArray[display].writeDisplay();
}

void printValueWithSingleUnit(double val, int display) {

    String data = String(abs(val), 10); // Convert absolute value to string
    int absLength = data.length();
    Serial.print("Double data: ");
    Serial.println(data);
    Serial.print("Double Data Length: ");
    Serial.println(absLength);
    // lop off the trailing zero
    Serial.print("Test1: ");
    Serial.println(data[absLength - 1]);

    // find the decimal
    char key = '.';

    int decimalPointPosition = 0;
    for (int i = 0; i < data.length(); i++) {
        if (data[i] == key) {
            decimalPointPosition = i;
            break;
        }
    }
    Serial.print("Decimal point @: ");
    Serial.println(decimalPointPosition);

    bool isDone = false;
    while (!isDone) {
        int length = data.length();
        if (length > decimalPointPosition + 3) {
            data.remove(data.length() - 1);
        } else {
            isDone = true;
        }
    }

    Serial.print("Final Cleaned double data: ");
    Serial.println(data);

    char zero = '0';
    if (data[data.length() - 1] == zero) {
        data.remove(data.length() - 1);
    }

    if (data[data.length() - 1] == zero) {
        data.remove(data.length() - 1);
        data.remove(data.length() - 1); // remove the decimal
    }


    Serial.print("Final final Cleaned double data: ");
    Serial.println(data);
    
    
    String lastVal = data.substring(absLength, absLength);
    if (lastVal.toInt() == 0) {
        data.remove(absLength - 1);
    }
    Serial.print("Double data after: ");
    Serial.println(data);

    for (int i = 0; i < data.length(); i++) {
        if (data[i] == key) {
            decimalPointPosition = i;
            Serial.print("Going to remove:");
            Serial.println(data[i]);
            data.remove(i, 1);
        }
    }

    Serial.print("Final num to be printed: ");
    Serial.println(data);

    if (decimalPointPosition == 1) {
        displaysArray[7].writeDigitRaw(0, 0x80);
    } else if (decimalPointPosition == 2) {
        displaysArray[7].writeDigitRaw(1, 0x80);
    }

    absLength = data.length();

    if (absLength == 4) {
        data.remove(absLength - 1);
    }

    absLength = data.length();

    switch(absLength) {
        case 0  :
            // if empty do nothing
            break;
        case 1  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitAscii(1, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
            }

            displaysArray[display].writeDigitAscii(2, data[0]);    
            break;

        case 2  :
            Serial.println("Case 2");
            if (val < 0) {
                displaysArray[display].writeDigitAscii(0, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
            }

            for (int i = 0; i < absLength; i++) {
                Serial.print(i);
                Serial.print("Data ");
                Serial.print(data[i]);
    
                if (i == 0) {
                    Serial.println("Print with decimal!!");
                    if (data[i] == '0') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x400 | 0x800);    // 0.
                    } else if (data[i] == '1') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x2 | 0x4 );                                             // 1.
                    } else if (data[i] == '2') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x8 | 0x10 | 0x40 | 0x80);                   // 2.
                    } else if (data[i] == '3') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x80);                           // 3.
                    } else if (data[i] == '4') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x2 | 0x4 | 0x20 | 0x40 | 0x80);                         // 4.
                    } else if (data[i] == '5') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x8 | 0x20 | 0x40 | 0x2000);                       // 5.
                    } else if (data[i] == '6') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x4 | 0x8 | 0x10| 0x20 | 0x40 | 0x80);             // 6.
                    } else if (data[i] == '7') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 );                                       // 7.
                    } else if (data[i] == '8') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80);      // 8.
                    } else if (data[i] == '9') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x20 | 0x40 | 0x80);                   // 9.
                    }
                }
                else {
                    displaysArray[display].writeDigitAscii(i + 1, data[i]);
                }
            }
            break;
        
        case 3  :
            Serial.println("Case 3");
            for (int i = 0; i < absLength; i++) {
                //displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80);      // 8.
                // Serial.print("Quad num i:");
                // Serial.println(i);
                // Serial.print("Data on this quad num:");
                // Serial.println(data[i]);    
                if (i == 1) {
                    if (data[i] == '0') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x400 | 0x800);    // 0.
                    } else if (data[i] == '1') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x2 | 0x4 );                                             // 1.
                    } else if (data[i] == '2') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x8 | 0x10 | 0x40 | 0x80);                   // 2.
                    } else if (data[i] == '3') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x80);                           // 3.
                    } else if (data[i] == '4') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x2 | 0x4 | 0x20 | 0x40 | 0x80);                         // 4.
                    } else if (data[i] == '5') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x8 | 0x20 | 0x40 | 0x2000);                       // 5.
                    } else if (data[i] == '6') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x4 | 0x8 | 0x10| 0x20 | 0x40 | 0x80);             // 6.
                    } else if (data[i] == '7') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 );                                       // 7.
                    } else if (data[i] == '8') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80);      // 8.
                    } else if (data[i] == '9') {
                        displaysArray[display].writeDigitRaw(1, 0x4000 | 0x1 | 0x2 | 0x4 | 0x20 | 0x40 | 0x80);                   // 9.
                    }
                } else {
                    displaysArray[display].writeDigitAscii(i, data[i]);
                }
            }
            break;
        // case 4  :
        //     for (int i = 0; i < absLength; i++) {
        //         displaysArray[display].writeDigitAscii(i, data[i]);
        //     }
        //     break;
    }
    displaysArray[display].writeDisplay();
}

void printValueWithDoubleUnit(int val, int display) {

    String data = String(abs(val), 10); // Convert absolute value to string
    int absLength = data.length();

    switch(absLength) {
        case 0  :
            // if empty do nothing
            break;
        case 1  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitAscii(1, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
            }

            displaysArray[display].writeDigitAscii(2, data[0]);    
            break;

        case 2  :
            if (val < 0) {
                displaysArray[display].writeDigitAscii(0, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
            }

            for (int i = 0; i < absLength; i++) {
                displaysArray[display].writeDigitAscii(i + 1, data[i]);
            }
            break;
        
        case 3  :
            for (int i = 0; i < absLength; i++) {
                displaysArray[display].writeDigitAscii(i, data[i]);
            }
            break;
    }  
    displaysArray[display].writeDisplay();
}

void printValueWithNoUnits(int val, int display) {
    
    String data = String(abs(val), 10); // Convert absolute value to string
    int absLength = data.length();

    switch(absLength) {
        case 0  :
            // if empty do nothing
            break;
        case 1  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
                displaysArray[display].writeDigitAscii(2, '-');
                displaysArray[display].writeDigitAscii(3, data[0]); // write the payload  
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
                displaysArray[display].writeDigitRaw(2, 0x0);
                displaysArray[display].writeDigitAscii(3, data[0]); // write the payload  
            }
            break;

        case 2  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitAscii(1, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
            }

            for (int i = 0; i < 2; i++) {
                displaysArray[display].writeDigitAscii(i + 2, data[i]);
            }

            break;
        
        case 3  :
            if (val < 0) {
                displaysArray[display].writeDigitAscii(0, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
            }

            for (int i = 0; i < 3; i++) {
                displaysArray[display].writeDigitAscii(i + 1, data[i]);
            }

            break;
    }  
    displaysArray[display].writeDisplay();
}

void printValueWithNoUnits(double val, int display) {
    
    String data = String(abs(val), 10); // Convert absolute value to string
    int absLength = data.length();

    switch(absLength) {
        case 0  :
            // if empty do nothing
            break;
        case 1  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
                displaysArray[display].writeDigitAscii(2, '-');
                displaysArray[display].writeDigitAscii(3, data[0]); // write the payload  
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
                displaysArray[display].writeDigitRaw(2, 0x0);
                displaysArray[display].writeDigitAscii(3, data[0]); // write the payload  
            }
            break;

        case 2  :
            if (val < 0) {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitAscii(1, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
                displaysArray[display].writeDigitRaw(1, 0x0);
            }

            for (int i = 0; i < 2; i++) {
                displaysArray[display].writeDigitAscii(i + 2, data[i]);
            }

            break;
        
        case 3  :
            if (val < 0) {
                displaysArray[display].writeDigitAscii(0, '-');
            } else {
                displaysArray[display].writeDigitRaw(0, 0x0);
            }

            for (int i = 0; i < 3; i++) {
                displaysArray[display].writeDigitAscii(i + 1, data[i]);
            }

            break;
    }  
    displaysArray[display].writeDisplay();
}
