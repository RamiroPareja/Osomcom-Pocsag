#ifndef POCSAGPHY_H
#define POCSAGPHY_H

#include "GenericTypeDefs.h"


#define TMR0_BAUDRATE -INSTRUCTION_FREQUENCY/(POCSAG_BAUDRATE)

typedef enum PocsagPhy_state { IDLE, SYNCH_TX, BATCH_TX, STOP_TX, TEST } PocsagPhy_state;

extern volatile UINT8 pocsagPhy_readyToTX_flag;
extern volatile UINT8 pocsagPhy_radioTX_flag;

extern UINT8 pocsagPhy_busy;


void pocsagPhy_init(void);
void pocsagPhy_interruptInit(void);
void pocsagPhy_interruptStop(void);
void pocsagPhy_ISR(void);

void pocsagPhy_processLoop(void);
void pocsagPhy_sendMsg(UINT16 numBytes);
void pocsagPhy_test(UINT8 testType);
void pocsagPhy_stopTest(void);


#endif 
