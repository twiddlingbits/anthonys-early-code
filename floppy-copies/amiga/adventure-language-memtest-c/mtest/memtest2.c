/* test 1st 10 bytes of external RAM */

#include "stdio.h"

#define PAT2 ((64+16+4+1)<<8)
#define PAT1 (128+32+8+2)

main()
{
register int i;
int k,numbytes;
short int q;
unsigned short *memptr;

memptr=2097152;

printf("enter number of bytes to test: ");
scanf("%d",&numbytes);

printf("\n\nStarting WORD Memory Test...\n");
k=0;
for (i=0;i<numbytes;i++)
   *(memptr+i)=(PAT1|PAT2);

for (i=0;i<100000;i++) ;
printf("checking the pattern...\n");

for (i=0;i<numbytes;i++)
   if ((q=*(memptr+i))!=(PAT1|PAT2)) {
      k++;
      printf("Word %d test's bad! Read: %x was %x\n",i,q,PAT1|PAT2);
      }

printf("\nTested %d Bytes.\n",numbytes);
printf("%d Bad bytes total.\n\n",k);
}

