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
int i,k;
double pi=3.14159265359;
double freq=375.0;
double rate=48000;
double sin(double);

/** Find base address of DD524 ***/

base=GetDD524Base();
printf("Found DD524 at %x\n",base);

/** Setup default state (consumer mode) **/

base[DD524_CONTROL]=CONTROL_DEFAULT;
base[DD524_CS_BITS]=CS_BITS_DEFAULT;

#if 0
/** First Read 100 bytes from receiver **/

base[DD524_CONTROL]=CONTROL_DEFAULT&(~4);	/** Enable receive FIFO **/
for (i=0; i < 20; i++) {
	if (base[DD524_STATUS]&1==0) /** Receive FIFO Empty? **/
		Delay(5);
	printf("byte %x is %x (cs bits are %x)\n",i,base[DD524_FIFO],base[DD524_CS_BITS]);
	}

#endif

/** Send a sine wave to the transmitter **/

base[DD524_CONTROL]=CONTROL_DEFAULT;	/** disable fifos **/

Disable();

for (i=0; i < 256; i++) {		/** Fill FIFO half full **/
	ival=(short)(32767.0*sin((double)((double)i*2.0*pi*freq/(double)rate)));
#if 0
	ival=(short)(400.0*sin((double)((double)i*2.0*pi*freq/(double)rate)));
	ival=ival/2;
	ival=ival+400;
	if (ival<0)
		ival=0;
	if (i < 128)
		ival=i*128;
	else
		ival=i*(256-i);
	if (i < 128)
		ival=30000;
	else
		ival=0;
	ival=30000;
#endif
	itab[i]=ival;
	msb=(ival>>8)&0xFF;
	lsb=(ival)&0xFF;
	base[DD524_FIFO]=msb;
	base[DD524_FIFO]=0;
	base[DD524_FIFO]=msb;
	base[DD524_FIFO]=0;
#if 0
	base[DD524_FIFO]=0;			/** fill xmit fifo with pattern **/
	base[DD524_FIFO]=0;
	base[DD524_FIFO]=msb;			/** fill xmit fifo with pattern **/
	base[DD524_FIFO]=lsb;
#endif
	}

#if 0
base[DD524_CONTROL]=(CONTROL_DEFAULT-2);	/** Enable xmit FIFO **/
#endif

#if 0
while(TRUE) {
/*printf("%x\n",base[DD524_STATUS]);*/
	while ((base[DD524_STATUS]&32)==0) ;
	base[DD524_FIFO]=0;
	}
#endif

for (k=0; k < 750; k++) {
	for (i=0; i < 256; i++) {
/*		msb=itab[i]/256;*/
		msb=(itab[i]>>8)&0xFF;
		lsb=(itab[i])&0xFF;
	base[DD524_FIFO]=msb;
	base[DD524_FIFO]=0;
	base[DD524_FIFO]=msb;
	base[DD524_FIFO]=0;
#if 0
		base[DD524_FIFO]=0;			/** fill xmit fifo with pattern **/
		base[DD524_FIFO]=0;
		base[DD524_FIFO]=msb;			/** fill xmit fifo with pattern **/
		base[DD524_FIFO]=lsb;
#endif
		}
	while ((base[DD524_STATUS]&8)==0) ;	/** Xmite FIFO > half full? **/
	}

Enable();
exit(0);


#if 0
/** First Read 100 bytes from receiver **/

base[DD524_CONTROL]=CONTROL_DEFAULT&(~6);	/** Enable both FIFOs **/
for (i=0; i < 100; i++) {
	if (base[DD524_STATUS]&1==0) /** Receive FIFO Empty? **/
		Delay(5);
	printf("byte %x is %x (cs bits are %x)\n",i,base[DD524_FIFO],base[DD524_CS_BITS]);
	}
#endif
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

