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

int sendToDisplay(const uint8_t *receiverMacAddress, uint32_t can_id, double valueToSend1) {

    struct_message payload;

    payload.can_id = can_id;
    payload.double_value_1 = valueToSend1;

    
    esp_err_t result = 0;
    result = esp_now_send(receiverMacAddress, (uint8_t *) &payload, sizeof(payload));
    // Serial.println("Result: ");
    // Serial.println(result == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

    return result;
}