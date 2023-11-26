#include <exec/types.h>
#include <exec/exec.h>
#include "/include/studio16.h"
#include "/include/smpte.h" 

#define SF_24FPS	24
#define SF_25FPS	25
#define SF_30FPS	30
#define SF_30FPS_DF	30

extern struct StudioBase *StudioBase;

/** NEED LIBRARY COMMENTS ***/
int SmpteSourceCount=0;
struct Task *STtask;
USHORT TOflag=0;
int LastTC;
/**********************************/
void __asm InjectTimeCodeError(register __d1 struct SmpteSource *,register __d3 int);
/**************************************************************/
void SmpteTimeout()
{
struct timeval currentval;
struct MsgPort *timerport;
struct timerequest *timermsg;
static int ltc=0,tcflag=0;
int tc;
int QuerySmpteTime();

timerport = (struct MsgPort *) CreatePort( 0, 0 );
timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );

while (TOflag==1)
	{
	currentval.tv_secs = 0;
	currentval.tv_micro = StudioBase->defaults.SmpteTimeOut*1000;
	timermsg->tr_node.io_Command = TR_ADDREQUEST;
	timermsg->tr_time = currentval;
	SendIO((struct IORequest *) timermsg );

	Wait(1<<timerport->mp_SigBit);

	tc=QuerySmpteTime();
	if(tcflag==1)
		{
		if (ltc==tc) 
			{
			StudioBase->flags&=~SB_SMPTE_ON;
			StudioBase->CurrentSmpteTime=LastTC;
			tc=LastTC;
			InjectTimeCodeError(StudioBase->CurrentSmpteSource,SMPTE_ERROR_TIMEOUT);
			tcflag=0;
			}
		}
	else
		{
		if (ltc!=tc) {tcflag=1;StudioBase->flags|=SB_SMPTE_ON;}
		}
	ltc=tc;
	}
DeletePort(timerport);
CloseDevice( (struct IORequest *) timermsg );
DeleteExtIO( (struct IORequest *) timermsg );
TOflag=0;

}

/*############################################################
NAME	
	AddSmpteSink
FUNCTION
	Adds a Smpte Sink into the sink chain, it uses a priority to decide
	where in the chain it should go.
INPUTS
	ns - NewSmpteSink structure
RESULTS
	a pointer to the created SmpteSink structure.
##############################################################*/

struct SmpteSink *AddSmpteSink(ns)
struct NewSmpteSink *ns;
{
struct SmpteSink *ss;

ss=(struct SmpteSink *)AllocMem(sizeof(struct SmpteSink),MEMF_CLEAR);
if (ss) {
	ss->node.ln_Pri=ns->priority;
	ss->func=ns->func;
	Disable();
	ss->userdata=ns->userdata;
	Enqueue(&StudioBase->SmpteSinks, ss);
	Enable();
	}

return(ss);
}

/*############################################################
NAME	
	AddSmpteSource
FUNCTION
	Adds a Smpte Source into the source chain. The new source becomes
	the active one. This function also lanches the 
	task, "STUDIO_SMPTE_TIMEOUT" that is used to determine whether
	or not a smpte timeout has occured.
INPUTS
	ns - NewSmpteSource structure
RESULTS
	a pointer to the created SmpteSource structure.
##############################################################*/

struct SmpteSource *AddSmpteSource(ns)
struct NewSmpteSource *ns;
{
struct SmpteSource *ss;
void SmpteTimeout();

while (TOflag==2) WaitTOF();

ss=(struct SmpteSource *)AllocMem(sizeof(struct SmpteSource),MEMF_CLEAR);
if (ss) {
	ss->name=(char *)AllocMem(strlen(ns->name)+1,0);
	if (ss->name==0) {
		FreeMem(ss, sizeof(struct SmpteSource));
		return(0);
		}
	strcpy(ss->name,ns->name);
	ss->flags=ns->flags;
	Disable();
	AddTail(&StudioBase->SmpteSources,ss);

	if (SmpteSourceCount==0 && TOflag==0) {STtask=(struct Task *)CreateTask("STUDIO_SMPTE_TIMEOUT",0,SmpteTimeout,2200);TOflag=1;}
	SmpteSourceCount++;
	Enable();

	if (StudioBase->CurrentSmpteSource==NULL) {
		if(stricmp(ss->name,StudioBase->defaults.SmpteSourceName)==0){
			if (ss->unit==StudioBase->defaults.SmpteSourceUnit) {
				StudioBase->CurrentSmpteSource=ss;
				}
			}
		}
	}

return(ss);
}
/*############################################################
NAME	
	DeleteSmpteSink
FUNCTION
	Removes a Smpte Sink from the chain.
INPUTS
	ss - pointer to the SmpteSink that is to be removed.
	
RESULTS
	NONE
##############################################################*/
void DeleteSmpteSink(ss)
struct SmpteSink *ss;
{
Disable();
Remove(ss);
Enable();
FreeMem(ss, sizeof(struct SmpteSink));
}
/*############################################################
NAME	
	DeleteSmpteSource
FUNCTION
	Removes a SmpteSource from the chain.  If this source is the active
	one it sets StudioBase->CurrentSmpteSource=NULL, if this is the last
	smpte source to be deleted, it will remove the timeout task.
INPUTS
	ss - a pointer to the smpte source to be removed.
RESULTS
	
##############################################################*/
void DeleteSmpteSource(ss)
struct SmpteSource *ss;
{
Disable();
Remove(ss);
if (ss==StudioBase->CurrentSmpteSource)
	StudioBase->CurrentSmpteSource=NULL;

SmpteSourceCount--;
if (SmpteSourceCount==0) TOflag=2;

Enable();
while(TOflag==2) WaitTOF();
FreeMem(ss->name,strlen(ss->name)+1);
FreeMem(ss, sizeof(struct SmpteSource));
}
/*############################################################
NAME	
	QuerySmpteTime
FUNCTION
	Gets the current smpte time code.
INPUTS
	none
RESULTS
	current time code value (int).
##############################################################*/

int QuerySmpteTime()
{
return(StudioBase->CurrentSmpteTime);
}

/*############################################################
NAME
	SetSmpteSource
FUNCTION
	This function changes the current Smpte Souce to the new
	one.  you may find out the CurrentSmpteSource by reading the
	StudioBase->CurrentSmpteSource variable.
INPUTS
	ss - new Smpte Source
RESULTS
	returns the old Smpte Source
##############################################################*/

struct SmpteSource *SetSmpteSource(ss)
struct SmpteSource *ss;
{
struct SmpteSource *os;
os=StudioBase->CurrentSmpteSource;
StudioBase->CurrentSmpteSource=ss;
return(os);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*		These Are In 'TimeCodeFunc.asm'		      */
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/

/*############################################################
NAME
	InjectTimeCode
FUNCTION
	This function is used to propagate Smpte time code throughout the
	Sink Chain.  However, before the function does this it first
	checks to make sure that the Smpte Source Input is the Current
	Smpte Source.
INPUTS
	(d1) Smpte Source Pointer
	(d2) Smpte TimeCode
RESULTS
	None
##############################################################*/

/*############################################################
NAME
	InjectTimeCodeError
FUNCTION
	If a Smpte Error occures call this function to warn all of the
	Smpte Sinks of the errors. However, before the function does this
	it first checks to make sure that the Smpte Source Input is the
	Current Smpte Source.  Each Sink in the chain will be called 
	using the error code provided in function inputs and the current
	smpte time.
INPUTS
	(d1) Smpte Source Pointer
	(d3) Smpte Error int (see include/smpte.h for error list)
RESULTS
	None
##############################################################*/

/*############################################################
NAME
	TestTimeCode
FUNCTION
	This functions checks the input time code for out of bound errors.
	for example a second value of 61 would produce an error code of 2.
INPUTS
	(d2) Input Time Code
	(d3) Smpte Type
RESULTS
	0=No Error
	1=Error In Frames
	2=Error In Seconds
	3=Error In Minutes
	4=Error In Hours
##############################################################*/

/*############################################################
NAME
	AddTimeCode
FUNCTION
	Adds two time codes returning the result.
INPUTS
	(d0) Input Time Code1
	(d1) Input Time Code2
	(d3) Smpte Type
RESULTS
	InputTimeCode1 + InputTimeCode2
##############################################################*/

/*############################################################
NAME
	SubTimeCode
FUNCTION
	Subtracts two time codes
INPUTS
	(d0) Input Time Code1
	(d1) Input Time Code2
	(d3) Smpte Type
RESULTS
	InputTimeCode1 - InputTimeCode2
##############################################################*/

/*############################################################
NAME
	IncTimeCode
FUNCTION
	Increments the input time code. This is the same as adding 1 to the
	time code but this function is significantly faster than add.
INPUTS
	(d0) Input Time Code
	(d3) Smpte Type
RESULTS
	InputTimeCode + 1
##############################################################*/

