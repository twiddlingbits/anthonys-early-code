/*
	These routines provide limited IFF support for FutureSound.
	They will read and write single octave one-shot sounds.

	Additional octave and repeat support may be added later.

	Based on the "8SVX" IFF 8-Bit Sampled Voice document dated
	Febuary 7, 1985.

	Routines:

		ULONG SizeIFF(Filename)
		char *Filename;		File to open 

			returns size of sample or 0 if error.


		UWORD ReadIFF(Filename,Buffer)
		char *Filename;		File to read
		BYTE *Buffer;       Buffer to read sample into

			returns the record rate;

	Dan Lovy    May 1986

	Modified for use with FutureSound auxiliary routines - June 1986
		by Ralph Hebb 
*/

/* for comments see IFF spec,  I'm too tired to type them in now */

#include <stdio.h>
#include <fcntl.h>
#include <exec/types.h>
#include "SoundErrors.h"
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

typedef LONG Fixed;

#define Unity 0x10000L
#define sCmpNone		0
#define sCmpFibDelta	1
#define SUSTAIN_FLAG 16
#define NEND 100 /* dumb silence at end of sustain for third loop part */

typedef struct {
	ULONG	oneShotHiSamples,	/* # samples in the high octave 1-shot part */
			repeatHiSamples,	/* # samples in the high octave repeat part */
			samplesPerHiCycle;	/* # samples/cycle in high octave, else 0   */
	UWORD	samplesPerSec;		/* data sampling rate */
	UBYTE	ctOctave,			/* number of octaves of waveforms */
			sCompression;		/* data compression technique used */
	ULONG	volume;				/* playback volume from 0 to Unity */

	} Voice8Header;
long ReadIFF();
extern int hist();
extern int nu_Save_IFF();
#define MAXSECT 16
extern short period[MAXSECT][100];
extern short key_section[100];
extern long m;
extern int k;
extern float FloatJunk;
long IFF_begsamp,IFF_endsamp;
BYTE *IFF_buffer;
long fs,es,ls;
void CreateIFF();
Voice8Header head;
extern long LongJunk,nsamps,bufsiz,begsamp,endsamp;
extern long l,paste_start,paste_end,copy_start,copy_end,dif;
extern BYTE *orig,*dest,*buffer;
extern char current_buffer;
char oneshot=1;
#define Unity 0x10000L
extern int current_section,sr;
extern char comp;
extern void sot_edit();

#define MAXNAME 10
struct section
   {
   int firstsamp,lastsamp;
   int loopstart,loopend;   
   int flags;
   int frsize;
   int currbuf;   
   struct IntuiText *text;
   char name[MAXNAME];
   };
#define MAXSECT 16
extern struct section section[MAXSECT];
extern struct section section_defaults;
extern char RString[41];


ULONG SizeIFF(Filename)
char *Filename;		/* File to open */
/* returns size of sample or 0 if error. */
{
	FILE *fp;
	char *ckname = "    ";
	LONG dummy;
	Voice8Header Header;
	int k;

	if (fp = fopen(Filename,"r"))
	{
		fread(ckname,4,1,fp);
		if (strcmp(ckname,"FORM")==0)
		{
			fread(ckname,4,1,fp);	/* not a mistake! */
			fread(ckname,4,1,fp);
			if (strcmp(ckname,"8SVX")==0)
			{
				get_to("VHDR",fp);
				fread(&dummy,sizeof(LONG),1,fp);
				fread(&Header,sizeof(Header),1,fp);
				fclose(fp);
		dummy=Header.oneShotHiSamples+Header.repeatHiSamples;
		for(k=1;k<=Header.ctOctave;k++) dummy*=2;
			return(dummy);
			}
			else
			{
				fclose(fp);
				return(0);		
			}
		}
		else
		{
			fclose(fp);
			return(0);		
		}
	}
	else
	{
		return(0);
	}
}

long ReadIFF(Filename,IFFBuf0,bg)
char *Filename;		/* File to read */
BYTE *IFFBuf0;			/* Buffer to read sample into */
long bg; /* begsamp, max endsamp */
/* returns the record rate */
{
	FILE *fp;
	char *ckname = "    ";
	LONG dummy;
	char datname[20];
	Voice8Header Header;
	int i,k,n,nn;
	long templ;

	if (fp = fopen(Filename,"r"))
	{
		fread(ckname,4,1,fp);
		if (strcmp(ckname,"FORM")==0)
		{
/*			fread(ckname,4,1,fp);
			fread(ckname,4,1,fp); */
			if(get_to("8SVX",fp)==-1) {
				fclose(fp);
				return;
				}
			else
			{
				get_to("VHDR",fp);
				fread(&dummy,sizeof(LONG),1,fp);
				fread(&Header,sizeof(Header),1,fp);
				head=Header;
				show_head();
				l=Header.oneShotHiSamples+Header.repeatHiSamples;
				strcpy(datname,"IFF");
				get_to("NAME",fp);
				if(!feof(fp)) {
					fread(&dummy,sizeof(LONG),1,fp);
					if(dummy>19) dummy=19;
					fread(datname,dummy,1,fp);
					datname[19]='\0';
					} 
				fclose(fp);
				fp=fopen(Filename,"r");
				fread(ckname,4,1,fp);
				fread(ckname,4,1,fp);
				fread(ckname,4,1,fp);
				get_to("BODY",fp);
				fread(&dummy,sizeof(LONG),1,fp);

				for(k=0;k<Header.ctOctave;k++) {
					nn=-1;
					for(n=MAXSECT;n>=0;n--) {
						if((section[n].lastsamp)<=2) nn=n;
						}
					if((bg+l)>=bufsiz) {
						cue("Not enough room!\n");
						fclose(fp);
						return;
						}
					fread(&IFFBuf0[bg],l,1,fp);
					
					if(nn!= -1) {
					section[nn].firstsamp=(int)bg;
section[nn].loopstart=(int)(bg+l-((Header.repeatHiSamples)<<k));
			section[nn].loopend=(int)(bg+l);
				section[nn].lastsamp=(int)(bg+l+NEND);
					section[nn].currbuf=(int)current_buffer;
					section[nn].flags=SUSTAIN_FLAG;
					strncpy(section[nn].name,datname,MAXNAME-1);
for(i=(24+Header.ctOctave*12-k*12);i<(36+Header.ctOctave*12-k*12);i++)	
				key_section[i]=nn;	
	for(i=0;i<100;i++) period[nn][i]=(period[nn][i]*4)>>k; /* this tune? */
					} /* enf of nn!=-1 */
					bg=bg+l+NEND;
					for(i=0;i<NEND;i++) buffer[bg-i]=0;
					l=l<<1;
					}
				fclose(fp); /*enable key_split flag  later */
				sr=Header.samplesPerSec;
				return(bg);
			}
		}
		else
		{
			fclose(fp);
			return(IFF_ERROR);		
		}
	}
	else
	{
		return(OPEN_ERROR);
	}
} 

/* this routine walks through the iff file looking for chunks */
int get_to(ckname,fp)
char *ckname;
FILE *fp;
{
	char c;
	long  skip,i,dummy;
	int n,ic;
	n=0;
	ic=0;
	while(!feof(fp)) {
		fread(&c,1,1,fp);
		ic++;
		if(c != ckname[n]) n=0;
		else {
			if(++n >= strlen(ckname))
				{
				return(1);
				} 
			}
		}
return(-1);
}

int SaveIFF(Filename,IFFBuf)
char *Filename;
BYTE *IFFBuf;
{
int fp; /* this is different! */
ULONG len,dummy,namelen,bodylen;
int i,j;
char c;


fp=open(Filename,O_WRONLY|O_CREAT|O_TRUNC);
if(fp==-1) {
	return(-1);
	}	
if(oneshot==1) {
	head.oneShotHiSamples=IFF_endsamp-IFF_begsamp;
	head.repeatHiSamples=0;
	head.samplesPerHiCycle=0;
	head.ctOctave=1;
	} /* else, assume head has been filled out by CreateIFF */
head.samplesPerSec=sr;
head.sCompression=0;
head.volume=Unity;

show_head();
/*bodylen=head.oneShotHiSamples+head.repeatHiSamples;*/
bodylen=IFF_endsamp-IFF_begsamp;
if((bodylen & 1) == 1) {
	bodylen++;
	}
/*for(k=1;k<=head.ctOctave;k++) bodylen*=2;*/
/* fix dummy for no oneshot case here */
strcpy(RString,"Andy's toy");
namelen=strlen(RString);

len=4+28+(namelen+8)+(bodylen+8);
j=(UWORD)(1.0/((float)sr*.279365*1e-6)); /* this goes in the head, dummy */

if((i=write(fp,"FORM",4))==-1) {
	return(-2);
	}
write(fp,&len,4);
write(fp,"8SVXVHDR",8);
len=20;
write(fp,&len,4);

write(fp,&head,20);

write(fp,"NAME",4);
strcpy(RString,"Andy's toy");
namelen=strlen(RString);
write(fp,&namelen,4);
write(fp,RString,namelen);
if(namelen&1) {
	c=0;
	write(fp,&c,1);
	}

write(fp,"BODY",4);
write(fp,&bodylen,4);

write(fp,IFF_buffer,bodylen);
close (fp);
return(0);
}

void CreateIFF()
{
int nocts_up=3;
int nocts_dn=1;
int incr,n,k;
/* if this is a section, not just sliders, use loopend, not endsamp */
/* assign these guys to even lengths in decimation cases */
if(cue("Do you wish to create multiple octaves?")) oneshot=0;
	else oneshot=1;
if(oneshot==1) {
	IFF_begsamp=begsamp;
	IFF_endsamp=endsamp;
	IFF_buffer=buffer;
	return;
	}
if((section[current_section].flags & SUSTAIN_FLAG) != SUSTAIN_FLAG) {
	cue("You must set a sustain point");
	return;
	}
begsamp=section[current_section].firstsamp;
endsamp=section[current_section].loopend;
l=(endsamp-begsamp);

m=l;
for(k=1;k<=nocts_dn;k++) m+=(l<<(k));
for(k=1;k<=nocts_up;k++) m+=(l>>(k));

orig=buffer;
if((bufsiz-nsamps)<m) {
	buffer_toggle();
	if((bufsiz-nsamps)<m) {
		cue("No room for this instrument.");
		buffer_toggle();
		return;
		}
	}

dest=buffer;
m=nsamps;
head.repeatHiSamples = (endsamp-section[current_section].loopstart)>>nocts_up;
if(head.repeatHiSamples&1) head.repeatHiSamples--;
head.oneShotHiSamples=(section[current_section].loopstart-begsamp)>>nocts_up;
if(head.oneShotHiSamples&1) head.oneShotHiSamples--;
head.ctOctave=nocts_up+nocts_dn+1;
/*FloatJunk=(1000000./(period[current_section][57]*.279365));
head.samplesPerHiCycle = (FloatJunk/440.)+.5;*/

/* try to make an integer factor of repeat samps */
/*k=(float)((head.repeatHiSamples/head.samplesPerHiCycle)+.5);
head.samplesPerHiCycle=head.repeatHiSamples/k;
k=head.repeatHiSamples%k;
head.oneShotHiSamples+=k;
head.repeatHiSamples-=k;
					*/
for(k=nocts_up;k>=0;k--) {
	incr=1<<k;
	fs=m;
	ls=m+((endsamp-section[current_section].loopstart)>>k);
	for(l=begsamp;l<endsamp;l=l+incr) 
		dest[m++]=orig[l];
	if(!(m&1)) dest[m++]=0; /* pad to keep this even */
	es=m;
	put_into_sec();
	}
for(k=1;k<=nocts_dn;k++) {
	incr=((int)1<<k);
	fs=m;
	ls=m+((endsamp-section[current_section].loopstart)<<k);
	for(l=begsamp;l<endsamp;l++) 
		for(n=0;n<incr;n++)
			dest[m++]=orig[l];
	es=m;
	put_into_sec();	
	}
m=m+2; /* make sure this fits? */
IFF_begsamp=nsamps;
IFF_endsamp=m;
IFF_buffer=buffer;
nsamps=m;
endsamp=m;
hist(0,nsamps);
return;
}

int put_into_sec()
{
int nn,n;
nn=-1;
for(n=MAXSECT;n>=0;n--) 
	if((section[n].lastsamp)<=2) nn=n;
if(nn!=-1) { 
	if(fs&1) fs++;
	if(es&1) es++;
	if(ls&1) ls++;
	section[nn].firstsamp=fs;
	section[nn].lastsamp=es;	
	section[nn].loopstart=ls;
	section[nn].currbuf=current_buffer;
	section[nn].loopend=es; /*danger here, caught by play? */
	section[nn].flags = SUSTAIN_FLAG; /* any others? */
	strcpy(section[nn].name,"IFF");
	}
else cue("No free sections!");
}

show_head()
{
/*
printf("\n-------\n");
printf("oneShot - %ld\n",head.oneShotHiSamples);
printf("repeat - %ld\n",head.repeatHiSamples);
printf("samps/per - %ld\n",head.samplesPerHiCycle);
printf("sr - %d\n",head.samplesPerSec);
printf("ctO - %d\n",head.ctOctave);
printf("sComp - %d\n",head.sCompression);
printf("volume - %ld\n",head.volume);
printf("-------\n");
*/
}

