#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include "/include/gadlib.h"
#include "/include/soundedit.h"
#include "/include/studio16.h"


extern struct StudioBase *StudioBase;
extern struct Window *edit_win;
extern long zoom_scale;		/* samples per pixel */
extern struct EditState *thisstate;
extern struct EditInstance EI[EI_MAX];

struct SampleFilePointer *SampleCreateRevision();
struct SampleFilePointer *SampleOpenInDefPath();
struct SampleFilePointer *SampleOpen();
char *FindSample();
void CalcCopyBufferName();

/************************************************************/

DoCopyEdit(ei,to)
struct EditInstance *ei;
char *to;
{
long len; 
int error;
struct SampleFilePointer *wsfp;
char copyname[80];

if (to) {
	wsfp=SampleOpen(to,SP_MODE_NEWFILE,0);
	}
else {
	CalcCopyBufferName(ei,copyname);
	if (stricmp(copyname,StrFileName(ei->samp_name))==0)
		return(STDERR_COPYBUFFER_IN_USE);

	wsfp=SampleOpenInDefPath(copyname,SP_MODE_NEWFILE,0);
	}
if (wsfp) {
	new_pointer(PTR_BUSY);
	SampleSeek(ei->sfp, ei->start_mark, SS_OFFSET_BEGINNING);
	len=ei->end_mark-ei->start_mark+1;
	error=SampleCopy(wsfp,ei->sfp,len);
	CopySampDataExceptRegions(&wsfp->tag,&ei->sfp->tag);
	SampleClose(wsfp);
	new_pointer(PTR_NORMAL);
	}
else {
	error=STDERR_CANT_CREATE_SAMPLE;
	}
return(error);
}

/************************************************************/

void CalcCopyBufferName(ei,copyname)
struct EditInstance *ei;
char *copyname;
{
short i;

i=0;
while (ei!=&EI[i]) i++;

if (i==0)
	sprintf(copyname,"CopyBuffer");
else
	sprintf(copyname,"CopyBuffer#%d",i);
}

/************************************************************/

DoDeleteEdit(ei)
struct EditInstance *ei;
{
struct SampleFilePointer *wsfp;
int error;

if (ei->sfp->tag.length-(ei->end_mark-ei->start_mark+1) < STDERR_SAMPLE_TOO_SMALL)
	return(STDERR_SAMPLE_TOO_SMALL);

if ((wsfp=SampleCreateRevision(ei->samp_name,ei->sfp->ff))==0) {
	return(STDERR_CANT_GET_WRITE_LOCK);
	}

error=NO_ERROR;
new_pointer(PTR_BUSY);
SampleSeek(ei->sfp, 0, SS_OFFSET_BEGINNING);
if ((error=SampleCopy(wsfp,ei->sfp,ei->start_mark))==NO_ERROR) {
		SampleSeek(ei->sfp, ei->end_mark+1, SS_OFFSET_BEGINNING);
		error=SampleCopy(wsfp,ei->sfp,ei->sfp->tag.length-(ei->end_mark+1));
	}

CopySampData(&wsfp->tag,&ei->sfp->tag);
SampleClose(wsfp);
if (error==NO_ERROR) {
	SampleClose(ei->sfp);
	error=UseRevisedSample(ei);
	}

ei->marked_samp=FALSE;	/** not marked any more! **/
RegenGraph(ei, FALSE);
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoKeepEdit(ei)
struct EditInstance *ei;
{
struct SampleFilePointer *wsfp;
int error;

if ((wsfp=SampleCreateRevision(ei->samp_name,ei->sfp->ff))==0) {
	return(STDERR_CANT_GET_WRITE_LOCK);
	}

new_pointer(PTR_BUSY);

SampleSeek(ei->sfp, ei->start_mark, SS_OFFSET_BEGINNING);
error=SampleCopy(wsfp,ei->sfp,ei->end_mark-ei->start_mark+1);

CopySampData(&wsfp->tag,&ei->sfp->tag);
SampleClose(wsfp);
if (error==NO_ERROR) {
	SampleClose(ei->sfp);
	error=UseRevisedSample(ei);
	}

ei->marked_samp=FALSE;	/** not marked any more! **/
RegenGraph(ei, FALSE);
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoPasteEdit(ei,ptype)
struct EditInstance *ei;
int ptype;
{
struct SampleFilePointer *rsfp;
struct SampleFilePointer *wsfp;
int error;
long insert_point;
char copyname[80];

CalcCopyBufferName(ei,copyname);

if ((rsfp=SampleOpenInDefPath(copyname,SP_MODE_OLDFILE,0))==0) {
	return(STDERR_CANT_FIND_COPYBUFFER);
	}

if ((wsfp=SampleCreateRevision(ei->samp_name,ei->sfp->ff))==0) {
	SampleClose(rsfp);
	return(STDERR_CANT_GET_WRITE_LOCK);
	}

error=NO_ERROR;
new_pointer(PTR_BUSY);
SampleSeek(ei->sfp, 0, SS_OFFSET_BEGINNING);
if (ptype==PT_OVERWRITE) {
	if ((error=SampleCopy(wsfp,ei->sfp,ei->start_mark))==NO_ERROR)
		if ((error=SampleCopy(wsfp,rsfp,rsfp->tag.length))==NO_ERROR) {
			SampleSeek(ei->sfp, ei->end_mark+1, SS_OFFSET_BEGINNING);
			error=SampleCopy(wsfp,ei->sfp,ei->sfp->tag.length-ei->end_mark-1);
			}
	}
else {	/** must be PT_INSERT **/
	if (ptype==PT_INSERT_START)
		insert_point=ei->start_mark;
	else
		insert_point=ei->end_mark+1;
	if ((error=SampleCopy(wsfp,ei->sfp,insert_point))==NO_ERROR)
		if ((error=SampleCopy(wsfp,rsfp,rsfp->tag.length))==NO_ERROR)
			error=SampleCopy(wsfp,ei->sfp,ei->sfp->tag.length-insert_point);
	}

CopySampData(&wsfp->tag,&ei->sfp->tag);
SampleClose(wsfp);
SampleClose(rsfp);	
if (error==NO_ERROR) {
	SampleClose(ei->sfp);
	error=UseRevisedSample(ei);
	}

RegenGraph(ei, FALSE);
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoCompressEdit(ei)
struct EditInstance *ei;
{
struct SampleFilePointer *wsfp;
int error;

if ((wsfp=SampleCreateRevision(ei->samp_name,ei->sfp->ff))==0) {
	return(STDERR_CANT_CREATE_SAMPLE);
	}

new_pointer(PTR_BUSY);
SampleSeek(ei->sfp, 0, SS_OFFSET_BEGINNING);
error=SampleCopy(wsfp,ei->sfp,ei->sfp->tag.length);
CopySampData(&wsfp->tag,&ei->sfp->tag);
SampleClose(wsfp);

if (error==NO_ERROR) {
	SampleClose(ei->sfp);
	error=UseRevisedSample(ei);
	}

RegenGraph(ei, FALSE);
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoFlipEdit(ei)
struct EditInstance *ei;
{
short *amem;
short *bmem;
int buflen;
int fliplen;
int astart, bend;
short temp;
int thislen;
int i;
int error;

buflen=StudioBase->defaults.copy_buf_size/2;
amem=(short *)AllocMem(buflen*2, 0);
if (amem==0) {
	return(OUT_OF_MEMORY);
	}

bmem=(short *)AllocMem(buflen*2, 0);
if (bmem==0) {
	FreeMem(amem, buflen*2);
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);

astart=ei->start_mark;
bend=ei->end_mark;
fliplen=max((bend-astart+1)/2,1);	/** 1 so no crashes **/

error=NO_ERROR;

do {
	thislen= min(fliplen, buflen);
	SampleSeek(ei->sfp, astart, SS_OFFSET_BEGINNING);
	SampleRead(ei->sfp, amem, thislen);
	SampleSeek(ei->sfp, bend-thislen+1, SS_OFFSET_BEGINNING);
	if (SampleRead(ei->sfp, bmem, thislen)!=thislen)
		error=READ_ERROR;
	for (i=0; i < thislen; i++) {
		temp=amem[i];
		amem[i]=bmem[thislen-i-1];
		bmem[thislen-i-1]=temp;
		}
	SampleSeek(ei->sfp, astart, SS_OFFSET_BEGINNING);
	SampleWrite(ei->sfp, amem, thislen);
	SampleSeek(ei->sfp, bend-thislen+1, SS_OFFSET_BEGINNING);
	if (SampleWrite(ei->sfp, bmem, thislen)!=thislen)
		error=WRITE_ERROR;
	fliplen -= thislen;
	astart += thislen;
	bend -= thislen;
	} while (fliplen && error==NO_ERROR);

FreeMem(amem, buflen*2);
FreeMem(bmem, buflen*2);

RegenGraph(ei, FALSE);
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoZeroEdit(ei)
struct EditInstance *ei;
{
short *mem;
long buflen;
long dolen;
long thislen;
int error;

buflen=StudioBase->defaults.copy_buf_size/2;
mem=(short *)AllocMem(buflen*2, MEMF_CLEAR);
if (mem==0) {
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);
dolen=ei->end_mark-ei->start_mark+1;
ScaleMemDclips(ei,ei->sfp->tag.dclips,ei->start_mark,0, 0, dolen/ei->gb_scale);

SampleSeek(ei->sfp, ei->start_mark, SS_OFFSET_BEGINNING);
error=NO_ERROR;

do {
	thislen=min(dolen, buflen);
	if (SampleWrite(ei->sfp, mem, thislen)!=thislen)
		error=WRITE_ERROR;
	dolen -= thislen;
	} while (dolen && error==NO_ERROR);

FreeMem(mem, buflen*2);

GenEditGraphBuffer(ei);		/** update working graph **/
BuildEIRP(ei);
DrawAllRP();
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoInvertEdit(ei)
struct EditInstance *ei;
{
short *mem;
long buflen;
long dolen;
long thislen;
int error;
int i;

buflen=StudioBase->defaults.copy_buf_size/2;
mem=(short *)AllocMem(buflen*2, 0);
if (mem==0) {
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);
dolen=ei->end_mark-ei->start_mark+1;
SampleSeek(ei->sfp, ei->start_mark, SS_OFFSET_BEGINNING);
error=NO_ERROR;

do {
	thislen=min(dolen, buflen);
	if (SampleRead(ei->sfp, mem, thislen)!=thislen)
		error=READ_ERROR;
	for (i=0; i < thislen; i++) {
		if (mem[i]== -32768) mem[i]++;	/** avoid overflows **/
		mem[i]=-mem[i];
		}
	SampleSeek(ei->sfp, -thislen, SS_OFFSET_CURRENT);
	if (SampleWrite(ei->sfp, mem, thislen)!=thislen)
		error=WRITE_ERROR;
	dolen -= thislen;
	} while (dolen && error==NO_ERROR);

FreeMem(mem, buflen*2);
if (zoom_scale==1) {
	BuildEIRP(ei); 
	DrawAllRP();
	}
new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoNormalizeEdit(ei)
struct EditInstance *ei;
{
short *mem;
long buflen;
long dolen;
long thislen;
int error;
int i;
int maxval,scale;
char msg[80];

buflen=StudioBase->defaults.copy_buf_size/2;
mem=(short *)AllocMem(buflen*2, 0);
if (mem==0) {
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);
dolen=ei->end_mark-ei->start_mark+1;
SampleSeek(ei->sfp, ei->start_mark, SS_OFFSET_BEGINNING);
error=NO_ERROR;

maxval=0;

do {
	thislen=min(dolen, buflen);
	if (SampleRead(ei->sfp, mem, thislen)!=thislen)
		error=READ_ERROR;
	for (i=0; i < thislen; i++) {
		if (abs(mem[i]) > maxval)
			maxval=abs(mem[i]);
		}
	SampleSeek(ei->sfp, -thislen, SS_OFFSET_CURRENT);
	if (SampleWrite(ei->sfp, mem, thislen)!=thislen)
		error=WRITE_ERROR;
	dolen -= thislen;
	} while (dolen && error==NO_ERROR);

FreeMem(mem, buflen*2);
new_pointer(PTR_NORMAL);

if (error!=NO_ERROR)
	return(error);

if (maxval==0) {
	telluser("Marked range only contains zeros.","Can't normalize.",0);
	return(NO_ERROR);
	}

scale=(0x8000*100)/maxval;
sprintf(msg,"Max value was %d.  Normalize by scaling %d%%?",maxval,scale);
if (askuser(msg,0,0)) {
	scale=(0x8000*512)/maxval;
	return(DoScaleEdit(ei, scale, scale));
	}
return(NO_ERROR);
}

/************************************************************/

DoGenSilence(ei)
struct EditInstance *ei;
{
short *mem;
int error;
int len;
long rate;
double time;
struct SampleFilePointer *sfp;

if (ei->flags&EI_IN_USE)
	rate=ei->sfp->tag.parms.rate;
else
	rate=44100;
time=1.0;
if (!GenReq(ei,"Gen Silence",&rate,&time,0))
	return(NO_ERROR);
len=(int)((double)rate*time);
mem=(short *)AllocMem(len*2, MEMF_CLEAR);
if (mem) {
/* ---->>>>>>>>>>>> DEBUG STUFF --->>*/
	for (error=0; error < len; error++)
		mem[error]=0x8000;
/* ---->>>>>>>>>>>> DEBUG STUFF --->>*/
	new_pointer(PTR_BUSY);
	if ((sfp=SampleOpenInDefPath("CopyBuffer",SP_MODE_NEWFILE,0))==0) {
		return(STDERR_CANT_CREATE_SAMPLE);
		}
	if (SampleWrite(sfp, mem, len)==len)
		error=NO_ERROR;
	else
		error=WRITE_ERROR;
	FreeMem(mem, len*2);
	if (ei->flags&EI_IN_USE) {
		CopySampData(&sfp->tag,&ei->sfp->tag);
		sfp->tag.parms.rate=rate;
		}
	else {
		sfp->tag.parms.rate=rate;
		sfp->tag.parms.filter3db=rate/2;
		sfp->tag.parms.volume=ZERO_DB;
		sfp->tag.parms.pan=ZERO_DB;
		}
	SampleClose(sfp);
	new_pointer(PTR_NORMAL);
	if (error==NO_ERROR)
		telluser("Gen Silence successful:","Silence now in CopyBuffer.",0);
	}
else {
	error=OUT_OF_MEMORY;
	}
return(error);
}

/************************************************************/

DoGenSine(ei)
struct EditInstance *ei;
{
double sin(double);
short *mem;
int error;
long len,k;
long rate;
double time,freq;
struct SampleFilePointer *sfp;
double pi=3.14159265359;

if (ei->flags&EI_IN_USE)
	rate=ei->sfp->tag.parms.rate;
else
	rate=44100;
time=1.0;
freq=1000.0;
if (!GenReq(ei,"Gen Sine Wave",&rate,&time,&freq))
	return(NO_ERROR);

len=(int)((double)rate*time);
mem=(short *)AllocMem(len*2, MEMF_CLEAR);
if (mem) {
	new_pointer(PTR_BUSY);
	for (k=0; k < len; k++) {
		mem[k]=(short)(32767.0*sin((double)((double)k*2.0*pi*freq/(double)rate)));
		}
	if ((sfp=SampleOpenInDefPath("CopyBuffer",SP_MODE_NEWFILE,0))==0) {
		return(STDERR_CANT_CREATE_SAMPLE);
		}
	if (SampleWrite(sfp, mem, len)==len)
		error=NO_ERROR;
	else
		error=WRITE_ERROR;
	FreeMem(mem, len*2);
	if (ei->flags&EI_IN_USE) {
		CopySampData(&sfp->tag,&ei->sfp->tag);
		sfp->tag.parms.rate=rate;
		}
	else {
		sfp->tag.parms.rate=rate;
		sfp->tag.parms.filter3db=rate/2;
		sfp->tag.parms.volume=ZERO_DB;
		sfp->tag.parms.pan=ZERO_DB;
		}
	SampleClose(sfp);
	new_pointer(PTR_NORMAL);
	if (error==NO_ERROR)
		telluser("GenSine successful:","Sine Wave now in CopyBuffer.",0);
	}
else {
	error=OUT_OF_MEMORY;
	}
return(error);
}

/************************************************************/
#if 0 /*** DOESNT WORK ****/
DoGenWhiteNoise(ei)
struct EditInstance *ei;

{
short *mem;
int error;
long len,k;
long rate;
double time;
struct SampleFilePointer *sfp;
double seed=100000;
double m=1;
double mymod(double, double);

rate=ei->sfp->tag.parms.rate;
time=1.0;
if (!GenReq(ei,"Gen White Noise",&rate,&time,0))
	return(NO_ERROR);

for (k=0; k < 32; k++)
	m=m*2;
len=(int)((double)rate*time);
mem=(short *)AllocMem(len*2, MEMF_CLEAR);
if (mem) {
	new_pointer(PTR_BUSY);
	for (k=0; k < len; k++) {
		mem[k]=(short)(seed/0x10000);
		seed=mymod((1664525.0*seed+32767.0),m);
		}
	if ((sfp=SampleOpenInDefPath("CopyBuffer",SP_MODE_NEWFILE,0))==0) {
		return(STDERR_CANT_CREATE_SAMPLE);
		}
	if (SampleWrite(sfp, mem, len)==len)
		error=NO_ERROR;
	else
		error=WRITE_ERROR;
	FreeMem(mem, len*2);
	if (ei->flags&EI_IN_USE) {
		CopySampData(&sfp->tag,&ei->sfp->tag);
		sfp->tag.parms.rate=rate;
		}
	else {
		sfp->tag.parms.rate=rate;
		sfp->tag.parms.filter3db=rate/2;
		sfp->tag.parms.volume=ZERO_DB;
		sfp->tag.parms.pan=ZERO_DB;
		}
	SampleClose(sfp);
	new_pointer(PTR_NORMAL);
	}
else {
	error=OUT_OF_MEMORY;
	}
return(error);
}

/************************************************************/

double mymod(a,b)
double a,b;
{
unsigned long val;

val = (unsigned long)(a/b);
val=(unsigned long)(b*((a/b)-(double)val));
return((double)val);
}
#endif

/************************************************************/

int DoCutEdit(ei)
struct EditInstance *ei;
{
int error;

error=DoCopyEdit(ei,0);
if (error==NO_ERROR)
	error=DoDeleteEdit(ei);
return(error);
}

/************************************************************/

int DoCopyAsEdit(ei)
struct EditInstance *ei;
{
int error,temp;
char name[40];
char *path;

strcpy(name,StrFileName(ei->samp_name));
temp=FileRequest("Select filename to save as:",thisstate->copy_path,name,FR_SINGLE_SELECT);
if (temp) {
	path=(char *)AllocCompletePath(thisstate->copy_path,name);
	error=DoCopyEdit(ei,path);
	FreeMem(path,strlen(path)+1);
	}
else {
	error=NO_ERROR;
	}
return(error);
}