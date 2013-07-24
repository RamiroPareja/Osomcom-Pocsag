#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
#include "rs232.h"
#include "pocsag.h"
#include "pocsagPhy.h"
#include "GenericTypeDefs.h"
#include "config.h"
#include "massSend.h"



void ui_processCommandSend(char *param1, char *param2, char *param3);
void ui_processCommandConfig(char *param1, char *param2);
void ui_processCommandSaveConfig();
void ui_processCommandLoadConfig();
void ui_processCommandDefaultConfig();

#ifndef NO_MASS_SEND
void ui_processCommandStartMassSend(char *param1, char *param2);
void ui_processCommandStopMassSend();
void ui_processCommandResumeMassSend();
void ui_processCommandStatusMassSend();
#endif

#ifndef DEBUG_MODE
void ui_processCommandTest(char *param1);
void ui_processCommandStopTest();
#endif



// buffer: cadena a procesar
// comando, parame1 y param2: puntero al puntero donde se devolvera el comando y los params
// No me gusta usar punteros a punteros, pero la alternativa es usar unas
// variables globales y eso me gusta casi que menos.
void ui_processBuffer(char *buffer, char **pComando, char **pParam1, char **pParam2, char **pParam3) {

    // Procesamos la cadena recibida

    *pComando = strtok(buffer," \r\n");
    *pParam1 = strtok(NULL," \r\n");
    *pParam2 = strtok(NULL," \r\n");
    *pParam3 = strtok(NULL,"\r\n");     //Notese como aqui no buscamos espacios. Asi el mensaje podra contenerlos

}

void ui_processCommand(char *comando, char *param1, char *param2, char *param3) {

    if (strcmp(comando, "S") == 0) {
        ui_processCommandSend(param1, param2, param3);
    } else if (strcmp(comando, "CONFIG") == 0) {
        ui_processCommandConfig(param1, param2);
    } else if (strcmp(comando, "SAVECFG") == 0) {
        ui_processCommandSaveConfig();
    } else if (strcmp(comando, "LOADCFG") == 0) {
        ui_processCommandLoadConfig();

#ifndef NO_MASS_SEND
    } else if (strcmp(comando, "START_MS") == 0) {
        ui_processCommandStartMassSend(param1,param2);
    } else if (strcmp(comando, "STOP_MS") == 0) {
        ui_processCommandStopMassSend();
    } else if (strcmp(comando, "RESUME_MS") == 0) {
        ui_processCommandResumeMassSend();
    } else if (strcmp(comando, "STATUS_MS") == 0) {
        ui_processCommandStatusMassSend();
#endif
    }

#ifndef DEBUG_MODE

    else if (strcmp(comando, "TEST") == 0) {
        ui_processCommandTest(param1);
    } else if (strcmp(comando, "STOP_TEST") == 0) {
        ui_processCommandStopTest();
    }
#endif

    else {
        rs232_putString(
            "Unknow command\r\n"
            "\r\n"
            "Commands:\r\n"
            " S (send)\r\n\r\n"
            " CONFIG\r\n"
            " SAVECFG\r\n"
            " LOADCFG\r\n"
#ifndef NO_MASS_SEND
            " START_MS\r\n"
            " STOP_MS\r\n"
            " RESUME_MS\r\n"
            " STATUS_MS\r\n\r\n"
#endif

#ifndef DEBUG_MODE
            " TEST\r\n"
            " STOP_TEST\r\n\r\n"
#endif
                );
        return;
    }

    rs232_putString("OK\r\n");

}

void ui_processCommandSend(char *param1, char *param2, char *param3) {

    UINT32 tmp;
    UINT8 type;

    if (strcmp(param1,"I")==0) {
        pocsagPhy_sendMsg(pocsag_createMsg(NULL, NULL, FUNCTION_IDLE, FALSE));
        return;
    } else if (strcmp(param1,"N")==0) {
        type=FUNCTION_NUMERIC;
    } else {
        type= FUNCTION_ALPHA;
    }

    if (param2 == NULL || param3 == NULL) {
        rs232_putString("Format: S (A|N|I) [RIC MSG]\r\n");
        return;
    }

#ifndef NO_MASS_SEND
    if (massSend_scanning) {
        rs232_putString("Stop MS first\r\n");
        return;
    }
#endif

    tmp = atol(param2);

    pocsagPhy_sendMsg(pocsag_createMsg(tmp, param3, type, FALSE));
}


void ui_processCommandConfig(char *param1, char *param2) {

    UINT32 tmp;
    char tmpBuffer[10];

    if (param1 == NULL || param2 == NULL) {
        rs232_putString("Format: CONFIG PARAM VALUE\r\n\r\nCurrent config:\r\n");
        rs232_putString(" FREQ: ");
        sprintf(tmpBuffer,"%lu", config.frequency);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n DEVIATION: ");
        sprintf(tmpBuffer,"%u", config.deviation);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n BAUDS: ");
        sprintf(tmpBuffer,"%u", config.bauds);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n POWER: ");
        sprintf(tmpBuffer,"%u", config.power);
        rs232_putStringRAM(tmpBuffer);
#ifndef NO_MASS_SEND
        rs232_putString("\r\n\r\n MS_SAVE_FREQ: ");
        sprintf(tmpBuffer,"%u", config.massSendSaveFreq);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n MS_DELAY: ");
        sprintf(tmpBuffer,"%u", config.massSendDelay);
        rs232_putStringRAM(tmpBuffer);
#endif
        rs232_putString("\r\n\r\n");

        return;
    }

    // Comprobamos el parametro y lo actualizamos

    tmp = atol(param2);

    if (strcmp(param1, "FREQ") == 0) {
        config.frequency = tmp;
    } else if (strcmp(param1, "DEVIATION") == 0) {
        config.deviation = (UINT16) tmp;
    } else if (strcmp(param1, "BAUDS") == 0) {        
        config.bauds = (UINT16) tmp;
        if (config.bauds==512) {
            config.tmr0Bauds = TMR0_BAUDRATE_512;
        } else if (config.bauds==1200) {
            config.tmr0Bauds = TMR0_BAUDRATE_1200;
        } else {
            config.tmr0Bauds = TMR0_BAUDRATE_2400;
            config.bauds=2400;
        }
    } else if (strcmp(param1, "POWER") == 0) {
       config.power = (UINT8) tmp;
#ifndef NO_MASS_SEND
    } else if (strcmp(param1, "MS_SAVE_FREQ") == 0) {
       config.massSendSaveFreq = (UINT16) tmp;
    } else if (strcmp(param1, "MS_DELAY") == 0) {
       config.massSendDelay = (UINT16) tmp;
#endif
    } else {
        rs232_putString("Unknow param\r\n");
    }

}


void ui_processCommandSaveConfig(void) {
    config_save();
}

void ui_processCommandLoadConfig(void) {
    config_load();
}


#ifndef NO_MASS_SEND
void ui_processCommandStartMassSend(char *param1, char *param2) {

    UINT32 startRIC;
    UINT32 stopRIC;


    if (param1 == NULL || param2 == NULL) {
        rs232_putString("Format: START_MS START_RIC STOP_RIC\r\n");
        return;
    }

    startRIC = atol(param1);
    stopRIC = atol(param2);

    if (startRIC > stopRIC) {
        rs232_putString("START_RIC should be lower than STOP_RIC\r\n");
        return;
    }

    massSend_scan(startRIC,stopRIC);
}


void ui_processCommandStopMassSend() {
    massSend_stop();
}


void ui_processCommandResumeMassSend() {
    massSend_resume();
}

void ui_processCommandStatusMassSend() {

    char tmpBuffer[10];

    rs232_putString("MS status: ");
    if (massSend_scanning)
        rs232_putString("Running\r\n");
    else
        rs232_putString("Stopped\r\n");

    rs232_putString("Current RIC: ");
    sprintf(tmpBuffer,"%ld", config.massSendCurrentRic);
    rs232_putStringRAM(tmpBuffer);
    
    rs232_putString("\r\nStop RIC: ");
    sprintf(tmpBuffer,"%ld", config.massSendStopRic);
    rs232_putStringRAM(tmpBuffer);
    rs232_putString("\r\n\r\n");

}
#endif

#ifndef DEBUG_MODE
void ui_processCommandTest(char *param1) {

    UINT8 tmp;

    if (param1 == NULL) {
        rs232_putString("Format: TEST [0,1,2]\r\n");
        return;
    }

    tmp = atoi(param1);
    pocsagPhy_test(tmp);
}

void ui_processCommandStopTest() {
    pocsagPhy_stopTest();
}

#endif