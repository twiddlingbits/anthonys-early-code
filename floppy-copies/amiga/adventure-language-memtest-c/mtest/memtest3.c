/* test 1st 10 bytes of external RAM */

#include "stdio.h"


main()
{
register int i;
int k,off;
short int j,q;
unsigned short *memptr;

memptr=2097152;

printf("Enter byte to test: ");
scanf("%d",&off);

printf("\n\nStarting Single WORD Memory Test...\n");

   for (j=0;j<256;j++) {
      printf("Testing with %d\n",j);
      k=0;
      *(memptr+off)=j;
      for (i=0;i<100000;i++)
         if ((q=*(memptr+off))!=j) {
            if (k<10) printf("BAD, was %x is now %x\n",j,q);
            k++;
            }
      }
}

