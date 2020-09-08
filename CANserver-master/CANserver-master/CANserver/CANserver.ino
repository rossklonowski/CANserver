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

static int BattVolts_temp = 0;
static int BattVolts = 0;        //ID 132 byte 0+1 scale .01 V

static int BattAmps_temp = 0;
static int BattAmps = 0;         //ID 132 byte 2+3 scale -.1 offset 0 A

static int MinBattTemp_temp = 0;
static int MinBattTemp = 0;      //ID 312 SB 44 u9 scale .25 offset -25 C

static int BattPower_temp = 0;   //V*A
static int BattPower = 0;        //V*A

static int RearTorque = 0;       //ID 1D8 startbit 24 signed13 scale 0.25 NM
static int BattCoolantRate = 0;  //ID 241 SB 0 u9 scale .01 LPM
static int PTCoolantRate = 0;    //ID 241 SB 22 u9 scale .01 LPM
static int MaxRegen = 0;         //ID 252 Bytes 0+1 scale .01 kW
static int MaxDisChg = 0;        //ID 252 Bytes 2+3 scale .01 kW
static int VehSpeed = 0;         //ID 257 SB 12 u12 scale .08 offset -40 KPH
static int SpeedUnit = 0;        //ID 257
static int BattCoolantTemp = 0;  //ID 321 SB0 u10 scale 0.125 offset -40 C
static int PTCoolantTemp = 0;    //ID 321 SB10 u10 scale 0.125 offset -40 C
static int BattRemainKWh = 0;    //ID 352 byte 1/2 scale .1 kWh
static int BattFullKWh = 0;      //ID 352 byte 0/1 scale .1 kWh
static int steering_angle = 0;
static int accelPedalPos = 0;
static int brakePedalPos = 0;
static int BMSmaxPackTemperature = 0;
static int BMSminPackTemperature = 0;
static int DisplayOn = 1;       //to turn off displays if center screen is off
static int ChargeLineVoltage = 0;
static int ChargeLineCurrent = 0;
static int ChargeLinePower = 0;

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
        request->send(200, "text/plain", String(RearTorque).c_str());
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
            
            // MinBattTemp = MinBattTemp + 1;
            // if (MinBattTemp == 100) {
            //     MinBattTemp = -32;
            // }

            // RearTorque = RearTorque + 1;
            // if (RearTorque == 150) {
            //     RearTorque = -150;
            // }

            // BattVolts = BattVolts + 1;
            // if (BattVolts == 150) {
            //     BattVolts = -150;
            // }

            // BattAmps = BattAmps + 1;
            // if (BattAmps == 500) {
            //     BattAmps = -99;
            // }

            BattPower = BattPower + 1;
            if (BattPower == 150) {
                BattPower = -150;
            }

            sendToDisplay(0x132, BattVolts, BattAmps, BattPower, "V", "A", "KW");
            
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
            
            if (serial_switch) {
                Serial.print("Battery: Volts: ");
                Serial.print(BattVolts);
                Serial.print(" V Amps: ");
                Serial.print(BattAmps);
                Serial.print(" A Power: ");
                Serial.print(BattPower);
                Serial.print(" kW");
                
                Serial.print("\n");

                Serial.print("Min Battery Temperature: ");
                Serial.print((MinBattTemp * 9/5) + 32);
                Serial.print(" F");
                
                Serial.print("\n");

                Serial.print("Max Regen: ");
                Serial.print(MaxRegen);
                Serial.print(" kW");
                Serial.print(" Max Discharge: ");
                Serial.print(MaxDisChg);
                Serial.print(" kW");

                Serial.print("\n");

                Serial.print("Steering Angle: ");
                Serial.print(steering_angle);
                Serial.print(" Deg");
                Serial.print(" accelPedalPos: ");
                Serial.print(accelPedalPos);
                Serial.print(" VehSpeed: ");
                Serial.print(VehSpeed);
                Serial.print(" mph");
                Serial.print(" Rear Torque: ");
                Serial.print(RearTorque);
                Serial.print(" NM");

                Serial.print("\n");

                Serial.print("BMSmaxPackTemperature: ");
                Serial.print((BMSmaxPackTemperature * 9/5) + 32);
                Serial.print(" F");
                Serial.print(" BMSminPackTemperature: ");
                Serial.print((BMSminPackTemperature * 9/5) + 32);
                Serial.print(" F");

                Serial.print("\n");
                Serial.print("***************************");
                Serial.print("\n");
            }
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
                    DisplayOn = analyzeMessage.getSignal(message.data.uint64, 5, 1, 1, 0, false, littleEndian);  //SG_ UI_displayOn : 5|1@1+ (1,0) [0|1] ""
                }
                break;

            case 0x129: // SteeringAngle
                if (message.length == 8) {
                    int steering_angle = analyzeMessage.getSignal(message.data.uint64, 16, 14, 0.1, -819.2, false, littleEndian);
                }
                break;
                
            case 0x132: // HVBattAmpVolt
                if (message.length == 8) {
                    int tempvolts;
                    tempvolts = analyzeMessage.getSignal(message.data.uint64, 0, 16, 0.01, 0, false, littleEndian);
                    if ((tempvolts > 290) && (tempvolts < 420)) { //avoid some bad messages
                        BattVolts = tempvolts;
                        BattAmps = analyzeMessage.getSignal(message.data.uint64, 16, 16, -0.1, 0, true, littleEndian); //signed 15, mask off sign
                        BattPower = BattVolts * BattAmps / 100;
                    }
                }
                break;
                
            case 0x1D8: // RearTorque
                if (message.length == 8) {
                    int temptorque;
                    temptorque = analyzeMessage.getSignal(message.data.uint64, 24, 13, 0.25, 0, true, littleEndian);  //signed13, mask off sign
                    if ((temptorque < 5000) && (temptorque > -2000)) {  //reduce errors
                        RearTorque = temptorque;
                    }
                }
                break;
            
            case 0x241: // VCFRONT_coolant
                BattCoolantRate = analyzeMessage.getSignal(message.data.uint64, 0, 9, 0.1, 0, false, littleEndian);  //ID 241 SB 0 u9 scale .01 LPM
                //PTCoolantRate = analyzeMessage.getSignal(message.data.uint64, 22, 9, 0.1, 0, false, littleEndian);    //ID 241 SB 22 u9 scale .01 LPM
                break;

            case 0x252: // BMS_powerAvailable
                MaxRegen =  analyzeMessage.getSignal(message.data.uint64, 0, 16, 0.01, 0, false, littleEndian);
                MaxDisChg = analyzeMessage.getSignal(message.data.uint64, 16, 16, 0.01, 0, false, littleEndian);
                break;
                
            case 0x257: // VehSpeed = 0;     //ID 257 SB 12 u12 scale .08 offset -40 KPH
                if (message.length == 8) {
                    ///SpeedUnit = analyzeMessage.getSignal(message.data.uint64, 32, 1, 1, 0, false, littleEndian); //strange this doesn't change with UI setting! Location?
                    VehSpeed = analyzeMessage.getSignal(message.data.uint64, 12, 12, 0.08, -40, false, littleEndian);
                }
                break;

            case 0x264: // DIR_torque
                if (message.length == 8) {
                    ChargeLineVoltage = analyzeMessage.getSignal(message.data.uint64, 0, 14, .0333, 0, false, littleEndian);
                    ChargeLineCurrent = analyzeMessage.getSignal(message.data.uint64, 14, 9, 0.1, 0, false, littleEndian);
                    ChargeLinePower = analyzeMessage.getSignal(message.data.uint64, 24, 8, 0.1, 0, false, littleEndian);
                }
                break;
                
            case 0x293: // UI_chassisControl    
                if (message.length == 8) {
                    SpeedUnit = analyzeMessage.getSignal(message.data.uint64, 13, 1, 1, 0, false, littleEndian); //UI distance setting to toggle speed display units
                }
                break;

            case 0x280: // DriveSystemStatus    
                if (message.length == 8) {
                    accelPedalPos = analyzeMessage.getSignal(message.data.uint64, 32, 8, 0.4, 0, false, littleEndian);
                }
                break;

            case 0x312: // BMSthermal
                if (message.length == 8) {
                    MinBattTemp = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    MinBattTemp = MinBattTemp * (9/5) + 32; // convert to f
                }
                break;

            case 0x352: // BMS_energyStatus
                if (message.length == 8) {
                    BattRemainKWh = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    BattFullKWh = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                }
                break;

            default:
                break;
        }
    }
}