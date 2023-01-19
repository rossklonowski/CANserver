//
//  CANslave.ino
//
//  Created by Ross Klonowski on January 16 2021.
//

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>
#include <LiquidCrystal.h>
#include "bargraph.h"
#include "button.h" 
#include "lcd.h"

#include "payload.h"
#include "carDataVariables.h"
#include "display_settings.h"
#include "program_settings.h"

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

static bool isIdle = true;
static bool isFirstRecv = true;
static bool debug = false;
static int lastCount = 0;
static int refreshCount = 0;
static bool page_button_pressed = false;
static bool reset_data_button_pressed = false;
static int page = 1;
const int max_pages = 13;

unsigned long previouscycle = 0;
unsigned long update_previouscycle = 0;

static int interval = 1000;
static int update_interval = 250;

static int messages_received_counter = 0;
static int last_messages_received_counter = 0;

bool connectedToMaster = false;

double data_rate = 0.0;

void handle_received_data(payload payload) {

    switch (payload.can_id) {
        case 0x312 : // batt temp
            minBattTemp = payload.double_value_1;
            minBattTemp = minBattTemp * (9/5) + 32; // convert to f
            
            maxBattTemp = payload.double_value_2;
            maxBattTemp = maxBattTemp * (9/5) + 32; // convert to f

            avgBattTemp = (minBattTemp + maxBattTemp) / 2.0;

            break;

        case 0x336 : // max regen, min discharge
            maxDischarge = payload.int_value_1;
            maxRegen = payload.int_value_2;
            
            break;

        case 0x132 : // HVBattAmpVolt
            battVolts = payload.int_value_1;
            battAmps = payload.int_value_2;
            battPower = payload.int_value_3;
            
            break;

        case 0x001 : // 0-60 time
            // write 0-60
            if (displayZeroToSixty) {
                //printValueWithSingleUnit(payload.double_value_1, 7);
            }

            break;

        case 0x2E5 : // frontPower
            frontPower = payload.int_value_1;
            frontPowerLimit = payload.int_value_2;
            maxRegen = payload.int_value_3;

            if (frontPower >= frontPowerMax) {
                frontPowerMax = frontPower;
            }
            
            if (displayFrontPowerBarGraph) {
                sendToBarGraphPower("front", frontPower, frontPowerLimit, maxRegen);
            }
            
            break;

        case 0x292 : // BMS_SOC
            socAVE = payload.double_value_1;
            battTempPct = payload.double_value_2;
            
            break;

        case 0x257 : // DIspeed
            UIspeed = payload.double_value_1;
            UIspeed = UIspeed * 0.621371; // for mph

            break;

        case 0x264 : // ID264ChargeLineStatus
            chargeLineCurrent = payload.int_value_1;
            chargeLineVoltage = payload.int_value_2;
            chargeLinePower = payload.int_value_3;

            break;

        case 0x266 : // rearPower

            rearPower = payload.int_value_1;
            rearPowerLimit = payload.int_value_2;
            maxRegen = payload.int_value_3;
            
            if (rearPower >= rearPowerMax) {
                rearPowerMax = rearPower;
            }

            if (displayRearPowerBarGraph) {
                sendToBarGraphPower("rear", rearPower, rearPowerLimit, maxRegen);
            }
            
            break;

        case 0x321 :
            tempCoolandBatInlet = payload.int_value_1;
            tempCoolandBatPTlet = payload.int_value_2;

            break;

        case 0x352 : // bms_energystatus
            nominalEnergyRemaining = payload.double_value_1;
            nominalFullPackEnergy = payload.double_value_2;

            break;

        case 0x376 : // frontInverterTemps
            frontInverterTemp = payload.double_value_1;
            frontInverterTemp = frontInverterTemp * (9/5) + 32;
            break;

        case 0x315 : // rearInverterTemps
            rearInverterTemp = payload.double_value_1;
            rearInverterTemp = rearInverterTemp * (9/5) + 32;

            break;

        case 0x383 : // VCRIGHT_thsStatus
            cabin_temp = payload.int_value_1;
            cabin_temp = cabin_temp * (9/5) + 32;
            
            break;

        case 0x2B3 : // VCRIGHT_logging1Hz
            cabin_humidity = payload.int_value_1;

            break;

        case 0x3E7 : // custom uptime
            Serial.println(payload.int_value_1);
            masterUpTime = payload.int_value_1;

            break;

        case 0x3B6 : // odometer
            odometer = payload.double_value_1;

            break;
    }
}

// callback function that tells us when data from Master is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

    messages_received_counter = messages_received_counter + 1;

    payload new_data;
    memcpy(&new_data, incomingData, sizeof(new_data));
    // Serial.println("Incoming payload size: " + String(sizeof(new_data)));

    handle_received_data(new_data); // decode message received and update data variables
}  

void setup() {
    setupBarGraphs();
    // displayLoadingAnimationBarGraph();
    
    setupLCD();
    // displayLoadingAnimationLCD();

    setup_buttons();

    // setupAlphaNum();
    // displayLoadingAnimationAlphaNum();
    
    Serial.begin(115200);
 
    // put esp32 in WIFI station mode
    WiFi.mode(WIFI_STA);
    // Serial.print("Mac Address in Station: ");
    // Serial.println(WiFi.macAddress());
    
    // init esp now (connection to slave wia wifi)
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // create call back (OnDataRecv will run every time a message is received via esp now)
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // button stuff    
    page_button_pressed = check_page_button(page_button);
    if (page_button_pressed) {
        page = page + 1;
        if (page == max_pages + 1) {
            page = 1; // reset
        } 
    }

    reset_data_button_pressed = check_reset_button(reset_data_button);
    if (reset_data_button_pressed) {
        lastNominalEnergyRemaining = nominalEnergyRemaining;
    }

    // get data rate stats and update after each time interval (1 second)
    long currentMillis = millis();
    if (currentMillis - previouscycle >= interval) {
        previouscycle = currentMillis;

        int messages_received_last_interval = messages_received_counter - last_messages_received_counter;
        double messages_received_last_interval_double = (double)messages_received_last_interval;
        double interval_seconds = (double)interval/1000.00;
        data_rate = messages_received_last_interval_double / interval_seconds;
        last_messages_received_counter = messages_received_counter;
    }

    // update displays each specified interval
    long update_currentMillis = millis();
    if (update_currentMillis - update_previouscycle >= update_interval) {
        update_previouscycle = update_currentMillis;
            
        if (page == 1) {
            if (true) {
                sendToLCD(0, "Batt:" + String(battTempPct) + "%", 1, String(minBattTemp) + "F/" + String(maxBattTemp) + "F");
            }
        }

        if (page == 2) {
            if (true) {
                sendToLCD(0, "Max Regen/Disch", 1, String(maxRegen) + "KW   " + String(maxDischarge) + "KW");
            }
        }

        if (page == 3) {
            if (true) {
                sendToLCD(0, "FR Motor Power", 1, String(frontPower) + "KW     " + String(rearPower) + "KW");
            }
        }

        if (page == 4) {
            if (true) {
                sendToLCD(0, "HV Battery", 1, String(battVolts) + "V " + String(battAmps) + "A " + String(battPower) + "KW");
            }
        }

        if (page == 5) {
            if (true) {
                sendToLCD(0, "SoCavg", 1, String(socAVE) + "%");
            }
        }

        if (page == 6) {
            if (true) {
                sendToLCD(0, "Inverter F/R", 1, String(frontInverterTemp) + "F " + String(rearInverterTemp) + "F");
            }
        }

        if (page == 7) {
            if (true) {
                sendToLCD(0, "Cabin:Temp/Humid", 1, String(cabin_temp) + "F " + String(cabin_humidity) + "%");
            }
        }

        if (page == 8) {
            if (true) {
                sendToLCD(0, "Nominal rem/full", 1, String(nominalEnergyRemaining) + "/" + String(nominalFullPackEnergy) + "KWh");
            }
        }

        if (page == 9) {
            if (true) {
                energyCounter = lastNominalEnergyRemaining - nominalEnergyRemaining;
                sendToLCD(0, "Energy Counter", 1, String(energyCounter*-1.0) + " KWh");
            }
        }

        if (page == 10) {
            if (true) {
                sendToLCD(0, "mps/ total msgs", 1, String(data_rate) + " " + String(messages_received_counter) + "");
            }
        }

        if (page == 11) {
            if (true) {
                sendToLCD(0, "Up time (Master)", 1, String(masterUpTime) + " seconds");
            }
        }

        if (page == 12) {
            if (true) {
                sendToLCD(0, "Connected", 1, String(connectedToMaster));
            }
        }

        if (page == 13) {
            if (true) {
                sendToLCD(0, "Odometer", 1, String(odometer));
            }
        }
    }
}
