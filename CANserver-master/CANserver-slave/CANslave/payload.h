#include <esp_now.h>

typedef struct payload {
    uint32_t can_id; // 4 bytes
    int int_value_1; // 4 bytes
    int int_value_2; // 4 bytes
    int int_value_3; // 4 bytes
    double double_value_1; // 8 bytes
    double double_value_2; // 8 bytes
    double double_value_3; // 8 bytes
} payload;