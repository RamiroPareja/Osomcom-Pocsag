#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

void interrupts_init(void);
void interrupts_stop(void);

void interrupt interrupts_hISR(void);
void interrupt low_priority interrupts_lISR(void);



#endif	

