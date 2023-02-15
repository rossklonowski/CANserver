//
//  carDataVariables.h
//
//  Created by Ross Klonowski on January 16 2021.
//

static double nominalEnergyRemaining = 0.0;
static int expectedEnergyRemaining = 0.0;
static double nominalFullPackEnergy = 0.0;
static double energyBuffer = 0.0;

static double batteryCoolTarget = 0.0;
static double batteryHeatTarget= 0.0;

static double frontInverterTemp = 0.0;
static double rearInverterTemp = 0.0;

static double minBattTemp = 0.0;
static double maxBattTemp = 0.0;
static double avgBattTemp = 0.0;

static int cabin_humidity = 0.0;
static int cabin_temp = 0.0;

static double maxRegen = 0.0;
static double maxDischarge = 0.0;

static double frontPower = 0.0;
static double rearPower = 0.0;
static double frontPowerLimit = 0;
static double rearPowerLimit = 0;

static double battVolts = 0.0;
static double battAmps = 0.0;
static double battPower = 0.0;

static int frontTorque = 0;
static int rearTorque = 0;

static double socAVE = 0.0;
static double battTempPct = 0.0;

static double coolantFlowBatActual = 0.0;
static double coolantFlowPTActual = 0.0;

static double tempCoolandBatInlet = 0;
static double tempCoolandBatPTlet = 0;

static double chargeLineVoltage = 0;
static double chargeLineCurrent = 0;
static double chargeLinePower = 0;

static int displayOn = 0;

static float odometer = 0.0;

// custom signals
static double UIspeed = 0.0;
static double temp_UIspeed = 0.0;
static double testZeroToSixty = 0.0;
static int masterUpTime = 0;
