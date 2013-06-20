#ifndef __RS232_H
#define __RS232_H

#include "config.h"

#define RS232_BUFFER_SIZE 32
#define BRG_9600BPS ((int) ((_XTAL_FREQ/RS232_BAUD)/16) -1)


extern volatile UINT8 rs232_dataReceived_flag;

void rs232_init(void);
void rs232_putChar(char);
void rs232_putString(const char*);
void rs232_putStringRAM(const char*);

void rs232_interruptInit(void);
void rs232_interruptStop(void);
void rs232_ISR(void);

void rs232_processLoop(void);
void rs232_processBuffer(void);
void rs232_processCommand(void);
void rs232_processCommandSend(void);
void rs232_processCommandConfig(void);
void rs232_processCommandSaveConfig(void);
void rs232_processCommandLoadConfig(void);
void rs232_processCommandDefaultConfig(void);

#endif
