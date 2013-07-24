#include <string.h>
#include "pocsag.h"
#include "rs232.h"
#include "GenericTypeDefs.h"



// NOTA:
//
// En la la primera version teniamos un array de Batch_t que ibamos rellenando
// y una funcion en pocsagPhy que los iba transmitiendo.
// Por desgracia, el acceso a estructuras Batch_t era MUY lento incluso si se
// hacia por medio de un puntero de tipo Byte* (medio segundo para asignar la
// direccion del Batch al puntero).
//
// Para solventarlo, ahora tendremos un Batch_t temporal donde formateamos el
// mensaje y luego lo volcaremos a un array de bytes. Asi, el acceso a los datos
// sera mucho mas rapido cuando haya que transmitirlos.

Batch_t batchTmp;
BYTE batchBuffer[NUM_BATCHES*sizeof(Batch_t)];

void pocsag_initBatch(Batch_t *);
void pocsag_copyFrame2Batch(Frame_t *frame, Batch_t *batch);
void pocsag_copyCWs2Batch(CodeWord_t *codeword_H, CodeWord_t *codeword_L, Batch_t *batch);
void formatAddressFrame(Batch_t *, UINT32, UINT8);
UINT8 pocsag_formatNumericMessageCodeWord(CodeWord_t *, char *);
UINT8 pocsag_formatAlphaMessageCodeWord(CodeWord_t *, char *, BOOL init);
void pocsag_calculate_CRC(CodeWord_t *);
void pocsag_encodeNumericMessage(char *);
UINT16 pocsag_dumpBatch2Buffer(BYTE *buffer, Batch_t *batch);

// Funcion que inicializa el batch con todos los frames a Idle

void pocsag_initBatch(Batch_t *batch) {

    BYTE i;
    
    batch->sc.dword = SYNCH_CODEWORD;

    for (i = 0; i < 8; i++) {
        batch->frames[i].codeword_L.dword = IDLE_CODEWORD;
        batch->frames[i].codeword_H.dword = IDLE_CODEWORD;
    }

}

// Copia un frame a todos los frames de un batch
void pocsag_copyFrame2Batch(Frame_t *frame, Batch_t *batch) {

    BYTE i;

    for (i = 0; i < 8; i++) {
        batch->frames[i].codeword_H.dword = frame->codeword_H.dword;
        batch->frames[i].codeword_L.dword = frame->codeword_L.dword;
    }

}

// Copia el codeword alto y bajo a todos los frames de un batch. Si algun codeword es NULL, no lo copia
void pocsag_copyCWs2Batch(CodeWord_t *codeword_H, CodeWord_t *codeword_L, Batch_t *batch) {

    BYTE i;

    for (i = 0; i < 8; i++) {
        if (codeword_L != NULL)
            batch->frames[i].codeword_L.dword = codeword_L->dword;

        if (codeword_H != NULL)
            batch->frames[i].codeword_H.dword = codeword_H->dword;
    }
}



// Formatea un codeword como direccion

void pocsag_formatAddressCodeWord(CodeWord_t *cw, UINT32 address, UINT8 function) {


    // Los 3 bits LSB de la direccion indican en que frame hay que usar
    //BYTE frameNumber = address & 0x00000007;

    cw->dword=0;

    address >>= 3;

    cw->messageType = 0; // Tipo direccion    
    cw->address_L = address & 0x000000007;
    address >>= 3;
    cw->address_H = address & 0x000000FF;
    address >>= 8;
    cw->address_HH = address & 0x0000007F;

    cw->function = function;

    // Calcular CRC y paridad
    pocsag_calculate_CRC(cw);

}

// El parametro *number es un puntero a un array de 5 caracteres numericos

UINT8 pocsag_formatNumericMessageCodeWord(CodeWord_t *cw, char *number) {

    cw->dword = 0;

    pocsag_encodeNumericMessage(number);

    cw->messageType = 1; // Tipo Mensaje
    cw->message_HH = (number[0] << 3) | (number[1] >> 1);
    cw->message_H = (number[1] << 7) | (number[2] << 3) | (number[3] >> 1);
    cw->message_L = ((number[3]&0x01) << 4) | (number[4]);

    // Calcular CRC y paridad
    pocsag_calculate_CRC(cw);

    return 5;   // Devuelve numero de caracteres procesados

}


UINT8 pocsag_formatAlphaMessageCodeWord(CodeWord_t *cw, char *string, BOOL init) {

    static UINT8 i;

    UINT8 charsProcessed = 0;
    UINT32 buffer = 0;
    UINT8 bitCounter = 20;

    if (init) {
        i = 0;
    }

    // Este while hay que refactorizarlo por que me parece cutrisimo
    do {

        for (;i<7;i++) {

            buffer <<=1;
            buffer |= *string & 1;
            (*string)>>=1;

            bitCounter--;
            if (bitCounter==0) {
                i++;
                break;
            }
        }

        if (bitCounter==0) {
            break;
        }

        i = 0;
        charsProcessed++;
        string++;


    } while (TRUE);


    cw->dword = 0;

    cw->messageType = 1; // Tipo Mensaje
     
    
    cw->message_L = buffer & 0x1F;  // 5 bits  de menor peso
    buffer >>=5;
    cw->message_H = buffer & 0xFF;  // 8 bits de menor peso
    buffer >>=8;
    cw->message_HH = buffer & 0x7F;  // 7 bits de menor peso

    // Calcular CRC y paridad
    pocsag_calculate_CRC(cw);

    return charsProcessed;   // Devuelve numero de caracteres procesados
}


UINT16 pocsag_createMsg(UINT32 address, char *data, UINT8 function, BOOL massSend) {

    INT8 digitsToTX = strlen(data);
    UINT8 processedDigits;
    INT8 frameNumber;
    UINT16 copiedBytes = 0;
    CodeWord_t *cw;
    BOOL highCW = TRUE;
    UINT8 batchCounter = 1;

    #ifdef DEBUG_RS232
    UINT16 j;
    BYTE *ptrTmp;
    #endif



    pocsag_initBatch(&batchTmp);

    if (function==FUNCTION_IDLE)
        return pocsag_dumpBatch2Buffer(batchBuffer, &batchTmp);

    frameNumber = 7 - (address & 0x00000007);
    cw = &(batchTmp.frames[frameNumber].codeword_H);
    pocsag_formatAddressCodeWord(cw, address, function);


    cw = &(batchTmp.frames[frameNumber].codeword_L);

    if (function==FUNCTION_NUMERIC)
        processedDigits = pocsag_formatNumericMessageCodeWord(cw, data); // Formateo de los 5 primeros digitos
    else {
        digitsToTX++;   // Enviamos el ultimo 0 como fin de mensaje
        data[digitsToTX]=0;

        data[digitsToTX+1]=0;   // Por la manera en la que funciona pocsag_formatAlphaMessageCodeWord,
        data[digitsToTX+2]=0;   // tenemos que asegurarnos que los ultimos 3 bytes (caso peor) son ceros
        processedDigits = pocsag_formatAlphaMessageCodeWord(cw, data, TRUE); // Formateo de los 2.5 primeros digitos
    }

    digitsToTX -= processedDigits;

    if (massSend) {
        // Enviamos el mismo mensaje a los 8 RIC del batch
        pocsag_copyFrame2Batch(&(batchTmp.frames[frameNumber]), &batchTmp);
        pocsag_dumpBatch2Buffer(batchBuffer, &batchTmp);

        pocsag_initBatch(&batchTmp);
        copiedBytes = pocsag_dumpBatch2Buffer(NULL, &batchTmp);

        #ifdef DEBUG_RS232
        ptrTmp = batchBuffer;
        for (j=0; j<copiedBytes; j++) {
            rs232_putch(*ptrTmp);
            ptrTmp++;
        }
        rs232_putString("\n\r\n\r");
        #endif

        

        return copiedBytes;
    }

    while (digitsToTX>0) {

        if (highCW) {
            frameNumber--;            

            if (frameNumber==-1) {
                batchCounter++;
                frameNumber=7;
                // Siguiente Batch
                if (copiedBytes==0) {
                    // No hemos ejecutado antes un dumpBatch. Tenemos que pasarle la direccion del buffer
                    copiedBytes = pocsag_dumpBatch2Buffer(batchBuffer, &batchTmp);
                } else
                    copiedBytes = pocsag_dumpBatch2Buffer(NULL, &batchTmp);

                pocsag_initBatch(&batchTmp);
            }
            
            cw = &(batchTmp.frames[frameNumber].codeword_H);
        } else {
            if (frameNumber== 0 && batchCounter==NUM_BATCHES) {
                //El estandar pocsag dice que al menos hemos de dejar un codeword a idle tras la transmision del mensaje
                // Comprobamos si estamos en el ultimo codeword del ultimo batch y si es asi, lo dejamos a idle y terminamos
                // la transmision
                break;
            }
            cw = &(batchTmp.frames[frameNumber].codeword_L);
        }
        
        data += processedDigits;

        if (function==FUNCTION_NUMERIC)
            processedDigits = pocsag_formatNumericMessageCodeWord(cw, data); // Formateo de los 5 primeros digitos
        else
            processedDigits = pocsag_formatAlphaMessageCodeWord(cw, data, FALSE); // Formateo de los 2.5 primeros digitos

        digitsToTX -= processedDigits;

        highCW = !highCW;
        
    }


    
        if (copiedBytes==0) {
            // No hemos ejecutado antes un dumpBatch. Tenemos que pasarle la direccion del buffer
            copiedBytes = pocsag_dumpBatch2Buffer(batchBuffer, &batchTmp);
        } else
            copiedBytes = pocsag_dumpBatch2Buffer(NULL, &batchTmp);

    #ifdef DEBUG_RS232
    ptrTmp = batchBuffer;
    for (j=0; j<copiedBytes; j++) {
        rs232_putch(*ptrTmp);
        ptrTmp++;
    }
    rs232_putString("\n\r\n\r");
    #endif

    return copiedBytes;

}



// Si detecta el fin de cadena, hace un padding de espacios

void pocsag_encodeNumericMessage(char *number) {

    BYTE i = 0;    

    for (; i < 5; i++) {

        if (number[i] == 0)
            break;


        if ((number[i] >= '0') && (number[i] <= '9'))
            number[i] -= '0';
        else if (number[i] == 'U')
            number[i] = 0x0B;
        else if (number[i] == ' ')
            number[i] = 0x0C;
        else if (number[i] == '-')
            number[i] = 0x0D;
        else if (number[i] == ')')
            number[i] = 0x0E;
        else if (number[i] == '(')
            number[i] = 0x0F;
        else
            number[i] = 0x0C;

    }

    // Bucle que rellena el resto de los 5 caracteres del CodeWord con espacios.

    for (; i < 5; i++)
        number[i] = 0x0C;

    // Hay que hacer un mirror los nibbles por que se empaquetan como LSB en vez de MSB.
    // Tendria que mejorar esto un poco y optimizarlo
    for (i=0; i<5;i++) {
        number[i] = ((number[i] & 1) <<3) | ((number[i] & 2) <<1) | ((number[i] & 4) >>1) | ((number[i] & 8)>>3);
    }

}



// Rutina basada en: http://people.kth.se/~e92_spe/projects/pocsag.html

void pocsag_calculate_CRC(CodeWord_t *codeWord) {

    UINT32 cw = (*codeWord).dword;
    UINT32 cwTmp = cw;

    UINT8 i;
    UINT8 parity = 0;


    // No me gusta un pelo como se genera el CRC. Mejorarlo
    for (i = 0; i < 21; i++, cw <<= 1)
        if (cw & 0x80000000) cw ^= 0xED200000;

    cwTmp |= (cw >> 21);

    cw = cwTmp; /* parity */
    for (i = 0; i < 32; i++, cwTmp <<= 1)
        if (cwTmp & 0x80000000) parity++;

    if (parity % 2)
        cw++;

    (*codeWord).dword = cw;


}


// Copia el batch indicado al buffer de bytes.
// Si el puntero buffer es igual a NULL, copia el batch al continuacion de donde
// se copio el ultimo batch.
// Devuelve el numero de bytes que ha copiado ya.
UINT16 pocsag_dumpBatch2Buffer(BYTE *buffer, Batch_t *batch) {



    UINT8 i;
    static UINT16 copiedBytes;
    static BYTE *ptrTarget;
    BYTE *ptrSrc = &(batch->bytes[67]);

    if (buffer != NULL) {
        copiedBytes = 0;
        ptrTarget = buffer;
    }

    for (i=0;i<68; i++) {

        *ptrTarget = *ptrSrc;
        ptrTarget++;
        ptrSrc--;
        copiedBytes++;

    }

    return copiedBytes;

}