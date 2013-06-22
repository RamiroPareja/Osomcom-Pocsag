#include "GenericTypeDefs.h"
#include "config.h"
#include "eeprom.h"



UINT32 config_frequency;
UINT16 config_deviation;
UINT8 config_power;
UINT16 config_bauds;
CHAR config_inicializado;

UINT32 config_massSendCurrentRic;
UINT32 config_massSendStopRic;
UINT16 config_massSendSaveFreq;
UINT16 config_massSendDelay;


void config_setDefault(void) {

    // Cargar configuracion por defecto
    config_inicializado = CONFIG_BYTE;
    config_frequency = DEFAULT_CONFIG_FREQUENCY;
    config_deviation = DEFAULT_CONFIG_DEVIATION;
    config_power = DEFAULT_CONFIG_POWER;
    config_bauds = DEFAULT_CONFIG_BAUDS;
    
    config_massSendCurrentRic = DEFAULT_CONFIG_MASS_SEND_CURRENT_RIC;
    config_massSendStopRic = DEFAULT_CONFIG_MASS_SEND_STOP_RIC;
    config_massSendSaveFreq = DEFAULT_CONFIG_MASS_SEND_SAVE_FREQ;
    config_massSendDelay = DEFAULT_CONFIG_MASS_SEND_DELAY;

}

void config_load(void) {

    // Cargar configuracion por defecto
    config_inicializado = readEEPROM(EEADR_CONFIG_INICIALIZADO);
    config_frequency = readUINT32EEPROM(EEADR_CONFIG_FREQUENCY);
    config_deviation = readUINT16EEPROM(EEADR_CONFIG_DEVIATION);
    config_power = readEEPROM(EEADR_CONFIG_POWER);
    config_bauds = readUINT16EEPROM(EEADR_CONFIG_BAUDS);
    
    config_massSendCurrentRic = readUINT32EEPROM(EEADR_CONFIG_MASS_SEND_CURRENT_RIC);
    config_massSendStopRic = readUINT32EEPROM(EEADR_CONFIG_MASS_SEND_STOP_RIC);
    config_massSendSaveFreq = readUINT16EEPROM(EEADR_CONFIG_MASS_SEND_SAVE_FREQ);
    config_massSendDelay = readUINT16EEPROM(EEADR_CONFIG_MASS_SEND_DELAY);

}

void config_save(void) {

    // salvar
    writeEEPROM(EEADR_CONFIG_INICIALIZADO,config_inicializado);
    writeUINT32EEPROM(EEADR_CONFIG_FREQUENCY,config_frequency);
    writeUINT16EEPROM(EEADR_CONFIG_DEVIATION,config_deviation);
    writeEEPROM(EEADR_CONFIG_POWER,config_power);
    writeUINT16EEPROM(EEADR_CONFIG_BAUDS,config_bauds);
    
    writeUINT16EEPROM(EEADR_CONFIG_MASS_SEND_SAVE_FREQ, config_massSendSaveFreq);
    writeUINT16EEPROM(EEADR_CONFIG_MASS_SEND_DELAY, config_massSendDelay);
    config_saveMassSendStatus();

}

void config_saveMassSendStatus(void) {

    writeUINT32EEPROM(EEADR_CONFIG_MASS_SEND_CURRENT_RIC, config_massSendCurrentRic);
    writeUINT32EEPROM(EEADR_CONFIG_MASS_SEND_STOP_RIC, config_massSendStopRic);

}

void config_init(void) {

    config_load();

    // Si la eeprom no esta seteada por que el micro esta recien programado,
    // cargamos la configuracion por defecto.
    // Detectamos si la eeprom esta virgen si el registro "default" esta a 0

    // Si utilizaramos una variable boolean para definir en la eeprom si se ha
    // inicializado o no el sistema, tendriamos un 50% de posibilidades de que
    // sin haberlo inicializado tuvieramos un falso positivo. Para evitar esto,
    // ponemos un digito de configuracion raro.

    if (config_inicializado != CONFIG_BYTE) {
        eraseEEPROM();
        config_setDefault();
        config_save();
    }


}


