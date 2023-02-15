//
//  generalCANSignalAnalysis.h
//  To be used with CANserver created by Josh Wardell
//
//  Created by Chris Allemang on July 4 2020.
//

#ifndef sendhelper_h
#define sendhelper_h

#include "esp32_can.h"  //RX GPIO16 TX GPIO 17 https://github.com/collin80/esp32_can

int sendToDisplay(const uint8_t *, uint32_t, int);

int sendToDisplay(const uint8_t *, uint32_t, int, int);

int sendToDisplay(const uint8_t *, uint32_t, int, int, int);

int sendToDisplay(const uint8_t *, uint32_t, double);

int sendToDisplay(const uint8_t *, uint32_t, double, double);

int sendToDisplay(const uint8_t *, uint32_t, double, double, double);
int sendToDisplay(const uint8_t *, uint32_t, double, double, double, double);

#endif /* sendHelper_h */
