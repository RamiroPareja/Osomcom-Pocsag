#include <xc.h>
#include "adf7012.h"
#include "io.h"
#include "spi.h"
#include "GenericTypeDefs.h"
#include "config.h"


Adf7012Config_t adf7012Config;

void adf7012_initRegisters(void);
void adf7012_configure(BYTE, BYTE, BYTE, BYTE);
void adf7012_configure24(BYTE, BYTE, BYTE);
void adf7012_configureRegisters(void);
void adf7012_setModulation(BYTE);
void adf7012_setFrequency(UINT32);
void adf7012_setFSKDeviation(UINT32);
void adf7012_setPower(BYTE);
void adf7012_setMuxout(BYTE);


BOOL adf7012_init() {

    UINT8 i;

    adf7012_powerOn();

    RADIO_DATA = 0;
    RADIO_CLK = 0;

    spi_init();

    adf7012_initRegisters();

    adf7012_setModulation(MODULATION_FSK);
    adf7012_setFrequency(config.frequency);
    adf7012_setFSKDeviation(config.deviation);
    adf7012_setPower(config.power);
    adf7012_setMuxout(MUXOUT_LOGIC_DIGITAL_LOCKOUT);

    adf7012Config.register0bits.vcoAdjust_L = 0;
    adf7012Config.register0bits.vcoAdjust_H = 1;

    
    adf7012_configureRegisters();


    // Esperamos al que lockup del pll antes de activar el PA
    i=0;
    while (MUXOUT == 0) {
        _delay(40000);
        i++;
        if (i==255)
            return TRUE;    //TIMEOUT. El PLL no puede hacer el lockup
    }

    adf7012Config.register3bits.paEnable = 1;
    adf7012_configureRegisters();

    return FALSE;
}

// Setea los registros de configuracion a algo conocido
void adf7012_initRegisters(void) {

    // Hay que asegurarse de al menos cargar correctamente los bits de direccion
    adf7012Config.register0 = 0x00000000;
    adf7012Config.register1 = 0x00000001;
    adf7012Config.register2 = 0x00000002;
    adf7012Config.register3 = 0x00000003;

}


void adf7012_configure(BYTE byte3, BYTE byte2, BYTE byte1, BYTE byte0) {
    
    
    LE = 0;
    _delay(10);

    spi_putch(byte3);
    spi_putch(byte2);
    spi_putch(byte1);
    spi_putch(byte0);

    _delay(20);
    LE = 1;
    _delay(30);
    LE = 0;
    


}

void adf7012_configure24(BYTE byte2, BYTE byte1, BYTE byte0) {


    LE = 0;
    _delay(10);

    spi_putch(byte2);
    spi_putch(byte1);
    spi_putch(byte0);

    LE = 1;
    _delay(40);
    LE = 0;
    _delay(40);

}



void adf7012_configureRegisters(void) {

    adf7012_configure(adf7012Config.register0Bytes.byte3,
                      adf7012Config.register0Bytes.byte2,
                      adf7012Config.register0Bytes.byte1,
                      adf7012Config.register0Bytes.byte0);

    __delay_ms(1);

    adf7012_configure24(adf7012Config.register1Bytes.byte2,
                      adf7012Config.register1Bytes.byte1,
                      adf7012Config.register1Bytes.byte0);

    __delay_ms(1);

    adf7012_configure(adf7012Config.register2Bytes.byte3,
                      adf7012Config.register2Bytes.byte2,
                      adf7012Config.register2Bytes.byte1,
                      adf7012Config.register2Bytes.byte0);

    __delay_ms(1);

    adf7012_configure(adf7012Config.register3Bytes.byte3,
                      adf7012Config.register3Bytes.byte2,
                      adf7012Config.register3Bytes.byte1,
                      adf7012Config.register3Bytes.byte0);

    __delay_ms(1);


}

void adf7012_setModulation(BYTE modulation) {

    switch (modulation) {
        case MODULATION_FSK:
            adf7012Config.register2bits.modControl = 0;
            adf7012Config.register2bits.gook = 0;
            break;

        case MODULATION_GFSK:
            adf7012Config.register2bits.modControl = 1;
            adf7012Config.register2bits.gook = 0;
            break;

        case MODULATION_ASK:
            adf7012Config.register2bits.modControl = 2;
            adf7012Config.register2bits.gook = 0;
            break;

        case MODULATION_OOK:
            adf7012Config.register2bits.modControl = 3;
            adf7012Config.register2bits.gook = 0;
            break;

        case MODULATION_GOOK:
            adf7012Config.register2bits.modControl = 3;
            adf7012Config.register2bits.gook = 1;
            break;
    }

}

void adf7012_setFrequency(UINT32 frequency) {

    UINT8 Nint;
    UINT16 Nfrac;
    UINT32 tmp;
    UINT32 tmp2;
    UINT32 vcoFrequency = frequency;


    // La frecuencia de salida del VCO deberia ser mayor de 550MHz
    // Si no es asi, utilizamos el divisor para conseguirlo.

    if (frequency < LIMIT_FREQ_DIVIDER_8) {
        //vcoFrequency *= 8;
        adf7012Config.register0bits.outputDivider = 3;
    }
    else if (frequency < LIMIT_FREQ_DIVIDER_4) {
        //vcoFrequency *= 4;
        adf7012Config.register0bits.outputDivider = 2;
    }
    else if (frequency < LIMIT_FREQ_DIVIDER_2) {
        //vcoFrequency *= 2;
        adf7012Config.register0bits.outputDivider = 1;
    }
    else {
        adf7012Config.register0bits.outputDivider = 0;
    }

    // R=1, por lo que Fpfd = _RF_XTAL_FREQ
    adf7012Config.register0bits.rDivider_H=0;
    adf7012Config.register0bits.rDivider_L=1;
    


    // Calculo de la parte entera del divisor del PLL
    Nint = vcoFrequency/_RF_XTAL_FREQ;

    
    // Calculo de la parte fraccional del divisor del PLL
    
    // *************************************************************************
    //                        HACKER CODE AHEAD!!!
    // *************************************************************************

    // El algoritmo es:
    //    Nfrac = ((vcoFrequency % Fpfd) * 2^12) / Fpfd
    //
    // Teniendo que Fpfd puede llegar a ser hasta 8MHz, tendriamos una
    // multiplicacion de un termino de 23 bits por otro de 12 bits. Se
    // desbordaria la variable de 32 bits.
    //
    // La solucion de utilizar variables de 64 bits no es aplicable en el XC8,
    // pues solo soporta hasta 32 bits.
    //
    // Optamos por dividir el algoritmo en varios pasos con mucho cuidado de
    // no desbordar en ningun momento la precision de la variable.
    //
    //
    // !!!!!!!!!!!WARNING!!!!!!!!!!!!
    //
    // Si el Fpfd (_RF_XTAL_FREQ en este caso) no es multiplo de 16, VAMOS A
    // A PERDER PRECISION EN EL CALCULO!!!
    // Como por estamos usando un cristal de 4MHz, no pasa nada.


    tmp = (vcoFrequency % _RF_XTAL_FREQ);       // 23 bits
    tmp *= 512;                                 // 23 bits + 9 = 32 bits

    // *************************************************************************
    //                        COMBO! HACKER HIT x 2
    // *************************************************************************

    tmp2 = _RF_XTAL_FREQ / 16;                   // 23 bits - 4 bits = 19 bits
    Nfrac = tmp / tmp2 ;                        // 32 bits - 19 bits = 13 bits

    // Esta ultima division deberiamos hacerla en coma flotante y redondear al
    // entero mas proximo para tener la mayor precision.
    //
    // Para acelerar el calculo, evitar floats y la libreria math.h, hemos hecho
    // la division como enteros y vamos a redondear fijandonos en el bit menos
    // significativo del resultado division.
    //
    // La variable tmp2 deberia haber sido calculada como _RF_XTAL_FREQ/8 para
    // que al final hubieramos multiplicado el subproducto por 2^12.
    // Al dividir por 16 tenemos 13 bits en Nfrac en vez de 12. Ese bit de mas
    // que nos sobra nos servira para saber si tenemos que redondear para arriba
    // o no. Luego tenemos que descartarlo.
    //
    // NOTA: La variable tmp2 realmente es una constante. Si se activa las
    //       optimizaciones, el compilador deberia tratarlo como tal y
    //       precalcular su valor literal en tiempo de compilacion.

    if (Nfrac & 1) {
        Nfrac >>= 1;
        Nfrac++;
    }
    else
        Nfrac >>= 1;

    adf7012Config.register1bits.integerN_L = Nint & 0b00000011;
    adf7012Config.register1bits.integerN_H = (Nint>>2) & 0b00111111;

    if (Nint >= 91)
        adf7012Config.register1bits.prescaler = 1;
    else
        adf7012Config.register1bits.prescaler = 0;

    adf7012Config.register1bits.fractionalN_L = Nfrac & 0b0000000000111111;
    adf7012Config.register1bits.fractionalN_H = (Nfrac>>6) & 0b0000000000111111;

    

    adf7012Config.register3bits.pllEnable = 1;
    adf7012Config.register3bits.chargePump = 1;
    adf7012Config.register3bits.vcoBias = 6;

}

void adf7012_setFSKDeviation(UINT32 frequency) {

    // La formula para calcular el valor del Modulation Number es:
    //      ModulationNumber = (Fdeviation * 2^14)/Fpfd
    //
    // Hay que tener cuidado de no desbordar las variables.

    UINT16 modNumber;
    UINT32 tmp;

    // Vamos a repetir el mismo truco que en la funcion adf7012_setFrequency()
    // para calcular el valor del registro Modulation Number evitando coma
    // flotante y la funcion de redondeo de math.h
    // Ver en la funcion adf7012_setFrequency() la explicacion de lo que hacemos

    tmp = frequency * 16384;             // 18 bits + 14 bits = 32 bits
    modNumber = tmp / (_RF_XTAL_FREQ/2);     // 32 bits - 22 bits = 10 bits

    if (modNumber & 1) {
        modNumber >>= 1;
        modNumber++;
    }
    else
        modNumber >>= 1;


    adf7012Config.register2bits.modDeviation_L = modNumber & 0b0000000000011111;
    adf7012Config.register2bits.modDeviation_H = (modNumber>>5) & 0b0000000000001111;

}

void adf7012_setPower(BYTE power) {

    adf7012Config.register2bits.powerAmplifier_L = power & 0b00000111;
    adf7012Config.register2bits.powerAmplifier_H = (power>>3) & 0b00000111;
    //adf7012Config.register3bits.paBias = 4;
    adf7012Config.register3bits.paBias =4;
    //adf7012Config.register3bits.paEnable = 1;

}

void adf7012_setMuxout(BYTE muxout) {

    adf7012Config.register3bits.muxout = muxout;

}

