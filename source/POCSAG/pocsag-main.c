#include <stdio.h>
#include "pocsag.h"

void main(void)
{
  Batch* batch_top = NULL;
  Batch* batch_now = NULL;
  int j = 0, i = 0;
  char tone = 2;
  char text[] = { 2, 3, 5, 10, 3, 5/*, 7, 9, 12, 14, 10, 4, 2, 3, 5, 2, 8, 6, \
		    5, 3, 5, 8, 9, 0, 7, 5, 5, 9, 7, 9, 3, 4, 2, 5, 6, 8, 9, \
		      5, 3, 0, 6, 4, 2, 4, 6, 7, 4, 8, 9, 5, 6, 3, 10, 15, \
			12, 11, 4, 5, 6, 2, 6, 6, 5, 5, 4, 5, 8, 4, 2, 8, 6,\
			  3, 10, 12, 13, 11, 10, 9, 0 */};
  CodeWord packed_text[100];

#define TEXT_SIZE sizeof(text)/sizeof(text[0])
#define MY_RIC 1123600
#define MAGNUSO_RIC 175170
#define SUPER_RIC 255632

  batch_top = make_fill_batch(SUPER_RIC, &tone, TEXT_SIZE, Tone);
  batch_now = batch_top;
  while (batch_now) {
    printf("SYNCH :  0x%0x\n", batch_now->sc);
    for(j = 0; j < 8; j++){
      printf("Frame %d :", j);
      for(i = 0; i < 2; i++)
	printf("\t0x%0x  ",batch_now->frame[j][i]);
      printf("\n");
    }
    batch_now = batch_now->next;
  }
  kill_batch(batch_top);
}
