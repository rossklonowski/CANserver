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
                socAVE = 12;
            }

            battTempPct = battTempPct + 1;
            if (battTempPct > 80) {
                battTempPct = 12;
            }

            masterUpTime = masterUpTime + 1;

            nominalFullPackEnergy = nominalFullPackEnergy + 1;
            if (nominalFullPackEnergy > 80) {
                nominalFullPackEnergy = 12;
            }

            frontInverterTemp = frontInverterTemp + 0.1;
            if (frontInverterTemp > 80) {
                frontInverterTemp = 12;
            }

            rearInverterTemp = rearInverterTemp + 0.1;
            if (rearInverterTemp > 80) {
                rearInverterTemp = 12;
            }

            cabin_humidity = cabin_humidity + 1;
            if (cabin_humidity > 80) {
                cabin_humidity = 12;
            }

            cabin_temp = cabin_temp + 1;
            if (cabin_temp > 80) {
                cabin_temp = 12;
            }
            if (cabin_temp == 0) {
                cabin_temp == 1;
            }

            maxBattTemp = maxBattTemp + 0.1;
            if (maxBattTemp > 80) {
                maxBattTemp = 12;
            }

            minBattTemp = minBattTemp + 0.1;
            if (minBattTemp > 80) {
                minBattTemp = 12;
            }

            nominalEnergyRemaining = nominalEnergyRemaining + 0.1;
            if (nominalEnergyRemaining > 50) {
                nominalEnergyRemaining = 4;
            }

            battVolts = battVolts + 1;
            if (battVolts == 400) {
                battVolts = 300;
            }

            battAmps = battAmps + 1;
            if (battAmps == 150) {
                battAmps = -99;
            }

            battPower = battPower + 1;
            if (battPower == 300) {
                battPower = -50;
            }

            chargeLineCurrent = chargeLineCurrent + 1;
            if (chargeLineCurrent == 99) {
                chargeLineCurrent = 50;
            }
            
            chargeLinePower = chargeLinePower + 1;
            if (chargeLinePower == 99) {
                chargeLinePower = 50;
            }
            
            chargeLineVoltage = chargeLineVoltage + 1;
            if (chargeLineVoltage == 99) {
                chargeLineVoltage = 50;
            }

            maxRegen = maxRegen + 1;
            if (maxRegen == 99) {
                maxRegen = 50;
            }

            maxDischarge = maxDischarge + 1;
            if (maxDischarge == 99) {
                maxDischarge = 50;
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


            // sendToDisplay(receiverMacAddress, 0x3E7, masterUpTime);
}
