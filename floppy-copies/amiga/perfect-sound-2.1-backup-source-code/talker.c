#include <stdio.h>


/****************************/
/**                        **/
/**       TALKER           **/
/**                        **/
/****************************/

#define SAMPSIZE 1024*50

main()

{

FILE *fp;
char *base;
unsigned short *aud0lch,*aud0len,*aud0vol,*aud0per,*aud0dat,*dmaconw;
unsigned short word,*intreq,*intreqr;
static int delay,maxval,minval,k;
static unsigned short i;
register unsigned short sam;
static unsigned char *icr,*data,*direction,sample[SAMPSIZE];
static char choice;
unsigned int *aud0lc,val;
static unsigned char *data2,*direction2;

base=0xDEF000;
dmaconw=(unsigned short *)(base+0x096);
aud0lch=(unsigned short *)(base+0x0A0);
aud0len=(unsigned short *)(base+0x0A4);
aud0vol=(unsigned short *)(base+0x0A8);
aud0per=(unsigned short *)(base+0x0A6);
aud0dat=(unsigned short *)(base+0x0AA);
aud0lc=(int *)aud0lch;
intreq=(unsigned short *)(base+0x09C);
intreqr=(unsigned short *)(base+0x1E);

direction=0xBFE301;     /* set bits here to a 1 for output */
data=0xBFE101;
icr=0xBFED01;
data2=0xBFD000;
direction2=0xBFD200;

*direction=0;           /* all bits are read */
*data=0;
*direction2=2+4;      /* pa1 & pa2 write */
*data2=2;            /* pa2 -- select right channel */

/* put up menu */

top:
printf("\n\n\nThe Incredible Digitizer Program\n\n");
printf("You have the following Choices, so choose carefully:\n\n\n");
printf("1. Display max/min values\n2. digitize a sample\n\n");

choice=getchar();

if (choice == '1') {
   for(delay=0;delay<40;delay++) {
   maxval=0;
   minval=999;
   for (k=0;k<30;k++) {
   if ((i=*data) < minval) minval=i;
   if ((i=*data) > maxval) maxval=i;
   printf("max= %d ; min= %d\n",maxval,minval);
   }}
   exit(0);
}
if (choice == '2') {
   printf("Enter a delay factor: ");
   scanf("%d",&delay);
   printf("\n\nStart talking Monkey boy...\n\n");
   Disable();
   for (k=0;k<SAMPSIZE;k++) {
   for(i=0;i<delay;i++);
   sample[k]=(*data)-128;
   }
   Enable();
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

if (choice =='3') {
   printf("enter period: ");
   scanf("%d",&val);
   printf("enter loop delay: ");
   scanf("%d",&delay);
   Disable();
/*   *aud0lc=&sample[0];
   *aud0len=SAMPSIZE/2; */
   *aud0vol=64;
   *aud0per=1;
/*   *dmaconw=(0x8000+1);  */     /*start dma conversion */
while (1==1) {

for (i=0;i<delay;i++) ;
i=0;
sam=(*data-128);
sam=sam&0xFF;
i=sam|(sam<<8);
*aud0dat=i;
*intreq=128;
   }
printf("%x %x",sam,i);
}
goto top;
}

