//
//  simulation.cpp
//
//

#include "simulation.h"
#include "carDataVariables.h"
#include <ArduinoJson.h>
#include "sendHelper.h"
#include "constants.h"


void simulate() {

            odometer = odometer + 1;

            socAVE = socAVE + .1;
            if (socAVE > 80) {
                socAVE = 0;
            }

            battTempPct = battTempPct + 0.1;
            if (battTempPct > 80) {
                battTempPct = 0;
            }

            masterUpTime = masterUpTime + 1;

            nominalFullPackEnergy = nominalFullPackEnergy + 0.1;
            if (nominalFullPackEnergy > 80) {
                nominalFullPackEnergy = 0;
            }

            frontInverterTemp = frontInverterTemp + 0.1;
            if (frontInverterTemp > 80) {
                frontInverterTemp = 0;
            }

            rearInverterTemp = rearInverterTemp + 0.1;
            if (rearInverterTemp > 80) {
                rearInverterTemp = 0;
            }

            cabin_humidity = cabin_humidity + 1;
            if (cabin_humidity > 80) {
                cabin_humidity = 0;
            }

            cabin_temp = cabin_temp + 1;
            if (cabin_temp > 80) {
                cabin_temp = -32;
            }
            maxBattTemp = maxBattTemp + 0.1;
            if (maxBattTemp > 80) {
                maxBattTemp = -32;
            }

            minBattTemp = minBattTemp + 0.1;
            if (minBattTemp > 80) {
                minBattTemp = -32;
            }

            nominalEnergyRemaining = nominalEnergyRemaining + 0.1;
            if (nominalEnergyRemaining > 50) {
                nominalEnergyRemaining = 4;
            }

            battVolts = 350.00;
            // if (battVolts == 400) {
            //     battVolts = 300;
            // }

            battAmps = 171.428;
            // battAmps = battAmps + 1;
            // if (battAmps > 150) {
            //     battAmps = -100;
            // }

            battPower = battAmps * battVolts;

            chargeLineCurrent = chargeLineCurrent + 1;
            if (chargeLineCurrent == 99) {
                chargeLineCurrent = -50;
            }
            
            chargeLinePower = chargeLinePower + 1;
            if (chargeLinePower == 99) {
                chargeLinePower = -50;
            }
            
            chargeLineVoltage = chargeLineVoltage + 1;
            if (chargeLineVoltage == 99) {
                chargeLineVoltage = -50;
            }

            maxRegen = maxRegen + 1;
            if (maxRegen == 99) {
                maxRegen = 0;
            }

            maxDischarge = maxDischarge + 1;
            if (maxDischarge == 99) {
                maxDischarge = 0;
            }

            frontPower = frontPower + 1;
            if (frontPower >= 150) {
                frontPower = -50;
            } 

            rearPower = rearPower + 1;
            if (rearPower >= 150) {
                rearPower = -50;
            }

            frontPowerLimit = frontPowerLimit + 1;
            if (frontPowerLimit >= 150) {
                frontPowerLimit = -50;
            } 

            rearPowerLimit = rearPowerLimit + 1;
            if (rearPowerLimit >= 150) {
                rearPowerLimit = -50;
            }

            sendToDisplay(receiverMacAddress, 0x132, battVolts, battAmps, battPower);
            sendToDisplay(receiverMacAddress, 0x2E5, frontPower, frontPowerLimit, maxRegen);
            sendToDisplay(receiverMacAddress, 0x266, rearPower, rearPowerLimit, maxRegen);
            sendToDisplay(receiverMacAddress, 0x292, socAVE, battTempPct);
            sendToDisplay(receiverMacAddress, 0x312, minBattTemp, maxBattTemp);
            sendToDisplay(receiverMacAddress, 0x383, cabin_temp);
            sendToDisplay(receiverMacAddress, 0x315, rearInverterTemp);
            sendToDisplay(receiverMacAddress, 0x376, frontInverterTemp);
            sendToDisplay(receiverMacAddress, 0x352, nominalEnergyRemaining, nominalFullPackEnergy);
            sendToDisplay(receiverMacAddress, 0x336, maxDischarge, maxRegen);
            sendToDisplay(receiverMacAddress, 0x264, chargeLineCurrent, chargeLineVoltage, chargeLinePower);
            sendToDisplay(receiverMacAddress, 0x3B6, odometer);
            sendToDisplay(receiverMacAddress, 0x2B3, cabin_humidity);
}
