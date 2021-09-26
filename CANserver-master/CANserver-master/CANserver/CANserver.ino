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
*/

//#include <ArduinoJson.h>
#include "esp32_can.h"  //RX GPIO16 TX GPIO 17 https://github.com/collin80/esp32_can
#include "generalCANSignalAnalysis.h" //https://github.com/iChris93/ArduinoLibraryForCANSignalAnalysis
#include "ESPAsyncWebServer.h" //https://github.com/me-no-dev/ESPAsyncWebServer
#include <stdio.h> 
#include <AsyncJson.h>
#include <WiFi.h>
#include <esp_now.h> // for ESP32 to ESP32 wifi communication
#include "SPIFFS.h" // for web server files (html,styling)
#include <math.h>
//#include "simulation.cpp"

#define LED1 1    //shared with serial tx - try not to use
#define LED2 2    //onboard blue LED

#define BITRATE 500000  //CAN bitrate, Tesla=500000
#define littleEndian true
#define bigEndian false

bool activeZeroToSixtyTime = false;
int totalZeroToSixtyTime = 0;
int currentTime_speed = 0;

static bool debug = false;
static bool serial_switch = false;
static int debug_counter = 0;

static int frontPower = 0;
static int rearPower = 0;    

static int battVolts_temp = 0;
static int battVolts = 0;        //ID 132 byte 0+1 scale .01 V

static int battAmps_temp = 0;
static int battAmps = 0;         //ID 132 byte 2+3 scale -.1 offset 0 A

static int frontPowerLimit = 150;
static int rearPowerLimit = 150;

static double minBattTemp = 0.0;      //ID 312 SB 44 u9 scale .25 offset -25 C
static double maxBattTemp = 0.0;
static double avgBattTemp = 0.0;
static double avgBattTemp_temp = 0.0;

static double testZeroToSixty = 0.0;

static int battPower_temp = 0;   // V*A
static int battPower = 0;        // V*A
static int battPowerW = 0;        // V*A

static int gradeEST_temp = 0;
static int gradeEST = 0;

static int gradeESTinternal_temp = 0;
static int gradeESTinternal = 0;

static int rearTorque = 0;       //ID 1D8 startbit 24 signed13 scale 0.25 NM

static int battCoolantRate = 0;  //ID 241 SB 0 u9 scale .01 LPM
static int ptCoolantRate = 0;    //ID 241 SB 22 u9 scale .01 LPM
static int battCoolantTemp = 0;  //ID 321 SB0 u10 scale 0.125 offset -40 C
static int ptCoolantTemp = 0;    //ID 321 SB10 u10 scale 0.125 offset -40 C

static int battRemainKWh = 0;    //ID 352 byte 1/2 scale .1 kWh

static int bmsMaxPackTemperature_temp = 0;
static int bmsMaxPackTemperature = 0;

static int nominalFullPackEnergy = 0;

static int nominalEnergyRemaining = 0;

static int expectedEnergyRemaining = 0;

static int bmsMinPackTemperature_temp = 0;
static int bmsMinPackTemperature = 0;

static double chargeLineVoltage_temp = 0;
static double chargeLineVoltage = 0;

static double chargeLineCurrent_temp = 0;
static double chargeLineCurrent = 0;

static double chargeLinePower_temp = 0;
static double chargeLinePower = 0;

static int displayOn = 0;       //to turn off displays if center screen is off

static int maxRegen = 0;         //ID 252 Bytes 0+1 scale .01 kW
static int maxRegen_temp = 0;         //ID 252 Bytes 0+1 scale .01 kW
static int maxDischarge = 0;        //ID 252 Bytes 2+3 scale .01 kW
static int battBeginningOfLifeEnergy = 0;

static double socAVE = 0;
static double socAVE_temp = 0;

// custom signals
static int instantaneousEfficiency = 0;
static double UIspeed = 0.0;
static double temp_UIspeed = 0.0;

unsigned long previouscycle = 0;
static int interval = 100;

// replace with your desired AP credentials
const char* ssid = "Tesla Server";
const char* password = "ellenpassword";

generalCANSignalAnalysis analyzeMessage; //initialize library

// slave's MAC address
uint8_t receiverMacAddress1[] = { 0xAC, 0x67, 0xB2, 0x2C, 0x3C, 0xD0 }; // Segmented Displays
uint8_t receiverMacAddress2[] = { 0xAC, 0x67, 0xB2, 0x2C, 0x21, 0x30 }; // LCD

// create AsyncWebServer object on port 80
AsyncWebServer server(80);

//StaticJsonDocument<200> doc;
DynamicJsonDocument doc(2048);

// message struct
typedef struct struct_message {

    uint32_t can_id;
    int int_value_1;
    int int_value_2;
    int int_value_3;
    double double_value_1;
    // double double_value_2; // this causes errors
    // double double_value_3;
    String unit1;
    String unit2;
    String unit3;
} struct_message;

// callback function - gives us feedback about the sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (debug) {
        Serial.print("\r\nLast Packet Send Status:\t");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }
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

    Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress1, (uint8_t *) &payload, sizeof(payload));
    result = esp_now_send(receiverMacAddress2, (uint8_t *) &payload, sizeof(payload));
    
    if (debug) {
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
    }

    return result;
}

int sendToDisplay(uint32_t can_id, double valueToSend1, String unit1) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = -1;
    payload.int_value_2 = -1;
    payload.int_value_3 = -1;
    payload.double_value_1 = valueToSend1;
    payload.unit1 = unit1;
    payload.unit2 = "";
    payload.unit3 = "";

    Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress1, (uint8_t *) &payload, sizeof(payload));
    result = esp_now_send(receiverMacAddress2, (uint8_t *) &payload, sizeof(payload));
    
    if (debug) { 
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        }
        else {
            Serial.println("Error sending the data");
        }

        Serial.print("Bytes Sent: ");
        Serial.println(sizeof(payload));

        Serial.print("double_value_1 sent: ");
        Serial.println(payload.double_value_1);
        
        Serial.print("unit1 sent: ");
        Serial.println(payload.unit1);

        Serial.print("\n");
    }

    return result;
}

int sendToDisplay(uint32_t can_id, int valueToSend1, int valueToSend2, String unit1, String unit2) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    payload.unit1 = unit1;
    payload.unit2 = unit2;

    Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress1, (uint8_t *) &payload, sizeof(payload));
    result = esp_now_send(receiverMacAddress2, (uint8_t *) &payload, sizeof(payload));
  
    if (debug) {
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
    }

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

    Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress1, (uint8_t *) &payload, sizeof(payload));
    result = esp_now_send(receiverMacAddress2, (uint8_t *) &payload, sizeof(payload));
  
    if (debug) {
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
    }

    return result;
}

void setup(){

    pinMode(LED2, OUTPUT); // configure blue LED

    Serial.begin(115200);
    delay(100);

    // Serial.print("ESP Board MAC Address:  ");
    // Serial.println(WiFi.macAddress());

    CAN0.begin(BITRATE);
    CAN0.watchFor(); // then let everything else through anyway

    // File root = SPIFFS.open("/");
    // File file = root.openNextFile();

    // while(file){
    //     Serial.print("FILE: ");
    //     Serial.println(file.name());
    //     file = root.openNextFile();
    // }

    // bool formatted = SPIFFS.format();
    // if (formatted) {
    //     Serial.println("\n\nSuccess formatting");
    // } else {
    //     Serial.println("\n\nError formatting");
    // }

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
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    memcpy(peerInfo.peer_addr, receiverMacAddress1, 6);
    // add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }

    memcpy(peerInfo.peer_addr, receiverMacAddress2, 6);
    // add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }

    // server stuff

    // route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/webpage.html", String(), false);
    });
    
    // route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });

    // route to refesh
    server.on("/refresh", HTTP_GET, [](AsyncWebServerRequest *request){
        String input = "";         // string that will contain the json doc
        serializeJson(doc, input); // serialize json before we send!
        
        request->send(200, "text/plain", input); // send the json in string format
    });

    // start server
    server.begin();

    // Serial.println("1");
    // File file = SPIFFS.open("/data/fonts/Gotham-Medium.otf");
    
    // if (!file) {
    //     Serial.println("There was an error opening the file for reading");
    // } else {
    //     Serial.println("File opened Successfully");
    // }
    // Serial.println("2");

    // const char * EspClass::getSdkVersion(void)
    // {
    //     return esp_get_idf_version();
    // }
    
    // file.close();
}

void loop() {

    // update json object that will be sent to display over wifi
    doc["battAmps"] = battAmps;
    doc["battBeginningOfLifeEnergy"] = battBeginningOfLifeEnergy;
    doc["battPower"] = battPower;
    doc["battRemainKWh"] = battRemainKWh;
    doc["battVolts"] = battVolts;
    doc["chargeLineCurrent"] = chargeLineCurrent;
    doc["chargeLinePower"] = chargeLinePower;
    doc["chargeLineVoltage"] = chargeLineVoltage;
    doc["displayOn"] = displayOn;
    doc["gradeEST"] = gradeEST;
    doc["gradeESTinternal"] = gradeESTinternal;
    doc["minBattTemp"] = minBattTemp;
    doc["maxBattTemp"] = maxBattTemp;
    doc["rearTorque"] = rearTorque;
    doc["nominalFullPackEnergy"] = nominalFullPackEnergy;
    doc["nominalEnergyRemaining"] = nominalEnergyRemaining;
    doc["maxRegen"] = maxRegen;
    doc["maxDischarge"] = maxDischarge;
    doc["nominalFullPackEnergy"] = nominalFullPackEnergy;
    doc["frontPower"] = frontPower;
    doc["rearPower"] = rearPower;
    doc["totalZeroToSixtyTime"] = totalZeroToSixtyTime;

    unsigned long currentMillis = millis();

    if (Serial) {
        long currentMillis = millis();
        if (currentMillis - previouscycle >= interval) {
            previouscycle = currentMillis;

            digitalWrite(LED2, !digitalRead(LED2)); // flash led for loop iter
            
            // minBattTemp = minBattTemp + 0.1;
            // if (minBattTemp > 80) {
            //     minBattTemp = 10;
            // }

            // maxBattTemp = maxBattTemp + 0.1;
            // if (maxBattTemp > 74) {
            //     maxBattTemp = 16;
            // }

            avgBattTemp = avgBattTemp + 0.1;
            if (avgBattTemp > 80) {
                avgBattTemp = 12;
            }

            // rearTorque = rearTorque + 1;
            // if (rearTorque == 150) {
            //     rearTorque = -150;
            // }

            // battVolts = battVolts + 1;
            // if (battVolts == 400) {
            //     battVolts = 300;
            // }

            // battAmps = battAmps + 1;
            // if (battAmps == 150) {
            //     battAmps = -99;
            // }

            // battPower = battPower + 1;
            // if (battPower == 300) {
            //     battPower = -50;
            // }
            
            // battBeginningOfLifeEnergy = battBeginningOfLifeEnergy + 1;
            // if (battBeginningOfLifeEnergy == 80) {
            //     battBeginningOfLifeEnergy = 60;
            // }

            // expectedEnergyRemaining = expectedEnergyRemaining + 1;
            // if (expectedEnergyRemaining == 80) {
            //     expectedEnergyRemaining = 60;
            // }

            // nominalFullPackEnergy = nominalFullPackEnergy + 1;
            // if (nominalFullPackEnergy == 99) {
            //     nominalFullPackEnergy = 50;
            // }

            // nominalEnergyRemaining = nominalEnergyRemaining + 1;
            // if (nominalEnergyRemaining == 99) {
            //     nominalEnergyRemaining = 50;
            // }

            chargeLineCurrent = chargeLineCurrent + 1;
            if (chargeLineCurrent == 99) {
                chargeLineCurrent = 50;
            }
            
            chargeLinePower = chargeLinePower + 1;
            if (chargeLinePower == 99) {
                chargeLinePower = 50;
            }
            
            chargeLineVoltage = chargeLineVoltage + 1;
            if (chargeLineVoltage == 99) {
                chargeLineVoltage = 50;
            }

            maxRegen = maxRegen + 1;
            if (maxRegen == 99) {
                maxRegen = 50;
            }

            maxDischarge = maxDischarge + 1;
            if (maxDischarge == 99) {
                maxDischarge = 50;
            }

            // gradeEST = gradeEST + 1;
            // if (gradeEST == 40) {
            //     gradeEST = -40;
            // }
        
            // gradeESTinternal = gradeESTinternal + 1;
            // if (gradeESTinternal == 40) {
            //     gradeESTinternal = -40;
            // }

            // if (UIspeed == 0.0 && debug_counter < 90) {
            //     UIspeed = 0.0;
            //     debug_counter = debug_counter + 1;
            // }

            // if (debug_counter >= 30) {
            //     debug_counter = 0;
            // }

            UIspeed = UIspeed + 1;
            if (UIspeed > 50) {
                UIspeed = 0.0;
            }

            // testZeroToSixty = testZeroToSixty + 0.1;
            // if (testZeroToSixty > 15) {
            //     testZeroToSixty = 0.0;
            // }

            // GPSSpeedMPH = GPSSpeedMPH + 1;
            // if (GPSSpeedMPH > 70) {
            //     GPSSpeedMPH = 0;
            // }

            instantaneousEfficiency = instantaneousEfficiency + 1;
            if (instantaneousEfficiency > 200) {
                instantaneousEfficiency = -100;
            }

            // frontPower = frontPower + 1;
            // if (frontPower >= 150) {
            //     frontPower = -50;
            // } 

            // rearPower = rearPower + 1;
            // if (rearPower >= 150) {
            //     rearPower = -50;
            // }

            socAVE = socAVE + 0.01;
            if (socAVE >= 100.0) {
                socAVE = 0.0;
            }

            battPowerW = battPower * 1000;

            if (UIspeed == 0) {
                instantaneousEfficiency = 0;
            } else {
                instantaneousEfficiency = round(battPowerW / UIspeed); // UI Speed is in miles per hour
                if (instantaneousEfficiency >= 999) {
                    instantaneousEfficiency = 999;
                }
            }
            sendToDisplay(0x000, instantaneousEfficiency, "wh/m");

            //sendToDisplay(0x132, battVolts, battAmps, battPower, "V", "A", "KW");
            sendToDisplay(0x266, rearPower, rearPowerLimit, maxRegen, "KW", "KW", "KW");
            sendToDisplay(0x2E5, frontPower, frontPowerLimit, "KW", "KW");

            if (socAVE != socAVE_temp) {
                sendToDisplay(0x292, socAVE, "%");
                socAVE_temp = socAVE;
            }

            if (avgBattTemp != avgBattTemp_temp) {
                sendToDisplay(0x312, avgBattTemp, "F");
                avgBattTemp_temp = avgBattTemp;
            }

            // sendToDisplay(0x264, chargeLineCurrent, chargeLineVoltage, chargeLinePower, "KW", "KW", "KW");

            sendToDisplay(0x336, maxDischarge, maxRegen, "KW", "KW");
            //sendToDisplay(0x001, testZeroToSixty, "s");
            //sendToDisplay(0x002, UIspeed, "mph");
        }
    }

    // Serial.print("Temp UI Speed: ");
    // Serial.println(temp_UIspeed);

    // Serial.print("UI Speed: ");
    // Serial.println(UIspeed);

    // Serial.print("activeZeroToSixty: ");
    // Serial.println(activeZeroToSixtyTime);

    // if (UIspeed == 0.0) {
    //     sendToDisplay(0x001, -2.0, "s");
    //     sendToDisplay(0x001, -2.0, "s");
    //     sendToDisplay(0x001, -2.0, "s");
    //     sendToDisplay(0x001, -2.0, "s");
    // }

    // if (temp_UIspeed == 0.0 && UIspeed != temp_UIspeed) {
    //     activeZeroToSixtyTime = true;
    //     currentTime_speed = millis(); // start the timer
    //     // Serial.println("Starting Timer! 0-60");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    //     sendToDisplay(0x001, -1.0, "s");
    // }
    // temp_UIspeed = UIspeed;

    // if (UIspeed >= 30.0 && activeZeroToSixtyTime) {
    //     activeZeroToSixtyTime = false;
    //     totalZeroToSixtyTime = millis() - currentTime_speed;
    //     Serial.println("Ending Timer! 0-60");
    //     Serial.print("Time: ");
    //     Serial.println(totalZeroToSixtyTime / 1000.00);
    //     //Serial.println(totalZeroToSixtyTime);
    //     double totalZeroToSixtyTime1 = totalZeroToSixtyTime / 1000.00;
    //     if (totalZeroToSixtyTime1 < 10.0) {
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //         sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
    //     } else {
    //         sendToDisplay(0x001, -3.0, "s");
    //         sendToDisplay(0x001, -3.0, "s");
    //         sendToDisplay(0x001, -3.0, "s");
    //         sendToDisplay(0x001, -3.0, "s");
    //     }
    // }
    
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
                        battPowerW = battVolts * battAmps; 
                        battPower = battVolts * battAmps / 1000;
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

            case 0x264:
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
                    socAVE = analyzeMessage.getSignal(message.data.uint64, 30, 10, 0.1, 0, false, littleEndian);

                }
                break;

            case 0x312: // BMSthermal
                if (message.length == 8) {
                    maxBattTemp = analyzeMessage.getSignal(message.data.uint64, 53, 9, 0.25, -25, false, littleEndian);
                    maxBattTemp = maxBattTemp * (9/5) + 32; // convert to f

                    minBattTemp = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    minBattTemp = minBattTemp * (9/5) + 32; // convert to f

                    avgBattTemp = (maxBattTemp + minBattTemp) / 2.0;
                }
                break;

            case 0x252: // 
                if (message.length == 8) {
                    maxRegen =  analyzeMessage.getSignal(message.data.uint64, 0, 16, 0.01, 0, false, littleEndian);
                    maxDischarge = analyzeMessage.getSignal(message.data.uint64, 16, 16, 0.01, 0, false, littleEndian);
                }
                break;

            case 0x352: // BMS_energyStatus
                if (message.length == 8) {
                    expectedEnergyRemaining = analyzeMessage.getSignal(message.data.uint64, 22, 11, 0.1, 0, false, littleEndian);
                    nominalEnergyRemaining = analyzeMessage.getSignal(message.data.uint64, 11,11, 0.1, 0, false, littleEndian);
                    nominalFullPackEnergy = analyzeMessage.getSignal(message.data.uint64, 0, 11, 0.1, 0, false, littleEndian);
                }
                break;
                    
            case 0x257: // Vehicle Speed
                if (message.length == 8) {
                    // UIspeed = analyzeMessage.getSignal(message.data.uint64, 12, 12, 0.08, -40, false, littleEndian); // this is in kph
                    // UIspeed = UIspeed * 0.621371; // for mph

                    UIspeed = analyzeMessage.getSignal(message.data.uint64, 24, 9, 1, 0, false, littleEndian); // this is in mph
                
                    if (UIspeed == 0.0) {
                        sendToDisplay(0x001, -2.0, "s");
                        sendToDisplay(0x001, -2.0, "s");
                        sendToDisplay(0x001, -2.0, "s");
                        sendToDisplay(0x001, -2.0, "s");
                    }

                    if (temp_UIspeed == 0.0 && temp_UIspeed != UIspeed) {
                        activeZeroToSixtyTime = true;
                        currentTime_speed = millis(); // start the timer
                        sendToDisplay(0x001, -1.0, "s");
                        sendToDisplay(0x001, -1.0, "s");
                        sendToDisplay(0x001, -1.0, "s");
                        sendToDisplay(0x001, -1.0, "s");
                    }
                    temp_UIspeed = UIspeed;

                    if (UIspeed >= 60.0 && activeZeroToSixtyTime) {
                        activeZeroToSixtyTime = false;
                        totalZeroToSixtyTime = millis() - currentTime_speed;
                        double totalZeroToSixtyTime1 = totalZeroToSixtyTime / 1000.00;
                        if (totalZeroToSixtyTime1 < 10.0) {
                            sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
                            sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
                            sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
                            sendToDisplay(0x001, totalZeroToSixtyTime1, "s");
                        } else {
                            sendToDisplay(0x001, -3.0, "s");
                            sendToDisplay(0x001, -3.0, "s");
                            sendToDisplay(0x001, -3.0, "s");
                            sendToDisplay(0x001, -3.0, "s");
                        }
                    }
                }
                break;

            case 0x2E5: // frontinverterpower
                if (message.length == 8) {
                    frontPower = analyzeMessage.getSignal(message.data.uint64, 0, 11, 0.5, 0, true, littleEndian);
                    frontPowerLimit = analyzeMessage.getSignal(message.data.uint64, 48, 9, 1, 0, false, littleEndian);
                }
                break;

            case 0x266: // rearinverterpower
                if (message.length == 8) {
                    rearPower = analyzeMessage.getSignal(message.data.uint64, 0, 11, 0.5, 0, true, littleEndian);
                    rearPowerLimit = analyzeMessage.getSignal(message.data.uint64, 48, 9, 1, 0, false, littleEndian);
                }
                break;
    
            default:
                break;
        }
    }
}
