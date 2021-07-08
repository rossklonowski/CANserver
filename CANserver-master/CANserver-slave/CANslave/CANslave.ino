#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

static bool isIdle = true;
static bool isFirstRecv = true;
static bool debug = true;

static bool displayBattPower = true;
static bool displayBattAmps = false;
static bool displayBattVolts = false;
static bool displayGrade = false;
static bool displayBattTemp = true;
static bool displayLimits = false;
static bool displayInstantaneousEfficiency = true;
static bool displayVehicleSpeed = false;

Adafruit_AlphaNum4 alpha1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha2 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha3 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha5 = Adafruit_AlphaNum4();

Adafruit_AlphaNum4 displaysArray[] = {alpha1, alpha2, alpha3, alpha4, alpha5};
static int displaysArraySize = 5;

typedef struct payload_struct {
    uint32_t can_id;
    int int_value_1;
    int int_value_2;
    int int_value_3;
    double double_value_1;
    String unit1;
    String unit2;
    String unit3;
} payload_struct;

void displayLoadingText() {

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

    // Display 5
    if (displayBattTemp) {
        alpha3.writeDigitAscii(3, 'F');  
    }

    if (displayInstantaneousEfficiency) {
        alpha1.writeDigitAscii(0, 'w');
        alpha1.writeDigitAscii(1, 'h');
        alpha1.writeDigitAscii(2, '/');
        alpha1.writeDigitAscii(3, 'm');  
    }


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
    // Serial.print("Double Data Length: ");
    // Serial.println(absLength);
    // lop off the trailing zero
    // Serial.print("Test1: ");
    // Serial.println(data[absLength - 1]);

    // find the decimal
    char key = '.';

    int decimalPointPosition = 0;
    for (int i = 0; i < data.length(); i++) {
        if (data[i] == key) {
            decimalPointPosition = i;
            break;
        }
    }
    // Serial.print("Decimal point @: ");
    // Serial.println(decimalPointPosition);

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


    ///
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

void writeLCD(payload_struct payload) { // decode message received and display to quadalphanumeric displays

    switch (payload.can_id) {
        case 0x267 : // gradeEST
            if (displayGrade) {
                printValueWithSingleUnit(payload.int_value_1, 7);
            }
            break;

        case 0x312 : // min batt temp
            if (displayBattTemp) {
                //printValueWithSingleUnit(payload.int_value_1, 7);
                printValueWithSingleUnit(payload.double_value_1, 7); 
            }
            break;

        case 0x336 : // max regen, min discharge
            if (displayLimits) {
                printValueWithSingleUnit(payload.int_value_1, 9);
            } 
            break;

        case 0x132 : // HVBattAmpVolt
            // write power to display
            if (displayBattPower) {
                printValueWithNoUnits(payload.int_value_3, 5);
            }
            break;

        case 0x000 : // instantaneousEfficiency
            // write instantaneousEfficiency
            if (displayInstantaneousEfficiency) {
                printValueWithNoUnits(payload.int_value_1, 8);
            }
            break;

        case 0x3D9 : // gps veh speed
            // write instantaneousEfficiency
            if (displayVehicleSpeed) {
                printValueWithNoUnits(payload.int_value_1, 8);
            }
            break;
    }
}

// callback function that tells us when data from Master is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

    if (isFirstRecv) {
        initDisplayUnits();

        isFirstRecv = false;
    }

    isIdle = false; // on our first received message, flip idle status

    payload_struct myData;
    memcpy(&myData, incomingData, sizeof(myData));

    if (debug) {
        Serial.print("Bytes received: ");
        Serial.println(len);

        Serial.print("can_id received: ");
        Serial.println(myData.can_id);

        Serial.print("int_value_1 received: ");
        Serial.println(myData.int_value_1);

        Serial.print("int_value_2 received: ");
        Serial.println(myData.int_value_2);

        Serial.print("int_value_3 received: ");
        Serial.println(myData.int_value_3);

        Serial.print("double_value_1 received: ");
        Serial.println(myData.double_value_1);
        
        Serial.print("Unit1 received: ");
        Serial.println(myData.unit1);

        Serial.print("Unit2 received: ");
        Serial.println(myData.unit2);

        Serial.print("Unit3 received: ");
        Serial.println(myData.unit2);

        Serial.print("\n");
    }


    // alpha1.setBrightness(15);
    // alpha1.writeDisplay();
    // alpha2.setBrightness(15);
    // alpha2.writeDisplay();
    // alpha3.setBrightness(15);
    // alpha3.writeDisplay();
    // alpha4.setBrightness(15);
    // alpha4.writeDisplay();
    // alpha5.setBrightness(15);
    // alpha5.writeDisplay();

    writeLCD(myData); // decode message received and display to quadalphanumeric displays
}

void setup() {

    Serial.begin(115200);
    delay(250);
 
    // put esp32 in WIFI station mode 
    WiFi.mode(WIFI_STA);
    Serial.print("Mac Address in Station: "); 
    Serial.println(WiFi.macAddress());

    // begin each display
    alpha1.begin(0x70);  // pass in the address
    alpha2.begin(0x71);  // pass in the address
    alpha3.begin(0x72);  // pass in the address
    alpha4.begin(0x73);  // pass in the address
    alpha5.begin(0x74);  // pass in the address

    // display loading animation
    displayLoadingText();
    
    // init esp now (connection to slave wia wifi)
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
    return;
    }

    // create call back (OnDataRecv will run every time a message is received via esp now)
    esp_now_register_recv_cb(OnDataRecv);
}

// nothing to do in loop - program is asynchronous
void loop() {
}