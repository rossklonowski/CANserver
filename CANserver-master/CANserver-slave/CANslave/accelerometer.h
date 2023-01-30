//
//  accelerometer.h
//

#include <Adafruit_Sensor.h>

#ifndef accelerometer_h
#define accelerometer_h

void accel_setup();

void accel_get_g_force(float&, float&, float&);

#endif /* accelerometer_h */