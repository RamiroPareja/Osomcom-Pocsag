#include <xc.h>
#include "massSend.h"
#include "config.h"
#include "pocsagPhy.h"
#include "pocsag.h"



BOOL massSend_scanning;    // Indica si estamos escaneando

#ifndef NO_MASS_SEND
UINT16 massSend_batchCounter;     // Guarda cuantos batches se han transmitido desde el ultimo backup

void massSend_processLoop(void) {

    static UINT16 delayCounter = 0;

    char tmp[] = {'A',0};


    if (massSend_scanning && !pocsagPhy_busy) {

        delayCounter++;

        if (delayCounter<config.massSendDelay) {
            __delay_us(100);
            return;
        }

        delayCounter = 0;
        
        pocsagPhy_sendMsg(pocsag_createMsg(config.massSendCurrentRic, tmp, FUNCTION_ALPHA, TRUE));

        config.massSendCurrentRic += 8;
        massSend_batchCounter++;

        if (massSend_batchCounter >= config.massSendSaveFreq) {
            massSend_batchCounter = 0;
            config_saveMassSendStatus();
        }

        if (config.massSendCurrentRic > config.massSendStopRic) {
            massSend_stop();
        }

    }

}

void massSend_init(void) {

    massSend_scanning = FALSE;

}

void massSend_scan(UINT32 startRIC, UINT32 stopRIC) {

    massSend_scanning = TRUE;
    massSend_batchCounter = 0;

    config.massSendCurrentRic = startRIC & 0xFFFFFFF8; //Eliminamos los 3 LSB
    config.massSendStopRic = stopRIC;
    config_saveMassSendStatus();

}

void massSend_stop(void) {

    massSend_scanning = FALSE;

    config_saveMassSendStatus();

}

BOOL massSend_resume(void) {

    if (config.massSendCurrentRic <= config.massSendStopRic) {
        massSend_scanning = TRUE;
        massSend_batchCounter = 0;

        return TRUE;
    } else
        return FALSE;

}

#endif