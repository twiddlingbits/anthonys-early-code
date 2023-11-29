/* test 1st 10 bytes of external RAM */

#include "stdio.h"

#define NUMBYTES 1024

main()
{
int i;
char *memptr;

memptr=2097152;

printf("Before setting:\n");
for (i=0;i<NUMBYTES;i++)
   printf("%d\n",*(memptr+i));

printf("\nAfter setting:\n");

for (i=0;i<NUMBYTES;i++)
   *(memptr+i)=i;


for (i=0;i<NUMBYTES;i++)
   printf("%d\n",*(memptr+i));
}

