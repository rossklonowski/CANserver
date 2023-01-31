//
//  carDataVariables.h
//
//  Created by Ross Klonowski on January 16 2021.
//

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

static int maxRegen = 0;
static int maxDischarge = 0;

static int frontPower = 0;
static int rearPower = 0;
static int frontPowerMax = 0;
static int rearPowerMax = 0;

static double UIspeed = 0.0;

static int battVolts = 0;
static int battAmps = 0;
static int battPower = 0;

static int frontPowerLimit = 0;
static int rearPowerLimit = 0;

static double socAVE = 0;
static double battTempPct = 0;

static int tempCoolandBatInlet = 0;
static int tempCoolandBatPTlet = 0;

static int chargeLineVoltage = 0;
static int chargeLineCurrent = 0;
static int chargeLinePower = 0;

static double odometer = 0;
static double tripOdometer = 0;

// custom stuff
static double energyCounter = 0.0;
static int masterUpTime = 0;
static float max_g_vector = 0;
static float max_accel_vector = 0;
static float accel_offset = 0.0;
static float accel_vector = 0.0;
static float sampled_energy_counter = 0.0;
