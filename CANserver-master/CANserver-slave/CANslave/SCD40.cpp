//
//  scd40.cpp
//  To be used with CANserver created by Josh Wardell
//

// OLED
#include <SPI.h>
#include "Custom_Adafruit_GFX.h"

// SCD40 Temp and C02
#include <SensirionI2CScd4x.h>
#include "SCD40.h"

SensirionI2CScd4x scd4x;
bool isSetup = false;

void scd40_setup() {
    scd4x.begin(Wire);
	scd4x.stopPeriodicMeasurement(); // stop any previous measurements
    scd4x.startPeriodicMeasurement(); // start periodic measurements
	isSetup = true;
}

bool scd40_data_ready() {
	uint16_t dataReady;
	scd4x.getDataReadyStatus(dataReady);
	if (dataReady ==  32774) {
		return true;
	} else {
		return false;
	}
}

void scd40_get_data(float &c02, float &temp_c, float &humidity) {
	
    uint16_t error = 0;
    char errorMessage[256];

	uint16_t dataReady;
	scd4x.getDataReadyStatus(dataReady);
	
	if (dataReady ==  32774) {
		uint16_t co2_local;
		float temp_c_local, humidity_local;
		error = scd4x.readMeasurement(co2_local, temp_c_local, humidity_local);
		if (error) {
			Serial.print("Error trying to execute readMeasurement(): ");
			errorToString(error, errorMessage, 256);
		} else {
			float temp_f = (temp_c_local * 9/5) + 32;
			temp_c = temp_f; 
			humidity = humidity_local;
			c02 = co2_local; 
		}
	}
}

bool scd40IsSetup() {
	return isSetup;
}
