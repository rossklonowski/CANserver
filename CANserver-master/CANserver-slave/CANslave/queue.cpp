//
//  queue.cpp
//
//

#include "queue.h"
#include <Adafruit_Sensor.h> // allows serial to work??


my_queue::my_queue() {
    queue[size] = { 0 };
}

void my_queue::push(int val) {
    // Serial.println("Pushing... " + String(val));
    for (int i = size - 2; i >= 0; i--) {
        int temp = queue[i];
        queue[i + 1] = temp;
    } 
    queue[0] = val;
}

void my_queue::print() {
    for (int i = 0; i < size; i++) {
        Serial.print(String(queue[i]) + ",");
    }
    Serial.println("");
}

int my_queue::get_size() {
    return size;
}

int my_queue::get_val(int index) {
    return queue[index];
}
