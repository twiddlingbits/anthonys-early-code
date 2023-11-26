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
/* Seconds to wait if triggering from middle */
extern struct NewModule ModData;
/**************************************************************/
extern long preload_sigbit;
extern struct Task *task;
extern struct ExecBase *SysBase;
extern short se_info_smpte;
extern short se_cmd_pp_sample_dump;
extern struct Module *mod;
extern struct StudioBase *StudioBase;
extern struct list *CueL;
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
short se_cmd_trigger_timecode;
/**************************************************************/
/**************************************************************/
/**************************************************************/
void PLAYproc()
{
struct ActiveSample *ps;
struct Task *pptask=(struct Task *)FindTask(NULL);
struct MsgPort *PPport;
struct PPStartupMessage *PP_SUM;
struct CueEntry *ce;
struct list_entry *le;
int flg=0,flags;
struct ChanKey *key;
/**********************************/
ULONG mask,Smask,ReRead=0,ReRead2;
int CurTC,TrigTC,ChanTC;
struct PPsinkStruct PPss;
long trigger_sigbit=NULL;
long unload_sigbit=NULL;
long getchan_sigbit=NULL;
long seek_sigbit=NULL;
int add=ADD;
ULONG x;
int pan;
/**********************************/
void SinkFunc();	/* in file SMPTE.asm */
struct NewSmpteSink NewSMPTEsnk = {10,NULL,SinkFunc,NULL};
struct SmpteSink *SMPTEsnk;
/**********************************/
while (0==(PPport=(struct MsgPort *)pptask->tc_UserData)) WaitTOF();
PP_SUM=(struct PPStartupMessage *)GetMsg(PPport);
le=PP_SUM->le;
ce=(struct CueEntry *)le->user_data;

if (!strnicmp(&"PAN",ce->Rem,3)) 
	{
	stcd_i(&ce->Rem[4],&pan);
	if (pan<0) pan=0;
	if (pan>200) pan=200;
	ce->regn.parms.pan=pan<<5;
	}

flags=PP_SUM->flags;

/********** Function is now on its own *****************/ 
if (!(ce->regn.samp_name[0])) {return;}

ce->regn.start=ce->Rstart;
/**************************************************************/
if (flags & PP_TRIGGER_NOW)
/**************************************************************/
	{
	LockMod(mod->state);

	pptask->tc_UserData=0;
	if ((ps=(struct ActiveSample *)ASOpen(&ce->regn, AS_PLAY|AS_AUTO_DATA))==0) {telluser("Play Process:","Can not open disk sample",NULL);UnlockMod(mod->state);return;}

	ps->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
	if (ps->key==0)
		{
		ASClose(ps);
		telluser("Play Process:","Can't Allocate a play channel!",NOTICE_RETURN_NOW);
		UnlockMod(mod->state);
		return;
		}
	key=ps->key;
	ASTrigger(ps);

	ASClose(ps);
	if (key) FreeChan(key);
	UnlockMod(mod->state);
	}
/**************************************************************/
else
/**************************************************************/
{
se_cmd_trigger_timecode	=GetEventID("SE_CMD_TRIGGER_TIMECODE");

if ((trigger_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Trigger_SigMask=1<<trigger_sigbit;

if ((unload_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Unload_SigMask=1<<unload_sigbit;

if ((seek_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Seek_SigMask=1<<seek_sigbit;

if ((getchan_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.GetChan_SigMask=1<<getchan_sigbit;

TrigTC=ce->regn.parms.starttimecode;
ChanTC=SubTimeCode(TrigTC,1L,StudioBase->defaults.smpte_mode);

ce->UnloadSigMask=PPss.Unload_SigMask;
PPss.task=(struct Task *)FindTask(NULL);
ce->task=PPss.task;
pptask->tc_UserData=0;

CurTC=QuerySmpteTime();
if(CurTC>=ce->end) {ce->task=NULL;return;} /* Sample is already past */

if (CurTC>TrigTC && CurTC<ce->end)  /* Triggering From Middle */
	{
	TrigTC=AddTimeCode(CurTC&0xFFFFFF00|ce->regn.parms.starttimecode&0x000000FF,1<<8,StudioBase->defaults.smpte_mode);
	if(TrigTC>=ce->end) {ce->task=NULL;return;}
	ChanTC=SubTimeCode(TrigTC,1L,StudioBase->defaults.smpte_mode);
	if (ce->regn.parms.smpte_sampling_rate!=0) x=(ULONG)((ce->regn.parms.smpte_sampling_rate)*((float)(Tc2Sec(TrigTC)-Tc2Sec(ce->regn.parms.starttimecode))));
	else {x=((ULONG)ce->regn.parms.rate)*(Tc2Sec(TrigTC)-Tc2Sec(ce->regn.parms.starttimecode));}
	ce->regn.start=x+ce->Rstart;
	}

if ((ps=(struct ActiveSample *)ASOpen(&ce->regn, AS_PLAY|AS_AUTO_DATA))==0) {telluser("Play Process:","Can not open disk sample",NULL);ce->task=NULL;return;}
PPss.Start=ce->regn.parms.starttimecode;
PPss.End=ce->end;
PPss.Chan=ChanTC;
PPss.Trig=TrigTC;
PPss.Cmd=MIN(SubTimeCode(CurTC,0x00000100,StudioBase->defaults.smpte_mode),CurTC);

NewSMPTEsnk.userdata=(APTR)&PPss;
SMPTEsnk=(struct SmpteSink *)AddSmpteSink(&NewSMPTEsnk);

Smask=PPss.Trigger_SigMask|PPss.Unload_SigMask|PPss.Seek_SigMask|PPss.GetChan_SigMask;
while (flg==0)
	{
	mask=Wait(Smask);

	if (mask & PPss.GetChan_SigMask) 
		if (ps->key==0)
			ps->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);

	if (mask & PPss.Trigger_SigMask)
		{
		if (ps->key==0)	ps->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
		if (ps->key==0)	telluser("Play Process:","Can't Allocate a play channel!",NULL);
		else	{
			ps->flags |= AS_IS_ACTIVE;
			BroadcastEventParms(se_cmd_trigger_timecode,ps,PPss.Trig,0,0,0,0);
/*			ASTrigger(ps);*/
			ce->flags|=CEF_PLAYING;

			highlight_list_entry(CueL,le,1);

/** NOTE: This ReRead is Asyncronus with DiskIO's ASRead()s. **/
/** Do not set AS_AUTO_CLOSE with this method, since it could **/
/** result in reading a closed file **/

			while (ReRead) {ASRead(ps); ReRead--;}
			}
		flg=1;
		mask=0;
		}

	if (mask & PPss.Seek_SigMask) /* If misses recalled by SigBit */
		{
		CurTC=QuerySmpteTime();
		TrigTC=AddTimeCode((CurTC&0xFFFFFF00)|(ce->regn.parms.starttimecode&0x000000FF),(add<<8),StudioBase->defaults.smpte_mode);
		if(TrigTC<ce->end)
			{
			ReRead2=ASFlush(ps,0);
			ReRead=ReRead+ReRead2;

			if (ce->regn.parms.smpte_sampling_rate!=0) x=(ULONG)((ce->regn.parms.smpte_sampling_rate)*((float)(Tc2Sec(TrigTC)-Tc2Sec(ce->regn.parms.starttimecode))));
			else {x=((ULONG)ce->regn.parms.rate)*(Tc2Sec(TrigTC)-Tc2Sec(ce->regn.parms.starttimecode));}

			ASSeek(ps,x+ce->Rstart, ASS_OFFSET_BEGINNING);
			ASRead(ps);ReRead--;
			CurTC=QuerySmpteTime();
			if(AddTimeCode(CurTC,0x0000000a,StudioBase->defaults.smpte_mode)<TrigTC)
				{
/* assumptiom: */		while(AddTimeCode(QuerySmpteTime(),0x0000010F,StudioBase->defaults.smpte_mode)<TrigTC && ReRead)
/* Read will take less than*/		{
/* 1.1 Seconds */			ASRead(ps);
					ReRead--;
					}
				Disable();
				PPss.Trig=TrigTC;
				PPss.Cmd=MIN(SubTimeCode(CurTC,0x00000100,StudioBase->defaults.smpte_mode),CurTC);
				SetSignal(0,PPss.Trigger_SigMask);
				SetSignal(0,PPss.Seek_SigMask);
				SetSignal(0,PPss.GetChan_SigMask);
				Enable();
				}
			}
		else flg=1;
		}

	if (mask & PPss.Unload_SigMask) flg=2;
	}

DeleteSmpteSink(SMPTEsnk);
key=ps->key;
if (flg!=2)
	while ((ps->flags&AS_IS_ACTIVE) && (!((SetSignal(0,0)) & PPss.Unload_SigMask)))
		{WaitTOF();WaitTOF();}
ce->flags&=~CEF_PLAYING;
if ((ps->flags&AS_IS_ACTIVE)) ASStop(ps);

ASClose(ps);
if (key) FreeChan(key);
ce->task=NULL;

unhighlight_list_entry(CueL,le);

FreeSignal(trigger_sigbit);
FreeSignal(unload_sigbit);
FreeSignal(seek_sigbit);
FreeSignal(getchan_sigbit);


Signal(task,1<<preload_sigbit);
ce->task=NULL;
ce->as=NULL;
ce->regn.start=0;
ce->UnloadSigMask=NULL;
}

}

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
if (pptask->tc_UserData!=0) {telluser("Play Proc Startup:","Error #1 - Please report to SunRize, ask for Todd",NOTICE_RETURN_NOW);pptask->tc_UserData=0;}

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
