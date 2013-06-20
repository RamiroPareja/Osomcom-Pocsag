#ifndef _ADF7012_H_
#define _ADF7012_H_

#define LIMIT_FREQ_DIVIDER_8 130000000
#define LIMIT_FREQ_DIVIDER_4 270000000
#define LIMIT_FREQ_DIVIDER_2 550000000

#define MODULATION_FSK      0
#define MODULATION_GFSK     1
#define MODULATION_ASK      2
#define MODULATION_OOK      3
#define MODULATION_GOOK     4

#define POWER_MAX          63
#define POWER_13dBm        63
#define POWER_10dBm        58
#define POWER_6dBm         50
#define POWER_0dBm         39
#define POWER_MIN           0

#define MUXOUT_LOGIC_LOW     0
#define MUXOUT_LOGIC_HIGH    1
#define MUXOUT_LOGIC_REGULATOR_READY   3
#define MUXOUT_LOGIC_DIGITAL_LOCKOUT   4
#define MUXOUT_LOGIC_ANALOG_LOCKOUT    5
#define MUXOUT_R_DIVIDIR               6
#define MUXOUT_N_DIVIDIR               7
#define MUXOUT_RF_DIVIDIR              8
#define MUXOUT_DATA_RATE               9
#define MUXOUT_BATTERY_235            10
#define MUXOUT_BATTERY_275            11
#define MUXOUT_BATTERY_300            12
#define MUXOUT_BATTERY_325            13





typedef struct {
    BYTE byte0;
    BYTE byte1;
    BYTE byte2;
    BYTE byte3;
} RegisterBytes_t;

typedef struct {
    unsigned address : 2; // Address (00)
    unsigned freqErrorCorrection_L : 6;

    unsigned freqErrorCorrection_H : 5;
    unsigned rDivider_L : 3;

    unsigned rDivider_H : 1;
    unsigned xtalDivider : 1;
    unsigned xoeb : 1;
    unsigned clkOutDivider : 4;
    unsigned vcoAdjust_L : 1;

    unsigned vcoAdjust_H : 1;
    unsigned outputDivider : 2;
    unsigned : 5;

} Adf7012Register0_t;

typedef struct {
    unsigned address : 2; // Address (01)
    unsigned fractionalN_L : 6;

    unsigned fractionalN_H : 6;
    unsigned integerN_L : 2;

    unsigned integerN_H : 6;
    unsigned prescaler : 1;
    unsigned : 1;

} Adf7012Register1_t;

typedef struct {
    unsigned address : 2; // Address (10)
    unsigned modControl : 2;
    unsigned gook : 1;
    unsigned powerAmplifier_L : 3;

    unsigned powerAmplifier_H : 3;
    unsigned modDeviation_L : 5;

    unsigned modDeviation_H : 4;
    unsigned gfskModControl : 3;
    unsigned indexCounter_L : 1;

    unsigned indexCounter_H : 1;
    unsigned testBits : 3; //Should be 0
    unsigned : 4;

} Adf7012Register2_t;

typedef struct {
    unsigned address : 2; // Address (11)
    unsigned pllEnable : 1;
    unsigned paEnable : 1;
    unsigned clkOutEnable : 1;
    unsigned dataInvert : 1;
    unsigned chargePump : 2;

    unsigned bleedCurrent : 2;
    unsigned vcoDisable : 1;
    unsigned muxout : 4;
    unsigned ldPrecision : 1;

    unsigned vcoBias : 4;
    unsigned paBias : 3;
    unsigned pllTestModes_L : 1;

    unsigned pllTestModes_H : 4;
    unsigned sdModes : 4;

} Adf7012Register3_t;

typedef struct {

    union {
        UINT32 register0;
        RegisterBytes_t register0Bytes;
        Adf7012Register0_t register0bits;
    };

    union {
        UINT32 register1;
        RegisterBytes_t register1Bytes;
        Adf7012Register1_t register1bits;
    };

    union {
        UINT32 register2;
        RegisterBytes_t register2Bytes;
        Adf7012Register2_t register2bits;
    };

    union {
        UINT32 register3;
        RegisterBytes_t register3Bytes;
        Adf7012Register3_t register3bits;
    };


} Adf7012Config_t;


#define adf7012_powerOn() CE=1;
#define adf7012_powerOff() CE=0;

void adf7012_init(void);
void adf7012_initRegisters(void);
void adf7012_configure(BYTE, BYTE, BYTE, BYTE);
void adf7012_configure24(BYTE, BYTE, BYTE);
void adf7012_configureRegisters(void);
void adf7012_setModulation(BYTE);
void adf7012_setFrequency(UINT32);
void adf7012_setFSKDeviation(UINT32);
void adf7012_setPower(BYTE);
void adf7012_setMuxout(BYTE);


#endif