

#ifndef EEPROM_H
#define	EEPROM_H

#define EEADR_CONFIG_INICIALIZADO 0
#define EEADR_CONFIG_FREQUENCY 1
#define EEADR_CONFIG_DEVIATION 5
#define EEADR_CONFIG_POWER 7
#define EEADR_CONFIG_BAUDS 9

unsigned char readEEPROM(unsigned char Address);
void writeEEPROM(unsigned char Address, unsigned char Data);
void writeUINT16EEPROM(unsigned char Address, UINT16 Data);
void writeUINT32EEPROM(unsigned char Address, UINT32 Data);
UINT16 readUINT16EEPROM(unsigned char Address);
UINT32 readUINT32EEPROM(unsigned char Address);
void eraseEEPROM(void);


#endif	/* EEPROM_H */

