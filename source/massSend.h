#ifndef MASSSEND_H
#define	MASSSEND_H

extern UINT8 massSend_scanning;

void massSend_processLoop(void);
void massSend_init(void);
void massSend_scan(UINT32 startRIC, UINT32 stopRIC);
void massSend_stop(void);
BOOL massSend_resume(void);

#endif	