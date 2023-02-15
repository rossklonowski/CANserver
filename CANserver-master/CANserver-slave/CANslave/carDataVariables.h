//
//  carDataVariables.h
//
//  Created by Ross Klonowski on January 16 2021.
//

#include "Value.h"

Value nominalEnergyRemaining("KWh");
Value nominalFullPackEnergy("KWh");
Value energyBuffer("KWh");
Value batteryHeatTarget("F");
Value batteryCoolTarget("F");

Value minBattTemp("F");
Value maxBattTemp("F");
Value avgBattTemp("F");
Value maxDischargeClass("W", "KW");
Value maxRegenClass("W", "KW");
Value frontPowerClass("W", "KW");
Value rearPowerClass("W", "KW");
Value UIspeed("mph");
Value battVoltsClass("V", "KV");
Value battAmpsClass("A", "KA");
Value battPowerClass("W", "KW");
Value frontPowerLimitClass("W", "KW");
Value rearPowerLimitClass("W", "KW");
Value frontInverterTempClass("F");
Value rearInverterTempClass("F");
Value socAVE("%");
Value battTempPct("%");
Value coolantFlowBatActualClass("LPM");
Value coolantFlowPTActualClass("LPM");
Value tempCoolantBatInletClass("F");
Value tempCoolantPTInletClass("F");
Value chargeLineVoltageClass("V");
Value chargeLineCurrentClass("A");
Value chargeLinePowerClass("W", "KW");
Value odometer("mi");
Value startOfTripOdometer("mi");

// custom stuff
static double energyCounter = 0.0;
static int masterUpTime = 0;
static float max_g_vector = 0;
static float max_accel_vector = 0;
static float accel_offset = 0.0;
static float accel_vector = 0.0;
static float sampled_energy_counter = 0.0;
static double lastNominalEnergyRemaining = 0.0;
static double lastNominalFullPackEnergy = 0.0;
Value powerNotFromMotors("W", "KW");
Value tripDistance("mi");
Value sampledEnergyCounter("KWh");
Value efficiency("wh/mi");
Value chargerEfficiency("%");
