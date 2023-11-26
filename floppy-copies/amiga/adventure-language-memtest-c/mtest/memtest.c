
/* test 1st 10 bytes of external RAM */

#include "stdio.h"

#define NUMBYTES 2*1024
#define PAT2 (64+16+4+1)
#define PAT1 (128+32+8+2)

main()
{
register int i;
int k,j;
unsigned char *memptr;

memptr=2097152;

printf("\nStarting Memory Test...\n");
k=0;

for(j=0;j<10;j++) {
   for (i=0;i<NUMBYTES;i++)
      *(memptr+i)=PAT1;
   for (i=0;i<NUMBYTES;i++) 
       if (*(memptr+i)!=PAT1)
         printf("Byte %d test's bad! Read: %x was %x\n",i,*(memptr+i),PAT1);

   for (i=0;i<NUMBYTES;i++)
       *(memptr+i)=PAT2;
    for (i=0;i<NUMBYTES;i++) 
      if (*(memptr+i)!=PAT2) {
         printf("Byte %d test's bad! Read: %x was %x\n",i,*(memptr+i),PAT2);
         k++;
         }
   printf("Tested pass %d\n",j);
   
   }

   printf("\nTested %d Bytes.\n",NUMBYTES);
   printf("%d Bad bytes total.\n\n",k);
   }

