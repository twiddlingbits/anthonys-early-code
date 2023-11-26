#include "exec/types.h"
#include "exec/exec.h"
#include "libraries/configvars.h"

/**************************************************************/

short *GetDD524Base();

/**************************************************************/

#define DD524_FIFO			0/2		/** /2 because short * **/
#define DD524_STATUS 		2/2
#define DD524_CONTROL		2/2
#define DD524_CS_BITS		4/2
#define DD524_INT_ENABLE	6/2

#define CONTROL_DEFAULT		0xa6		/* 48K samp rate, SEL=1 **/
#define CS_BITS_DEFAULT		0xf5		/* consumer, 48K, copy ok, */

/**************************************************************/

static short itab[512];

main()
{
short *base;
short msb,lsb,ival;
int i,k,j;
double pi=3.14159265359;
double freq=1500.0;
double rate=48000;
double sin(double);

/** Find base address of DD524 ***/

base=GetDD524Base();
printf("Found DD524 at %x\n",base);

/** Setup default state (consumer mode) **/

base[DD524_CONTROL]=CONTROL_DEFAULT;
base[DD524_CS_BITS]=CS_BITS_DEFAULT;

/** Send a sine wave to the transmitter **/

base[DD524_CONTROL]=CONTROL_DEFAULT;	/** disable fifos **/

Disable();

for (i=0; i < 32; i++) {		/** Fill FIFO half full **/
	ival=(short)(32767.0*sin((double)((double)i*2.0*pi*freq/(double)rate)));
	itab[i]=ival;
	msb=(ival>>8)&0xFF;
	lsb=(ival)&0xFF;
	base[DD524_FIFO]=msb;
	base[DD524_FIFO]=lsb;
	}

for (i=0; i < 100; i++) {
Disable();
base[DD524_CONTROL]=(CONTROL_DEFAULT-2);	/** Enable xmit FIFO **/

while (base[DD524_STATUS]&16) ;     /** Not empty? **/
base[DD524_CONTROL]=(CONTROL_DEFAULT);	/** Disable xmit FIFO **/
for (k=0; k < 1024; k++) {
	if((base[DD524_STATUS]&32)==0) {
		Enable();
		printf("Error 1 (fifo prematurly full!)!\n");
		exit(10);
		}
	base[DD524_FIFO]=0xAA;
	msb=base[DD524_STATUS];
	base[DD524_FIFO]=0x33;
	}

Enable();

if((base[DD524_STATUS]&32)==0)
	printf("OK! FIFO full at correct spot\n");
else
	printf("FIFO not full but should be!\n");
}

exit(0);
}

/**************************************************************/

struct ExpansionBase *ExpansionBase=NULL;

short *GetDD524Base()
{
struct ConfigDev *ConfigDev=NULL;
struct ConfigDev *FindConfigDev();
short *base;

if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",0))==0) {
	printf("Can't Open expansion.library\n");
	exit(10);
	}
ConfigDev = FindConfigDev(0, 2127, 3);
if (ConfigDev) {
	base=(short *)ConfigDev->cd_BoardAddr;
	}
else {
	CloseLibrary(ExpansionBase);
	printf("Can't find DD524 card.\n");
	exit(10);
	}
CloseLibrary(ExpansionBase);
return(base);
}

