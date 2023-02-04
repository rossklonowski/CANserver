#include "esp32_can.h"  //RX GPIO16 TX GPIO 17 https://github.com/collin80/esp32_can
#include "sendHelper.h"
#include <esp_now.h> // for ESP32 to ESP32 wifi communication
#include "payload.h"

static bool send_debug = false;


int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1) {

    payload payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = -1; 
    payload.int_value_3 = -1;
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));


    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1, int valueToSend2) {

    payload payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1, int valueToSend2, int valueToSend3) {

    payload payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    payload.int_value_3 = valueToSend3;
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1) {

    payload payload;

    payload.can_id = can_id;
    payload.double_value_1 = valueToSend1;
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1, double valueToSend2) {
    payload payload;

    payload.can_id = can_id;
    payload.double_value_1 = valueToSend1;
    payload.double_value_2 = valueToSend2;

    // Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1, double valueToSend2, double valueToSend3) {
    payload payload;

    payload.can_id = can_id;
    payload.double_value_1 = valueToSend1;
    payload.double_value_2 = valueToSend2;
    payload.double_value_3 = valueToSend3;
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}
