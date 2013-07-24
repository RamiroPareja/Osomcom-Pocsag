#include <xc.h>
#include "GenericTypeDefs.h"
#include "config.h"
#include "eeprom.h"
#include "pocsagPhy.h"



Config_t config;

// Cargamos en la EEPROM los datos por defecto
__EEPROM_DATA_DWORD(DEFAULT_CONFIG_FREQUENCY);
__EEPROM_DATA_WORD(DEFAULT_CONFIG_DEVIATION);
__EEPROM_DATA_WORD(DEFAULT_CONFIG_BAUDS);
__EEPROM_DATA_WORD(DEFAULT_CONFIG_TMR0_BAUDS);
__EEPROM_DATA_BYTE(DEFAULT_CONFIG_POWER);
#ifndef NO_MASS_SEND
__EEPROM_DATA_DWORD(DEFAULT_CONFIG_MASS_SEND_CURRENT_RIC);
__EEPROM_DATA_DWORD(DEFAULT_CONFIG_MASS_SEND_STOP_RIC);
__EEPROM_DATA_WORD(DEFAULT_CONFIG_MASS_SEND_SAVE_FREQ);
__EEPROM_DATA_WORD(DEFAULT_CONFIG_MASS_SEND_DELAY);
#endif


void config_load(void) {

    readBYTEARRAYEEPROM(EEADR_CONFIG,(BYTE *) &config, sizeof(Config_t));

}

void config_save(void) {

    writeBYTEARRAYEEPROM(EEADR_CONFIG,(BYTE *) &config, sizeof(Config_t));

}

#ifndef NO_MASS_SEND
void config_saveMassSendStatus(void) {

    writeUINT32EEPROM(EEADR_CONFIG_MASS_SEND_CURRENT_RIC, config.massSendCurrentRic);
    writeUINT32EEPROM(EEADR_CONFIG_MASS_SEND_STOP_RIC, config.massSendStopRic);

}
#endif

void config_init(void) {

    config_load();

}


