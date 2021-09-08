//
//  button.cpp
//  To be used with CANserver created by Josh Wardell
//
//  Created by Ross Klonowski on September 3 2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <esp_now.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <stdio.h>
#include <stdlib.h>
#include <LiquidCrystal.h>
#include "display.h"

#include "alphanumeric.h"
#include "bargraph.h"
#include "lcd.h"

// int lastSwitchValue = 0;

// static bool isIdle = true;
// static bool isFirstRecv = true;
// static bool debug = false;

// static int lastCount = 0;

// String valueToPrint = "";

// static int refreshCount = 0;

// static bool displayBattPower = true;
// static bool displayBattAmps = false;
// static bool displayBattVolts = false;
// static bool displayGrade = false;
// static bool displayBattTemp = false;
// static bool displayLimits = false;
// static bool displayInstantaneousEfficiency = true;
// static bool displayVehicleSpeed = false;
// static bool displayZeroToSixty = false;
// static bool displaySpeed = false;

// static bool displayFrontPower = true;
// static bool displayRearPower = true;

// void setupDisplays() {
//     setupAlphaNum();
//     setupBargraph();
//     setupLCD();

//     displayLoadingAnimationAlpha();
//     displayLoadingAnimationBarGraph();
//     displayLoadingAnimationLCD();
// }
