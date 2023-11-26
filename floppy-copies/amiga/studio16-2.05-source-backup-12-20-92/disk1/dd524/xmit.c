#include "exec/types.h"
#include "exec/exec.h"
#include "libraries/configvars.h"
#include "libraries/dos.h"
#include <hardware/intbits.h>
#include "/include/studio16.h"

/**************************************************************/

short *GetDD524Base();
long DD524IntHandler();
void WaitAllReplys();
void UnInitDD524();
struct BlockMsg *SendBlock();
void RemInt();
struct SampleFilePointer *SampleOpenInDefPath();

/**************************************************************/

#define DD524_FIFO			0/2		/** /2 because short * **/
#define DD524_STATUS 		2/2
#define DD524_CONTROL		2/2
#define DD524_CS_BITS		4/2
#define DD524_INT_ENABLE	6/2

#define CONTROL_DEFAULT		0x66	/*e6=48*/	/* 44K samp rate, SEL=1, int 6 **/
#define CS_BITS_DEFAULT		0xf5		/* consumer, 48K, copy ok, */

/**************************************************************/

#define XMIT_READ_SIZE  1024*1024

/**************************************************************/

short *dd524_base=0;
struct MsgPort *xmit_block_port;
struct MsgPort *xmit_reply_port;
struct Interrupt *dd524_int;
char *buffer;
long int_index=0;
long half_count=0;

/**************************************************************/

extern struct StudioBase *StudioBase;

/**************************************************************/

StartExport()
{
long i;
long readlen;
struct SampleFilePointer *sfp;

int_index=0;
buffer=(char *)AllocMem(XMIT_READ_SIZE, 0);
if (buffer==0) {
	telluser("Export:","Not enough memory for buffer",0);
	return(FALSE);
	}

sfp=SampleOpenInDefPath("CopyBuffer",SP_MODE_OLDFILE,0);
if (sfp==0) {
	telluser("Export:","Can't open 'CopyBuffer'",0);
	FreeMem(buffer,XMIT_READ_SIZE);
	return(FALSE);
	}

setmem(buffer, XMIT_READ_SIZE, 0);
readlen=SampleRead(sfp,buffer,XMIT_READ_SIZE/2);
SampleClose(sfp);

kprintf("read %04x%04x\n",readlen);

InitDD524();

while (int_index < XMIT_READ_SIZE) {
	i=half_count;
	if (i > 2)
		kprintf("index %04x%04x fudge %04x%04x\n",int_index,half_count);
	}
	
UnInitDD524();
FreeMem(buffer,XMIT_READ_SIZE);
return(TRUE);
}

/**************************************************************/

struct ExpansionBase *ExpansionBase=NULL;

short *GetDD524Base()
{
struct ConfigDev *ConfigDev=NULL;
struct ConfigDev *FindConfigDev();
short *base;

if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",0))==0) {
	kprintf("Can't Open expansion.library\n");
	exit(10);
	}
ConfigDev = FindConfigDev(0, 2127, 3);
if (ConfigDev) {
	base=(short *)ConfigDev->cd_BoardAddr;
	}
else {
	CloseLibrary(ExpansionBase);
	kprintf("Can't find DD524 card.\n");
	exit(10);
	}
CloseLibrary(ExpansionBase);
return(base);
}

/**********************************************************/

int InitDD524()
{
short i;

if (dd524_base==0)
	dd524_base=GetDD524Base();

/** Setup DD524 registers default state (consumer mode) **/

dd524_base[DD524_CONTROL]=CONTROL_DEFAULT;
dd524_base[DD524_CS_BITS]=CS_BITS_DEFAULT;

/*** Add New Interupt to handle FIFO half empty interupt ****/

if (AddMyInt()==0) {
	return(1);
	}

/*** Turn on FIFO interupts ***/

for (i=0; i < 512; i++)	{ /** fill fifo to get ints going **/
	dd524_base[DD524_FIFO]=0x80;
	dd524_base[DD524_FIFO]=0x0;
	dd524_base[DD524_FIFO]=0x0;
	dd524_base[DD524_FIFO]=0x0;
	}

i=dd524_base[DD524_STATUS];	/** clear any old ints pending **/
dd524_base[DD524_INT_ENABLE]=1;
dd524_base[DD524_CONTROL]=(CONTROL_DEFAULT-2);	/** Turn on fifo **/ 

return(0);
}

/**************************************************************/

void UnInitDD524()
{
dd524_base[DD524_INT_ENABLE]=0;		/** int's off **/
RemInt();
}

/**************************************************************/

AddMyInt()
{
extern struct Interrupt *MakeInt();

dd524_int = MakeInt("DD524",0,DD524IntHandler,0);
if (dd524_int==0) {
	return(0);
	}
AddIntServer(INTB_EXTER, dd524_int);
return(1);
}

/*****************************************************************/

void RemInt()
{
void FreeInt();

if (dd524_int) {
	RemIntServer(INTB_EXTER,dd524_int);
	FreeInt(dd524_int);
	}
}

/*****************************************************************/

struct Interupt *MakeInt(name,pri,code,data)
char *name;
int pri;
void (*code)();
APTR data;
{
struct Interrupt *intr;

intr=(struct Interrupt *)AllocMem(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
if (intr==0) return(0);

intr->is_Node.ln_Pri=pri;
intr->is_Node.ln_Type=NT_INTERRUPT;
intr->is_Node.ln_Name=name;
intr->is_Data = data;
intr->is_Code = code;

return(intr);
}

/*****************************************************************/

void FreeInt(intr)
struct Interrupt *intr;
{
if (intr==0) return;
if (intr->is_Node.ln_Type != NT_INTERRUPT) return;
intr->is_Node.ln_Type=0;
FreeMem(intr,sizeof(struct Interrupt));
}

/**************************************************************/

struct SampleFilePointer *SampleOpenInDefPath(samp_name,mode,ff)
char *samp_name;
long mode;
APTR ff;
{
char *str;
struct SampleFilePointer *sfp;

str=(char *)AllocCompletePath(StudioBase->defaults.samp_paths, samp_name);
sfp=(struct SampleFilePointer *)SampleOpen(str,mode,ff);
FreeMem(str,strlen(str)+1);
return(sfp);
}

/**************************************************************/
