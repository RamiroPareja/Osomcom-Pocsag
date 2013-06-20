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

//Batch_t batchBuffer[NUM_BATCHES];
Batch_t batchTmp;
BYTE batchBuffer[NUM_BATCHES*sizeof(Batch_t)];

BYTE *ptrBatchBuffer;

// Funcion que inicializa el batch con todos los frames a Idle

void pocsag_initBatch(Batch_t *batch) {

    BYTE i;

    
    batch->sc.dword = SYNCH_CODEWORD;

    for (i = 0; i < 8; i++) {
        batch->frames[i].codeword_L.dword = IDLE_CODEWORD;
        batch->frames[i].codeword_H.dword = IDLE_CODEWORD;
    }

}

/*
// Funcion que inicializa todo el array de buffers con idles

void pocsag_initBatchBuffer(void) {

    BYTE i;

    for (i = 0; i < NUM_BATCHES; i++) {
        pocsag_initBatch(&batchBuffer[i]);
    }

}*/

// Formatea un frame como direccion

void pocsag_formatAddressCodeWord(CodeWord *cw, UINT32 address, UINT8 function) {


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

void pocsag_formatNumericMessageCodeWord(CodeWord *cw, char *number) {

    cw->dword = 0;

    pocsag_encodeNumericMessage(number);

    cw->messageType = 1; // Tipo Mensaje
    cw->message_HH = (number[0] << 3) | (number[1] >> 1);
    cw->message_H = (number[1] << 7) | (number[2] << 3) | (number[3] >> 1);
    cw->message_L = ((number[3]&0x01) << 4) | (number[4]);

    // Calcular CRC y paridad
    //calculate_CRC(&(batchBuffer->frames[frameNumber].codeword_H));
    pocsag_calculate_CRC(cw);

}


//Crea un batch con todos los Frames a idle. 
//Solo es util para probar si el busca recibe cobertura
UINT16 pocsag_createIdleMsg(void) {

    pocsag_initBatch(&batchTmp);
    ptrBatchBuffer = batchBuffer;
    pocsag_dumpBatch2Buffer();


    return 68;

}



UINT16 pocsag_createAlphaMsg(UINT32 address, char *string) {

    BYTE i;
    INT8 digitsToTX = strlen(string);    
    BYTE frameNumber = 7 - (address & 0x00000007);    
    CodeWord *cw;

    pocsag_initBatch(&batchTmp);
    ptrBatchBuffer = batchBuffer;

    //cw = &(batchTmp.frames[frameNumber].codeword_H);
    //pocsag_formatAddressCodeWord(cw, address, FUNCTION_ALPHA);

      cw = &(batchTmp.frames[0].codeword_H);
      pocsag_formatAddressCodeWord(cw, address, FUNCTION_ALPHA);

      batchTmp.frames[1].codeword_H = batchTmp.frames[0].codeword_H;
      batchTmp.frames[2].codeword_H = batchTmp.frames[0].codeword_H;
      batchTmp.frames[3].codeword_H = batchTmp.frames[0].codeword_H;
      batchTmp.frames[4].codeword_H = batchTmp.frames[0].codeword_H;
      batchTmp.frames[5].codeword_H = batchTmp.frames[0].codeword_H;
      batchTmp.frames[6].codeword_H = batchTmp.frames[0].codeword_H;
      batchTmp.frames[7].codeword_H = batchTmp.frames[0].codeword_H;





    
    //cw = &(batchTmp.frames[frameNumber].codeword_L);
    cw = &(batchTmp.frames[0].codeword_L);
    cw->dword = 0;

    cw->messageType = 1; // Tipo Mensaje

    for (i=0; i<7; i++) {        
        cw->message_HH <<=1;
        
        if (string[0] & 1)
            cw->message_HH |= 1;
        
        string[0] >>=1;        
    }
    
    for (i=0; i<7; i++) {        
        cw->message_H <<=1;
        
        if (string[1] & 1)
            cw->message_H |= 1;
        
        string[1] >>=1;        
    }
    
    cw->message_H <<=1;
    
    cw->message_L = 0;

    pocsag_calculate_CRC(cw);

    batchTmp.frames[1].codeword_L = batchTmp.frames[0].codeword_L;
    batchTmp.frames[2].codeword_L = batchTmp.frames[0].codeword_L;
    batchTmp.frames[3].codeword_L = batchTmp.frames[0].codeword_L;
    batchTmp.frames[4].codeword_L = batchTmp.frames[0].codeword_L;
    batchTmp.frames[5].codeword_L = batchTmp.frames[0].codeword_L;
    batchTmp.frames[6].codeword_L = batchTmp.frames[0].codeword_L;
    batchTmp.frames[7].codeword_L = batchTmp.frames[0].codeword_L;

    pocsag_dumpBatch2Buffer();

    for (i = 1; i < 3; i++) {

        pocsag_initBatch(&batchTmp);


        pocsag_dumpBatch2Buffer();

    }

    return ptrBatchBuffer - batchBuffer;

}




// Crea un mensaje numerico.
// La funcion controla corta el numero si es mayor de lo que se puede enviar.
// TO FIX: Si el numero de caracteres que tiene el mensaje numerico no es
// multiplo de 5, las funciones de formateo de numero desbordaran el puntero.
// No es grave, pues haran un padding de ceros casi seguro, pero no es nada
// elegante.

// Devuelve el numero de bytes que ocupa el mensaje en el buffer

UINT16 pocsag_createNumericMsg(UINT32 address, char *number) {

    BYTE i;
    INT8 digitsToTX = strlen(number);
    //INT8 digitsToTX = 5;
    BYTE frameNumber = 7 - (address & 0x00000007);
    //BYTE frameNumber = address & 0x00000007;    Invertimos el orden en el que
                              // Se rellena el array para poder volcar los bytes
    CodeWord *cw;

    pocsag_initBatch(&batchTmp);
    ptrBatchBuffer = batchBuffer;

    cw = &(batchTmp.frames[frameNumber].codeword_H);
//    cw = &(batchTmp.frames[0].codeword_H);
//    cw = &(batchTmp.frames[1].codeword_H);
//    cw = &(batchTmp.frames[2].codeword_H);
//    cw = &(batchTmp.frames[3].codeword_H);
//    cw = &(batchTmp.frames[4].codeword_H);
//    cw = &(batchTmp.frames[5].codeword_H);
//    cw = &(batchTmp.frames[6].codeword_H);
//    cw = &(batchTmp.frames[7].codeword_H);



    pocsag_formatAddressCodeWord(cw, address, FUNCTION_NUMERIC);


    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[frameNumber].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[0].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[1].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[2].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[3].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[4].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[5].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[6].codeword_L), number); // Formateo de los 5 primeros digitos
//    pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[7].codeword_L), number); // Formateo de los 5 primeros digitos


    digitsToTX -=5;

    pocsag_dumpBatch2Buffer();

    for (i = 1; i < 2; i++) {

        pocsag_initBatch(&batchTmp);

        if (digitsToTX>0) {
            number += 5;
            pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[frameNumber].codeword_H), number);
            digitsToTX -=5;
        }

        if (digitsToTX>0) {
            number += 5;
            pocsag_formatNumericMessageCodeWord(&(batchTmp.frames[frameNumber].codeword_L), number);
            digitsToTX -=5;
        }

    pocsag_dumpBatch2Buffer();

    }

    return ptrBatchBuffer - batchBuffer;

}

// Si detecta el fin de cadena, hace un padding de espacios

void pocsag_encodeNumericMessage(char *number) {

    BYTE i = 0;
    BYTE tmp = 0;

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
        tmp = ((number[i] & 1) <<3) | ((number[i] & 2) <<1) | ((number[i] & 4) >>1) | ((number[i] & 8)>>3);
        number[i] = tmp;
    }

}


//
//Batch *make_fill_batch(CodeWord RIC,
//		       char *data, int size_of_data,
//		       MsgType type)
//{
//  Batch* now = NULL;
//  Batch* top = NULL;
//  int frame_addr = 0;
//  int i,j,k = 0;
//  CodeWord packed_text[100];
//  int packed_length = 0;
//
//  /* Create first batch with address and so on */
//  now = create_batch();
//  top = now;  /* Pointer to first batch in the row */
//
//  /* Fill in addressframe with address and Function Bits */
//  frame_addr = RIC & 7; /* Frameaddress == The three LSBits */
//  now->frame[frame_addr][0] = 0;
//  now->frame[frame_addr][0] = (RIC >> 3) << 13;
//
//  /* Set functionbits according to what type of data */
//  switch (type)
//    {
//    case Tone:    /* Tonecoding just sets functionbits */
//      printf("Tone:\n");
//      now->frame[frame_addr][0] &= 0xFFFFE700;
//      switch(data[0])
//	{
//	case 0:
//	  break;
//	case 1:
//	  now->frame[frame_addr][0] |= 0x00000800;
//	  break;
//	case 2:
//	  now->frame[frame_addr][0] |= 0x00001000;
//	  break;
//	case 3:
//	  now->frame[frame_addr][0] |= 0x00001800;
//	  break;
//	default:
//	  fprintf(stderr, "Illegal Tone requested\n");
//	  exit(1);
//	}
//    case Numerical:
//      /* Don't set any bits */
//      break;
//    case Alpha:
//      now->frame[frame_addr][0] |= 0x00001800;
//      break;
//    default:
//      fprintf(stderr,"Unknown format in make_fill_batch\n");
//      exit(1);
//    }
//
//  /* Calculate checksumm and parity for address CodeWord */
//  now->frame[frame_addr][0] = calc_bch_and_parity( now->frame[frame_addr][0] );
//
//  /* Start filling in data in frames */
//  /* Warning very tricky part with i, j and k */
//  if (type != Tone){  /* But only if it's alpha or numerical */
//    packed_length = pack_string(packed_text, data, size_of_data, type);
//    for(i = 0, k = 1, j = frame_addr; i < packed_length; k=0, j++){
//      for(; k <= 1; k++, i++){
//	if (i == packed_length) break;
//	if (j == 8){             /* If we filled a whole batch, */
//	  j = 0; k = 0;          /* start over with a new batch */
//	  now->next = create_batch();
//	  now = now->next;
//	}
//	now->frame[j][k] = packed_text[i];
//	now->frame[j][k] <<= 11; /* Make room for CRC and parity */
//	now->frame[j][k] |= 0x80000000; /* Set msg CW-bit */
//	now->frame[j][k] = calc_bch_and_parity( now->frame[j][k] );
//      }
//    }
//  }
//  return top;
//}
//
//
///*


// Rutina basada en: http://people.kth.se/~e92_spe/projects/pocsag.html

void pocsag_calculate_CRC(CodeWord *codeWord) {

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
//
///* Returns nuf CodeWord packed down to. Only the 20 LSBits are used in *
// * CodeWord.                                                           */
//int pack_string(CodeWord *packed, char *unpacked, int size_up, MsgType type)
//{
//  int real_size = 0;
//  int upi = 0, pi = 0;
//
//  switch (type)
//    {
//    case Tone:
//      fprintf(stderr, "Severe Error; Trying to pack Tone data\n");
//      exit(1);
//    case Numerical:
//      real_size = ((size_up/5)+1)*5; /* One CodeWord = 5 BCD's */
//      for(upi=0, pi=0; upi<real_size; upi++, pi=(upi%5?pi:pi+1)){
//	if (upi<size_up)
//	  packed[pi] = (packed[pi]<<4) + (unpacked[upi] & 0x0F);
//	else
//	  packed[pi] = (packed[pi]<<4);
//      }
//      return real_size/5;
//    case Alpha:
//      fprintf(stderr, "Boring Error; I'm not intrested to invent an ");
//      fprintf(stderr, "algorithm for pack ascii. Quiting!\n");
//      exit(1);
//      break;
//    default:
//      fprintf(stderr, "Severe Error; Illegal MsgType to pack_string\n");
//      exit(1);
//    }
//  return 0;
//}


void pocsag_dumpBatch2Buffer(void) {

        UINT8 i;
    BYTE *ptr = &(batchTmp.bytes[67]);

    for (i=0;i<68; i++) {

        *ptrBatchBuffer = *ptr;
        ptrBatchBuffer++;
        ptr--;

    }


}

void pocsag_printBatchBuffer(void) {



}