#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "rs232.h"

#include "pocsag.h"
#include "pocsagPhy.h"
#include "GenericTypeDefs.h"
#include "config.h"


volatile UINT8 rs232_dataReceived_flag;
volatile char rs232_rxByte;


char rs232Buffer[RS232_BUFFER_SIZE];
char rs232BufferIndex;
char *rs232Comando;
char *rs232Parametro1;
char *rs232Parametro2;

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




    if (rs232_dataReceived_flag == 1) {        
        // Recibido un byte        

        rs232_dataReceived_flag = 0;

        rs232Buffer[rs232BufferIndex] = rs232_rxByte;
        if (rs232BufferIndex < RS232_BUFFER_SIZE-1)        // Evitamos un buffer overflow
                rs232BufferIndex++;
        

        if (rs232_rxByte == '\n') {

            rs232Buffer[rs232BufferIndex] = '\0';   // Hay que poner el 0 en el ultimo caracter para que STRTOK funcione
            rs232_processBuffer();
            rs232_processCommand();

            rs232BufferIndex = 0;                   // "borramos" el buffer

            
        }

        
    }

}

void rs232_processBuffer(void) {

    // Procesamos la cadena recibida


    rs232Comando = strtok(rs232Buffer," \r\n");
    rs232Parametro1 = strtok(NULL," \r\n");
    rs232Parametro2 = strtok(NULL," \r\n");

}

void rs232_processCommand(void) {

    
//    if (rs232Comando == NULL)       // No hay comando. Salimos
//        return;

    //if (strcmppgm2ram(rs232Comando, "SENT") == 0)
    if (strcmp(rs232Comando, "SEND") == 0) {
        rs232_processCommandSend();
        return;
    } else if (strcmp(rs232Comando, "CONFIG") == 0) {
        rs232_processCommandConfig();
        return;
    } else if (strcmp(rs232Comando, "SAVECONFIG") == 0) {
        rs232_processCommandSaveConfig();
        return;
    } else if (strcmp(rs232Comando, "LOADCONFIG") == 0) {
        rs232_processCommandLoadConfig();
        return;
    } else if (strcmp(rs232Comando, "DEFAULTCONFIG") == 0) {
        rs232_processCommandDefaultConfig();
        return;
    }

    rs232_putString("Comando no reconocido\r\n");

}

void rs232_processCommandSend(void) {

    UINT32 tmp;

    if (rs232Parametro1 == NULL || rs232Parametro2 == NULL) {
        rs232_putString("Formato: SEND RIC MENSAJE\r\n");
        return;
    }

    tmp =  atol(rs232Parametro1);
    pocsagPhy_sendMsg(pocsag_createNumericMsg(tmp, rs232Parametro2));
}


void rs232_processCommandConfig(void) {

    UINT32 tmp;
    char tmpBuffer[10];

    if (rs232Parametro1 == NULL || rs232Parametro2 == NULL) {
        rs232_putString("Formato: CONFIG PARAMETRO VALOR\r\n\r\nConfiguracion actual:\r\n");
        rs232_putString("   FRECUENCIA: ");
        sprintf(tmpBuffer,"%ld", config_frequency);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   DESVIACION: ");
        sprintf(tmpBuffer,"%d", config_deviation);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   BAUDIOS: ");
        sprintf(tmpBuffer,"%d", config_bauds);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   POTENCIA: ");
        sprintf(tmpBuffer,"%d", config_power);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n\r\n");

        return;
    }

    // Comprobamos el parametro y lo actualizamos

    tmp = atol(rs232Parametro2);

    if (strcmp(rs232Parametro1, "FRECUENCIA") == 0) {
        config_frequency = tmp;
    } else if (strcmp(rs232Parametro1, "DESVIACION") == 0) {
        config_deviation = (UINT16) tmp;
    } else if (strcmp(rs232Parametro1, "BAUDIOS") == 0) {
        config_bauds = (UINT16) tmp;
    } else if (strcmp(rs232Parametro1, "POTENCIA") == 0) {
       config_power = (UINT8) tmp;
    } else {
        rs232_putString("Parametro desconocido\r\n");
    }
 
}


void rs232_processCommandSaveConfig(void) {
    config_save();
}

void rs232_processCommandLoadConfig(void) {
    config_load();
}

void rs232_processCommandDefaultConfig(void) {
    config_setDefault();
}