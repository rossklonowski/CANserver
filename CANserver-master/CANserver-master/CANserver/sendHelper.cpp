#include "esp32_can.h"  //RX GPIO16 TX GPIO 17 https://github.com/collin80/esp32_can
#include "sendHelper.h"
#include <esp_now.h> // for ESP32 to ESP32 wifi communication

static bool send_debug = false;

// message struct
typedef struct struct_message {
    uint32_t can_id;
    int int_value_1;
    int int_value_2;
    int int_value_3;
    double double_value_1;
    double double_value_2;
    double double_value_3;
} struct_message;

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1, String unit1) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = -1; 
    payload.int_value_3 = -1;
    // payload.unit1 = unit1;
    // payload.unit2 = "";
    // payload.unit3 = "";

    // Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));
    
    // if (send_debug) {
    //     if (result == ESP_OK) {
    //         Serial.println("Sent with success");
    //     }
    //     else {
    //         Serial.println("Error sending the data");
    //     }

    //     Serial.print("Bytes Sent: ");
    //     Serial.println(sizeof(payload));

    //     Serial.print("int_value_1 sent: ");
    //     Serial.println(payload.int_value_1);
        
    //     Serial.print("unit1 sent: ");
    //     Serial.println(payload.unit1);

    //     Serial.print("\n");
    // }

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1, String unit1) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = -1;
    payload.int_value_2 = -1;
    payload.int_value_3 = -1;
    // payload.double_value_1 = valueToSend3;
    // payload.double_value_2 = valueToSend3;
    // payload.double_value_3 = valueToSend3;
    // payload.unit1 = unit1;
    // payload.unit2 = "";
    // payload.unit3 = "";

    // Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1, int valueToSend2, String unit1, String unit2) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    // payload.unit1 = "";
    // payload.unit2 = "";

    // Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1, int valueToSend2, int valueToSend3, String unit1, String unit2,  String unit3) {

    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    payload.int_value_2 = valueToSend2;
    payload.int_value_3 = valueToSend3;
    // payload.unit1 = unit1;
    // payload.unit2 = unit2;
    // payload.unit3 = unit3;

    // Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1, double valueToSend2) {
    struct_message payload;

    payload.can_id = can_id;
    payload.double_value_1 = valueToSend1;
    payload.double_value_2 = valueToSend2;

    // Serial.println("Size of the payload: " + String(sizeof(payload)));
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1) {
    struct_message payload;

    payload.can_id = can_id;
    payload.double_value_1 = valueToSend1;
    // Serial.println("in Sending:");
    // Serial.println(can_id);
    // Serial.println(valueToSend1);
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, int valueToSend1) {
    struct_message payload;

    payload.can_id = can_id;
    payload.int_value_1 = valueToSend1;
    Serial.println("in Sending:");
    Serial.println(can_id);
    Serial.println(valueToSend1);
    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));

    return result;
}

