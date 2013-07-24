#include <xc.h>
#include "config.h"
#include "adf7012.h"


#include "config.h"
#include "pocsagPhy.h"
#include "pocsag.h"
#include "GenericTypeDefs.h"
#include "io.h"
#include "tools.h"
#include "rs232.h"

volatile UINT8 pocsagPhy_readyToTX_flag;
volatile UINT8 pocsagPhy_radioTX_flag;

UINT8 pocsagPhy_busy;       // Indica si esta ocupado el modulo

PocsagPhy_state pocsagPhy_state;

UINT16 contador;
UINT8 contadorBits;
UINT16 numBytesTX;
UINT8 testType;
BYTE *ptrBuffer;


void pocsagPhy_SM_synchTX(void);
void pocsagPhy_SM_BatchTX(void);
void pocsagPhy_SM_StopTX(void);
void pocsagPhy_SM_test(void);

void pocsagPhy_interruptInit(void) {

    pocsagPhy_readyToTX_flag = 0;

    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 1;
    T0CONbits.T0PS = 0;


    TMR0 = config.tmr0Bauds;

    TMR0IP = 1; // Asignamos a la interrupcion TMR0 prioridad ALTA
    TMR0IF = 0;
    TMR0IE = 1;


}

void pocsagPhy_interruptStop(void) {

    TMR0IE = 0;

}

void pocsagPhy_ISR(void) {

    if (!(TMR0IE && TMR0IF)) {
        return;
    }

    TMR0 = config.tmr0Bauds;

    TMR0IF = 0;

    pocsagPhy_readyToTX_flag = 1;

#ifndef POCSAG_INVERTED
    RADIO_DATA = !pocsagPhy_radioTX_flag;
    GP0 = !pocsagPhy_radioTX_flag;
#else
    RADIO_DATA = pocsagPhy_radioTX_flag;
    GP0 = pocsagPhy_radioTX_flag;
#endif



}

BOOL pocsagPhy_init(void) {

    pocsagPhy_readyToTX_flag = 0;
    pocsagPhy_radioTX_flag= 0;
    RADIO_DATA = 0;
    pocsagPhy_state = IDLE;

    pocsagPhy_busy = 0;

    return adf7012_init();

}

void pocsagPhy_sendMsg(UINT16 numBytes) {

    if (pocsagPhy_state != IDLE)
        return;

    if (pocsagPhy_init()) {
        rs232_putString("Init error");
        return;
    }

    pocsagPhy_busy = 1;
    numBytesTX = numBytes;

    pocsagPhy_state = SYNCH_TX;
    contador = PREAMBLE_LENGTH;

}

#ifndef DEBUG_MODE

void pocsagPhy_test(UINT8 tstType) {
    if (pocsagPhy_state != IDLE)
        return;

    if (pocsagPhy_init()) {
        rs232_putString("Init error");
        return;
    }
    pocsagPhy_busy = 1;

    pocsagPhy_state = TEST;
    testType = tstType;
}

void pocsagPhy_stopTest() {

    RADIO_DATA = 0;
    adf7012_powerOff();
    pocsagPhy_busy = 0;
    pocsagPhy_state = IDLE;
}

#endif

void pocsagPhy_processLoop() {

    switch (pocsagPhy_state) {
        case IDLE:
            break;

        case SYNCH_TX:
            pocsagPhy_SM_synchTX();
            break;

        case BATCH_TX:
            pocsagPhy_SM_BatchTX();
            break;

        case STOP_TX:
            pocsagPhy_SM_StopTX();
            break;

#ifndef DEBUG_MODE
        case TEST:
            pocsagPhy_SM_test();
            break;
#endif
    }

}

void pocsagPhy_SM_synchTX(void) {

    if (!pocsagPhy_readyToTX_flag)
        return;

    pocsagPhy_readyToTX_flag = 0;
    pocsagPhy_radioTX_flag = !pocsagPhy_radioTX_flag;



    contador--;
    if (contador == 0) {
        pocsagPhy_state = BATCH_TX;
        contador = 0;
        contadorBits = 0;
        ptrBuffer = batchBuffer;

    }

}

void pocsagPhy_SM_BatchTX(void) {


    if (!pocsagPhy_readyToTX_flag)
        return;

    pocsagPhy_readyToTX_flag = 0;


    if (*ptrBuffer & 0x80) {
        pocsagPhy_radioTX_flag = 1;
    }
    else {
        pocsagPhy_radioTX_flag = 0;
    }

    *ptrBuffer <<= 1;
    contadorBits++;

    if (contadorBits >= 8) {
        contadorBits = 0;
        numBytesTX--;

        if (numBytesTX == 0) {

            pocsagPhy_state = STOP_TX;
            contadorBits = 0;
            return;

        }

        ptrBuffer++;
    }

}

void pocsagPhy_SM_StopTX(void) {

    if (!pocsagPhy_readyToTX_flag)
        return;

    pocsagPhy_readyToTX_flag = 0;

    contadorBits++;

    if (contadorBits<=1)
        return;

    pocsagPhy_radioTX_flag= 0;
    RADIO_DATA = 0;

    adf7012_powerOff();

    pocsagPhy_busy = 0;
    pocsagPhy_state = IDLE;

}



// Funcion que envia una trama infinita de unos y ceros.
// Util para analizar con el RTL-SDR o un espectrografo si la desviacion de la
// modulacion FSK es la correcta
#ifndef DEBUG_MODE
void pocsagPhy_SM_test(void) {


    if (!pocsagPhy_readyToTX_flag)
        return;

    pocsagPhy_readyToTX_flag = 0;

    if (testType == 0)
        pocsagPhy_radioTX_flag = 0;
    else if (testType == 1)
        pocsagPhy_radioTX_flag = 1;
    else
        pocsagPhy_radioTX_flag = !pocsagPhy_radioTX_flag;  

}
#endif
