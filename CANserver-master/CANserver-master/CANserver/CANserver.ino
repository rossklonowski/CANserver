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

#include "generalCANSignalAnalysis.h" //https://github.com/iChris93/ArduinoLibraryForCANSignalAnalysis
#include <stdio.h>
#include <AsyncJson.h> // for wifi??
#include <esp_now.h> // for ESP32 to ESP32 wifi communication
#include "sendHelper.h"
#include "carDataVariables.h"
#include "simulation.h"
#include "constants.h"
#include "payload.h"
#include "PriUint64.h"

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
static bool simulation = false;

/////////////////////////    TIMERS    //////////////////////////
// up time timer
static int interval = 1000;
unsigned long previouscycle = 0;

// simulation timer
static int simulate_interval = 50;
unsigned long simulate_previous_cycle = 0; 

// sending im up message timer
static int intervalReceiver = 1000;
unsigned long previouscycleReceiver = 0;

// timer for since last ack to im up
long timeSinceLastReceiverPing = 0;
unsigned long millisAtLastPing = 0;
/////////////////////////    TIMERS    //////////////////////////

bool connectedToSlave = true;

generalCANSignalAnalysis analyzeMessage; // initialize library

unsigned long currentMillisSlave = millis();

void handle_received_data(payload payload) {
    switch (payload.can_id) {
        case 0x3E6 : // Received an Ack
            connectedToSlave = true;
            digitalWrite(LED2, LOW);

            timeSinceLastReceiverPing = 0;
            millisAtLastPing = millis();

            if (payload.double_value_1 == 0.0) {
                simulation = false;
            } else {
                simulation = true;
            }
        
            break;
    }
}

// callback function that tells us when data from Master is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

    payload new_data;
    memcpy(&new_data, incomingData, sizeof(new_data));

    handle_received_data(new_data); // decode message received and update data variables
}


// callback function - gives us feedback about the sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        // TODO track successes per second
    } else {
        // TODO track failures per second
    }
}


void setup(){

    pinMode(LED2, OUTPUT); // configure blue LED
    digitalWrite(LED2, HIGH);

    Serial.begin(115200);
    delay(100);
    Serial.println("Starting setup");

    // uint64_t x = 85964712;
    // Serial.print(PriUint64<DEC>(x));
    // Serial.print("\n");


    CAN0.begin(BITRATE);
    CAN0.watchFor(); // then let everything else through anyway

    // init wifi station (STA means device that is capable of 802.11 protocol)
    WiFi.mode(WIFI_STA);

    // init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // create call back (OnDataRecv will run every time a message is received via esp now)
    esp_now_register_recv_cb(OnDataRecv);

    // connect function that runs when data is sent
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo;
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
    // add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("Completed Setup");

}

void loop() {

    unsigned long currentMillis = millis();
    timeSinceLastReceiverPing = currentMillis - millisAtLastPing;
    if (timeSinceLastReceiverPing > 2000) {
        if (connectedToSlave) {
            connectedToSlave = false;    
        }
        if (connectedToSlave == false) {
            digitalWrite(LED2, HIGH);
        }
    }

    if (simulation) {
        unsigned long simulate_millis = millis();
        if (simulate_millis - simulate_previous_cycle >= simulate_interval) {
            simulate_previous_cycle = simulate_millis;
            simulate();
        }
    }

    unsigned long up_time_currentMillis = millis();
    if (up_time_currentMillis - previouscycle >= interval) {
        previouscycle = up_time_currentMillis;
        masterUpTime = masterUpTime + 1;
        sendToDisplay(receiverMacAddress, 0x3E7, masterUpTime);
    }

    unsigned long you_up_millis = millis();
    if (you_up_millis - previouscycleReceiver >= intervalReceiver) {
        previouscycleReceiver = you_up_millis;
        // digitalWrite(LED2, !digitalRead(LED2)); // flash led for loop iter
        sendToDisplay(receiverMacAddress, 0x3E6, 1); // ask slave if they are up
    }

    // can message processing follows
    CAN_FRAME message;

    if (CAN0.read(message)) {
        if (debug) {
            Serial.print("message ID(hex): ");
            Serial.print(message.id, HEX);
            
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

        
        switch (message.id) {
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
                        battPower = battVolts * battAmps; 
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
                // if (message.length == 8) {
                    chargeLineVoltage = analyzeMessage.getSignal(message.data.uint64, 0, 14, .0333, 0, false, littleEndian);
                    chargeLineCurrent = analyzeMessage.getSignal(message.data.uint64, 14, 9, 0.1, 0, false, littleEndian);
                    chargeLinePower = analyzeMessage.getSignal(message.data.uint64, 24, 8, 0.1, 0, false, littleEndian);
                // }
                break;

            case 0x292: // BMS_SOC 
                if (message.length == 8) {
                    socAVE = analyzeMessage.getSignal(message.data.uint64, 30, 10, 0.1, 0, false, littleEndian);
                    battTempPct = analyzeMessage.getSignal(message.data.uint64, 50, 8, 0.4, 0, false, littleEndian);
                }
                break;

            case 0x312: // BMSthermal
                if (message.length == 8) {
                    maxBattTemp = analyzeMessage.getSignal(message.data.uint64, 53, 9, 0.25, -25, false, littleEndian);
                    minBattTemp = analyzeMessage.getSignal(message.data.uint64, 44, 9, 0.25, -25, false, littleEndian);
                    batteryCoolTarget = analyzeMessage.getSignal(message.data.uint64, 17, 9, 0.25, -25, false, littleEndian);
                    batteryHeatTarget = analyzeMessage.getSignal(message.data.uint64, 35, 9, 0.25, -25, false, littleEndian);

                }
                break;

            case 0x252: // 
                if (message.length == 8) {
                    maxRegen =  analyzeMessage.getSignal(message.data.uint64, 0, 16, 0.01, 0, false, littleEndian);
                    maxDischarge = analyzeMessage.getSignal(message.data.uint64, 16, 16, 0.01, 0, false, littleEndian);
                }
                break;

            case 0x321: //VCFRONT_sensors
                // if (message.length == 8) {
                    tempCoolandBatInlet = analyzeMessage.getSignal(message.data.uint64, 0, 10, 0.125, -40, false, littleEndian);
                    tempCoolandBatPTlet = analyzeMessage.getSignal(message.data.uint64, 10,11, 0.125, -40, false, littleEndian);
                // }
                break;

            case 0x352: // BMS_energyStatus
                if (message.length == 8) {
                    int multiplexor = analyzeMessage.getSignal(message.data.uint64, 0, 2, 1, 0, false, littleEndian);
                    if (multiplexor == 0) {
                        nominalFullPackEnergy = analyzeMessage.getSignal(message.data.uint64, 16, 16, 0.02, 0, false, littleEndian);
                        nominalEnergyRemaining = analyzeMessage.getSignal(message.data.uint64, 32, 16, 0.02, 0, false, littleEndian);
                    } else if (multiplexor == 1) {
                        energyBuffer = analyzeMessage.getSignal(message.data.uint64, 16, 16, 0.01, 0, false, littleEndian);
                    } else if (multiplexor == 2) {
                        expectedEnergyRemaining = analyzeMessage.getSignal(message.data.uint64, 48, 16, 0.02, 0, false, littleEndian);
                    }
                }
                
                break;
                    
            case 0x257: // Vehicle Speed
                if (message.length == 8) {
                    UIspeed = analyzeMessage.getSignal(message.data.uint64, 12, 12, 0.08, -40, false, littleEndian); // this is in kph
                }
                break;

            case 0x2E5: //
                if (message.length == 8) {
                    frontPower = analyzeMessage.getSignal(message.data.uint64, 0, 11, 0.5, 0, true, littleEndian);
                    frontPowerLimit = analyzeMessage.getSignal(message.data.uint64, 48, 9, 1, 0, false, littleEndian);
                }
                break;

            case 0x266: // 
                if (message.length == 8) {
                    rearPower = analyzeMessage.getSignal(message.data.uint64, 0, 11, 0.5, 0, true, littleEndian);
                    rearPowerLimit = analyzeMessage.getSignal(message.data.uint64, 48, 9, 1, 0, false, littleEndian);
                }
                break;

            case 0x383: // VCRIGHT_thsStatus
                if (message.length == 8) {
                    cabin_temp = analyzeMessage.getSignal(message.data.uint64, 30, 11, 0.1, -40, false, littleEndian);
                }
                break;

            case 0x376: // frontInverterTemps
                if (message.length == 8) {
                    int multiplexor = analyzeMessage.getSignal(message.data.uint64, 0, 3, 1, 0, false, littleEndian);
                    if (multiplexor == 0) {
                        frontInverterTemp = analyzeMessage.getSignal(message.data.uint64, 16, 8, 1, -40, false, littleEndian);
                    }
                }

            case 0x315: // reartInverterTemps
                if (message.length == 8) {
                    int multiplexor = analyzeMessage.getSignal(message.data.uint64, 0, 3, 1, 0, false, littleEndian);
                    if (multiplexor == 1) {
                        rearInverterTemp = analyzeMessage.getSignal(message.data.uint64, 8, 8, 1, -40, false, littleEndian);
                    }
                }
                break;

            case 0x3B6: // odometer
                if (message.length == 4) {
                    Serial.println("message length was " + String(message.length));
                    Serial.println("ODOMETER: ");
                    
                    Serial.print("message ID(hex): ");
                    Serial.println(message.id, HEX);
                    
                    Serial.print("message length(dec): ");
                    Serial.print(message.length, DEC);
                
                    for (int i = 0; i < message.length; i++) {
                        Serial.print("byte ");
                        Serial.print(i);
                        Serial.print(": ");
                        Serial.print(message.data.byte[i], HEX);
                        Serial.print(" ");
                    }

                    Serial.print("message.data.uint64: ");
                    Serial.println(PriUint64<HEX>(message.data.uint64));
                    uint64_t odometer_64 = message.data.uint64 & 0xFFFFFFFF;
                    Serial.print("After bit mask: ");
                    Serial.println(PriUint64<HEX>(odometer_64));
                    
                    odometer = float(odometer_64 * 0.001);
                    Serial.println(odometer * 0.621371);
                    Serial.print("-----------------------------\n");

                    // if (message.length == 4) {
                    //     // do our own decoding since
                    //     // there are other values on this same
                    //     // signal and the normal method is
                    //     // not working
                    //     float kilometers = float(message.data.uint64 * 0.001);
                    //     if (kilometers <= 50000) {
                    //         odometer = kilometers;
                    //     }
                    // }
                } else {
                    Serial.println("message length was actually" + String(message.length));
                }
                break;

            case 0x2B3: // VCRIGHT_logging1Hz
                if (message.length == 8) {
                    int multiplexor = analyzeMessage.getSignal(message.data.uint64, 0, 4, 1, 0, false, littleEndian);
                    if (multiplexor == 11) {
                        cabin_humidity = analyzeMessage.getSignal(message.data.uint64, 24, 8, 1, 0, false, littleEndian);
                    }
                }
                break;

            case 0x241: // VCRIGHT_logging1Hz
                if (message.length == 7) {
                    coolantFlowBatActual = analyzeMessage.getSignal(message.data.uint64, 0, 9, 0.1, 0, false, littleEndian);
                    coolantFlowPTActual = analyzeMessage.getSignal(message.data.uint64, 22, 9, 0.1, 0, false, littleEndian);
                }
                break;
    
            default:
                break;
        }
        
        if (connectedToSlave) {
            switch (message.id) {
                case 0x132: // HVBattAmpVolt
                    sendToDisplay(receiverMacAddress, 0x132, battVolts, battAmps, battPower);
                    
                    break;
                    
                case 0x1D8: // RearTorque

                    break;

                case 0x264: // charge line
                    sendToDisplay(receiverMacAddress, 0x264, chargeLineCurrent, chargeLineVoltage, chargeLinePower);
                    
                    break;

                case 0x267: // DI_vehicleEstimates

                    break;

                case 0x292: // BMS_SOC 
                    sendToDisplay(receiverMacAddress, 0x292, socAVE, battTempPct);

                    break;

                case 0x312: // BMSthermal
                    sendToDisplay(receiverMacAddress, 0x312, minBattTemp, maxBattTemp, batteryCoolTarget, batteryHeatTarget);

                    break;

                case 0x252: // 
                    sendToDisplay(receiverMacAddress, 0x252, maxDischarge, maxRegen);

                    break;

                case 0x352: // BMS_energyStatus
                    sendToDisplay(receiverMacAddress, 0x352, nominalEnergyRemaining, nominalFullPackEnergy, energyBuffer);

                    break;

                case 0x2E5: // frontinverterpower
                    sendToDisplay(receiverMacAddress, 0x2E5, frontPower, frontPowerLimit);
                    
                    break;

                case 0x266: // rearinverterpower
                    sendToDisplay(receiverMacAddress, 0x266, rearPower, rearPowerLimit);
                    
                    break;

                case 0x376: // frontInverterTemps
                    sendToDisplay(receiverMacAddress, 0x376, frontInverterTemp);
                    
                    break;

                    sendToDisplay(receiverMacAddress, 0x383, cabin_temp);
                    
                    break;
        
                case 0x315: // rearInverterTemps
                    sendToDisplay(receiverMacAddress, 0x315, rearInverterTemp);

                    break;

                case 0x2B3: // VCRIGHT_thsStatus
                    sendToDisplay(receiverMacAddress, 0x2B3, cabin_humidity);
                    
                    break;

                case 0x3B6: // odometer
                    sendToDisplay(receiverMacAddress, 0x3B6, odometer);
        
                    break; 

                case 0x241: // cooland
                    sendToDisplay(receiverMacAddress, 0x241, coolantFlowBatActual, coolantFlowPTActual);
        
                    break; 

                case 0x321: // coolant temps
                    sendToDisplay(receiverMacAddress, 0x321, tempCoolandBatInlet, tempCoolandBatPTlet);
        
                    break; 

                default:
                    
                    break;
            }
        }
    }
}
