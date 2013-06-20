#include <xc.h>
#include "spi.h"
#include "io.h"


// OBSERVACIONES:
//
// Se podria haber utilizado el modulo MSSP del PIC para transmitir los datos,
// pero dado a que los registros son de 32 bits y no de 8 (lo que complicaria
// ligeramente el control), prefiero hacerlo por software.

void spi_init() {

    SCK = 0;
    SDO = 0;

}

void spi_putch(unsigned char c) {

    unsigned char i;

    for (i=0; i<8; i++) {

        SDO  = ((c & 0x80) != 0);
        _delay(40);
        SCK = 1;
        _delay(40);
        SCK = 0;
        _delay(4);      // El propio bucle mete bastante retardo

        c <<=1;

    }
   
}
