// #include <LiquidCrystal.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

bool isIdle = true;
bool isFirstRecv = true;

static int displaysArraySize = 3;

Adafruit_AlphaNum4 alpha1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha2 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha3 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha5 = Adafruit_AlphaNum4();

Adafruit_AlphaNum4 displaysArray[] = {alpha1, alpha2, alpha3};

typedef struct payload_struct {
    uint32_t can_id;
    int int_value_1;
    int int_value_2;
    int int_value_3;
    String unit1;
    String unit2;
    String unit3;
} payload_struct;

void displayLoadingText(Adafruit_AlphaNum4 &disp1, Adafruit_AlphaNum4 &disp2, Adafruit_AlphaNum4 &disp3, Adafruit_AlphaNum4 &disp4, Adafruit_AlphaNum4 &disp5) {

    for (int i = 0; i < 4; i++) {
        disp1.writeDigitRaw(i, 0xFFFF);
        disp2.writeDigitRaw(i, 0xFFFF);
        disp3.writeDigitRaw(i, 0xFFFF);
        disp4.writeDigitRaw(i, 0xFFFF);
        disp5.writeDigitRaw(i, 0xFFFF);
    }

    disp1.writeDisplay();
    disp2.writeDisplay();
    disp3.writeDisplay();
    disp4.writeDisplay();
    disp5.writeDisplay();

    delay(1000);
    
    disp5.writeDigitAscii(0, 'S');
    disp5.writeDigitAscii(1, 'T');
    disp5.writeDigitAscii(2, 'A');
    disp5.writeDigitAscii(3, 'R');

    disp4.writeDigitAscii(0, 'T');
    disp4.writeDigitAscii(1, 'I');
    disp4.writeDigitAscii(2, 'N');
    disp4.writeDigitAscii(3, 'G');

    disp3.writeDigitAscii(0, 46, true);
    disp3.writeDigitAscii(1, 46, true);
    disp3.writeDigitAscii(2, 46, true);
    disp3.writeDigitAscii(3, 46, true);

    disp2.writeDigitAscii(0, 46, true);
    disp2.writeDigitAscii(1, 46, true);
    disp2.writeDigitAscii(2, 46, true);
    disp2.writeDigitAscii(3, 46, true);

    disp1.writeDigitAscii(0, 46, true);
    disp1.writeDigitAscii(1, 46, true);
    disp1.writeDigitAscii(2, 46, true);
    disp1.writeDigitAscii(3, 46, true);

    disp1.writeDisplay();
    disp2.writeDisplay();
    disp3.writeDisplay();
    disp4.writeDisplay();
    disp5.writeDisplay();
}

void clearDisplays() {

    alpha1.clear();
    alpha2.clear();
    alpha3.clear();
    alpha4.clear();
    alpha5.clear();
    
    alpha1.writeDisplay();
    alpha2.writeDisplay();
    alpha3.writeDisplay();
    alpha4.writeDisplay();
    alpha5.writeDisplay();

    // for (int j = 0; j < displaysArraySize; j++) {
    //     for (int k = 0; k < 4; k++) {
    //         displaysArray[j].writeDigitRaw(k, 0x0);
    //         displaysArray[j].writeDisplay();
    //     }
    // }

    // for (int j = 0; j < 3; j++) {
    //     for (int k = 0; k < 4; k++) {
    //         displaysArray[j].writeDigitRaw(k, 0x0);
    //         displaysArray[j].writeDisplay();
    //     }
    // }
}

void begin_loading_animation() {

    Serial.println("In function!");

    Adafruit_AlphaNum4 displays[] = {alpha3, alpha2, alpha1};

    while(isIdle) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                displays[j].writeDigitRaw(k, 0x3FFF);
                displays[j].writeDisplay();
                delay(500);
                displays[j].writeDigitRaw(k, 0x0);
                displays[j].writeDisplay();
                Serial.print("display: ");
                Serial.print(j);
                Serial.print("Segment: ");
                Serial.print(k);
                Serial.println();
                if (!isIdle) {
                    return;
                }
            }
        }
    }
    Serial.println("End of function!");
}

void initDisplayUnits() {

    alpha4.writeDigitAscii(0, 'K'); 
    alpha4.writeDigitAscii(1, 'W'); 
 
    alpha3.writeDigitAscii(3, 'V');

    alpha2.writeDigitAscii(3, 'A');

    alpha1.writeDigitAscii(3, 'F'); 

    alpha1.writeDisplay();
    alpha2.writeDisplay();
    alpha3.writeDisplay();
    alpha4.writeDisplay();
}

void writeLCD(payload_struct payload) { // decode message received and display to quadalphanumeric displays

    switch (payload.can_id) {

        case 0x312 : { // min batt temp
             // handle negative values
            if (payload.int_value_1 < 0) {
                alpha1.writeDigitAscii(0, '-');    
            }
            else {
                alpha1.writeDigitRaw(0, 0x0);
            }

            // write value to quadalphanum
            String data = String(abs(payload.int_value_1), 10); // Convert absolute value to string 
            
            if (data.length() == 1) {
                alpha1.writeDigitAscii(2, data[0]);
                alpha1.writeDigitRaw(1, 0x0);
            } else if (data.length() == 2) {
                for (int i = 0; i < data.length(); i++) {
                    alpha1.writeDigitAscii(i + 1, data[i]);
                }
            } else if (data.length() == 3) {
                for (int i = 0; i < data.length(); i++) {
                    alpha1.writeDigitAscii(i, data[i]);
                }
            }

            alpha1.writeDisplay();
        }
            break;

        case 0x132 : { // HVBattAmpVolt

            // write power value to quadalphanum
            String data0 = String(abs(payload.int_value_3), 10); // Convert absolute value to string 

            if (data0.length() == 2) {
                alpha5.writeDigitRaw(3, 0x0);
            } else if (data0.length() == 1) {
                alpha5.writeDigitRaw(2, 0x0);
                alpha5.writeDigitRaw(3, 0x0);
            }

            // handle negative sign
            if (payload.int_value_3 < 0) {      // if negative num
                alpha5.writeDigitAscii(0, '-'); // write negative sign    
            } else {                            // if positive num
                alpha5.writeDigitRaw(0, 0x0);    // clear negative sign
            }

            for (int i = 0; i < data0.length(); i++) {
                alpha5.writeDigitAscii(i + 1, data0[i]);
            }

            alpha5.writeDisplay(); // finally write to display

            // write volts value to quadalphanum
            String data1 = String(abs(payload.int_value_1), 10); // Convert absolute value to string 
            
            if (data1.length() == 2) {
                alpha3.writeDigitRaw(2, 0x0);
            } else if (data0.length() == 1) {
                alpha3.writeDigitRaw(1, 0x0);
                alpha3.writeDigitRaw(2, 0x0);
            }

            for (int i = 0; i < data1.length(); i++) {
                alpha3.writeDigitAscii(i, data1[i]);
            }
            
            alpha3.writeDisplay();

            // write amps value to quadalphanum
            String data2 = String(abs(payload.int_value_2), 10); // Convert absolute value to string 

            if (data0.length() == 1) {
                alpha2.writeDigitRaw(2, 0x0);
            }

            // handle neg sign
            if (payload.int_value_2 < 0) {
                alpha2.writeDigitAscii(0, '-');    
            } else {
               alpha2.writeDigitRaw(0, 0x0); 
            }

            if (abs(payload.int_value_2) < 100) { // handle writing num
                for (int i = 0; i < data2.length(); i++) {
                    alpha2.writeDigitAscii(i + 1, data2[i]);
                }
            } else {
                for (int i = 0; i < data2.length(); i++) {
                    alpha2.writeDigitAscii(i, data2[i]);
                }
            }

            alpha2.writeDisplay();
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

    Serial.print("Bytes received: ");
    Serial.println(len);

    Serial.print("can_id received: ");
    Serial.println(myData.can_id);

    Serial.print("int_value_1 received: ");
    Serial.println(myData.int_value_1);

    Serial.print("int_value_2 received: ");
    Serial.println(myData.int_value_2);
    
    Serial.print("Unit1 received: ");
    Serial.println(myData.unit1);

    Serial.print("Unit2 received: ");
    Serial.println(myData.unit2);

    Serial.print("\n");

    writeLCD(myData); // decode message received and display to quadalphanumeric displays
}

void setup() {

    Serial.begin(115200);
    delay(250);
 
    WiFi.mode(WIFI_STA);
    Serial.print("Mac Address in Station: "); 
    Serial.println(WiFi.macAddress());

    alpha1.begin(0x70);  // pass in the address
    alpha2.begin(0x71);  // pass in the address
    alpha3.begin(0x72);  // pass in the address
    alpha4.begin(0x73);  // pass in the address
    alpha5.begin(0x74);  // pass in the address

    displayLoadingText(alpha1, alpha2, alpha3, alpha4, alpha5);
    
    delay(2000);

    clearDisplays();

    // alpha1.clear();
    // alpha2.clear();
    // alpha3.clear();
    
    // alpha1.writeDisplay();
    // alpha2.writeDisplay();
    // alpha3.writeDisplay();

    //begin_loading_animation();
    
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
    return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}