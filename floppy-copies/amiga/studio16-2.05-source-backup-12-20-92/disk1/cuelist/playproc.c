#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "intuition/intuition.h"
#include "clib/macros.h"
#include "/include/psound.h"
#include "cl.h"
#include "clib/macros.h"
#include "exec/execbase.h"

#define ADD	2
/**************************************************************/
extern long preload_sigbit;
extern struct Task *task;
extern struct ExecBase *SysBase;
extern struct Module *mod;
extern struct StudioBase *StudioBase;
extern struct list *CueL;
/**************************************************************/
extern short se_cmd_pp_sample_dump;
extern short se_cmd_trigger_timecode;
extern short se_info_smpte;
/**************************************************************/
void PlayNow();
void PlayTimeCode();
/**************************************************************/
/**************************************************************/
struct PPStartupMessage
	{
	struct Message msg;
	int type;
	int flags;
	struct list_entry *le;
	};
/**************************************************************/
/**************************************************************/
ULONG Tc2Sec(x)
int x;
{
ULONG y;
y=((x&0xFF000000)>>24)*60;
y=(y+((x&0x00FF0000)>>16))*60;
y=y+((x&0x0000FF00)>>8);
return(y);
}
/**************************************************************/
/**************************************************************/
ULONG Tc2Frames(x)
int x;
{
ULONG y,z;
if (StudioBase->defaults.smpte_mode)
	{
	y=((x&0xFF000000)>>24)*60;
	y=(y+((x&0x00FF0000)>>16))*60;
	y=(y+((x&0x0000FF00)>>8))*StudioBase->defaults.smpte_mode;
	y=y+(x&0x000000FF);
	}
else
	{
	y=((x&0xFF000000)>>24)*60;
	y=(y+((x&0x00FF0000)>>16))*60;
	y=(y+((x&0x0000FF00)>>8))*30;
	y=y+(x&0x000000FF);
	y=y-(((x&0xFF000000)>>24)*108); /* Sub Hours */
	z=((x&0x00FF0000)>>16); /* Sub Mins */
	y=y-(z*2);
	z=(z/10)+1;
	y=y+(z*2);
	}
return(y);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void PLAYproc()
{
long unload_sigbit=NULL;
struct Task *pptask=(struct Task *)FindTask(NULL);
struct MsgPort *PPport;
struct PPStartupMessage *PP_SUM;
struct CueEntry *ce;
struct list_entry *le;
struct Region region;
/**********************************/
int flags;
/**********************************/
/**********************************/
LockMod(mod->state);
/**********************************/
while (0==(PPport=(struct MsgPort *)pptask->tc_UserData)) WaitTOF();
PP_SUM=(struct PPStartupMessage *)GetMsg(PPport);
/**********************************/
le=PP_SUM->le;
ce=(struct CueEntry *)le->user_data;
flags=PP_SUM->flags;
/********** Function is now on its own *****************/ 
if (!(ce->regn.samp_name[0])) {UnlockMod(mod->state);return;}

if ((unload_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant unload get SigBit",NULL);return;}

ce->UnloadSigMask=1<<unload_sigbit;
ce->task=(struct Task *)FindTask(NULL);

pptask->tc_UserData=0;
/*******************************************************/ 
region=ce->regn;          /* Use my OWN copy of region */
/*******************************************************/ 
if (!strnicmp(&"PAN ",ce->Rem,4))
	{
	stcd_i(&ce->Rem[4],&region.parms.pan);
	if (region.parms.pan<0) region.parms.pan=0;
	if (region.parms.pan>200) region.parms.pan=200;
	region.parms.pan=region.parms.pan<<5;
	}
/**************************************************************/
if (flags & PP_TRIGGER_NOW) PlayNow(&region);
/**************************************************************/
else PlayTimeCode(&region,ce,le);
/**************************************************************/
ce->task=NULL;
ce->UnloadSigMask=NULL;
FreeSignal(unload_sigbit);
Signal(task,1<<preload_sigbit);
UnlockMod(mod->state);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
struct PPsinkStruct
	{
	struct Task *task;
	ULONG Trigger_SigMask;
	ULONG Unload_SigMask;
	ULONG GetChan_SigMask;
	ULONG Seek_SigMask;
	ULONG Start;
	ULONG End;
	ULONG Trig;
	ULONG Chan;
	ULONG Cmd;
	ULONG flags;
	ULONG user_data;
	};
/**************************************************************/
/**************************************************************/
void PlayTimeCode(region,ce,le)
struct Region *region;
struct CueEntry *ce;
struct list_entry *le;
{
struct PPsinkStruct PPss;
long trigger_sigbit=NULL;
long getchan_sigbit=NULL;
long seek_sigbit=NULL;
float smpte_mode;
int CurTC,TrigTC,ChanTC;

void SinkFunc();		/* in file SMPTE.asm */
struct NewSmpteSink NewSMPTEsnk = {10,NULL,SinkFunc,NULL};
struct SmpteSink *SMPTEsnk;

ULONG mask,Smask;
ULONG ReRead=0;

struct ChanKey *key;
struct ActiveSample *ps;

int add=ADD;
int flg=0;
ULONG x;
/*-----------------------------------*/

PPss.Unload_SigMask=ce->UnloadSigMask;

if ((trigger_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Trigger_SigMask=1<<trigger_sigbit;

if ((seek_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Seek_SigMask=1<<seek_sigbit;

if ((getchan_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.GetChan_SigMask=1<<getchan_sigbit;

PPss.task=ce->task;
/*-----------------------------------*/
TrigTC=region->parms.starttimecode;
ChanTC=SubTimeCode(TrigTC,1L,StudioBase->defaults.smpte_mode);
CurTC=QuerySmpteTime();
/*-----------------------------------*/
if(CurTC>=ce->end) {return;} /* Sample is already past */
/*-----------------------------------*/
if (CurTC>TrigTC && CurTC<ce->end)  /* Triggering From Middle */
	{
Signal(FindTask(NULL),1<<seek_sigbit);
/*	TrigTC=AddTimeCode(CurTC,2<<8,StudioBase->defaults.smpte_mode);
	if(TrigTC>=ce->end) return;
	ChanTC=SubTimeCode(TrigTC,1L,StudioBase->defaults.smpte_mode);
	if (StudioBase->defaults.smpte_mode) smpte_mode=StudioBase->defaults.smpte_mode;
	else smpte_mode=29.97;
	if (region->parms.smpte_sampling_rate!=0) x=((ULONG)((region->parms.smpte_sampling_rate)*(((float)(Tc2Frames(TrigTC)-Tc2Frames(region->parms.starttimecode)))/smpte_mode)));
	else {x=((ULONG)(((float)region->parms.rate) * (((float)(Tc2Frames(TrigTC)-Tc2Frames(region->parms.starttimecode)))/smpte_mode)));}
	region->start=x+region->start;

	kprintf("%x%x  %x%x\n",x,region->start);*/
	}
/*-----------------------------------*/
if ((ps=(struct ActiveSample *)ASOpen(region, AS_PLAY|AS_AUTO_DATA))==0) {telluser("Play Process:","Can not open disk sample (Low in memory?)",NULL);return;}

PPss.Start=region->parms.starttimecode;
PPss.End=ce->end;
PPss.Chan=ChanTC;
PPss.Trig=TrigTC;

PPss.Cmd=MIN(SubTimeCode(CurTC,0x00000100,StudioBase->defaults.smpte_mode),CurTC);

NewSMPTEsnk.userdata=(APTR)&PPss;
SMPTEsnk=(struct SmpteSink *)AddSmpteSink(&NewSMPTEsnk);
/*-----------------------------------*/
Smask=PPss.Trigger_SigMask|PPss.Unload_SigMask|PPss.Seek_SigMask|PPss.GetChan_SigMask;
/*-----------------------------------*/
while (flg==0)
	{
	mask=Wait(Smask);
/*-----------------------------------*/
	if (mask & PPss.GetChan_SigMask) 
		if (ps->key==0)
			ps->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
/*-----------------------------------*/
	if (mask & PPss.Trigger_SigMask)
		{
		if (ps->key==0)	ps->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
		if (ps->key==0)	telluser("Play Process:","Can't Allocate a play channel!",NULL);
		else	{
			ps->flags |= AS_IS_ACTIVE;
			BroadcastEventParms(se_cmd_trigger_timecode,ps,PPss.Trig,0,0,0,0);
/*			ASTrigger(ps);*/
			highlight_list_entry(CueL,le,1);
			while (ReRead) {ASRead(ps); ReRead--;}
			}
		flg=1;
		mask=0;
		}
/*-----------------------------------*/
	if (mask & PPss.Unload_SigMask) {flg=2;mask=NULL;}
/*-----------------------------------*/
	if (mask & PPss.Seek_SigMask) /* If misses recalled by SigBit */
		{
		CurTC=QuerySmpteTime();
		TrigTC=AddTimeCode(CurTC,(add<<8),StudioBase->defaults.smpte_mode);
		if(TrigTC<ce->end)
			{
			ReRead=ReRead+ASFlush(ps,0);

			if (StudioBase->defaults.smpte_mode) smpte_mode=StudioBase->defaults.smpte_mode;
			else smpte_mode=29.97;
			if (region->parms.smpte_sampling_rate!=0) x=((ULONG)((region->parms.smpte_sampling_rate)*(((float)(Tc2Frames(TrigTC)-Tc2Frames(region->parms.starttimecode)))/smpte_mode)));
			else {x=((ULONG)(((float)region->parms.rate) * (((float)(Tc2Frames(TrigTC)-Tc2Frames(region->parms.starttimecode)))/smpte_mode)));}

			ASSeek(ps,x+region->start, ASS_OFFSET_BEGINNING);
			ASRead(ps);ReRead--;
			CurTC=QuerySmpteTime();

			if(AddTimeCode(CurTC,0x0000000a,StudioBase->defaults.smpte_mode)<TrigTC)
				{
				while(AddTimeCode(QuerySmpteTime(),0x0000010F,StudioBase->defaults.smpte_mode)<TrigTC && ReRead)
					{
					ASRead(ps);
					ReRead--;
					}
				}
				Disable(); /* RECENTLY TAKEN OUT OF LOOP! */
				PPss.Trig=TrigTC;
				PPss.Cmd=MIN(SubTimeCode(CurTC,0x00000a00,StudioBase->defaults.smpte_mode),CurTC);
				SetSignal(0,PPss.Trigger_SigMask);
				SetSignal(0,PPss.Seek_SigMask);
				SetSignal(0,PPss.GetChan_SigMask);
				Enable();
			}
		else {flg=1;}
		}
	}
/*-----------------------------------*/

DeleteSmpteSink(SMPTEsnk);
if (flg!=2)
	while ((ps->flags&AS_IS_ACTIVE) && (!((SetSignal(0,0)) & PPss.Unload_SigMask)))
		{WaitTOF();WaitTOF();}

if ((ps->flags&AS_IS_ACTIVE)) ASStop(ps);

key=ps->key;
ASClose(ps);
if (key) FreeChan(key);
ce->task=NULL;

unhighlight_list_entry(CueL,le);

FreeSignal(trigger_sigbit);
FreeSignal(seek_sigbit);
FreeSignal(getchan_sigbit);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void PlayNow(region)
struct Region *region;
{
struct ActiveSample *ps;
struct ChanKey *key;

key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
if (key==0)
	{
	telluser("Play Process:","Can't Allocate a play channel!",NOTICE_RETURN_NOW);
	return;
	}

if ((ps=(struct ActiveSample *)ASOpen(region, AS_PLAY|AS_AUTO_DATA))==0) {telluser("Play Process:","Can not open disk sample",NULL);if (key) FreeChan(key);return;}
ps->key=key;
ASTrigger(ps);
ASClose(ps);
if (key) FreeChan(key);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
ULONG __aligned my_seg[4];
/**************************************************************/
struct Task *CreatePlayProc(le,flags)
struct list_entry *le;
int flags;
{
struct CueEntry *ce=(struct CueEntry *)le->user_data;
struct MsgPort *gaddfltport;
struct MsgPort *CreatePort(),*CreateProc();
struct Task *pptask;
char procname[100];
struct PPStartupMessage PPSUM;
APTR port;
ULONG code=(ULONG)PLAYproc;

if (!(ce->regn.samp_name[0])) {return(NULL);} /* Sample is Not Loaded */

my_seg[0]=16;
my_seg[1]=0;
my_seg[2]=0x4EF90000|((code>>16)&0xFFFF);
my_seg[3]=0x00000000|((code&0xFFFF)<<16);

if (!(flags & PP_TRIGGER_NOW)) sprintf(procname,"CUE:%s %08x",ce->regn.region_name,ce->regn.parms.starttimecode);
else sprintf(procname,"PLAY:%s",ce->regn.region_name);
if (SysBase->LibNode.lib_Version>=37) CacheClearU();

if((port=(APTR)CreatePort(0,0))==0) {telluser("Play Proc:","Could Not Create MsgPort",0);return(NULL);}

if (0==(gaddfltport=CreateProc(procname,4,((ULONG)(&my_seg[1]))>>2,12000))) {telluser("Play Proc:","Could Not Create Process",0);DeletePort(port);return(NULL);}

pptask=(struct Task *)((int)gaddfltport - sizeof(struct Task));
if (pptask->tc_UserData!=0) {telluser("Play Proc Startup Error: Task.UserData != NULL","Probably caused by a poorly written PD program. (such as JobManager)",NOTICE_RETURN_NOW);pptask->tc_UserData=0;}

PPSUM.le=le;
PPSUM.flags=flags;

PutMsg(port,&PPSUM);
pptask->tc_UserData=port;
while((ULONG)pptask->tc_UserData) WaitTOF();
DeletePort(port);
return(pptask);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
