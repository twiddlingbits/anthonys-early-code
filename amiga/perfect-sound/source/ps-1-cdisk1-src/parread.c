#include <stdio.h>


/****************************/
/**                        **/
/**       TALKER           **/
/**                        **/
/****************************/

#define SAMPSIZE 1024*10

main()

{

FILE *fp;
static int i,maxval,minval,k;
static unsigned char *data,*direction,sample[SAMPSIZE];
static char choice;

direction=0xBFE301;     /* set bits here to a 1 for output */
data=0xBFE101;

*direction=0;           /* all bits are read */
*data=0;

/* put up menu */

top:
printf("\n\n\nThe Incredible Digitizer Program\n\n");
printf("You have the following Choices, so choose carefully:\n\n\n");
printf("1. Display max/min values\n2. digitize a sample\n\n");

choice=getchar();

if (choice == '1') {
   for (k=0;k<30000;k++) {
   printf(" %d\n",*data);
   }
   exit(0);
}
if (choice == '2') {
   printf("\n\nStart talking Monkey boy...\n\n");
   for (k=0;k<SAMPSIZE;k++) {
   sample[k]=(*data)-127;
   for(i=0;i<15;i++);
   }
   printf("\n\nTimes up Monkey Boy, Were going back to the 8th dimension.\n");

   if ((fp=Open("TALK.OUT",1006))==0) {
      printf("Can't open output file.\n");
      exit(20);
   }
   k=SAMPSIZE;
   Write(fp,&k,4);
   Write(fp,sample,SAMPSIZE);
   Close(fp);
   exit(0);
}
goto top;
}


