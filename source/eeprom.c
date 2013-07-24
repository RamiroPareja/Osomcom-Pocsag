#include <xc.h>

#include "eeprom.h"
#include <plib/EEP.h>

//==================================
// EEPROM read routine

unsigned char readEEPROM(unsigned char Address) {
    
    return Read_b_eep(Address);
}


//==================================
// EEPROM write routine

void writeEEPROM(unsigned char Address, unsigned char Data) {  

    Write_b_eep(Address, Data);
}

void writeBYTEARRAYEEPROM(unsigned char address, BYTE *data, UINT8 dataLength) {

    int i;

    for (i=dataLength; i>0; i--) {
        writeEEPROM(address, *data);
        data++;
        address++;
    }

}

void writeUINT16EEPROM(unsigned char Address, UINT16 Data) {
    
    writeBYTEARRAYEEPROM(Address, (BYTE *) &Data, 2);

}

void writeUINT32EEPROM(unsigned char Address, UINT32 Data) {   

    writeBYTEARRAYEEPROM(Address,(BYTE *) &Data, 4);

}

void readBYTEARRAYEEPROM(unsigned char address, BYTE *data, UINT8 dataLength) {


    int i;

    for (i=dataLength; i>0; i--) {
        *data = readEEPROM(address);
        data++;
        address++;
    }


}

UINT16 readUINT16EEPROM(unsigned char Address) {

    UINT16 tmp;
  
    readBYTEARRAYEEPROM(Address, (BYTE *) &tmp, 2);

    return tmp;

}


UINT32 readUINT32EEPROM(unsigned char Address) {

    UINT32 tmp;   

    readBYTEARRAYEEPROM(Address, (BYTE *) &tmp, 4);

    return tmp;
}




void eraseEEPROM() {
    
    UINT16 i;

    for (i=0; i<256; i++) {
        writeEEPROM(i,0xFF);
    }

}