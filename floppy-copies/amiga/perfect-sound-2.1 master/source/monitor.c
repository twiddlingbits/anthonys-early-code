/* (C) 1986 Anthony J. Wood  */
/* For Perfect Sound digitizer */
/* 7-22-86 */



#include <exec/types.h>
#include <exec/exec.h>
#include <hardware/intbits.h>

struct Interrupt *VertB,*old;
short word;

main()
{
extern void listen();
UBYTE *direction,*direction2,*data2;
char *base;
unsigned short *aud0vol,*aud0per,*dmaconw,*intena,*aud0dat;
int period;

base=0xDEF000;
dmaconw=(unsigned short *)(base+0x096);
aud0vol=(unsigned short *)(base+0x0A8);
aud0per=(unsigned short *)(base+0x0A6);
aud0dat=(unsigned short *)(base+0x0AA);
intena=(unsigned short *)(base+0x9A);

printf("Enter a Period: ");
scanf("%d",&period);
printf("\n");

data2=0xBFD000;
direction2=0xBFD200;
*direction2=6;
*data2=2;     /* select channel */
direction=0xBFE301;
*direction=0;
*aud0vol=64;
*aud0per=period;

VertB=AllocMem(30,MEMF_PUBLIC);
if (VertB==0) {
   printf("not enough memory for interupt server.");
   exit (10);
   }

VertB->is_Node.ln_Type=NT_INTERRUPT;
VertB->is_Node.ln_Pri=0;
VertB->is_Node.ln_Name="listener";
VertB->is_Data=&word;    /* not used */
VertB->is_Code=listen;


if ((old=SetIntVector(INTB_AUD0,VertB))!=0) {
   printf("we just replaced the %s interrupt handler",old->is_Node.ln_Name);
   }

*intena=0x8000+128;     /* enable audio interupts */
*aud0dat=0;             /* get interupts started */
xx:
/* printf("%x \n",word); */
goto xx;
}

