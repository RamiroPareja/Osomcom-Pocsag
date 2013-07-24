#ifndef POCSAG_H
#define POCSAG_H

#include "GenericTypeDefs.h"

#define FUNCTION_NUMERIC 0
#define FUNCTION_ALPHA 3
#define FUNCTION_IDLE 10

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

#define PREAMBLE_LENGTH 576 + 2

#define SYNCH_CODEWORD 0x7CD215D8
//#define IDLE_CODEWORD  0x7A89C197   //Segun internet
#define IDLE_CODEWORD  0x7AC9C197  // Segun la itu

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
    }; // Estructura de un codeword de tipo address

    struct {
        unsigned parity : 1;
        unsigned checkBits_L : 7;

        unsigned checkBits_H : 3;
        unsigned message_L : 5;

        unsigned message_H : 8;

        unsigned message_HH : 7;
        unsigned messageType : 1;
    }; // Estructura de un codeword de tipo mensaje

} CodeWord_t;

typedef struct {
        CodeWord_t codeword_L;
        CodeWord_t codeword_H;
    } Frame_t;

typedef union {

    BYTE bytes[68];

    struct {
        Frame_t frames[8];
        CodeWord_t sc;
    };

} Batch_t;


typedef enum MsgType {
    Tone, Numerical, Alpha
} MsgType;



extern BYTE batchBuffer[NUM_BATCHES*sizeof(Batch_t)];


UINT16 pocsag_createMsg(UINT32 address, char *data,UINT8 function, BOOL massSend);

#endif 
