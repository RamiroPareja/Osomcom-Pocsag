#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "pocsagPhy.h"


    #define _XTAL_FREQ    16000000
    #define INSTRUCTION_FREQUENCY _XTAL_FREQ/4

    #define _RF_XTAL_FREQ  4000000          // Frecuencia del reloj del ADF


    
    // Dejar sin comentar la salida que prefieras
    //#define POCSAG_INVERTED

    #define RS232_BAUD 9600

    #define DEFAULT_CONFIG_FREQUENCY 148420000
    #define DEFAULT_CONFIG_DEVIATION 4500
    #define DEFAULT_CONFIG_POWER 20
    #define DEFAULT_CONFIG_BAUDS 2400
    #define DEFAULT_CONFIG_TMR0_BAUDS TMR0_BAUDRATE_2400

    
    #define DEFAULT_CONFIG_MASS_SEND_CURRENT_RIC 0
    #define DEFAULT_CONFIG_MASS_SEND_STOP_RIC 0
    #define DEFAULT_CONFIG_MASS_SEND_SAVE_FREQ 200
    #define DEFAULT_CONFIG_MASS_SEND_DELAY 50

    // Este define indica si hay que desactivar ciertas partes del codigo para
    // dejar espacio y asi poder depurar
    //#define DEBUG_MODE

    // Este defina indica si compilamos el soporte para el mass sending
    //#define NO_MASS_SEND

    // Este define indica si tenemos que sacar por el RS232 los bits a transmitir
    // NO USAR EN PRODUCCION! Mete un delay horrible que hara que no funcione
    #define DEBUG_RS232

    typedef struct {
        UINT32 frequency;
        UINT16 deviation;
        UINT16 bauds;
        UINT16 tmr0Bauds;
        UINT8 power;
        //BOOL inverted;

#ifndef NO_MASS_SEND
        UINT32 massSendCurrentRic;
        UINT32 massSendStopRic;
        UINT16 massSendSaveFreq;
        UINT16 massSendDelay;
#endif
    } Config_t;

    extern Config_t config;

    void config_load(void);
    void config_save(void);
    void config_saveMassSendStatus(void);
    void config_init(void);


#endif