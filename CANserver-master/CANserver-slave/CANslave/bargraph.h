//
//  bargraph.h
//
//
//  Created by Ross Klonowski on September 3 2021.
//

#ifndef bargraph_h
#define bargraph_h

void writeLED(int display, int led);

void sendToBarGraphPower(String, int power, int maxPower, int maxRegenPower);

void setBarGraphValue(int, int);

void setBarGraphValue(int, int, uint8_t);

void writeBarGraphs();

void setupBarGraphs();

void clearBarGraphs();

void displayLoadingAnimationBarGraph();

#endif /* bargraph_h */