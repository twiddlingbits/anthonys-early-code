/* test 1st 10 bytes of external RAM */

#include "stdio.h"

#define NUMBYTES 64*1024
#define PAT2 (64+16+4+1)
#define PAT1 (128+32+8+2)

main()
{
register int i;
int k;
unsigned short *memptr;

memptr=2097152;

printf("\nStarting Memory Test...\n");
k=0;
for (i=0;i<NUMBYTES;i++)
   *(memptr+i)=(PAT1|PAT2);
for (i=0;i<NUMBYTES;i++)
   if (*(memptr+i)!=(PAT1|PAT2))
      printf("Byte %d test's bad! Read: %x\n",i,*(memptr+i));

printf("\nTested %d Bytes.\n",NUMBYTES);
printf("%d Bad bytes total.\n\n",k);
}

