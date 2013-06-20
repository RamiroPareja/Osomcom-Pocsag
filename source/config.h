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
    #define DEFAULT_CONFIG_FREQUENCY 148420000
    #define DEFAULT_CONFIG_DEVIATION 4500
    #define DEFAULT_CONFIG_POWER 55
    #define DEFAULT_CONFIG_BAUDS 2400

    extern UINT32 config_frequency;
    extern UINT16 config_deviation;
    extern UINT8 config_power;
    extern UINT16 config_bauds;
    extern CHAR config_inicializado;

    void config_setDefault(void);
    void config_load(void);
    void config_save(void);
    void config_init(void);

#define RIC 1519000
#define RIC_end 1520000


#endif