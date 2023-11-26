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
	struct CueEntry *ce;
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
/**************************************************************/
/**************************************************************/
/**************************************************************/
void PLAYproc()
{
struct ActiveSample *ps;
struct Region region;
struct Disk_Samp *samp,*FindSample();
struct Task *pptask=(struct Task *)FindTask(NULL);
struct MsgPort *PPport;
struct PPStartupMessage *PP_SUM;
struct CueEntry *ce;
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

/**********************************/
void SinkFunc();	/* in file SMPTE.asm */
struct NewSmpteSink NewSMPTEsnk = {10,NULL,SinkFunc,NULL};
struct SmpteSink *SMPTEsnk;
/**********************************/
while (0==(PPport=(struct MsgPort *)pptask->tc_UserData)) WaitTOF();
/*WaitPort(PPport);*/
PP_SUM=(struct PPStartupMessage *)GetMsg(PPport);
ce=PP_SUM->ce;
flags=PP_SUM->flags;

/********** Function is now on its own *****************/ 
if (!(samp=FindSample(ce->Sample,-1))) {return;} /* Sample is Not Loaded */

/**************************************************************/
if (flags & PP_TRIGGER_NOW)
/**************************************************************/
	{
	LockMod(mod->state);

	pptask->tc_UserData=0;
	region.samp=samp;
	region.start=0;
	region.end=samp->length-1;
	region.owner=NULL;
	region.user_data=NULL;
	region.parms=samp->parms;
	region.parms.volume=(ce->Vol+100)<<5;

	if ((ps=(struct ActiveSample *)ASOpen(&region, AS_PLAY|AS_AUTO_DATA))==0) {telluser("Play Process:","Can not open disk sample",NULL);UnlockMod(mod->state);return;}

	ps->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
	if (ps->key==0)
		{
		ASClose(ps);
		telluser("Play Process:","Can't Allocate a play channel!",NOTICE_RETURN_NOW);
		UnlockMod(mod->state);
		return;
		}

	ASTrigger(ps);
	key=ps->key;

	ASClose(ps);
	if (key) FreeChan(key);
	UnlockMod(mod->state);
	}
/**************************************************************/
else
/**************************************************************/
{
if ((trigger_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Trigger_SigMask=1<<trigger_sigbit;

if ((unload_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Unload_SigMask=1<<unload_sigbit;

if ((seek_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.Seek_SigMask=1<<seek_sigbit;

if ((getchan_sigbit=AllocSignal(-1))==-1) {telluser("Play Proc:","Cant get SigBit",NULL);return;}
else PPss.GetChan_SigMask=1<<getchan_sigbit;

TrigTC=ce->Start;
ChanTC=SubTimeCode(TrigTC,1L,StudioBase->defaults.smpte_mode);

ce->UnloadSigMask=PPss.Unload_SigMask;
PPss.task=(struct Task *)FindTask(NULL);
ce->task=PPss.task;
pptask->tc_UserData=0;

region.samp=samp;
region.start=0;
region.end=samp->length-1;
region.owner=NULL;
region.user_data=NULL;
region.parms=samp->parms;
region.parms.volume=(ce->Vol+100)<<5;

CurTC=QuerySmpteTime();
if(CurTC>=ce->End) {ce->task=NULL;return;} /* Sample is already past */

if (CurTC>TrigTC && CurTC<ce->End)  /* Triggering From Middle */
	{
	TrigTC=AddTimeCode(CurTC&0xFFFFFF00|ce->Start&0x000000FF,1<<8,StudioBase->defaults.smpte_mode);
	if(TrigTC>=ce->End) {ce->task=NULL;return;}
	ChanTC=SubTimeCode(TrigTC,1L,StudioBase->defaults.smpte_mode);
	if (region.parms.smpte_sampling_rate!=0) x=(ULONG)((region.parms.smpte_sampling_rate)*((float)(Tc2Sec(TrigTC)-Tc2Sec(ce->Start))));
	else {x=((ULONG)region.parms.rate)*(Tc2Sec(TrigTC)-Tc2Sec(ce->Start));}
	region.start=x;
	}

if ((ps=(struct ActiveSample *)ASOpen(&region, AS_PLAY|AS_AUTO_DATA))==0) {telluser("Play Process:","Can not open disk sample",NULL);ce->task=NULL;return;}
PPss.Start=ce->Start;
PPss.End=ce->End;
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
			ASTrigger(ps);
			ce->flags|=CEF_PLAYING;
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
		TrigTC=AddTimeCode((CurTC&0xFFFFFF00)|(ce->Start&0x000000FF),(add<<8),StudioBase->defaults.smpte_mode);
		if(TrigTC<ce->End)
			{
			ReRead2=ASFlush(ps,0);
			ReRead=ReRead+ReRead2;

			if (region.parms.smpte_sampling_rate!=0) x=(ULONG)((region.parms.smpte_sampling_rate)*((float)(Tc2Sec(TrigTC)-Tc2Sec(ce->Start))));
			else {x=((ULONG)region.parms.rate)*(Tc2Sec(TrigTC)-Tc2Sec(ce->Start));}

			ASSeek(ps,x, ASS_OFFSET_BEGINNING);
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

FreeSignal(trigger_sigbit);
FreeSignal(unload_sigbit);
FreeSignal(seek_sigbit);
FreeSignal(getchan_sigbit);


Signal(task,1<<preload_sigbit);
ce->task=NULL;
ce->as=NULL;
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
struct Task *CreatePlayProc(ce,flags)
struct CueEntry *ce;
int flags;
{
struct MsgPort *gaddfltport;
struct MsgPort *CreatePort(),*CreateProc();
struct Task *pptask;
char procname[100];
struct PPStartupMessage PPSUM;
APTR port;
ULONG code=(ULONG)PLAYproc;

if (!(FindSample(ce->Sample,-1))) return(NULL); /* Sample is Not Loaded */

my_seg[0]=16;
my_seg[1]=0;
my_seg[2]=0x4EF90000|((code>>16)&0xFFFF);
my_seg[3]=0x00000000|((code&0xFFFF)<<16);

if (!(flags & PP_TRIGGER_NOW)) sprintf(procname,"CUE:%s %08x",ce->Sample,ce->Start);
else sprintf(procname,"PLAY:%s",ce->Sample);
if (SysBase->LibNode.lib_Version>=37) CacheClearU();

if((port=(APTR)CreatePort(0,0))==0) {telluser("Play Proc:","Could Not Create MsgPort",0);return(NULL);}

if (0==(gaddfltport=CreateProc(procname,4,((ULONG)(&my_seg[1]))>>2,8000))) {telluser("Play Proc:","Could Not Create Process",0);DeletePort(port);return(NULL);}

pptask=(struct Task *)((int)gaddfltport - sizeof(struct Task));
if (pptask->tc_UserData!=0) {telluser("Play Proc Startup:","Error #1 - Please report to SunRize, ask for Todd",NOTICE_RETURN_NOW);pptask->tc_UserData=0;}

PPSUM.ce=ce;
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
