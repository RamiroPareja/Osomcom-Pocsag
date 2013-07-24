#ifndef __RS232_H
#define __RS232_H

#include "config.h"


#ifdef NO_MASS_SEND
    #define RS232_BUFFER_SIZE 60
#else
    #define RS232_BUFFER_SIZE 52
#endif

//CASO MEJOR: NOMASSMODE && !DEBUG_RS232 -> 60   (libre 80)
//CASO PEOR:  !NOMASSMODE && DEBUG_RS232 -> 52   (libre 80)

//!DEBUG_RS232 +4
//NO_MASS_SEND +18

#define BRG_9600BPS ((int) ((_XTAL_FREQ/RS232_BAUD)/16) -1)


extern volatile UINT8 rs232_dataReceived_flag;

void rs232_init(void);
void rs232_putch(char);
void rs232_putString(const char*);
void rs232_putStringRAM(const char*);

void rs232_interruptInit(void);
void rs232_interruptStop(void);
void rs232_ISR(void);

void rs232_processLoop(void);


#endif
