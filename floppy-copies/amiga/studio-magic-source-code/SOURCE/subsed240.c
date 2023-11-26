/* subsed240.c 1-27-87 Milburn*/
#include "sed.include.h"
#include "sed.externals"
void ReallyNew();
void New();
float flabs();
int inabs();

typedef struct {
	ULONG	oneShotHiSamples,	/* # samples in the high octave 1-shot part */
			repeatHiSamples,	/* # samples in the high octave repeat part */
			samplesPerHiCycle;	/* # samples/cycle in high octave, else 0   */
	UWORD	samplesPerSec;		/* data sampling rate */
	UBYTE	ctOctave,			/* number of octaves of waveforms */
			sCompression;		/* data compression technique used */
	ULONG	volume;				/* playback volume from 0 to Unity */

	} Voice8Header;
extern Voice8Header head;
/* complexf.h */
#define cmplx( real_part , imaginary_part , x)\
	x.re = (float)real_part;\
	x.im = (float)imaginary_part;
#define add( x , y , z )\
	z.re = x.re + y.re;\
	z.im = x.im + y.im;
#define subtract( x , y, z)\
	z.re = x.re - y.re;\
	z.im = x.im - y.im;
#define multiply( x , y , z)\
	z.re = x.re * y.re - x.im * y.im;\
	z.im = x.re * y.im + x.im * y.re;

	
long zero_search();
#define MAXSEARCH 500
int mysgn();
void set_edit();
void sot_edit();
long interpolate();
long compress();
long expand();
void do_compress();
void do_expand();
void do_gravy();
extern float get_fvalue();
extern int get_ivalue();

digitze (Window,lp,rp,rate,maxnum,chan)
char *rp,*lp;
int rate,maxnum,chan;
struct Window *Window;
{
register char *reg1,*reg2;
register ULONG k;
static int i;
static UBYTE *data,*direction,*pa,*direction2,*data2;

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
pa=(UBYTE *)0xBFE001;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

reg1=lp;
reg2=rp;
*direction=0;           /* all bits are read */
*data=0;
*direction2=6;  /* pa1 and pa2 write */
*data2=chan;
post("Left button to start");
listen(chan);
unpost();
post("Left button to stop");
if ((*pa&64)==0) while((*pa&64)==0) ;
for (k=0;k<20;k++) ;    /* skip key bbbounce */
Disable();
k=0;
if (chan!=STEREO)
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);   /* delay loop */
      *reg1++=(*data)-128;
      }
else {
   rate=rate/2;
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);    /* delay loop */
      *data2=RIGHT;
      *reg2++=(*data)-128;
      *data2=LEFT;
      *reg1++=(*data)-128;
      }
   }
if(chan==STEREO) {
	buffer_toggle();
	endsamp+=(int)(k-1);
	buffer_toggle();
	}
unpost();
Enable();
return((int)(k-1));
}

/*********************************************/
/***        listen to digitizer            ***/
/*********************************************/

listen (chan)
int chan;
{
static UBYTE *base,*pa,*data,*direction,*data2,*direction2;
static unsigned short sam,i;
unsigned short *aud0lch,*aud0len,*aud0vol,*aud0per,*aud0dat,*dmaconw;
unsigned short *aud1lch,*aud1len,*aud1vol,*aud1per,*aud1dat;
unsigned short *intreq,*intreqr;
unsigned int *aud0lc,*aud1lc;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

*direction=0;           /* all bits are read */
*data=0;
*direction2=2+4;      /* pa1 & pa2 write */
Disable();

base=(UBYTE *)0xDEF000;
/* channel 0 chip addresses */
dmaconw=(unsigned short *)(base+0x096);
aud0lch=(unsigned short *)(base+0x0A0);
aud0len=(unsigned short *)(base+0x0A4);
aud0vol=(unsigned short *)(base+0x0A8);
aud0per=(unsigned short *)(base+0x0A6);
aud0dat=(unsigned short *)(base+0x0AA);
aud0lc=(int *)aud0lch;
/* channel 1 chip addresses */
aud1lch=(unsigned short *)(base+0x0B0);
aud1len=(unsigned short *)(base+0x0B4);
aud1vol=(unsigned short *)(base+0x0B8);
aud1per=(unsigned short *)(base+0x0B6);
aud1dat=(unsigned short *)(base+0x0BA);
aud1lc=(int *)aud1lch;
intreq=(unsigned short *)(base+0x09C);
intreqr=(unsigned short *)(base+0x1E);
pa=(UBYTE *)0xBFE001;

*aud0vol=64;
*aud0per=1;
*aud1vol=64;
*aud1per=1;
i=0;

if (chan==STEREO)
   while ((*pa&64)==64) {
      *data2=LEFT;
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud0dat=i;
      *intreq=128;    /* not in manual, but required to resart ch 0 */

      *data2=RIGHT;
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud1dat=i;
      *intreq=256;    /* needed to restart ch 1 */
      }
*data2=chan;
if (chan==RIGHT)
   while ((*pa&64)==64) {
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud1dat=i;
      *intreq=256;
      }

if (chan==LEFT)
   while ((*pa&64)==64) {
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud0dat=i;
      *intreq=128;
      }
while ((*pa&64)!=64) ;     /* wait for left mouse button release */
Enable();
}

a_m()
{
int shit;
shit=get_ivalue("Modulation period:","Period:",2,100,2);
if(shit==-1) return;
frsize=shit;
feedback=get_fvalue("Modulation Depth:","Depth:",.70,.99,feedback);
if(feedback==-1.) return;
post("Crunching");
make_table(shit,(float)(1.-feedback),1.);
k=0;
for(l=begsamp;l<endsamp;l++) {
	shit=(buffer[l]*table[k]);
	if(shit<0) buffer[l]=0-(0-shit)>>6;
	else buffer[l]=shit>>6;
	if(++k==frsize) k=0;
	}
unpost();
}

make_table(size,lo,hi)
int size;
float lo,hi;
{
float a,b,tt;
int l;
a=(float)(6.283)/(float)size;
if(size>MAXTABLESIZE) size=MAXTABLESIZE;
b=(hi-lo)/2.;
for(l=0;l<size;l++) {
	FloatJunk=(float)a*(float)l;
	tt=(float)sin(FloatJunk)*b+lo+b;
	table[l]=(int)(tt*64);
	}
}

int inabs(shit)
int shit;
{
if(shit<0) shit=0.-shit;
return(shit);
}

void fft(isi,nl,s)
int isi,nl;
char *s;
{
complex w,t;
int n,nv2;
register int i,j,k,l,m;
double pi,theta;
int mid=63;
int cx=53;
int amp;
int inc;
float mx;
static UBYTE *pa;

pi=3.1415926535897932384;
pa=(UBYTE *)0xBFE001;
post("Crunching");
for(k=0;k<nl;k++) {
	ss[k].re=(float)s[k];
	ss[k].im=0;
	}
n=nl;
nv2=n/2;
/* do bit reversal */
j=0;
for(i=0;i<n;++i) {
	if(i<j) {
		t=ss[j];
		ss[j]=ss[i];
		ss[i]=t;
	}
	k=nv2;
	while(j>=k) {
		j-=k;
		k=(k+1)/2;
	}
	j+=k;
}
/* do butterfly */
pi*=(double)isi;
k=1;
while(k<n) {
	j=k*2;
	for(m=1;m<=k;++m) {
		l=m-1;
		theta=(double)(pi*((double)l/(double)k));
		cmplx(cos(theta),sin(theta),w); /* w.re=cos(theta) */
		for(i=l;i<n;i+=j) {
			l=i+k;
			multiply(w,ss[l],t); /* complex mult */
			subtract(ss[i],t,ss[l]);
			add(ss[i],t,ss[i]);
		}
	}
	k=j;
}
if(isi>0) {
	theta=(double)n;
	theta=1./theta;
	pi=0.;
	cmplx(theta,0.,w);
	for(i=0;i<n;++i) {
		multiply(w,ss[i],ss[i]);
	}
}
mx=0;
for(k=0;k<nl/2;++k) {
	if(flabs(ss[k].re)>mx) mx=flabs(ss[k].re);
	}
SetAPen(rastPort,YELLOW);
Move(rastPort,20,mid);
Draw(rastPort,620,mid);

SetAPen(rastPort,WHITE);
RectFill(rastPort,20,11,620,66);
SetAPen(rastPort,BLACK);
Move(rastPort,20,11);
Draw(rastPort,620,11);
Draw(rastPort,620,66);
Draw(rastPort,20,66);
Draw(rastPort,20,11);
for(k=0;k<nl/2;k++) {
	amp=mid-((flabs(ss[k].re)/mx)*60.+1);
	if(amp<10) amp=10;
   Move(rastPort,cx,mid); 
   Draw(rastPort,cx,amp);
   Move(rastPort,cx+1,mid);
   Draw(rastPort,cx+1,amp);
   cx=cx+2;
}
FNText.FrontPen=BLACK;
FNText.BackPen=WHITE;
n=stcu_d(TempStr,0,4);
SPrint(rastPort,TempStr,24,20);
n=stcu_d(TempStr,sr/4,6);
SPrint(rastPort,TempStr,290,20);
n=stcu_d(TempStr,sr/2,6);
SPrint(rastPort,TempStr,580,20);
FNText.FrontPen=WHITE;
FNText.BackPen=BLUE;
buffer_toggle();
unpost();
if(cue("Do you want a detailed prinout?")!=0) {
	post("Printing");
	fp=fopen("PRT:","w");
	if(fp) {
	fprintf(fp,"Frequency     Amplitude\n");
fprintf(fp,"---------------------------------------------------------------------------------");

	for(k=1;k<nl/2;k++) {
		maxamp=(flabs(ss[k].re)/mx)*60.+1;
		FloatJunk=(float)k/(float)(nl>>1);
		n=FloatJunk*sr/2.;
		n=stcu_d(OutStr,n,6);
		strcat(OutStr,"            \0");
		OutStr[11]='|';
		n=flabs(ss[k].re)*65./mx;
		for(m=12;m<12+n;m++) OutStr[m]='*';
		OutStr[n+12]=0; /*eo string */
		printf("%s\n",OutStr);
		fprintf(fp,"\n%s",OutStr);
		}
	fclose(fp);
	}
	else printf("Printer error\n");
	unpost();
	}
}
void ReallyNew()
{
nsamps0=2;
nsamps1=2;
nsamps=2;
begsamp=0;
endsamp=2;
for(k=0;k<nsects;k++) {
	section[k]=section_defaults;
        RefreshGadgets(&SectionGadget[0],Window,NULL);
	}
for(k=0;k<8;k++) 
	SPrint(rastPort,SpaceStr,212,90+14*k,10);

/* also some flags to be fixed here? */
hist(begsamp,nsamps);
update_sliders();
}

void insert()
{
	           	/* insert */
                    	paste_start = (LimitInfo[lastmoved].HorizPot*nsamps/65535.);
			paste_end = paste_start+(copy_end-copy_start);
			post("Crunching");
			sot_edit();
			close_midi();
			preserve(paste_start,nsamps);
           		right_slide(paste_start,(copy_end-copy_start));
       		if(paste_start<copy_start && current_buffer == copy_buffer)
         			{
         			/* we just slid it over !!!! */
         			copy_start=copy_start+(copy_end-copy_start);
         			copy_end=copy_end+(copy_end-copy_start);
         			}
           		paste();   
			open_midi();
			unpost();
           		hist(begsamp,nsamps);      
			begsamp=0;
			endsamp=nsamps;
          		update_sliders();
           		return;   

}

void bevel()
{
			/* bevel */
FloatJunk=get_fvalue("Ramp duration (secs):","Ramp dur:",(float)(8./sr),
(float)(((endsamp-begsamp)/2.)/(float)sr),8./sr);
			if(FloatJunk==-1) return;
			frsize=FloatJunk*sr+.5;
			rescale=0.;
			FloatJunk=1./frsize;
			post("Crunching");
			preserve(begsamp,endsamp);
			for(l=begsamp;l<begsamp+frsize;l++)
				{
				buffer[l]=buffer[l]*rescale;
				rescale=rescale+FloatJunk;
				}
			rescale=0;
			for(l=endsamp;l>endsamp-frsize;l--)
				{
				buffer[l]=buffer[l]*rescale;
				rescale=rescale+FloatJunk;
				}
			unpost();
			hist(begsamp,nsamps);
return;
}

long zero_search()
{
int sold,snew,mm;
long ll,newes;
/*
mm=MAXSEARCH;
if(where+mm>nsamps) mm=nsamps-where;
sold=buffer[where];
for(ll=where+2;ll<where+mm;ll=ll+2) { 
	snew=buffer[ll];
	if((mysgn(snew)==-1) && (mysgn(snew) != mysgn(sold))) return(ll);
	sold=snew;
	}
*/
FloatJunk=(1000000./(period[current_section][57]*.279365));
ll = (FloatJunk/440.)+.5; /* this is samples per cycle */
mm=1;
while(pow(2.0,(double)mm)<ll) mm++;
mm--;
ll=(pow(2.0,(double)mm)+.5);
head.samplesPerHiCycle=ll;
newes=begsamp+ll;
while(newes<endsamp) newes+=ll;
endsamp=newes;
return(endsamp);
}

int mysgn(x)
int x;
{
if(x<0) x=-1;
else x=1; /* make zero a one case for the crossing */
return(x);
}



long interpolate(oldbeg,oldend,xx)
long oldbeg,oldend;
float xx;
   {
   long newbeg,newend,k,l;
   float temp;
   newbeg=oldbeg;
   newend=oldbeg+(oldend-oldbeg)*xx;
   post("Crunching");
   if (newend>nsamps)
      nsamps=newend;
   if(xx < 1)
      {
	xx=1./xx;
	temp=newbeg;
	for(k=newbeg;k<newend;k++) {
		buffer[k]=buffer[(int)temp];
		temp+=xx;
		}
       left_slide(oldend,oldend-newend);   
       }
    else
      {
      right_slide(oldend,newend-oldend);   
      k=newend;
      temp=oldend;
      xx=1./xx;
	for(k=newend;k>newbeg;k--) {
         temp-=xx;
         l=temp;
/*         buffer[k] = buffer[l] + ((buffer[l+1]-buffer[l])*(temp-l));*/
	buffer[k]=buffer[l];
         }
      }
    endsamp=newend;
unpost();
return(newend);
}


long compress()
	{
	int iptin,iptout,oldin;
	int atk,inincr;
	float f1;
	copy_start=begsamp;
	copy_end=endsamp;
	iptin = copy_start;
	iptout = copy_start;
	inincr = frsize/durmult;
	atk=frsize/4.;
	while((iptin + frsize) < copy_end)
		{
		oldin=iptin;
		l=iptin+frsize-atk;
		while(iptin<l)
			buffer[iptout++]=buffer[iptin++];
		l=iptin+atk;
		while(iptin<l)
			{
			f1=((float)(l-iptin)/atk);
			iptin++;
			buffer[iptout++]=buffer[iptin]*f1 +
					 buffer[iptin+inincr]*(1.-f1);
			}
		iptin=oldin+inincr;
		}
	l=iptout;
	return(l);	
	}

long expand()
	{
	int iptin,iptout,oldin;
	int atk,inincr;
	float f1,f2;
	copy_start=begsamp;
	copy_end=endsamp;
	l=(copy_end-copy_start)*durmult;
	if(copy_start+l>bufsiz)
		{
		FloatJunk=(bufsiz-copy_start);
		durmult=FloatJunk/(copy_end-copy_start);
		l=bufsiz-copy_start;
		}
	iptin = copy_end;
	iptout = copy_start+l;
	right_slide(copy_end+1,l-(copy_end-copy_start));
	inincr = frsize/durmult;
	atk=frsize/40.;
	while((iptout - frsize) > copy_start)
		{
		oldin=iptin;
		l=iptin-frsize+atk;
		while(iptin>l)
			buffer[iptout--]=buffer[iptin--];
		l=iptin-atk;
		while(iptin>l)
			{
			f1=((float)(iptin-l)/atk);
			iptin--;
			buffer[iptout--]=buffer[iptin]*f1 +
					 buffer[iptin-inincr]*(1.-f1);
			}
		iptin=oldin-inincr;
		}
	l=(copy_end-copy_start)*durmult+copy_start;
	copy_start=-2;
	copy_end=-1;
	return(l);
	}

void sot_edit()
{
orig=buffer;
dest=buffer;
	if(current_buffer != copy_buffer)
		{
		buffer_toggle();
		orig=buffer;
		buffer_toggle();
		}
}

void do_compress()
{
frsize=get_ivalue("Frame size:","Frame size:",12,500,80);
if(frsize == -1) return;
durmult=get_fvalue("Compressed duration:","Comp. dur:",.02,duration-.001,duration/2.);
if(durmult==-1) return;
durmult=durmult/duration;
post("Crunching");
preserve(begsamp,nsamps);
close_midi();
endsamp=compress();
unpost();	
copy_start=-2;
copy_end=-1;
hist(begsamp,nsamps);
update_sliders();		
open_midi();
}

void do_expand()
{
	frsize=get_ivalue("Frame size","Frame size:",12,500,80);
	if(frsize==-1) return;
   FloatJunk=((float)(bufsiz-nsamps)+sr*duration)/(sr*duration);
	FloatJunk=FloatJunk*duration;
durmult=get_fvalue("Expanded duration:","Exp. dur:",duration+.001,FloatJunk,FloatJunk/2.);
	if(durmult==-1) return;
	durmult=durmult/duration;
	     post("Crunching");
		     close_midi();
		     endsamp=expand();
		     if(endsamp>nsamps)
			nsamps=endsamp;	
		     unpost();	
		     hist(begsamp,nsamps);
		     update_sliders();		
		     open_midi();
}

void do_gravy()
{
			frsize=get_ivalue("Frame size:","Frame size:",12,500,80);
			if(frsize==-1) return;
			/* gravy */
			n=get_pitch_value("Choose an interval from middle C");
			if(n==-1) return;
                     FloatJunk=(n-60.)/12.;
                     xmult=1./pow(2.0,FloatJunk);
		     durmult=1./xmult;	
		     close_midi();
			preserve(begsamp,nsamps);
		     rescale=durmult; /* just a temp variable */
                     endsamp=interpolate(begsamp,endsamp,xmult);
		     if(durmult<1)
			{
			post("Compressing");
	               endsamp=compress();
			}
		     else
			{
			post("Expanding");
			endsamp=expand();
			}
		     if(nsamps<endsamp)
			nsamps=endsamp;
		     unpost();
		copy_start=0;
		copy_end=2;
		     hist(begsamp,endsamp);
		     update_sliders();	
		     open_midi();				
			durmult=rescale;
}

float flabs(xx)
float xx;
{
if(xx<0) xx=0.-xx;
return(xx);
}
