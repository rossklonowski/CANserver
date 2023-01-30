#include <ezButton.h>

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

// display stuff
#include <LiquidCrystal.h>
#include "bargraph.h"
#include "button.h" 
#include "lcd.h"
#include "sendHelper.h"
#include "oled.h"
#include "accelerometer.h"
// OLED
#include <SPI.h>
#include <Adafruit_SSD1325.h>

#include "payload.h"
#include "carDataVariables.h"
#include "display_settings.h"
#include "program_settings.h"

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

// Accel
#define I2C_SDA 33
#define I2C_SCL 32

// OLED
// If using software SPI, define CLK and MOSI
// #define OLED_CLK 18
// #define OLED_MOSI 23

// #define OLED_CS 21
// #define OLED_RESET 17
// #define OLED_DC 16

static bool isIdle = true;
static bool isFirstRecv = true;
static bool debug = false;
static int lastCount = 0;
static int refreshCount = 0;
static bool page_button_pressed = false;
static bool reset_data_button_pressed = false;
static int page = 1;
const int max_pages = 9;

unsigned long previouscycle = 0;
unsigned long update_previouscycle = 0;

static int interval = 1000;
static int update_interval = 100;

static int previouscycleCheckForMaster = 0;
static int intervalCheckForMaster = 5000;

static int messages_received_counter = 0;
static int last_messages_received_counter = 0;

unsigned long timeSinceLastAskPingFromMaster = 0;
long millisAtLastPing = 0;

bool connectedToMaster = true;

double data_rate = 0.0;

// button
ezButton button(36);

// OLED
// Adafruit_SSD1325 oled(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

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
            masterUpTime = payload.int_value_1;

            break;

        case 0x3B6 : // odometer
            odometer = payload.double_value_1;

            break;

        case 0x3E6 : // send im up message if we are asked for it
            // Serial.println("Received are you up message");
            timeSinceLastAskPingFromMaster = 0;
            millisAtLastPing = millis();
            connectedToMaster = true;
            // send response that I am up
            digitalWrite(LED2, HIGH); // flash led for loop iter
            
            sendToDisplay(masterMacAddress, 0x3E6, 1);

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
    pinMode(LED2, OUTPUT); // configure blue LED
    digitalWrite(LED2, HIGH);

    // I2C needed for C02 and Accelerometer
    Wire.begin(I2C_SDA, I2C_SCL);

    // setupBarGraphs();
    // displayLoadingAnimationBarGraph();
    
    // setupLCD();
    // displayLoadingAnimationLCD();

    setup_buttons();

    // setupAlphaNum();
    // displayLoadingAnimationAlphaNum();

    setupOLED();

    // Accel
    accel_setup();
    
    Serial.begin(115200);
 
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

    // register peer
    esp_now_peer_info_t peerInfo;
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    memcpy(peerInfo.peer_addr, masterMacAddress, 6);
    // add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    // button stuff    
    button.loop();
    if (button.isPressed()) {
        Serial.println("PRESSED");
        page = page + 1;
        if (page == max_pages + 1) {
            page = 1; // reset
        } 
    }

    // reset_data_button_pressed = check_reset_button(reset_data_button);
    // if (reset_data_button_pressed) {
    //     if (page == 9) {
    //         lastNominalEnergyRemaining = nominalEnergyRemaining;
    //     }
    //     // else if (page == 14) {
    //     //     // tripOdometer = odometer;
    //     // }
    // }

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

    timeSinceLastAskPingFromMaster = currentMillis - millisAtLastPing;
    if (timeSinceLastAskPingFromMaster > intervalCheckForMaster) {
        
        if (connectedToMaster) {
            Serial.println("Master is offline");
            connectedToMaster = false;   
            digitalWrite(LED2, LOW);
        }

        if (connectedToMaster == false) {
            digitalWrite(LED2, LOW);
            // write_oled(0, 7, "</>");
        }
    }


    // update displays each specified interval
    long update_currentMillis = millis();
    if (update_currentMillis - update_previouscycle >= update_interval) {
        update_previouscycle = update_currentMillis;

        // sendToLCD(0, "Batt:" + String(battTempPct) + "%", 1, String(minBattTemp) + "F/" + String(maxBattTemp) + "F");
        // sendToLCD(0, "Max Regen/Disch", 1, String(maxRegen) + "KW   " + String(maxDischarge) + "KW");
        // sendToLCD(0, String(frontPower) + "KW " + String(rearPower) + "KW", 1, String(frontInverterTemp) + "KW " + String(rearInverterTemp) + "KW");
        // sendToLCD(0, "HV Batt " + String(((double)battPower)/1000.00) + "KW", 1, String(battVolts) + "V " + String(battAmps) + "A ");
        // sendToLCD(0, "SoCavg " + String(socAVE) + "%", 1, "");
        // sendToLCD(0, "Motor Limits", 1, String(frontPowerLimit) + "KW " + String(rearPowerLimit) + "KW");
        // sendToLCD(0, "Nominal rem/full", 1, String(nominalEnergyRemaining) + "/" + String(nominalFullPackEnergy) + "KWh");
        // sendToLCD(0, "Up time (Master)", 1, String(masterUpTime) + " seconds");
        // sendToLCD(0, "Speed" + String(UIspeed) + "mph", 1, "Odom " + String(odometer));
        // sendToLCD(0, "Charge Line " + String(chargeLineVoltage) + "V", 1, String(chargeLineCurrent) + "A " + String(chargeLinePower) + "KW");
        // sendToLCD(0, "mps/ total msgs", 1, String(data_rate) + " " + String(messages_received_counter) + "");
        // sendToLCD(0, "Cabin:Temp/Humid", 1, String(cabin_temp) + "F " + String(cabin_humidity) + "%");
        // sendToLCD(0, "Custom wh/m", 1, String(energyCounter / tripOdometer));
        // sendToLCD(0, "Trip Distance", 1, String(tripOdometer));
        // sendToLCD(0, "Odometer", 1, String(odometer));

        // energyCounter = lastNominalEnergyRemaining - nominalEnergyRemaining;
        // sendToLCD(0, "Energy Counter", 1, String(energyCounter) + " KWh");

        // oled_clear();
        // send_to_oled_buffer(0, "REG/DIS:" + String(maxRegen) + "KW " + String(maxDischarge) + "KW");
        // send_to_oled_buffer(1, "SOC:" + String(socAVE) + "%" + " PCT:" + String(battTempPct) + "%");
        // send_to_oled_buffer(2, "NOM:" + String(nominalEnergyRemaining) + "/" + String(nominalFullPackEnergy) + "KWh");
        // send_to_oled_buffer(3, "BATT MIN:" + String(minBattTemp) + " MAX:" + String(maxBattTemp) + "F");
        // send_to_oled_buffer(4, "" + String(((double)battPower)/1000.00) + "KW " + String(battVolts) + "V " + String(battAmps) + "A");
        // oled_update();

        // oled_clear();
        // send_to_oled_buffer(0, "Max Regen  " + String(maxRegen) + "KW");
        // send_to_oled_buffer(1, "Max Disch  " + String(maxDischarge) + "KW");
        // send_to_oled_buffer(2, "SOC        " + String(socAVE) + "%");
        // send_to_oled_buffer(3, "NomEnergy  " + String(nominalEnergyRemaining) + "KWh");
        // send_to_oled_buffer(4, "NomFull    " + String(nominalFullPackEnergy) + "KWh");
        // send_to_oled_buffer(5, "Batt Min   " + String(minBattTemp) + "F");
        // send_to_oled_buffer(6, "Batt Max   " + String(maxBattTemp) + "F");
        // send_to_oled_buffer(7, "Temp %     " + String(battTempPct) + "%");
        // oled_update();

        // oled_clear();
        // float accel_x = 0.0;
        // float accel_y = 0.0;
        // float accel_z = 0.0;
        // accel_get_g_force(accel_x, accel_y, accel_z);
        // float g_x = accel_x / 9.81;
        // float g_y = accel_x / 9.81;
        // float g_z = accel_x / 9.81;
        // send_to_oled_buffer(0, "Accel X: " + String(accel_x) + "m/s^2");
        // send_to_oled_buffer(1, "G's   X: " + String(g_x) + "g(s)");
        // send_to_oled_buffer(2, "Accel Y: " + String(accel_y) + "m/s^2");
        // send_to_oled_buffer(3, "G's   Y: " + String(g_y) + "g(s)");
        // send_to_oled_buffer(4, "Accel Z: " + String(accel_z) + "m/s^2");
        // send_to_oled_buffer(5, "G's   Z: " + String(g_z) + "g(s)");
        // oled_update();

        // send_to_oled_buffer(7, "" + String(((double)battPower)/1000.00) + "KW " + String(battVolts) + "V " + String(battAmps) + "A");
        // send_to_oled_buffer(5, "FR:" + String(frontPower) + "KW " + String(rearPower) + "KW");
        // send_to_oled_buffer(6, "  :" + String(frontPowerLimit) + "KW " + String(rearPowerLimit) + "KW");
        // send_to_oled_buffer(7, "MPS/TOT:" + String(data_rate) + " " + String(messages_received_counter) + "");

    }
}
