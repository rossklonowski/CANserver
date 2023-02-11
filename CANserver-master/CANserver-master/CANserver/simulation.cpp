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

            if (odometer == 0) {
                odometer = 32186.88;
            }
            odometer = odometer + 0.01;

            socAVE = socAVE + 0.1;
            if (socAVE > 80.0) {
                socAVE = 0.0;
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

            // battAmps = 171.428;
            battAmps = battAmps + 1;
            if (battAmps > 700){
                battAmps = -400;
            }

            // battPower = battAmps * battVolts;
            battPower = battPower + 100;
            if (battPower > 50000) {
                battPower = -1000;
            }

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

            maxRegen = 102.00;
            // maxRegen = maxRegen + 1;
            // if (maxRegen == 99) {
            //     maxRegen = 0;
            // }

            maxDischarge = 252.00;
            // if (maxDischarge == 99) {
            //     maxDischarge = 0;
            // }

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

            coolantFlowBatActual = coolantFlowBatActual + 0.1;
            if (coolantFlowBatActual > 6.0) {
                coolantFlowBatActual = 0.0;
            }

            coolantFlowPTActual = coolantFlowPTActual + 0.2;
            if (coolantFlowPTActual > 6.0) {
                coolantFlowPTActual = 0.0;
            }


            tempCoolandBatInlet = tempCoolandBatInlet + 0.1;
            if (tempCoolandBatInlet > 80.0) {
                tempCoolandBatInlet = 0.0;
            }

            tempCoolandBatPTlet = tempCoolandBatPTlet + 0.2;
            if (tempCoolandBatPTlet > 80.0) {
                tempCoolandBatPTlet = 0.0;
            }

    
            sendToDisplay(receiverMacAddress, 0x132, battVolts, battAmps, battPower);
            sendToDisplay(receiverMacAddress, 0x2E5, frontPower, frontPowerLimit);
            sendToDisplay(receiverMacAddress, 0x266, rearPower, rearPowerLimit);
            sendToDisplay(receiverMacAddress, 0x292, socAVE, battTempPct);
            sendToDisplay(receiverMacAddress, 0x312, minBattTemp, maxBattTemp);
            
            sendToDisplay(receiverMacAddress, 0x383, cabin_temp);
            sendToDisplay(receiverMacAddress, 0x315, rearInverterTemp);
            sendToDisplay(receiverMacAddress, 0x376, frontInverterTemp);
            sendToDisplay(receiverMacAddress, 0x352, nominalEnergyRemaining, nominalFullPackEnergy);
            sendToDisplay(receiverMacAddress, 0x252, maxDischarge, maxRegen);
            
            sendToDisplay(receiverMacAddress, 0x264, chargeLineCurrent, chargeLineVoltage, chargeLinePower);
            sendToDisplay(receiverMacAddress, 0x3B6, odometer);
            sendToDisplay(receiverMacAddress, 0x2B3, cabin_humidity);
            sendToDisplay(receiverMacAddress, 0x241, coolantFlowBatActual, coolantFlowPTActual);
            sendToDisplay(receiverMacAddress, 0x321, tempCoolandBatInlet, tempCoolandBatPTlet);
}
