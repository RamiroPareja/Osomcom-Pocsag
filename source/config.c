#include "GenericTypeDefs.h"
#include "config.h"
#include "eeprom.h"



UINT32 config_frequency;
UINT16 config_deviation;
UINT8 config_power;
UINT16 config_bauds;
CHAR config_inicializado;


void config_setDefault(void) {

    // Cargar configuracion por defecto
    config_inicializado = CONFIG_BYTE;
    config_frequency = DEFAULT_CONFIG_FREQUENCY;
    config_deviation = DEFAULT_CONFIG_DEVIATION;
    config_power = DEFAULT_CONFIG_POWER;
    config_bauds = DEFAULT_CONFIG_BAUDS;
    

}

void config_load(void) {

    // Cargar configuracion por defecto
    config_inicializado = readEEPROM(EEADR_CONFIG_INICIALIZADO);
    config_frequency = readUINT32EEPROM(EEADR_CONFIG_FREQUENCY);
    config_deviation = readUINT16EEPROM(EEADR_CONFIG_DEVIATION);
    config_power = readEEPROM(EEADR_CONFIG_POWER);
    config_bauds = readUINT16EEPROM(EEADR_CONFIG_BAUDS);
    

}

void config_save(void) {

    // salvar
    writeEEPROM(EEADR_CONFIG_INICIALIZADO,config_inicializado);
    writeUINT32EEPROM(EEADR_CONFIG_FREQUENCY,config_frequency);
    writeUINT16EEPROM(EEADR_CONFIG_DEVIATION,config_deviation);
    writeEEPROM(EEADR_CONFIG_POWER,config_power);
    writeUINT16EEPROM(EEADR_CONFIG_BAUDS,config_bauds);

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
        //eraseEEPROM();
        config_setDefault();
        config_save();
    }


}


