//tools

#include <xc.h>
#include "config.h"
#include "tools.h"


static const char nybble_chars[] = "0123456789ABCDEF";


//Convierte a tipo UCHAR una cadena ascii que contiene un byte en binario
unsigned char ascii2bin(char *datos, unsigned char numBits) {
    unsigned char i;
    unsigned char tmp;

    tmp = 0;

    for (i=0; i < 8; i++) {
        tmp = (tmp << 1);
        if (*datos==1)
            tmp=tmp+1;
        datos++;
    }

    return tmp;
}

//imprime en binario un byte
void bin2ascii(char datoBinario, char *datos, unsigned char numBits) {
    unsigned char i;

    for (i=0; i < numBits; i ++) {
        if (datoBinario & 0x80)
            *datos=1;
        else
            *datos=0;

        datoBinario = datoBinario << 1;
        datos++;
    }
}

//imprime en hexadecimal un byte
void byte2hex( unsigned char c, char *output) {

    output[0] = nybble_chars[ ( c >> 4 ) & 0x0F ];
    output[1] = nybble_chars[ c & 0x0F ];

}

void nibble2hex( unsigned char c, char *output) {

    *output = nybble_chars[ c & 0x0F ];
}

void delay_s() {

    BYTE i;

    for (i=0; i<250; i++) {

        __delay_ms(4);

    }


}