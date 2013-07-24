#ifndef EEPROM_H
#define	EEPROM_H

#define EEADR_CONFIG 0
#define EEADR_CONFIG_FREQUENCY 0
#define EEADR_CONFIG_DEVIATION 4
#define EEADR_CONFIG_POWER 6
#define EEADR_CONFIG_BAUDS 7
#define EEADR_CONFIG_TMR0_BAUDS 9


#define EEADR_CONFIG_MASS_SEND_CURRENT_RIC 11
#define EEADR_CONFIG_MASS_SEND_STOP_RIC 15
#define EEADR_CONFIG_MASS_SEND_SAVE_FREQ 19
#define EEADR_CONFIG_MASS_SEND_DELAY 21


unsigned char readEEPROM(unsigned char Address);
void writeBYTEARRAYEEPROM(unsigned char address, BYTE *data, UINT8 dataLength);
void writeEEPROM(unsigned char Address, unsigned char Data);
void writeUINT16EEPROM(unsigned char Address, UINT16 Data);
void writeUINT32EEPROM(unsigned char Address, UINT32 Data);
void readBYTEARRAYEEPROM(unsigned char address, BYTE *data, UINT8 dataLength);
UINT16 readUINT16EEPROM(unsigned char Address);
UINT32 readUINT32EEPROM(unsigned char Address);
void eraseEEPROM(void);


// Macros para poner datos en la EEPROM. Similar a la __EEPROM_DATA() del XC8

#define __EEPROM_DATA_DWORD(dword) \
			 asm("\tpsect eeprom_data,class=EEDATA"); \
			 asm("\tdw\t" ___mkstr(dword & 0xFFFF) "," ___mkstr(dword>>16));

#define __EEPROM_DATA_WORD(word) \
			 asm("\tpsect eeprom_data,class=EEDATA"); \
			 asm("\tdw\t" ___mkstr(word));

#define __EEPROM_DATA_BYTE(byte) \
			 asm("\tpsect eeprom_data,class=EEDATA"); \
			 asm("\tdb\t" ___mkstr(byte));



#endif	/* EEPROM_H */

