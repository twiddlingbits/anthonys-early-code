#include <exec/types.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <hardware/intbits.h>
#include <stdio.h>
#include "/include/studio16.h"

#define STOP	1
#define PLAY	2
#define FF	3
#define REW	4

int resettc;
extern struct StudioBase *StudioBase;
extern struct SmpteSource *SMPTEsrc;
void __asm InjectTimeCode(register __d1 struct SmpteSource *,register __d2 int);

byte mode=1;

/* Local defines **/

short SmpteOn;								/* Broadcast smpte events? */
struct Task *tc_handler;				/* Handler task pointer */
unsigned long smpte_sigmask;			/* Signal from smpte timer */
struct Interrupt *smpte_int;			/* VertBlank Smpte timer */
int internal_smpte=NULL;		/* updated by inter. */
int  start_smpte=0;			/* used by play buttns, etc. */
int	smpte_inc_value=0;				/* added each frame */
int tc1=5;

extern struct AudioHandler *hand;


void Setup();
void RemInt();
struct Interupt *MakeInt();
void FreeInt();
void CloseDown();

int InitSmpteGen();
int FreeSmpteGen();
void PlayFunc(struct button *);
void StopFunc(struct button *);
void FastForwardFunc(struct button *);
void RewindFunc(struct button *);
void ResetFunc();

/***********************************************************/

void PlayFunc(b)
struct button *b;
{

Disable();
smpte_inc_value=1;
mode=PLAY;
if (!SmpteOn) {
	SmpteOn=TRUE;
	internal_smpte=start_smpte;
	}
Enable();
}

/***********************************************************/

void StopFunc(b)
struct button *b;
{
if (mode==REW || mode==FF) InjectTimeCode(SMPTEsrc,internal_smpte);
mode=STOP;
if (SmpteOn) {
	SmpteOn=FALSE;
	Disable();
	smpte_inc_value=0;
	start_smpte=internal_smpte;
	Enable();
	}
}

/***********************************************************/

void FastForwardFunc(b)
struct button *b;
{

Disable();

if (!SmpteOn) {
	SmpteOn=TRUE;
	internal_smpte=start_smpte;
	}
if (mode!=FF) {mode=FF;smpte_inc_value=5;}
else smpte_inc_value=AddTimeCode(smpte_inc_value,smpte_inc_value,StudioBase->defaults.smpte_mode);

Enable();
}

/***********************************************************/

void RewindFunc(b)
struct button *b;
{
Disable();
if (!SmpteOn) {
	SmpteOn=TRUE;
	internal_smpte=start_smpte;
	}

if (mode!=REW) {mode=REW;smpte_inc_value=SubTimeCode(0L,1L,StudioBase->defaults.smpte_mode);}
else if (smpte_inc_value==SubTimeCode(0L,1L,StudioBase->defaults.smpte_mode)) smpte_inc_value=SubTimeCode(0L,5L,StudioBase->defaults.smpte_mode);
else smpte_inc_value=AddTimeCode(smpte_inc_value,smpte_inc_value,StudioBase->defaults.smpte_mode);

Enable();
}

/***********************************************************/

void ResetFunc()
{
mode=STOP;
SmpteOn=FALSE;
Disable();
smpte_inc_value=0;
start_smpte=resettc;
internal_smpte=resettc;

Enable();
DisplaySmpte(&start_smpte);
InjectTimeCode(SMPTEsrc,start_smpte);
}
/***********************************************************/

void SetFunc()
{
char oset[21];

mode=STOP;
SmpteOn=FALSE;
oset[0]=0;
newgetstr(oset,"Reset TimeCode?",8*20);
resettc=TCstr2int(oset);

Disable();
smpte_inc_value=0;
start_smpte=resettc;
internal_smpte=resettc;

Enable();
DisplaySmpte(&start_smpte);
InjectTimeCode(SMPTEsrc,start_smpte);
}

/**********************************************************/

int FreeSmpteGen()
{
RemInt();
return(NO_ERROR);
}

/**********************************************************/

int InitSmpteGen()
{
int j;

/*** fill in variables used by vertical blanking interupts **/

tc_handler=(struct Task *)FindTask(0);

if ((j=AllocSignal(-1))==-1)  {		/**!!!! I NEVER FREE THESE GUYS!! **/
	return(OUT_OF_MEMORY);
	}

smpte_sigmask = 1L<<j;

if (AddMyInt()==0) {
	return(OUT_OF_MEMORY);
	}

SmpteOn=FALSE;
return(NO_ERROR);
}

/**************************************************************/

AddMyInt()
{
extern void SmpteTimer();
extern struct Interrupt *MakeInt();

smpte_int = MakeInt("SmpteGenerator",0,SmpteTimer,&internal_smpte);
if (smpte_int==0) {
	return(0);
	}
AddIntServer(INTB_VERTB, smpte_int); 
return(1);
}

/*****************************************************************/

void RemInt()
{
void FreeInt();

if (smpte_int) {
	RemIntServer(INTB_VERTB,smpte_int);
	FreeInt(smpte_int);
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
