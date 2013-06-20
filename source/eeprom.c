#include <xc.h>

#include "eeprom.h"
#include <EEP.h>

//==================================
// EEPROM read routine

unsigned char readEEPROM(unsigned char Address) {
    /*EEADR = Address; // Load the low byte of the EEPROM address
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1; // Do the read
    return EEDATA; // Return with the data*/

    return Read_b_eep(Address);
}


//==================================
// EEPROM write routine

void writeEEPROM(unsigned char Address, unsigned char Data) {
    static unsigned char GIE_Status; // Variable to save Global Interrupt Enable bit

    /*EEADR =  Address; // Load the low byte of the EEPROM address
    EEDATA = Data; // Load the EEPROM data
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1; // Enable EEPROM writes
    GIE_Status = INTCONbits.GIE; // Save the Global Interrupt Enable bit
    INTCONbits.GIE = 0; // Disable global interrupts
    EECON2 = 0x55; // Required sequence to start the write cycle
    EECON2 = 0xAA; // Required sequence to start the write cycle
    EECON1bits.WR = 1; // Required sequence to start the write cycle
    INTCONbits.GIE = GIE_Status; // Restore the Global Interrupt Enable bit
    EECON1bits.WREN = 0; // Disable EEPROM writes
    while (EECON1bits.WR); // Wait for the write cycle to complete*/

    Write_b_eep(Address, Data);
}

void writeUINT16EEPROM(unsigned char Address, UINT16 Data) {

    int i;
    unsigned char *ptr = (unsigned char*) &Data;

    for (i=0; i<2; i++) {
        writeEEPROM(Address, *ptr);
        ptr++;
        Address++;
    }

}

void writeUINT32EEPROM(unsigned char Address, UINT32 Data) {

    int i;
    unsigned char *ptr = (unsigned char*) &Data;

    for (i=0; i<4; i++) {
        writeEEPROM(Address, *ptr);
        ptr++;
        Address++;
    }

}

UINT16 readUINT16EEPROM(unsigned char Address) {

    UINT16 tmp;
    unsigned char *ptr = (unsigned char*) &tmp;
    int i;

    for (i=0; i<2;i++) {
        *ptr = readEEPROM(Address);
        ptr++;
        Address++;
    }

    return tmp;

}


UINT32 readUINT32EEPROM(unsigned char Address) {

    UINT32 tmp;
    unsigned char *ptr = (unsigned char*) &tmp;
    int i;

    for (i=0; i<4;i++) {
        *ptr = readEEPROM(Address);
        ptr++;
        Address++;
    }

    return tmp;

}




void eraseEEPROM() {
    unsigned char i;

    for (i=0; i<256; i++) {
        writeEEPROM(i,0xFF);
    }


}