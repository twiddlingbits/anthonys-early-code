#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#define FFT_N 512
#define FFT_M 9			/** 2^M = FFT_N  **/
#define GR_X 20
#define GR_Y 18
#define GR_TH 50
#define GR_FH 100

static struct NewWindow NewWindowStructure1 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   GR_X*2+FFT_N,GR_TH+GR_FH+GR_Y*3,   /* window width and height */
   BLACK,WHITE,   /* detail and block pens */
   CLOSEWINDOW+MOUSEMOVE,   /* IDCMP flags */
   WINDOWDRAG+WINDOWDEPTH+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH+REPORTMOUSE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   0,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   340,180,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct Window *fft_win;
double maxmag, minmag;
long maxbin;

extern struct StudioBase *StudioBase;

short *ReadFFTData();
struct Window *OpenNewWind();
void CalcSNR();
void CalcFFT();
void CalcINVFFT();
void CalcANYFFT();
void PlotInput();
void PlotFFT();
void PlotBin();

double sin(double);
double cos(double);
double sqrt(double);

/***********************************************************/

int DoFFTEdit(ei)
struct EditInstance *ei;
{
struct IntuiMessage *message;
ULONG class;
USHORT code,crosson;
char titlebar[80];

if (ReadFFTData(ei->sfp, ei->start_mark)==0)
	return(READ_ERROR);

sprintf(titlebar,"FFT of %s [%d,%d]",StrFileName(ei->samp_name),ei->start_mark,ei->start_mark+FFT_N-1);
NewWindowStructure1.Title=titlebar;
NewWindowStructure1.Screen=StudioBase->screen;
fft_win=OpenNewWind(&NewWindowStructure1,NULL, NULL,NULL);
if (fft_win==NULL) {
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);
PlotInput();
CalcFFT();
PlotFFT(ei);
crosson=InFFT(fft_win->MouseX,fft_win->MouseY);
if (crosson)
	new_pointer(PTR_CROSS);
else
	new_pointer(PTR_NORMAL);

while(TRUE) {
	WaitPort(fft_win->UserPort);
	while ((message=(struct IntuiMessage *)GetMsg(fft_win->UserPort))!=0) {
		HandleIntuitMess(fft_win,message);

		class=message->Class;
		code=message->Code;
		ReplyMsg(message);

		if (class==CLOSEWINDOW) {
			CloseWind(fft_win);
			return(NO_ERROR);
			}

		else if (class==MOUSEMOVE) {
			if (InFFT(fft_win->MouseX,fft_win->MouseY)) {
				if (!crosson) {
					new_pointer(PTR_CROSS);
					crosson=TRUE;
					}
				PlotBin(fft_win->MouseX); 
				}
			else if (crosson) {
				new_pointer(PTR_NORMAL);
				crosson=FALSE;
				}
			}
		}
	}
}

/***********************************************************/

static double xr[FFT_N+1],xi[FFT_N+1];
static double globrate;
static short data[FFT_N];

/***********************************************************/

short *ReadFFTData(struct SampleFilePointer *sfp, long start)
{
SampleSeek(sfp,start,SS_OFFSET_BEGINNING);
if (SampleRead(sfp,data,FFT_N)==FFT_N) {
	globrate=(double)sfp->tag.parms.rate/(double)FFT_N;
	return(data);
	}
else {
	return(0);
	}
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

pi=3.14159265359;
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
			arg=2*pi*(r-1)*(double)lim2/(double)FFT_N;
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

void PlotInput()
{
int i;

SetDrMd(fft_win->RPort, JAM1);
draw_indent(fft_win->RPort, GR_X-2, GR_Y-1, GR_X+FFT_N+2,GR_Y+GR_TH+1,
	StudioBase->defaults.colors.list.topleft_3d,
	StudioBase->defaults.colors.list.bottomright_3d);

SetDrMd(fft_win->RPort, COMPLEMENT);

for (i=0; i < FFT_N; i++) {
	Move (fft_win->RPort, i+GR_X, GR_Y+GR_TH/2);
	Draw (fft_win->RPort, i+GR_X, GR_Y+GR_TH/2-(data[i]*(GR_TH/2))/32768);
	}
}

/***********************************************************/

void PlotFFT(ei)
struct EditInstance *ei;
{
double mag;
int k;
char buf[80];

maxmag=0.0;
minmag=10000.0;

for (k=1; k <= FFT_N/2; k++) {
	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);
	if (mag > maxmag) {
		maxmag=mag;
		maxbin=k;
		}
	if (minmag > mag)
		minmag=mag;
	}

/******
sprintf(buf,"mag %f minmag %f maxmag %f\n",mag,minmag,maxmag);
kprintf(buf);

for (k=1; k <= FFT_N; k++) {
	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);
	if (mag > maxmag/4) {
		sprintf(buf,"peak of %f at %d HZ\n",mag, (int)(globrate*(double)k));
		kprintf(buf);
		}
	}
******/
/* maxmag=10.0*minmag;  FOR ZOOM VIEW ****/

SetDrMd(fft_win->RPort, JAM1);
draw_indent(fft_win->RPort, GR_X-2, 2*GR_Y+GR_TH-1, GR_X+FFT_N+2,2*GR_Y+GR_TH+GR_FH+1,
	StudioBase->defaults.colors.list.topleft_3d,
	StudioBase->defaults.colors.list.bottomright_3d);

SetDrMd(fft_win->RPort, JAM2);
SetAPen(fft_win->RPort,StudioBase->defaults.colors.list.draw_selected);
SetBPen(fft_win->RPort,StudioBase->defaults.colors.list.hitbox_unselected);

for (k=1; k <= FFT_N/2; k++) {
	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);
	if (mag > maxmag)
		mag=maxmag;
	Move (fft_win->RPort, bintox(k), 2*GR_Y+GR_TH+GR_FH);
	Draw (fft_win->RPort, bintox(k), 2*GR_Y+GR_TH+GR_FH-(int)((mag*GR_FH)/maxmag));

	Move (fft_win->RPort, bintox(k)+1, 2*GR_Y+GR_TH+GR_FH);
	Draw (fft_win->RPort, bintox(k)+1, 2*GR_Y+GR_TH+GR_FH-(int)((mag*GR_FH)/maxmag));
	}

Move (fft_win->RPort, bintox(1), 2*GR_Y+GR_TH+GR_FH+9);
Text(fft_win->RPort, "0 Hz",4);

sprintf(buf,"%d Hz",ei->sfp->tag.parms.rate/2);
Move (fft_win->RPort, bintox(FFT_N/2)-strlen(buf)*8, 2*GR_Y+GR_TH+GR_FH+9);
Text(fft_win->RPort, buf,strlen(buf));

sprintf(buf,"Bin Size %5.2fHz Peak %-7.1fHz",globrate,globrate*(double)maxbin);
Move (fft_win->RPort, bintox(FFT_N/2)-256, 2*GR_Y+GR_TH+9);
Text(fft_win->RPort, buf,strlen(buf));
PlotBin(fft_win->MouseY);
}

/***********************************************************/
/** Doesn't work **/

void CalcSNR()
{
double mag,sum;
int k,sigbin;
/**char buf[80];**/

maxmag=0.0;
sum=0.0;

for (k=1; k <= FFT_N/2; k++) {
/*	mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k]);*/
	mag=xr[k]*xr[k]+xi[k]*xi[k];
	sum=sum+mag;
	if (mag > maxmag) {
		maxmag=mag;
		sigbin=k;
		}
	}
/***
sprintf(buf,"max %f sum %f noise %f\n",maxmag,sum,sum-maxmag);
kprintf(buf);
***/
}

/***********************************************************/

bintox(k)
{
return(2*(k-1)+GR_X);
}

/***********************************************************/

void PlotBin(y)
{
int k;
char buf[80];
double mag;

k=(y-GR_X)/2+1;
if (k < 1) k=1;
if (k > FFT_N/2) k=FFT_N/2;

SetDrMd(fft_win->RPort, JAM2);
SetAPen(fft_win->RPort,StudioBase->defaults.colors.list.draw_selected);
SetBPen(fft_win->RPort,StudioBase->defaults.colors.list.hitbox_unselected);
mag=sqrt(xr[k]*xr[k]+xi[k]*xi[k])/maxmag;
sprintf(buf,"Bin %3d %7.1f Hz %4.3f Mag",k,globrate*(double)k,mag);
Move (fft_win->RPort, bintox(FFT_N/2)-256, 2*GR_Y+GR_TH+17);
Text(fft_win->RPort, buf,strlen(buf));
}

/***********************************************************/

InFFT(x,y)
{
return(x>=GR_X && y>= 2*GR_Y+GR_TH && x<= GR_X+FFT_N && y<=2*GR_Y+GR_TH+GR_FH);
}

/***********************************************************/
