#ifndef __CONFIG_H_
#define __CONFIG_H_

    #define _XTAL_FREQ    16000000
    #define INSTRUCTION_FREQUENCY _XTAL_FREQ/4

    #define _RF_XTAL_FREQ  4000000          // Frecuencia del reloj del ADF


    #define POCSAG_BAUDRATE 2400
//#define POCSAG_FREQUENCY 148420000        BUSCA PHILLIPS MENSATEL
    //#define POCSAG_FREQUENCY   143520000        //BUSCA NOP

    // Dejar sin comentar la salida que prefieras
    //#define POCSAG_INVERTED

    #define RS232_BAUD 9600

    #define CONFIG_BYTE 0x5A
    #define DEFAULT_CONFIG_FREQUENCY 148425000
    //#define DEFAULT_CONFIG_FREQUENCY 143520000  //BUSCA NOP
    #define DEFAULT_CONFIG_DEVIATION 4500
    #define DEFAULT_CONFIG_POWER 50
    #define DEFAULT_CONFIG_BAUDS 2400

    //#define DEFAULT_CONFIG_MASS_SEND_START_RIC 10000000
    #define DEFAULT_CONFIG_MASS_SEND_CURRENT_RIC 0
    #define DEFAULT_CONFIG_MASS_SEND_STOP_RIC 0
    #define DEFAULT_CONFIG_MASS_SEND_SAVE_FREQ 200
    #define DEFAULT_CONFIG_MASS_SEND_DELAY 2000


    extern UINT32 config_frequency;
    extern UINT16 config_deviation;
    extern UINT8 config_power;
    extern UINT16 config_bauds;
    extern CHAR config_inicializado;

    //extern UINT32 config_massSendStartRic;
    extern UINT32 config_massSendStopRic;
    extern UINT32 config_massSendCurrentRic;
    extern UINT16 config_massSendSaveFreq;
    extern UINT16 config_massSendDelay;

    void config_setDefault(void);
    void config_load(void);
    void config_save(void);
    void config_saveMassSendStatus(void);
    void config_init(void);

#endif