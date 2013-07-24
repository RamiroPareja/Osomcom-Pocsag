/********************************************************************
 FileName:     main.c
 Dependencies: See INCLUDES section
 Processor:		PIC18
 Hardware:		OMSOCOM POCSAG
 Compiler:  	XC8

 ********************************************************************
 File Description:

 ********************************************************************/

/** INCLUDES *******************************************************/

#include "GenericTypeDefs.h"

#include <xc.h>

#include "config.h"
#include "io.h"
#include "adf7012.h"
#include "rs232.h"
#include "interrupts.h"
#include "pocsagPhy.h"
#include "pocsag.h"
#include "massSend.h"



// Configuracion
#pragma config FOSC   = IRC
#pragma config PLLEN  = OFF
#pragma config PCLKEN  = OFF
#pragma config FCMEN  = OFF
#pragma config IESO   = OFF
#pragma config PWRTEN = OFF
#pragma config BOREN  = OFF
#pragma config BORV   = 30
#pragma config WDTEN  = OFF
#pragma config WDTPS  = 32768
#pragma config HFOFST = OFF
#pragma config MCLRE  = ON
#pragma config STVREN = ON
#pragma config LVP    = OFF
#pragma config BBSIZ  = OFF
#pragma config XINST  = OFF
#pragma config CP0    = OFF
#pragma config CP1    = OFF
#pragma config CPB    = OFF
#pragma config CPD    = OFF
#pragma config WRT0   = OFF
#pragma config WRT1   = OFF
#pragma config WRTB   = OFF
#pragma config WRTC   = OFF
#pragma config WRTD   = OFF
#pragma config EBTR0  = OFF
#pragma config EBTR1  = OFF
#pragma config EBTRB  = OFF     



/** P R I V A T E  P R O T O T Y P E S ***************************************/
void initSystem(void);


/** DECLARATIONS ***************************************************/

/******************************************************************************
 * Function:        void main(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Main program entry point.
 * Note:            None
 *****************************************************************************/
void main(void) {
 
    initSystem();
     
    rs232_putString("OsomCom POCSAG 0.3\r\nramiropareja@t4f.org 7/2013\r\n");

    // Process Loop
    while (1) {

        io_processLoop();
        pocsagPhy_processLoop();
        rs232_processLoop();

#ifndef NO_MASS_SEND
        massSend_processLoop();
#endif
      
    }

}


void initSystem(void) {

    OSCCON = 0x70;      //16 MHz Clock

    __delay_us(1);

    initPorts();
    rs232_init();
    config_init();
    interrupts_init();

#ifndef NO_MASS_SEND
    massSend_init();
#endif

}
