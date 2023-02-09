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

static double frontInverterTemp = 0.0;
static double rearInverterTemp = 0.0;

static double minBattTemp = 0.0;
static double maxBattTemp = 0.0;
static double avgBattTemp = 0.0;

static int cabin_humidity = 0;
static int cabin_temp = 0;

Value maxDischargeClass(0.0, "W", "KW");
Value maxRegenClass(0.0, "W", "KW");

Value frontPowerClass(0.0, "W", "KW");
Value rearPowerClass(0.0, "W", "KW");
static int frontPowerMax = 0;
static int rearPowerMax = 0;

static double UIspeed = 0.0;

Value battVoltsClass(0.0, "V", "KV");
Value battAmpsClass(0.0, "A", "KA");
Value battPowerClass(0.0, "W", "KW");

Value frontPowerLimitClass(0.0, "W", "KW");
Value rearPowerLimitClass(0.0, "W", "KW");


static double socAVE = 0;
static double battTempPct = 0;

Value coolantFlowBatActualClass(0.0, "LPM");
Value coolantFlowPTActualClass(0.0, "LPM");

static int tempCoolantBatInlet = 0;
static int tempCoolantPTInlet = 0;
static int tempCoolantPTInlet_f = 0;
static int tempCoolantBatInlet_f = 0;

static int chargeLineVoltage = 0;
static int chargeLineCurrent = 0;
static int chargeLinePower = 0;

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
