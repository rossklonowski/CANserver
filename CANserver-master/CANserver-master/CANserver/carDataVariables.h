//
//  carDataVariables.h
//
//  Created by Ross Klonowski on January 16 2021.
//

static double nominalEnergyRemaining = 0;
static int expectedEnergyRemaining = 0;
static double nominalFullPackEnergy = 0;

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
static int frontPowerLimit = 0;
static int rearPowerLimit = 0;

static int battVolts = 0;
static int battAmps = 0;
static int battPower = 0;
static int battPowerW = 0;

static int frontTorque = 0;
static int rearTorque = 0;

static double socAVE = 0;
static double battTempPct = 0;

static int tempCoolandBatInlet = 0;
static int tempCoolandBatPTlet = 0;

static int chargeLineVoltage = 0;
static int chargeLineCurrent = 0;
static int chargeLinePower = 0;

static int displayOn = 0;

static double odometer = 0;

// custom signals
static double UIspeed = 0.0;
static double temp_UIspeed = 0.0;
static double testZeroToSixty = 0.0;
static int masterUpTime = 0;
