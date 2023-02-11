//
//  CANslave.ino
//
//  Created by Ross Klonowski on January 16 2021.
//

#include <esp_now.h>
#include <WiFi.h>
#include <stdio.h>
#include <stdlib.h>

#include <ezButton.h>
// display stuff
#include <LiquidCrystal.h>
#include "bargraph.h"
#include "button.h" 
#include "lcd.h"
#include "sendHelper.h"
#include "oled.h"
#include "accelerometer.h"
#include "SCD40.h"
// OLED
#include <SPI.h>

#include "payload.h"
#include "carDataVariables.h"
#include "display_settings.h"
#include "program_settings.h"
#include "queue.h"

#include "ProgressBar.h"

#include "PriUint64.h"


#define LED1 1    // shared with serial tx - try not to use
#define LED2 2    // onboard blue LED

OLED oled_1;
// my_queue queue_1;

ezButton page_button(page_button_pin);
ezButton invert_color_button(invert_color_button_pin);
ezButton reset_button(reset_data_button_pin);

static bool isIdle = true;
static bool isFirstRecv = true;
static bool debug = false;
static int lastCount = 0;
static int refreshCount = 0;

// page stuff
static int start_page = 1;
const int max_pages = 6;
static int page = start_page;

unsigned long previouscycle = 0;
unsigned long update_previouscycle = 0;

static int interval = 1000;
static int update_interval = 100;

// graph
static int graph_last_update = 0;
static int graph_update_interval = 100;

// loop iterations
static int last_loop_iters = 0;
unsigned long previous_loop_iter_check = 0;
unsigned long loop_iter_check_interval = 1000;

static int previouscycleCheckForMaster = 0;
static int intervalCheckForMaster = 5000;

static int messages_received_counter = 0;
static int last_messages_received_counter = 0;

unsigned long timeSinceLastAskPingFromMaster = 0;
unsigned long  millisAtLastPing = 0;

bool connectedToMaster = true;

double data_rate = 0.0;

unsigned long energy_timer = 0.0;
unsigned long energy_last_timer = 0.0;

// SCD40 Vars
float temp_f = 0.0;
float humidity = 0.0;
float c02 = 0.0;

bool was_button_pressed(String button) {
    bool pressed = false;
    if (button == "reset") {
        pressed = reset_button.isPressed();
    } else if (button == "invert") {
        pressed = invert_color_button.isPressed();
    } else if (button == "page") {
        pressed = page_button.isPressed();
    }
    return pressed;
}

void handle_received_data(payload payload) {

    switch (payload.can_id) {
        case 0x312 : // batt temp
            minBattTemp.setValue(payload.double_value_1 * (9/5) + 32);
            maxBattTemp.setValue(payload.double_value_2 * (9/5) + 32);
            avgBattTemp.setValue((minBattTemp.getValue() + maxBattTemp.getValue()) / 2.0);

            break;

        case 0x252 : // max regen, min discharge
            maxDischargeClass.setValue(payload.double_value_1 * 1000.00);
            maxRegenClass.setValue(payload.double_value_2 * 1000.00);
            // queue_1.push(maxRegen);
            
            break;

        case 0x132 : // HVBattAmpVolt
            battVoltsClass.setValue(payload.double_value_1);
            battAmpsClass.setValue(payload.double_value_2);
            battPowerClass.setValue(payload.double_value_3);

            if (energy_last_timer == 0.0) {
                // skip but set it
                energy_last_timer = millis();
            } else {
                float time_since_last_timestamp = millis() - energy_last_timer;
                float time_since_last_timestamp_hrs = time_since_last_timestamp / (3.6*(1000000));
                float energy_over_last_interval = ((battAmpsClass.getValue() * battVoltsClass.getValue())/1000) * time_since_last_timestamp_hrs;
                
                sampled_energy_counter = sampled_energy_counter + energy_over_last_interval;
            }

            energy_last_timer = millis();

            break;

        case 0x001 : // 0-60 time
            // write 0-60
            if (displayZeroToSixty) {
                //printValueWithSingleUnit(payload.double_value_1, 7);
            }

            break;

        case 0x2E5 : // frontPower
            frontPowerClass.setValue(payload.double_value_1 * 1000.00);
            frontPowerLimitClass.setValue(payload.double_value_2 * 1000.00);

            if (frontPowerClass.getValue() >= frontPowerMax) {
                frontPowerMax = frontPowerClass.getValue();
            }
            
            break;

        case 0x292 : // BMS_SOC
            socAVE.setValue(payload.double_value_1);
            battTempPct.setValue(payload.double_value_2);
            
            break;

        case 0x257 : // DIspeed
            UIspeed = payload.double_value_1;
            UIspeed = UIspeed * 0.621371; // for mph

            break;

        case 0x264 : // ID264ChargeLineStatus
            chargeLineCurrentClass.setValue(payload.double_value_1);
            chargeLineVoltageClass.setValue(payload.double_value_2);
            chargeLinePowerClass.setValue(payload.double_value_3);

            break;

        case 0x266 : // rearPower

            rearPowerClass.setValue(payload.double_value_1 * 1000.00);
            rearPowerLimitClass.setValue(payload.double_value_2 * 1000.00);
            
            break;

        case 0x321 :
            tempCoolantBatInletClass.setValue(payload.double_value_1 * (9/5) + 32.00);
            tempCoolantPTInletClass.setValue(payload.double_value_2 * (9/5) + 32.00);

            break;

        case 0x352 : // bms_energystatus
            nominalEnergyRemaining = payload.double_value_1;
            nominalFullPackEnergy = payload.double_value_2;

            break;

        case 0x376 : // frontInverterTemps
            frontInverterTempClass.setValue(payload.double_value_1 * (9/5) + 32.00);

            break;

        case 0x315 : // rearInverterTemps
            rearInverterTempClass.setValue(payload.double_value_1 * (9/5) + 32.00);

            break;

        case 0x3E7 : // custom uptime
            masterUpTime = payload.int_value_1;

            break;

        case 0x3B6 : // odometer
            odometer = payload.double_value_1;
            if (startOfTripOdometer == 0) {
              startOfTripOdometer = odometer;
            }

            break;

        case 0x3E6 : // send im up message if we are asked for it
            timeSinceLastAskPingFromMaster = 0;
            millisAtLastPing = millis();
            connectedToMaster = true;
            // send response that I am up
            digitalWrite(LED2, LOW);
            
            sendToDisplay(masterMacAddress, 0x3E6, 1);

            break;

        case 0x241 :
            coolantFlowBatActualClass.setValue(payload.double_value_1);
            coolantFlowPTActualClass.setValue(payload.double_value_2);
    
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
    Serial.begin(115200);
    delay(200);
    Serial.println("Starting Setup...");

    pinMode(LED2, OUTPUT); // configure blue LED
    digitalWrite(LED2, HIGH);

    // I2C needed for C02 and Accelerometer
    Wire.begin(I2C_SDA, I2C_SCL);

    // setupBarGraphs();
    // displayLoadingAnimationBarGraph();    

    scd40_setup();

    // Accel
    accel_setup();

    oled_1.setupOLED();
 
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

    Serial.println("Finished with setup!");

    // uint64_t var = 0xD3A0000002306B0C;
    // Serial.println(PriUint64<HEX>(var));
    // // var = var << 2;
    // uint64_t mask = 0xFFFFFFFF;
    // var = var & mask;
    // Serial.println(PriUint64<HEX>(var));
}

void loop() {
    loop_counter++;
    long iter_loop_current = millis();
    if (iter_loop_current - previous_loop_iter_check >= loop_iter_check_interval) {
        previous_loop_iter_check = iter_loop_current;
        last_loop_iters = loop_counter;
    }

    invert_color_button.loop();
    reset_button.loop();
    page_button.loop();

    if (was_button_pressed("page")) {
        page = page + 1;
        if (page == max_pages + 1) {
            page = 1; // rollover
        } 
    }

    if (was_button_pressed("invert")) {
        oled_1.oled_invert_color();
    }

    if (was_button_pressed("reset")) {
        
        if (page == 6) {
            accel_offset = accel_vector;
            max_accel_vector = 0;

            max_g_vector = 0;
            max_accel_vector = 0;
        }

        if (page == 2) {
            lastNominalEnergyRemaining = nominalEnergyRemaining;
            startOfTripOdometer = 0.0;
            sampled_energy_counter = 0.0;
        }
    }

    // get data rate stats and update after each time interval (1 second)
    unsigned long  currentMillis = millis();
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
            connectedToMaster = false;   
            digitalWrite(LED2, HIGH);
        } else {
            digitalWrite(LED2, HIGH);
        }
    }

    // update displays each specified interval
    long update_currentMillis = millis();
    if (update_currentMillis - update_previouscycle >= update_interval) {
        update_previouscycle = update_currentMillis;

        String unit_space = " ";

        if (page == 1) {
            oled_1.clearDisplay();

            oled_1.send_to_oled_buffer(0, minBattTemp.getString() + " " + maxBattTemp.getString());
            oled_1.send_to_oled_buffer(0, "                " + socAVE.getString(1));
            oled_1.send_to_oled_buffer(1, battPowerClass.getString() + " " + battVoltsClass.getString());
            // ProgressBar prog1(1, 20, 126, 3, false);
            // double percentageOfBar = 0.0;
            // if (maxDischargeClass.isSet()) {
            //     percentageOfBar = log10((double)abs(battPowerClass.getValue())) / log10((double)maxDischargeClass.getValue()); // log10
            //     // percentageOfBar = (abs(battPower)) / (maxDischarge*1000); // linear
            //     prog1.setPercentFill(percentageOfBar);
            //     oled_1.draw(prog1);
            // }

            powerNotFromMotors.setValue((battPowerClass.getValue() - (frontPowerClass.getValue() + rearPowerClass.getValue())));
            oled_1.send_to_oled_buffer(3, powerNotFromMotors.getString());
            oled_1.send_to_oled_buffer(4, coolantFlowBatActualClass.getString(0) + " " + coolantFlowPTActualClass.getString(0));
        
            oled_1.send_to_oled_buffer(6, maxRegenClass.getString() + " " + maxDischargeClass.getString());
            oled_1.send_to_oled_buffer(7, String(nominalEnergyRemaining) + "/" + String(nominalFullPackEnergy) + "KWh");
            
            oled_1.oled_update();
        }

        if (page == 2) {
            oled_1.clearDisplay();

            oled_1.send_to_oled_buffer(0, "Trip");

            double trip_distance_miles = (double)(odometer - startOfTripOdometer) * 0.621371;
            oled_1.send_to_oled_buffer(1, " " + String(trip_distance_miles) + unit_space + "mi");
            
            oled_1.send_to_oled_buffer(2, " " + String(sampled_energy_counter) + unit_space + "KWh");
            
            float efficiency_whm = 0.0;
            if (trip_distance_miles != 0.0) {
                efficiency_whm = (sampled_energy_counter / trip_distance_miles) * 1000.00;
            }
            oled_1.send_to_oled_buffer(3, " " + String(efficiency_whm, 0) + unit_space + "Wh/mi");

            oled_1.send_to_oled_buffer(6, "Odometer");
            oled_1.send_to_oled_buffer(7, " " + String(odometer * 0.621371, 2) + unit_space + "mi");
            
            oled_1.oled_update();
        }

        // just front and rear motor power
        if (page == 3) {
            oled_1.clearDisplay();

            oled_1.send_to_oled_buffer(0, "Motors ");
            oled_1.send_to_oled_buffer(1, " F " + frontPowerClass.getString());
            // oled_1.send_to_oled_buffer(2, " FL " + frontPowerLimitClass.getString());
            oled_1.send_to_oled_buffer(2, " " + frontInverterTempClass.getString());
            
            oled_1.send_to_oled_buffer(4, " R " + rearPowerClass.getString());
            // oled_1.send_to_oled_buffer(5, " RL " + rearPowerLimitClass.getString());
            oled_1.send_to_oled_buffer(5, " " + rearInverterTempClass.getString());
            
            oled_1.oled_update();
        }

        // Charger stuff
        if (page == 4) {
            oled_1.clearDisplay();

            oled_1.send_to_oled_buffer(0, "Charge Line");
            oled_1.send_to_oled_buffer(1, chargeLinePowerClass.getString());
            oled_1.send_to_oled_buffer(2, chargeLineVoltageClass.getString());
            oled_1.send_to_oled_buffer(3, chargeLineCurrentClass.getString());

            
            oled_1.oled_update();
        }

        // things that don't change much
        if (page == 5) {
            oled_1.clearDisplay();
            
            oled_1.send_to_oled_buffer(0, "Temps");
            oled_1.send_to_oled_buffer(1, " Batt Clnt " + tempCoolantBatInletClass.getString());
            oled_1.send_to_oled_buffer(2, " PwTr Clnt " + tempCoolantPTInletClass.getString());
            oled_1.send_to_oled_buffer(3, " Batt Min  " + minBattTemp.getString());
            oled_1.send_to_oled_buffer(4, " Batt Max  " + maxBattTemp.getString());
            
            oled_1.oled_update();
        }

        if (page == 6) {
            if (scd40_data_ready()) {
                oled_1.clearDisplay();
                
                scd40_get_data(c02, temp_f, humidity);
                oled_1.send_to_oled_buffer(0, "SCD40");
                oled_1.send_to_oled_buffer(1, " Temp     " + String(temp_f) + unit_space + "F");
                oled_1.send_to_oled_buffer(2, " Humidity " + String(humidity) + unit_space + "%");
                oled_1.send_to_oled_buffer(3, " C02      " + String(c02) + unit_space + "ppm");
                
                oled_1.oled_update();
            }
        }

        if (page == 6) { // accelerometer stuff
            oled_1.clearDisplay();
            
            float accel_x = 0.0;
            float accel_y = 0.0;
            float accel_z = 0.0;
            accel_get_g_force(accel_x, accel_y, accel_z);
            accel_vector = sqrt( (accel_x*accel_x) + (accel_y*accel_y) + (accel_z*accel_z) );
            accel_vector = accel_vector - accel_offset;
            if (accel_vector > max_accel_vector) {
                max_accel_vector = accel_vector;
            }
            
            float g_x = accel_x / 9.81;
            float g_y = accel_x / 9.81;
            float g_z = accel_x / 9.81;
            float g_vector = accel_vector / 9.81;

            if (g_vector > max_g_vector) {
                max_g_vector = g_vector;
            }
            
            String sign1 = (accel_x >= 0) ? "+" : "";
            String sign2 = (g_x >= 0) ? "+" : "";
            String sign3 = (accel_y >= 0) ? "+" : "";
            String sign4 = (g_y >= 0) ? "+" : "";
            String sign5 = (accel_z >= 0) ? "+" : "";
            String sign6 = (g_z >= 0) ? "+" : "";
            String sign7 = (accel_vector >= 0) ? "+" : "";
            String sign8 = (max_accel_vector >= 0) ? "+" : "";
            String sign9 = (g_vector >= 0) ? "+" : "";
            String sign10 = (max_g_vector >= 0) ? "+" : "";

            oled_1.send_to_oled_buffer(0, 1, 1, "" + String(accel_vector, 1) + "m/s^2");
            oled_1.send_to_oled_buffer(1, 2, 9, "" + String(max_accel_vector, 1) + "m/s^2");
            oled_1.send_to_oled_buffer(2, 1, 24, "" + String(g_vector, 1) + "g");
            oled_1.send_to_oled_buffer(3, 2, 32, "" + String(max_g_vector, 1) + "g");

            oled_1.oled_update();
        }

        if (page == 7) {
            oled_1.clearDisplay();

            oled_1.send_to_oled_buffer(1, " Msgs/s  " + String(data_rate));

            oled_1.oled_update();
        }

        if (page == 8) {
            oled_1.clearDisplay();
            oled_1.oled_image(1);
            oled_1.oled_update();
        }
    }   
}
