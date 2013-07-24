#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "rs232.h"
#include "ui.h"
#include "pocsag.h"
#include "pocsagPhy.h"
#include "GenericTypeDefs.h"
#include "config.h"


volatile UINT8 rs232_dataReceived_flag;
volatile char rs232_rxByte;


char rs232Buffer[RS232_BUFFER_SIZE];
char rs232BufferIndex;


void rs232_init(void) {

    rs232_dataReceived_flag = 0;
    rs232BufferIndex = 0;

    // Configurando generador de baudios
    BAUDCONbits.BRG16 = 1;
    TXSTAbits.BRGH = 0;
    SPBRG = BRG_9600BPS;


    // Activando modulo EUSART
    TXSTAbits.SYNC = 0; // Asincrono
    RCSTAbits.SPEN = 1; // Activamos modulo EUSART

    //Configurando transmision
    TXSTAbits.TX9D = 0; // 8 bits
    BAUDCONbits.CKTXP = 0; // Polaridad normal en transmision
    PIE1bits.TXIE = 0; // Sin interrupcion por transmision
    TXSTAbits.TXEN = 1; // Activamos modulo de transmision

    // Configurando recepcion
    RCSTAbits.RX9 = 0; // 8 bits
    BAUDCONbits.DTRXP = 0; // Polaridad normal en recepcion
    RCSTAbits.CREN = 1; // Activamos modulo de recepcion


}

void rs232_putch(char c) {
    while (!TXSTAbits.TRMT) // TRMT1 is set when TSR is empty
        continue;
    TXREG = c; // load the register
}

void rs232_putString(const char *str) {
    while ((*str) != 0)
        rs232_putch(*str++);
}

// ESTO ES UN FALLO DEL COMPILADOR!
// El maldito compilador (XC8 1.12) no funciona correctamente.
// Esta funcion es usada pasando punteros al espacio de datos y al espacio de programa
// En teoria y segun el manual del compilador, se deberia haber usado un puntero mixto
// de 24 bits (o incluso 32). Sin embargo, usa uno de 8.
// Usamos dos funciones identicas usando la primera para FLASH y la segunda para RAM
// de modo que no use punteros mixtos.
void rs232_putStringRAM(const char *str) {
        while ((*str) != 0)
        rs232_putch(*str++);
}


void rs232_interruptInit(void) {

    rs232_dataReceived_flag = 0;

    RCIP = 0; // Baja prioridad
    //RCIF=0;   // RCIF es solo lectura, no escritura.
    RCIE = 1;

}

void rs232_interruptStop(void) {

    RCIE = 0;

}

void rs232_ISR(void) {

    if (!(RCIE && RCIF)) {
        return;
    }

    //RCIF=0;   // RCIF es solo lectura, no escritura.

    // Comprobamos que no se haya producido un error en la recepcion
/*    if (RCSTAbits.FERR RCSTAbits.OERR) {
        RCSTA2 &= 0xEF;
        RCSTA2 |= 0x10;
    }
*/ // PASO! En otro momento lo escribire.
    rs232_rxByte = RCREG;
    rs232_dataReceived_flag = 1;

}

void rs232_processLoop(void) {

    char *comando;
    char *parametro1;
    char *parametro2;
    char *parametro3;


    if (rs232_dataReceived_flag == 1) {        
        // Recibido un byte        

        rs232_dataReceived_flag = 0;

        rs232Buffer[rs232BufferIndex] = rs232_rxByte;
        if (rs232BufferIndex < RS232_BUFFER_SIZE-2)        // Evitamos un buffer overflow.
                                                           //Ponemos -2 para dejar los 3 ultimos bytes a 0. Ojo! el |Necesario para pocsag_formatAlphaMessageCodeWord()
                rs232BufferIndex++;
        

        if (rs232_rxByte == '\n') {

            rs232Buffer[rs232BufferIndex] = '\0';   // Hay que poner el 0 en el ultimo caracter para que STRTOK funcione
            ui_processBuffer(rs232Buffer, &comando, &parametro1, &parametro2, &parametro3);
            ui_processCommand(comando, parametro1, parametro2, parametro3);

            rs232BufferIndex = 0;                   // "borramos" el buffer            
        }        
    }
}
