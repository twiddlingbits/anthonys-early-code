#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#include "fft.data"

/***** AW, NOTES: THINGS TO TRY TO IMPROVE SOUND:
	1. use larger FFT (2048)
********/
/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_FFT";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Window *fft_win;
struct Module *thismod;

extern struct StudioBase *StudioBase;

void handle_studioevents();
void handle_intuievents();
void InitFFT();
void DoOpenFFT();
void DoCloseFFT();
struct Window *OpenNewWind();
void CalcFFT();
void CalcINVFFT();
void CalcANYFFT();
void PlotInput();
void PlotFFT();
void ShiftUpFFT();
void ScaleUpFFT();
void ShiftCopyBuffer();

double sin(double);
double cos(double);
double sqrt(double);

/** events I'm interested in **/

short se_cmd_select_module;
short se_cmd_kill_module;	 
short se_info_module_closed;
short se_info_module_opened;

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
StandardEnviroment();		/** opens studio16 customscreen **/
InitFFT();						/** initalize module **/

while (TRUE) {
	if (fft_win)
		waitmask=(1<<fft_win->UserPort->mp_SigBit) | (1<<thismod->notifyme->mp_SigBit);
	else
		waitmask=(1<<thismod->notifyme->mp_SigBit);

	mask=Wait(waitmask);

	if (fft_win)
		if (mask&(1<<fft_win->UserPort->mp_SigBit))
			handle_intuievents();
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	}
}

/***********************************************************/

void InitFFT()
{
static struct NewModule fft_data ={"FFT",0,sizeof(struct StandardModState),0};

if ((thismod=(struct Module *)AddModule(&fft_data))==0) {
	closelibraries();
	exit(10);
	}

se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_cmd_select_module, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
}

/***********************************************************/

void DoOpenFFT()
{
NewWindowStructure1.Screen=StudioBase->screen;
fft_win=OpenNewWind(&NewWindowStructure1,thismod ? thismod->state : NULL, NULL,NULL);
if (fft_win==NULL) {
	return;
	}

/** Let everyone else who cares know a new window is open **/

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
short *ReadFFTData();

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_cmd_select_module) {
			if ((struct Module *)event->arg1==thismod) {
				if (fft_win==0) {
					DoOpenFFT();
/*					ShiftCopyBuffer();*/
/*#if 0*/
					if (ReadFFTData()) {
						PlotInput();
						CalcFFT();  
						PlotFFT();

                  ShiftUpFFT(400); /** add 1000HZ to freq **/
                  CalcINVFFT();
                  PlotInput();

						CalcFFT();  
						PlotFFT();
						}
					else {
						telluser("FFT:","Can't Read CopyBuffer!",0);
						}
/*#endif*/
					}
				}
			}

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			DoCloseFFT();
			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

	ReplyMsg(event); 
	}
}

/***********************************************************/

void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;
struct Gadget *g;

while ((message=(struct IntuiMessage *)GetMsg(fft_win->UserPort))!=0) {
	HandleIntuitMess(fft_win,message);

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		DoCloseFFT();
		return;
		}

	else if (class==NEWSIZE) 
		{
		RefreshWind(fft_win);
		}
	}
}

/***********************************************************/

void DoCloseFFT()
{
if (fft_win) {
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
	CloseWind(fft_win);
	fft_win=0;
	}
}

/***********************************************************/

#define FFT_N 512
#define FFT_M 9			/** 2^M = FFT_N  **/

static double xr[FFT_N+1],xi[FFT_N+1];
static double globrate;
static short data[FFT_N];

/***********************************************************/

void ShiftCopyBuffer()
{
struct Disk_Samp *sampin;
struct SampleFilePointer *sfpin;
struct SampleFilePointer *sfpout;
short kount;

sampin=(struct Disk_Samp *)FindSample("CopyBuffer",-1);
if (sampin==NULL)
	return;

sfpin=(struct SampleFilePointer *)SampleOpen(sampin,SP_MODE_OLDFILE);
if (sfpin==NULL)
	return;

sfpout=(struct SampleFilePointer *)SampleCreate("ShiftOut");
if (sfpout==NULL) {
	SampleClose(sfpin);
	return;
	}

globrate=(double)sampin->parms.rate/(double)FFT_N;
kount=0;
while (SampleRead(sfpin,data,FFT_N)==FFT_N) {
	CalcFFT();
/*	ShiftUpFFT(150); */ /** add 150HZ to freq **/
	ScaleUpFFT(200);	/** shift up 100% **/
	if (++kount==4) {  
		kount=0;
		PlotFFT();
		}
	CalcINVFFT();
	if (SampleWrite(sfpout,data,FFT_N)!=FFT_N)
		break;
	kprintf("at %04x%04x out of %04x%04x\n",SampleSeek(sfpout, 0, SS_OFFSET_CURRENT),sampin->length);
	}

sfpout->disksamp->parms=sfpin->disksamp->parms;
sfpout->disksamp->type=sfpin->disksamp->type;

SampleClose(sfpin);
SampleClose(sfpout);
return;
}

/***********************************************************/

short *ReadFFTData()
{
struct Disk_Samp *samp;
struct SampleFilePointer *sfp;

samp=(struct Disk_Samp *)FindSample("CopyBuffer",-1);
if (samp==NULL)
	return(NULL);
sfp=(struct SampleFilePointer *)SampleOpen(samp,SP_MODE_OLDFILE);
if (sfp==NULL)
	return(NULL);
SampleRead(sfp,data,FFT_N);
SampleClose(sfp);
globrate=(double)samp->parms.rate/(double)FFT_N;
return(data);
}

/***********************************************************/

void CalcANYFFT(intt)
int intt;      /** 1 = do FFT, -1 = do INV FFT **/
{
int lim1,lim2,lim3,l,r,k,i,j;
double t,d,x1,x2,pi,b1,b2,c1,c2,arg,sin1,cos1;

/**** select Transform/Inverse transform ****/

t=(double)intt;
d=1;
if (t<0)
	d=FFT_N;

/**** Shuffle Input Data *****/

lim1=FFT_N-1;
lim2=FFT_N/2;
j=1;
for (i=1; i <= lim1; i++) {
	if (i>(j-1)) goto l2;
	x1=xr[j];
	x2=xi[j];
	xr[j]=xr[i];
	xi[j]=xi[i];
	xr[i]=x1;
	xi[i]=x2;
l2:l=lim2;
l3:if (l>(j-1)) goto l4;
	j=j-l;
	l=l/2;
	goto l3;
l4:j=j+l;
	}

/**** In-place transformation ****/

pi=3.141593;
for (i=1; i <=FFT_M; i++) {
	lim1=1<<(i-1);		/** 2^(i-1) **/
	lim2=1<<(FFT_M-i);		
	for (l=1; l <= lim2; l++) {
		for (r=1; r <= lim1; r++) {
			lim3=(r-1)+(l-1)*2*lim1+1;
			b1=xr[lim3];
			b2=xi[lim3];
			c1=xr[lim3+lim1];
			c2=xi[lim3+lim1];
			arg=2*pi*(r-1)*lim2/FFT_N;
			cos1=cos(arg);
			sin1=sin(arg);
			x1=c1*cos1+c2*sin1*t;
			x2=-c1*sin1*t+c2*cos1;
			xr[lim3]=b1+x1;
			xi[lim3]=b2+x2;
			xr[lim3+lim1]=b1-x1;
			xi[lim3+lim1]=b2-x2;
			}
		}
	}
for (k=1; k<= FFT_N; k++) {
	xr[k]=xr[k]/d;
	xi[k]=xi[k]/d;
	}
}

/***********************************************************/

void CalcFFT()
{
int k;

/**** Define input data ****/

for (k=1; k <= FFT_N; k++) {
	xr[k]=(double)data[k-1]/32768.0;	/** range [-1,1] **/
	xi[k]=0.0;
	}

CalcANYFFT(1);
}

/***********************************************************/

void CalcINVFFT()
{
int k;

CalcANYFFT(-1);

/**** Restore short data ****/

for (k=1; k <= FFT_N; k++) {
   data[k-1]=xr[k]*32768.0;   /** xi[k] should be zero **/
	}
}

/***********************************************************/

void ShiftUpFFT(hz)
int hz;
{
char buf[80]; 
int k;
int i;

i=hz/globrate;

/*sprintf(buf,"shift up i %d\n",i);
kprintf(buf);*/

/** Shift 1st half of FFT to right **/

for (k=FFT_N/2; k>i; k--) {
   xr[k]=xr[k-i];
   xi[k]=xi[k-i];
   }

for (k=1; k<=i; k++) {
   xr[k]=0.0;
   xi[k]=0.0;
   }

/** second half of FFT is mirror of 1st half **/
/** shift to left **/

for (k=FFT_N/2+1; k<=FFT_N-i; k++) {
   xr[k]=xr[k+i];
   xi[k]=xi[k+i];
   }

for (k=FFT_N-i+1; k<=FFT_N; k++) {
   xr[k]=0.0;
   xi[k]=0.0;
   }

}

/***********************************************************/
/** E.G: percent = 105 to shift up 5%, or 95 to shift down 5% **/
/***********************************************************/

void ScaleUpFFT(percent)
int percent;
{
char buf[80]; 
static double tr[FFT_N+1];
static double ti[FFT_N+1];
int k;
int i;

/** Zero out temporary array **/

for (k=1; k <=FFT_N; k++) {
	tr[k]=0.0;
	ti[k]=0.0;
	}

/** Shift 1st half of FFT to right && second half to left **/

for (k=1; k <=FFT_N/2; k++) {
	i=(k*percent)/100;
	if (i <= FFT_N/2) {
	   tr[i]+=xr[k];
	   ti[i]+=xi[k];
		/** second half of FFT is mirror of 1st half **/
		/** shift to left **/
	   tr[FFT_N-(i-1)]+=xr[FFT_N-(k-1)];
	   ti[FFT_N-(i-1)]+=xi[FFT_N-(k-1)];
		}
   }

/** Copy shifted data to orignal array **/

for (k=1; k <=FFT_N; k++) {
	xr[k]=tr[k];
	xi[k]=ti[k];
	}
}


/***********************************************************/

#define GR_X 20
#define GR_Y 20
#define GR_H 100

void PlotInput()
{
int i;

SetDrMd(fft_win->RPort, JAM1);
RectFill(fft_win->RPort, GR_X, GR_Y, GR_X+FFT_N,GR_Y+GR_H);

SetDrMd(fft_win->RPort, COMPLEMENT);

for (i=0; i < FFT_N; i++) {
	Move (fft_win->RPort, i+GR_X, GR_Y+GR_H/2);
	Draw (fft_win->RPort, i+GR_X, GR_Y+GR_H/2-(data[i]*(GR_H/2))/32768);
	}
}

/***********************************************************/
void PlotFFT()
{
double max,mag,min;
int k;
char buf[80];

max=0.0;
min=10000.0;

for (k=1; k <= FFT_N; k++) {
	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);
	if (mag > max)
		max=mag;
	if (min > mag)
		min=mag;
	}

/******
sprintf(buf,"mag %f min %f max %f\n",mag,min,max);
kprintf(buf);

for (k=1; k <= FFT_N; k++) {
	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);
	if (mag > max/4) {
		sprintf(buf,"peek of %f at %d HZ\n",mag, (int)(globrate*(double)k));
		kprintf(buf);
		}
	}
******/
/* max=10.0*min;  FOR ZOOM VIEW ****/

SetDrMd(fft_win->RPort, JAM1);
RectFill(fft_win->RPort, GR_X, 2*GR_Y+GR_H, GR_X+FFT_N,2*(GR_Y+GR_H));

SetDrMd(fft_win->RPort, COMPLEMENT);

for (k=1; k <= FFT_N; k++) {
	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);
	if (mag > max)
		mag=max;
	Move (fft_win->RPort, k+GR_X, 2*(GR_Y+GR_H));
	Draw (fft_win->RPort, k+GR_X, 2*(GR_Y+GR_H)-(int)((mag*GR_H)/max));
	}
}
