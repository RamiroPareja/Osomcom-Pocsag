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



void ui_processCommandSend(char *param1, char *param2);
void ui_processCommandConfig(char *param1, char *param2);
void ui_processCommandSaveConfig();
void ui_processCommandLoadConfig();
void ui_processCommandDefaultConfig();

void ui_processCommandStartMassSend(char *param1, char *param2);
void ui_processCommandStopMassSend();
void ui_processCommandResumeMassSend();
void ui_processCommandStatusMassSend();

void ui_processCommandTest(char *param1);
void ui_processCommandStopTest();



// buffer: cadena a procesar
// comando, parame1 y param2: puntero al puntero donde se devolvera el comando y los params
// No me gusta usar punteros a punteros, pero la alternativa es usar unas
// variables globales y eso me gusta casi que menos.
void ui_processBuffer(char *buffer, char **pComando, char **pParam1, char **pParam2) {

    // Procesamos la cadena recibida

    *pComando = strtok(buffer," \r\n");
    *pParam1 = strtok(NULL," \r\n");
    *pParam2 = strtok(NULL," \r\n");

}

void ui_processCommand(char *comando, char *param1, char *param2) {

    if (strcmp(comando, "SEND") == 0) {
        ui_processCommandSend(param1, param2);
        return;
    } else if (strcmp(comando, "CONFIG") == 0) {
        ui_processCommandConfig(param1, param2);
        return;
    } else if (strcmp(comando, "SAVECONFIG") == 0) {
        ui_processCommandSaveConfig();
        return;
    } else if (strcmp(comando, "LOADCONFIG") == 0) {
        ui_processCommandLoadConfig();
        return;
    } else if (strcmp(comando, "DEFAULTCONFIG") == 0) {
        ui_processCommandDefaultConfig();
        return;
    } else if (strcmp(comando, "START_MASS_SEND") == 0) {
        ui_processCommandStartMassSend(param1,param2);
        return;
    } else if (strcmp(comando, "STOP_MASS_SEND") == 0) {
        ui_processCommandStopMassSend();
        return;
    } else if (strcmp(comando, "RESUME_MASS_SEND") == 0) {
        ui_processCommandResumeMassSend();
        return;
    } else if (strcmp(comando, "STATUS_MASS_SEND") == 0) {
        ui_processCommandStatusMassSend();
        return;
    } else if (strcmp(comando, "TEST") == 0) {
        ui_processCommandTest(param1);
        return;
    } else if (strcmp(comando, "STOP_TEST") == 0) {
        ui_processCommandStopTest();
        return;
    }


    rs232_putString(
            "Comando no reconocido\r\n"
            "\r\n"
            "Comandos:\r\n"
            "  SEND\r\n\r\n"
            "  CONFIG\r\n"
            "  SAVECONFIG\r\n"
            "  LOADCONFIG\r\n"
            "  DEFAULTCONFIG\r\n\r\n"
            "  START_MASS_SEND\r\n"
            "  STOP_MASS_SEND\r\n"
            "  RESUME_MASS_SEND\r\n"
            "  STATUS_MASS_SEND\r\n\r\n"
            "  TEST\r\n"
            "  STOP_TEST\r\n\r\n");

}

void ui_processCommandSend(char *param1, char *param2) {

    UINT32 tmp;

    if (param1 == NULL || param2 == NULL) {
        rs232_putString("Formato: SEND RIC MENSAJE\r\n");
        return;
    }

    if (massSend_scanning) {
        rs232_putString("MASS SEND en marcha. Detengalo antes\r\n");
        return;
    }

    tmp =  atol(param1);
    pocsagPhy_sendMsg(pocsag_createNumericMsg(tmp, param2, FALSE));
}


void ui_processCommandConfig(char *param1, char *param2) {

    UINT32 tmp;
    char tmpBuffer[10];

    if (param1 == NULL || param2 == NULL) {
        rs232_putString("Formato: CONFIG PARAMETRO VALOR\r\n\r\nConfiguracion actual:\r\n");
        rs232_putString("   FRECUENCIA: ");
        sprintf(tmpBuffer,"%lu", config_frequency);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   DESVIACION: ");
        sprintf(tmpBuffer,"%u", config_deviation);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   BAUDIOS: ");
        sprintf(tmpBuffer,"%u", config_bauds);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   POTENCIA: ");
        sprintf(tmpBuffer,"%u", config_power);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n\r\n   MASS_SEND_SAVE_FREQ: ");
        sprintf(tmpBuffer,"%u", config_massSendSaveFreq);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n   MASS_SEND_DELAY: ");
        sprintf(tmpBuffer,"%u", config_massSendDelay);
        rs232_putStringRAM(tmpBuffer);
        rs232_putString("\r\n\r\n");

        return;
    }

    // Comprobamos el parametro y lo actualizamos

    tmp = atol(param2);

    if (strcmp(param1, "FRECUENCIA") == 0) {
        config_frequency = tmp;
    } else if (strcmp(param1, "DESVIACION") == 0) {
        config_deviation = (UINT16) tmp;
    } else if (strcmp(param1, "BAUDIOS") == 0) {
        config_bauds = (UINT16) tmp;
    } else if (strcmp(param1, "POTENCIA") == 0) {
       config_power = (UINT8) tmp;
    } else if (strcmp(param1, "MASS_SEND_SAVE_FREQ") == 0) {
       config_massSendSaveFreq = (UINT16) tmp;
    } else if (strcmp(param1, "MASS_SEND_DELAY") == 0) {
       config_massSendDelay = (UINT16) tmp;
    } else {
        rs232_putString("Parametro desconocido\r\n");
    }

}


void ui_processCommandSaveConfig(void) {
    config_save();
}

void ui_processCommandLoadConfig(void) {
    config_load();
}

void ui_processCommandDefaultConfig(void) {
    config_setDefault();
}


void ui_processCommandStartMassSend(char *param1, char *param2) {

    UINT32 startRIC;
    UINT32 stopRIC;


    if (param1 == NULL || param2 == NULL) {
        rs232_putString("Formato: START_MASS_SEND START_RIC STOP_RIC\r\n");
        return;
    }

    startRIC = atol(param1);
    stopRIC = atol(param2);

    if (startRIC > stopRIC) {
        rs232_putString("El RIC de inicio debe ser menor del de parada\r\n");
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

    rs232_putString("Estado Mass Send: ");
    if (massSend_scanning)
        rs232_putString("En ejecucion\r\n");
    else
        rs232_putString("Parado\r\n");

    rs232_putString("RIC actual: ");
    sprintf(tmpBuffer,"%ld", config_massSendCurrentRic);
    rs232_putStringRAM(tmpBuffer);
    
    rs232_putString("\r\nRIC de parada: ");
    sprintf(tmpBuffer,"%ld", config_massSendStopRic);
    rs232_putStringRAM(tmpBuffer);
    rs232_putString("\r\n\r\n");

}


void ui_processCommandTest(char *param1) {

    UINT8 tmp;

    if (param1 == NULL) {
        rs232_putString("Formato: TEST [0,1,2]\r\n");
        return;
    }

    tmp = atoi(param1);
    pocsagPhy_test(tmp);
}

void ui_processCommandStopTest() {
    pocsagPhy_stopTest();
}