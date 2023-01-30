//
//  accelerometer.cpp
//  To be used with CANserver created by Josh Wardell
//

#include <Adafruit_LIS3DH.h>
#include "accelerometer.h"
#include <Adafruit_Sensor.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void accel_setup() {

	Serial.println("LIS3DH test!");

	if (! lis.begin(0x18)) {
		Serial.println("Couldnt start");
		while (1) yield();
	}
	Serial.println("LIS3DH found!");

	// lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

	Serial.print("Range = "); Serial.print(2 << lis.getRange());
	Serial.println("G");

	// lis.setDataRate(LIS3DH_DATARATE_50_HZ);
	Serial.print("Data rate set to: ");
	switch (lis.getDataRate()) {
		case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
		case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
		case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
		case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
		case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
		case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
		case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

		case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
		case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
		case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
	}
}

void accel_get_g_force(float &accel_x, float &accel_y, float &accel_z) {
	
	lis.read(); // get X Y Z data at once

	sensors_event_t event;
	lis.getEvent(&event);

	Serial.println(event.acceleration.x);
	Serial.println(event.acceleration.y);
	Serial.println(event.acceleration.z);

	accel_x = (float)event.acceleration.x;
	accel_y = (float)event.acceleration.y;
	accel_z = (float)event.acceleration.z;
}
