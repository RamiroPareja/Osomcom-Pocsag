#ifndef POCSAGPHY_H
#define POCSAGPHY_H

#include "GenericTypeDefs.h"

#define TMR0_INT_DELAY_CTE  50      //Esta constante compensa el retardo de la ISR.
                                    // WARNING! Si se modifica la ISR o se cambia el compilador u optimizaciones, hay que recalcularla
#define TMR0_BAUDRATE_2400 -INSTRUCTION_FREQUENCY/(2400) + TMR0_INT_DELAY_CTE
#define TMR0_BAUDRATE_1200 -INSTRUCTION_FREQUENCY/(1200) + TMR0_INT_DELAY_CTE
#define TMR0_BAUDRATE_512 -INSTRUCTION_FREQUENCY/(512) + TMR0_INT_DELAY_CTE

typedef enum PocsagPhy_state { IDLE, SYNCH_TX, BATCH_TX, STOP_TX, TEST } PocsagPhy_state;

extern volatile UINT8 pocsagPhy_readyToTX_flag;
extern volatile UINT8 pocsagPhy_radioTX_flag;

extern UINT8 pocsagPhy_busy;

BOOL pocsagPhy_init(void);
void pocsagPhy_interruptInit(void);
void pocsagPhy_interruptStop(void);
void pocsagPhy_ISR(void);

void pocsagPhy_processLoop(void);
void pocsagPhy_sendMsg(UINT16 numBytes);
void pocsagPhy_test(UINT8 testType);
void pocsagPhy_stopTest(void);


#endif 
