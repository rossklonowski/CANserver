/*   
 *      This program has been modified from:
 *          CAN Server by Josh Wardell and Chris Whiteford
 *          http://www.jwardell.com/canserver/
 *   
 *      This file is to be downloaded onto the CAN server(master).   
 *   
 *      This program is made to read the CAN Bus of a Tesla Model 3.
 *      We use ESP-NOW for wireless communication between 2 ESP32 boards.
 *      The Slave board outputs to quad alphanumeric displays.
 *
 *      Board: Node32s
 *      (must press IO0 right button to start programming)
 *
 *      Modified by Ross Klonowski
 *
*/

#include "esp32_can.h"  //RX GPIO16 TX GPIO 17 https://github.com/collin80/esp32_can
#include "generalCANSignalAnalysis.h" //https://github.com/iChris93/ArduinoLibraryForCANSignalAnalysis
#include "ESPAsyncWebServer.h" //https://github.com/me-no-dev/ESPAsyncWebServer
#include <stdio.h> 
#include <AsyncJson.h>
#include <WiFi.h>
#include <esp_now.h> // for ESP32 to ESP32 wifi communication
#include "SPIFFS.h" // for web server files (html,styling)
#include <ArduinoJson.h>

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

#define BITRATE 500000  //CAN bitrate, Tesla=500000
#define littleEndian true
#define bigEndian false

static bool debug = false;
static bool serial_switch = false;

static int battVolts_temp = 0;
static int battVolts = 0;        //ID 132 byte 0+1 scale .01 V

static int battAmps_temp = 0;
static int battAmps = 0;         //ID 132 byte 2+3 scale -.1 offset 0 A

static int minBattTemp_temp = 0;
static int minBattTemp = 0;      //ID 312 SB 44 u9 scale .25 offset -25 C

static int battPower_temp = 0;   // V*A
static int battPower = 0;        // V*A

static int gradeEST_temp = 0;
static int gradeEST = 0;

static int gradeESTinternal_temp = 0;
static int gradeESTinternal = 0;

static int rearTorque = 0;       //ID 1D8 startbit 24 signed13 scale 0.25 NM
static int battCoolantRate = 0;  //ID 241 SB 0 u9 scale .01 LPM
static int PTCoolantRate = 0;    //ID 241 SB 22 u9 scale .01 LPM

static int battCoolantTemp = 0;  //ID 321 SB0 u10 scale 0.125 offset -40 C
static int ptCoolantTemp = 0;    //ID 321 SB10 u10 scale 0.125 offset -40 C
static int battRemainKWh = 0;    //ID 352 byte 1/2 scale .1 kWh
static int battFullKWh = 0;      //ID 352 byte 0/1 scale .1 kWh
static int steering_angle = 0;
static int accelPedalPos = 0;
static int brakePedalPos = 0;

static int bmsMaxPackTemperature_temp = 0;
static int bmsMaxPackTemperature = 0;

static int nominalFullPackEnergy_temp = 0;
static int nominalFullPackEnergy = 0;

static int bmsMinPackTemperature_temp = 0;
static int bmsMinPackTemperature = 0;

static int displayOn_temp = 1;
static int displayOn = 1;       //to turn off displays if center screen is off

static int chargeLineVoltage_temp = 0;
static int chargeLineVoltage = 0;
static int chargeLineCurrent_temp = 0;
static int chargeLineCurrent = 0;
static int chargeLinePower_temp = 0;
static int chargeLinePower = 0;

static int maxRegen_temp = 0;         //ID 252 Bytes 0+1 scale .01 kW
static int maxRegen = 0;         //ID 252 Bytes 0+1 scale .01 kW

static int maxDischarge_temp = 0;        //ID 252 Bytes 2+3 scale .01 kW
static int maxDischarge = 0;        //ID 252 Bytes 2+3 scale .01 kW

static int battBeginningOfLifeEnergy_temp = 0;
static int battBeginningOfLifeEnergy = 0;

unsigned long previouscycle = 0;

static int interval = 100;

// replace with your desired AP credentials
const char* ssid = "Tesla Server";
const char* password = "ellenpassword";

generalCANSignalAnalysis analyzeMessage; //initialize library

// slave's MAC address
uint8_t receiverMacAddress[] = {0xAC, 0x67, 0xB2, 0x2C, 0x3C, 0xD0};

// create AsyncWebServer object on port 80
AsyncWebServer server(80);

// message struct
typedef struct struct_message {

    uint32_t can_id;
    int int_value_1;
    int int_value_2;
    int int_value_3;
    String unit1;
    String unit2;
    String unit3;
} struct_message;

// callback function - gives us feedback about the sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
      
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

int sendToDisplay(uint32_t can_id, int valueToSend1, String unit1) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = -1;
    payload.int_value_3 = -1;
    payload.unit1 = unit1;
    payload.unit2 = "";
    payload.unit3 = "";
    
    esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));
  
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }

    Serial.print("Bytes Sent: ");
    Serial.println(sizeof(payload));

    Serial.print("int_value_1 sent: ");
    Serial.println(payload.int_value_1);
    
    Serial.print("unit1 sent: ");
    Serial.println(payload.unit1);

    Serial.print("\n");

    return result;
}

int sendToDisplay(uint32_t can_id, int valueToSend1, int valueToSend2, String unit1, String unit2) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    payload.unit1 = unit1;
    payload.unit2 = unit2;
    
    esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));
  
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }

    Serial.print("Bytes Sent: ");
    Serial.println(sizeof(payload));

    Serial.print("int_value_1 sent: ");
    Serial.println(payload.int_value_1);

    Serial.print("int_value_2 sent: ");
    Serial.println(payload.int_value_2);
    
    Serial.print("Unit1 sent: ");
    Serial.println(payload.unit1);

    Serial.print("Unit2 sent: ");
    Serial.println(payload.unit2);

    Serial.print("\n");

    return result;
}

int sendToDisplay(uint32_t can_id, int valueToSend1, int valueToSend2, int valueToSend3, String unit1, String unit2,  String unit3) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    payload.int_value_3 = valueToSend3;
    payload.unit1 = unit1;
    payload.unit2 = unit2;
    payload.unit3 = unit3;
    
    esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));
  
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }

    Serial.print("Bytes Sent: ");
    Serial.println(sizeof(payload));

    Serial.print("int_value_1 sent: ");
    Serial.println(payload.int_value_1);

    Serial.print("int_value_2 sent: ");
    Serial.println(payload.int_value_2);

    Serial.print("Unit1 sent: ");
    Serial.println(payload.unit1);

    Serial.print("Unit2 sent: ");
    Serial.println(payload.unit2);

    Serial.print("\n");

    return result;
}

void setup(){

    pinMode(LED2, OUTPUT); // configure blue LED

    Serial.begin(115200);
    delay(100);

    CAN0.begin(BITRATE);
    CAN0.watchFor(); // then let everything else through anyway

    // init SPIFFS
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    // init wifi station (STA means device that is capable of 802.11 protocol)
    WiFi.mode(WIFI_STA);

    // set device as a Wi-Fi access point
    WiFi.softAP(ssid, password);

    Serial.print("Please connect to the Station's IP Address at: ");
    IPAddress IP = WiFi.softAPIP();
    Serial.println(IP); // always 192.168.4.1

    // init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // connect function that runs when data is sent
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    // add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }

    // server stuff

    // route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false);
    });
    
    // route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });

    // route to refesh
    server.on("/refresh", HTTP_GET, [](AsyncWebServerRequest *request){
        //request->send(200, "application/json", doc);
        request->send(200, "text/plain", String(rearTorque).c_str());
    });

    // start server
    server.begin();
}

void loop() {

    unsigned long currentMillis = millis();

    if (Serial) {
        long currentMillis = millis();
        if (currentMillis - previouscycle >= interval) {
            previouscycle = currentMillis;

            //digitalWrite(LED2, !digitalRead(LED2)); // flash led for loop iter
            
            minBattTemp = minBattTemp + 1;
            if (minBattTemp == 100) {
                minBattTemp = -32;
            }

            rearTorque = rearTorque + 1;
            if (rearTorque == 150) {
                rearTorque = -150;
            }

            battVolts = battVolts + 1;
            if (battVolts == 450) {
                battVolts = 200;
            }

            battAmps = battAmps + 1;
            if (battAmps == 500) {
                battAmps = -99;
            }

            battPower = battPower + 1;
            if (battPower == 150) {
                battPower = -150;
            }

            displayOn = displayOn + 1;
            if (displayOn == 2) {
                displayOn = 0;
            }

            battCoolantRate = battCoolantRate + 1;
            if (battCoolantRate == 10) {
                battCoolantRate = 0;
            }

            nominalFullPackEnergy = nominalFullPackEnergy + 1;
            if (nominalFullPackEnergy == 99) {
                nominalFullPackEnergy = 50;
            }

            maxRegen = maxRegen + 1;
            if (maxRegen == 99) {
                maxRegen = 50;
            }

            maxDischarge = maxDischarge + 1;
            if (maxDischarge == 99) {
                maxDischarge = 50;
            }

            sendToDisplay(0x132, battVolts, battAmps, battPower, "V", "A", "KW");
            
            //sendToDisplay(0x1D8, RearTorque, "NM");

            // if ( (BattVolts_temp != BattVolts) || (BattAmps != BattAmps_temp) || (BattPower != BattPower) ) {
            //     sendToWebPage()
                
            //     sendToDisplay(0x132, BattVolts, BattAmps, BattPower, "V", "A", "KW");
                
            //     BattVolts_temp = BattVolts;
            //     BattAmps_temp = BattAmps;
            //     BattPower_temp = BattPower;
            // }

            // if (MinBattTemp != MinBattTemp_temp) {
            //     sendToDisplay(0x312, MinBattTemp, "F");
            //     MinBattTemp_temp = MinBattTemp;
            // }
            

            // displayOn
            // battVolts
            // battAmps 
            // battPower 
            // rearTorque 
            // battCoolantRate;
            // PTCoolantRate;
            // chargeLineVoltage;
            // chargeLineCurrent;
            // chargeLinePower; 
            // gradeEST; 
            // gradeESTinternal;
            // battBeginningOfLifeEnergy;
            // minBattTemp;
            // minBattTemp;
            // maxRegen;
            // maxDischarge; 
            // battRemainKWh; 
            // battFullKWh;
            // nominalFullPackEnergy; 
        }
    }
    
    // can message processing follows
    CAN_FRAME message;

    if (CAN0.read(message)) {

        if (debug) {
            Serial.print("message ID(hex): ");
            Serial.print(message.id, HEX);  ///debug display RX message
            
            Serial.print(" message length(dec): ");
            Serial.print(message.length, DEC);
        
            for (int i = 0; i < message.length; i++) {
                Serial.print("byte ");
                Serial.print(i);
                Serial.print(": ");
                Serial.print(message.data.byte[i], HEX);
                Serial.print(" ");
            }
            Serial.print("\n");
        }

        digitalWrite(LED2, !digitalRead(LED2)); //flash LED2 to show data Rx
        
        switch (message.id)
        {
            case 0x00C: // UI_status
                if (message.length == 8) {
                    displayOn = analyzeMessage.getSignal(message.data.uint64, 5, 1, 1, 0, false, littleEndian);  //SG_ UI_displayOn : 5|1@1+ (1,0) [0|1] ""
                }
                break;
                
            case 0x132: // HVBattAmpVolt
                if (message.length == 8) {
                    int tempvolts;
                    tempvolts = analyzeMessage.getSignal(message.data.uint64, 0, 16, 0.01, 0, false, littleEndian);
                    if ((tempvolts > 290) && (tempvolts < 420)) { //avoid some bad messages
                        battVolts = tempvolts;
                        battAmps = analyzeMessage.getSignal(message.data.uint64, 16, 16, -0.1, 0, true, littleEndian); //signed 15, mask off sign
                        battPower = battVolts * battAmps / 100;
                    }
                }
                break;
                
            case 0x1D8: // RearTorque
                if (message.length == 8) {
                    int tempTorque;
                    tempTorque = analyzeMessage.getSignal(message.data.uint64, 24, 13, 0.25, 0, true, littleEndian);  //signed13, mask off sign
                    if ((tempTorque < 5000) && (tempTorque > -2000)) {  //reduce errors
                        rearTorque = tempTorque;
                    }
                }
                break;
            
            case 0x241: // VCFRONT_coolant
                battCoolantRate = analyzeMessage.getSignal(message.data.uint64, 0, 9, 0.1, 0, false, littleEndian);  //ID 241 SB 0 u9 scale .01 LPM
                //PTCoolantRate = analyzeMessage.getSignal(message.data.uint64, 22, 9, 0.1, 0, false, littleEndian);    //ID 241 SB 22 u9 scale .01 LPM
                break;

            case 0x264: // DIR_torque
                if (message.length == 8) {
                    chargeLineVoltage = analyzeMessage.getSignal(message.data.uint64, 0, 14, .0333, 0, false, littleEndian);
                    chargeLineCurrent = analyzeMessage.getSignal(message.data.uint64, 14, 9, 0.1, 0, false, littleEndian);
                    chargeLinePower = analyzeMessage.getSignal(message.data.uint64, 24, 8, 0.1, 0, false, littleEndian);
                }
                break;

            case 0x267: // DI_vehicleEstimates
                if (message.length == 8) {
                    gradeEST = analyzeMessage.getSignal(message.data.uint64, 33, 7, 1, 0, false, littleEndian);
                    gradeESTinternal = analyzeMessage.getSignal(message.data.uint64, 48, 7, 1, 0, false, littleEndian);
                }
                break;

            case 0x292: // BMS_SOC 
                if (message.length == 8) {
                    battBeginningOfLifeEnergy = analyzeMessage.getSignal(message.data.uint64, 40, 10, 0.1, 0, false, littleEndian);
                }
                break;

            case 0x312: // BMSthermal
                if (message.length == 8) {
                    minBattTemp = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    minBattTemp = minBattTemp * (9/5) + 32; // convert to f
                }
                break;

            case 0x336: // 
                if (message.length == 8) {
                    maxRegen =  analyzeMessage.getSignal(message.data.uint64, 16, 8, 1, -100, false, littleEndian);
                    maxDischarge = analyzeMessage.getSignal(message.data.uint64, 0, 10, 1, 0, false, littleEndian);
                }
                break;

            case 0x352: // BMS_energyStatus
                if (message.length == 8) {
                    battRemainKWh = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    battFullKWh = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    nominalFullPackEnergy = analyzeMessage.getSignal(message.data.uint64, 0, 11, 0.1, 0, false, littleEndian);
                }
                break;
    
            default:
                break;
        }
    }
}