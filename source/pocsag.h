#ifndef POCSAG_H
#define POCSAG_H

#include "GenericTypeDefs.h"

#define FUNCTION_NUMERIC 0
#define FUNCTION_ALPHA 3

// Formato de un CodeWord de direccion:
// |31 | 30          13 | 12     11 | 10       1 |    0    |
// |-------------------------------------------------------|
// | 0 |     address    |  function |     CRC    |  parity |
// |-------------------------------------------------------|

// Formato de un CodeWord de mensaje:
// |31 | 30                      11 | 10       1 |    0    |
// |-------------------------------------------------------|
// | 1 |           mensaje          |     CRC    |  parity |
// |-------------------------------------------------------|

// Los codewords se transmiten MSB-first.

#define NUM_BATCHES			2		// Numero de batches que podemos transmitir
#define MAX_MESSAGE_FRAMES	((NUM_BATCHES*2)-2) // Num. maximo de frames de mensaje que podemos enviar 
#define MAX_CHARS_NUMERIC_MSG	(MAX_MESSAGE_FRAMES*5) // Num. maximo de caracteres numericos que podemos mandar
#define SIZE_BATCH_BUFFER	NUM_BATCHES*17*4	// Numero de bytes que ocupa el buffer donde almacenamos los batches
#define SIZE_BATCH              NUM_BATCHES*17*4	// Numero de bytes que ocupa el buffer donde almacenamos los batches

#define PREAMBLE_LENGTH (576)+2+150

#define SYNCH_CODEWORD 0x7CD215D8
#define IDLE_CODEWORD  0x7A89C197   //Segun internet
//define IDLE_CODEWORD  0x7AC9C197  // Segun la itu

typedef union {
    UINT32 dword;

    UINT8 bytes[4];

    struct {
        unsigned parity : 1;
        unsigned checkBits_L : 7;

        unsigned checkBits_H : 3;
        unsigned function : 2;
        unsigned address_L : 3;

        unsigned address_H : 8;

        unsigned address_HH : 7;
        unsigned messageType : 1;
    };

    struct {
        unsigned parity : 1;
        unsigned checkBits_L : 7;

        unsigned checkBits_H : 3;
        unsigned message_L : 5;

        unsigned message_H : 8;

        unsigned message_HH : 7;
        unsigned messageType : 1;
    };

} CodeWord;

typedef struct {
    CodeWord codeword_L;
    CodeWord codeword_H;
} Frame_t;

/*typedef struct {
    Frame_t frames[8];
    CodeWord sc;
} Batch_t;*/

typedef union {

    BYTE bytes[68];

    struct {
        Frame_t frames[8];
        CodeWord sc;
    };

} Batch_t;


typedef enum MsgType {
    Tone, Numerical, Alpha
} MsgType;



extern BYTE batchBuffer[NUM_BATCHES*sizeof(Batch_t)];

void pocsag_initBatch(Batch_t *);
void pocsag_copyFrame2Batch(Frame_t *frame, Batch_t *batch);
void pocsag_copyCWs2Batch(CodeWord *codeword_H, CodeWord *codeword_L, Batch_t *batch);
void pocsag_initBatchBuffer(void);
void formatAddressFrame(Batch_t *, UINT32, UINT8);
void pocsag_formatNumericMessageCodeWord(CodeWord *, char *);
UINT16 pocsag_createAlphaMsg(UINT32, char *);
UINT16 pocsag_createNumericMsg(UINT32 address, char *number,BOOL massSend);
UINT16 pocsag_createIdleMsg(void);
void pocsag_calculate_CRC(CodeWord *);
void pocsag_encodeNumericMessage(char *);
void pocsag_dumpBatch2Buffer(void);
void pocsag_printBatchBuffer(void);

#endif 
