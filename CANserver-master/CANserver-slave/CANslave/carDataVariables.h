//
//  carDataVariables.h
//
//  Created by Ross Klonowski on January 16 2021.
//

#include "Value.h"

static double nominalEnergyRemaining = 0.0;
static double lastNominalEnergyRemaining = 0.0;

static double nominalFullPackEnergy = 0.0;
static double lastNominalFullPackEnergy = 0.0;


Value minBattTemp("F");
Value maxBattTemp("F");
Value avgBattTemp("F");

static int cabin_humidity = 0;
static int cabin_temp = 0;

Value maxDischargeClass("W", "KW");
Value maxRegenClass("W", "KW");

Value frontPowerClass("W", "KW");
Value rearPowerClass("W", "KW");
static int frontPowerMax = 0;
static int rearPowerMax = 0;

static double UIspeed = 0.0;

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

static double odometer = 0.0;
static double startOfTripOdometer = 0;

// custom stuff
static double energyCounter = 0.0;
static int masterUpTime = 0;
static float max_g_vector = 0;
static float max_accel_vector = 0;
static float accel_offset = 0.0;
static float accel_vector = 0.0;
static float sampled_energy_counter = 0.0;
Value powerNotFromMotors("W", "KW");
