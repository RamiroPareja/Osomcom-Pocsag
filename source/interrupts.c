#include <xc.h>

#include "GenericTypeDefs.h"
#include "interrupts.h"
#include "pocsagPhy.h"
#include "rs232.h"



void interrupts_init(void) {

    IPEN=1;     // Enable priority levels on interrupts

    IPR1=0;     // Todas las interrupciones a prioridad baja
    IPR2=0;

    pocsagPhy_interruptInit();
    rs232_interruptInit();
    
    GIEL=1;     // Activamos interrupciones altas y bajas
    GIEH=1;

}

void interrupt interrupts_hISR(void) {

    pocsagPhy_ISR();

}

void interrupt low_priority interrupts_lISR(void) {

    rs232_ISR();

}


void interrupts_stop(void) {

    pocsagPhy_interruptStop();
    rs232_interruptStop();

}