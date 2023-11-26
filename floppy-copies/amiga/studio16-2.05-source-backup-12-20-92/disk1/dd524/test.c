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

/** Find base address of DD524 ***/

base=GetDD524Base();
printf("Found DD524 at %x\n",base);

/** Setup default state (consumer mode) **/

base[DD524_CONTROL]=CONTROL_DEFAULT;
base[DD524_CS_BITS]=CS_BITS_DEFAULT;

base[DD524_CONTROL]=CONTROL_DEFAULT;	/** disable fifos **/

for (i=0; i < 256; i++) {		/** Fill FIFO half full **/
	base[DD524_FIFO]=0x11;
	base[DD524_FIFO]=0x88;
	base[DD524_FIFO]=0xaa;
	base[DD524_FIFO]=0xff;
	}

base[DD524_CONTROL]=(CONTROL_DEFAULT-6);	/** Enable xmit & receive FIFO **/

/** First Read 100 bytes from receiver **/

for (i=0; i < 256; i++) {
	while (base[DD524_STATUS]&2==0) ;/** Receive FIFO Empty? **/
	printf("byte %x is %x (cs bits are %x)\n",i,base[DD524_FIFO],base[DD524_CS_BITS]);
	}

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

