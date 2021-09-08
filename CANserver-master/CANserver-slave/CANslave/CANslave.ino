#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>
#include <LiquidCrystal.h>
//#include "display.h"
#include "bargraph.h"
#include "button.h" 
#include "lcd.h"
#include "alphanumeric.h"

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

static bool isIdle = true;
static bool isFirstRecv = true;
static bool debug = false;
static int lastCount = 0;
static int refreshCount = 0;

static int maxRegenPower = 100;
static int tempMaxRegen = 0;
static int tempMaxDischarge = 0;

// batt I V P
static bool displayBattPower = true;
static bool displayBattAmps = false;
static bool displayBattVolts = false;
// grade
static bool displayGrade = false;
// batt temp
static bool displayBattTempLCD = true;
static bool displayBattTempAlphaNum = false;
// drive / regen limits
static bool displayLimits = false;
// wh/mi
static bool displayInstantaneousEfficiency = true;
static bool displayVehicleSpeed = false;
static bool displayZeroToSixty = false;
static bool displaySpeed = false;
static bool displaySOCAveLCD = true;

// front and rear power
static bool displayFrontPowerLCD = false;
static bool displayRearPowerLCD = false;
static bool displayRearPowerBarGraph = true;
static bool displayFrontPowerBarGraph = true;

// static int active = 0;

// dataTogglesRowZerop[active] = true;

// static bool dataTogglesRowZero[] = {
//                 displayBattTempLCD,
//                 displaySOCAveLCD, 
//                 displayFrontPowerLCD, 
//                 displayRearPowerLCD, 
//                 displayRearPowerBarGraph, 
//                 displayFrontPowerBarGraph
// };

// const int red_light_pin = 27;
// const int green_light_pin = 14;
// const int blue_light_pin = 12;

// // setting PWM properties
// const int freq = 5000;
// const int ledChannel = 5; // r
// const int ledChanne2 = 6; // g this works
// const int ledChanne3 = 4; // b
// const int resolution = 8;


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


void writeLCD(payload_struct payload) { // decode message received and display to quadalphanumeric displays

    switch (payload.can_id) {
        case 0x267 : // gradeEST
            //alphanumPrint(payload.int_value_1);
            if (displayGrade) {
                //printValueWithSingleUnit(payload.int_value_1, 7);
            }
            break;

        case 0x312 : // batt temp
            //alphanumPrint();
            if (displayBattTempAlphaNum) {
                printValueWithSingleUnit(payload.double_value_1, 7);
            }

            if (displayBattTempLCD) {
                sendToLCD(0, payload.double_value_1, "Batt" , "F");
            }
            break;

        case 0x336 : // max regen, min discharge
            //lcdPrint(payload.int_value_1);
            if (displayLimits) {
                // //printValueWithSingleUnit(payload.int_value_1, 9);

                // if (payload.int_value_1 != tempMaxDischarge || payload.int_value_2 != tempMaxRegen) {
                //     sendToLCD(0);
                //     sendToLCD(0, payload.int_value_1, "Batt" , "F", 0, payload.double_value_1, "Batt" , "F");
                //     lcd.setCursor(0, 0);
                //     lcd.print(String("Lims " + String(payload.int_value_1) + "KW" + "/" + String(payload.int_value_2) + "KW"));
                // }
            } 
            break;

        case 0x132 : // HVBattAmpVolt
            //alphanumPrint(payload.int_value_1);
            // write power to display
            if (displayBattPower) {
                //printValueWithNoUnits(payload.int_value_3, 5);
            }
            break;

        case 0x000 : // instantaneousEfficiency
            //alphanumPrint(payload.int_value_1);
            // write instantaneousEfficiency
            if (displayInstantaneousEfficiency) {
                //printValueWithNoUnits(payload.int_value_1, 8);
            }
            break;

        case 0x3D9 : // gps veh speed
            //alphanumPrint(payload.int_value_1);
            // write instantaneousEfficiency
            if (displayVehicleSpeed) {
                // printValueWithNoUnits(payload.int_value_1, 8);
            }
            break;

        case 0x001 : // 0-60 time
            {
                // write 0-60
                if (displayZeroToSixty) {
                    //printValueWithSingleUnit(payload.double_value_1, 7);
                }

                // lcd.setCursor(0, 1);
                // String message = "";
                // if (payload.double_value_1 == -1.0) {
                //     message = "Active     ";
                //     lcd.print("0-60:" + message);
                // } else if (payload.double_value_1 == -2.0) {
                //     message = "Armed      ";
                //     lcd.print("0-60:" + message);
                // } else if (payload.double_value_1 == -3.0) {
                //     message = "Run Ignored";
                //     lcd.print("0-60:" + message);
                // } else {
                //     lcd.print("0-60:" + String(payload.double_value_1) + String("secs"));
                // }
                break;
            }

        case 0x002 : // UI_Speed
            if (displaySpeed) {
                sendToLCD(1, payload.double_value_1, "Speed", "mph");
            }
            break;


        case 0x2E5 : // frontPower
            if (displayFrontPowerBarGraph) {
                sendToBarGraphPower("front", payload.int_value_1, payload.int_value_2, payload.int_value_3);
            }

            if (displayFrontPowerLCD) {
                sendToLCD(0, payload.int_value_1, "FP", "kW");
            }
            break;

        case 0x252 : // BMS

            break;

        case 0x266 : // rearPower
            if (displayRearPowerBarGraph) {
                sendToBarGraphPower("rear", payload.int_value_1, payload.int_value_2, payload.int_value_3);
            }

            if (displayRearPowerLCD) {
                sendToLCD(1, payload.int_value_1, "RP", "kW");
            }

            break;

        case 0x292 : // bms
            if (displaySOCAveLCD) {
                sendToLCD(1, payload.double_value_1, "Avg SOC ", "%");
            }
            break;
    }
}

// callback function that tells us when data from Master is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

    checkButton();

    // refreshCount = refreshCount + 1;

    // if (isFirstRecv) {
    //     initDisplayUnits();

    //     isFirstRecv = false;
    // }

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

    // if (refreshCount > 50) {
    //     alpha1.setBrightness(15);
    //     alpha1.writeDisplay();
    //     alpha2.setBrightness(15);
    //     alpha2.writeDisplay();
    //     alpha3.setBrightness(15);
    //     alpha3.writeDisplay();
    //     alpha4.setBrightness(15);
    //     alpha4.writeDisplay();
    //     alpha5.setBrightness(15);
    //     alpha5.writeDisplay();

    //     refreshCount = 0;
    // }

    writeLCD(myData); // decode message received and display to quadalphanumeric displays
}

void setup() {

    // ledcSetup(ledChannel, freq, resolution);
    // ledcSetup(ledChanne2, freq, resolution);
    // ledcSetup(ledChanne3, freq, resolution);

    // // attach the channel to the GPIO to be controlled
    // ledcAttachPin(red_light_pin, ledChannel);
    // ledcAttachPin(green_light_pin, ledChanne2);
    // ledcAttachPin(blue_light_pin, ledChanne3);

    //setupDisplays();

    setupBarGraphs();
    setupLCD();
    displayLoadingAnimationBarGraph();
    displayLoadingAnimationLCD();

    setupAlphaNum();
    displayLoadingAnimationAlphaNum();
    
    Serial.begin(115200);
    delay(250);
 
    // put esp32 in WIFI station mode
    WiFi.mode(WIFI_STA);
    Serial.print("Mac Address in Station: ");
    Serial.println(WiFi.macAddress());
    
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

    // for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    //     // changing the LED brightness with PWM
    //     ledcWrite(ledChannel, dutyCycle);
    //     ledcWrite(ledChanne2, dutyCycle);
    //     ledcWrite(ledChanne3, dutyCycle);
    //     delay(15);
    // }   
}
