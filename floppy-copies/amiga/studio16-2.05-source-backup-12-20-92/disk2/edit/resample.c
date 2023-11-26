#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"
#include <stdio.h>

#define SINC(m,tau) ((sinctab[m+FIR_TOP])[tau])
#define MAX_FIR_ORDER 101

struct SumData64 {
	long	op;
	long  result_ls;
	long  result_ms;
	};

/**************************************************************/
static struct TextAttr TOPAZ80 = {
	(STRPTR)"topaz.font",
	TOPAZ_EIGHTY,0,0
};

static UBYTE UNDOBUFFER[13];

static UBYTE newrateSIBuff[13];
static struct StringInfo newrateSInfo = {
	newrateSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

static struct Gadget str_newrate = {
	NULL,	/* next gadget */
	20,92,	/* origin XY of hit box relative to window TopLeft */
	64,10,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&newrateSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};


static struct NewWindow ResampleWin = {
	-1,-1,	/* window XY origin relative to TopLeft of screen */
	
	240,125,	/* window width and height */
	0,1,	/* detail and block pens */
	GADGETUP,	/* IDCMP flags */
	WINDOWDRAG+NOCAREREFRESH,	/* other window flags */
	&str_newrate,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	80,50,	/* minimum width and height */
	-1,-1,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

/**************************************************************/

extern struct StudioBase *StudioBase;
extern short se_cmd_kill_module;

short *FindOrgData();
void PutNewVal();
void FreeSincTab();
int FindResampVal(struct EditInstance *,long, long);
double sin(double);
long mysin(long);
void MyText();
void DrawMyText();
void UpdateEstTime();

void __asm Sum64(register __a0 struct SumData64 *);

/**************************************************************/

int FIR_ORDER=21;	/** make odd **/
int FIR_TOP=10;		/** make 1/2 FIR_ORDER! **/				
int SINC_TABLE_SIZE=2049;
unsigned char SINC_SHIFT=11;

/**************************************************************/

static struct NewButton okbutn = 
	{NULL,20,110,116,120,BUTN_COOL,0,0,0,0,0,0,"DO RESAMPLE"};

static struct NewButton cancelbutn = 
	{&okbutn,160,110,220,120,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

static struct NewButton res_normal = {
	&cancelbutn,20,30,30,38,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,0,1,0,0,"Normal"};

static struct NewButton res_high = {
	&res_normal,20,42,30,50,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE|BUTN_HIT,
	0,0,0,1,0,0,"High"};

static struct NewButton order5 = {
	&res_high,130,30,140,38,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,0,2,0,0," 5"};

static struct NewButton order11 = {
	&order5,130,42,140,50,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,0,2,0,0,"11"};

static struct NewButton order21 = {
	&order11,170,30,180,38,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE|BUTN_HIT,
	0,0,0,2,0,0," 21"};

static struct NewButton order101 = {
	&order21,170,42,180,50,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,0,2,0,0,"101"};

/**************************************************************/

int Resample(ei)
struct EditInstance *ei;
{
static char title_text[80];
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
long nrate;
int error;

/** Initalize gadgets **/

sprintf(newrateSIBuff,"%d",ei->sfp->tag.parms.rate);

/** Open the edit window **/

ResampleWin.Screen = StudioBase->screen;
ResampleWin.LeftEdge=-1;
ResampleWin.TopEdge=-1;
sprintf(title_text,"Resample Range '%s'",StrFileName(ei->samp_name));
ResampleWin.Title=title_text;
window=(struct Window *)OpenNewWind(&ResampleWin,NULL,NULL,NULL);
if (window==NULL)
	return(OUT_OF_MEMORY);

cancelbutn.c = &StudioBase->defaults.colors;
okbutn.c = &StudioBase->defaults.colors;
res_normal.c = &StudioBase->defaults.colors;
res_high.c = &StudioBase->defaults.colors;
order5.c = &StudioBase->defaults.colors;
order11.c = &StudioBase->defaults.colors;
order21.c = &StudioBase->defaults.colors;
order101.c = &StudioBase->defaults.colors;
okbutn.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutn.notifyme=okbutn.notifyme;
res_normal.notifyme=okbutn.notifyme;
res_high.notifyme=okbutn.notifyme;
order5.notifyme=okbutn.notifyme;
order11.notifyme=okbutn.notifyme;
order21.notifyme=okbutn.notifyme;
order101.notifyme=okbutn.notifyme;

create_gadget_chain(window, &order101);
DrawMyText(window,ei->sfp->tag.parms.rate);
UpdateEstTime(ei,window);
error=NO_ERROR;

while (TRUE) {
	Wait((1<<window->UserPort->mp_SigBit)|(1<<okbutn.notifyme->mp_SigBit));
	UpdateEstTime(ei,window);
	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
		}
	if (gadmsg=(struct GadMsg *)GetMsg(okbutn.notifyme)) {
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		if (gadpressed==cancelbutn.gad_ptr) {
			break;
			}
		else if (gadpressed==okbutn.gad_ptr) {
			nrate=atol(newrateSIBuff);
			if (nrate > 50 && nrate < 100000) {
				FIR_ORDER=FindFirOrder();
				FIR_TOP=FIR_ORDER/2;
				if (res_high.gad_ptr->flags&BUTN_HIT) {
					SINC_TABLE_SIZE=2049;
					SINC_SHIFT=11;
					if (ei->end_mark-ei->start_mark+1 > 2097151) {
						telluser("Range to large to resample.",
								"Try 'normal' resolution.",0);
						break;
						}
					}
				else {
					SINC_TABLE_SIZE=1025;
					SINC_SHIFT=10;
					if (ei->end_mark-ei->start_mark+1 > 4194303) {
						telluser("Range to large to resample.",
								"Try resampling in parts.",0);
						break;
						}
					}
				error=DoResampleEdit(ei, nrate);
				if (error==NO_ERROR)
					telluser("Resample successful:","Resampled range now in CopyBuffer.",0);
				break;
				}
			else {
				telluser("Resample:","Invalid rate selected",0);
				}
			}
		}
	}

/** Clean up **/

delete_gadget_chain(window);
DeletePort(okbutn.notifyme);
CloseWind(window);
return(error);
}

/**************************************************************/
void DrawMyText(w,rate)
struct Window *w;
int rate;
{
char str[30];

SetDrMd(w->RPort, JAM2);
SetAPen(w->RPort,StudioBase->defaults.colors.gadget_window.draw_unselected);
SetBPen(w->RPort,StudioBase->defaults.colors.list.hitbox_unselected);

MyText(w,20,18,"Resolution");
MyText(w,130,18,"Filter Order");
MyText(w,20,60,"Current Rate");
MyText(w,20,82,"New Rate");
MyText(w,130,82,"Time (25MHz)");
SetAPen(w->RPort,StudioBase->defaults.colors.button.draw_selected);
sprintf(str,"%d",rate);
MyText(w,20,70,str);
}

/**************************************************************/

void MyText(w,x,y,str)
struct Window *w;
int x,y;
char *str;
{
Move(w->RPort,x,y+7);
Text(w->RPort,str,strlen(str));
}

/**************************************************************/

void UpdateEstTime(ei,w)
struct EditInstance *ei;
struct Window *w;
{
int i;
char str[50];
double new_length;
long new_rate;
double t;

new_rate=atol(newrateSIBuff);
new_length=((double)(ei->end_mark-ei->start_mark+1)/(double)ei->sfp->tag.parms.rate)*(double)new_rate;
i=FindFirOrder();

t=new_length*(double)i/71000.0;	/** for FIR filter **/
t=t+0.33*(double)i;				/** for table fill **/
sprintf(str,"%6.2f mins",t/60.0);

SetDrMd(w->RPort, JAM2);
SetAPen(w->RPort,StudioBase->defaults.colors.button.draw_selected);
SetBPen(w->RPort,StudioBase->defaults.colors.list.hitbox_unselected);
MyText(w,130,92,str);
}

/**************************************************************/
int FindFirOrder()
{
if (order5.gad_ptr->flags&BUTN_HIT)
	return(5);
else if (order11.gad_ptr->flags&BUTN_HIT)
	return(11);
else if (order21.gad_ptr->flags&BUTN_HIT)
	return(21);
else
	return(101);
}

/**************************************************************/

static long orgmem_start,last_usable_n_inbuf;
static long newmem_pos;
static struct SampleFilePointer *newsfp;
static short *orgmem, *newmem;
static long buflen;
long *sinctab[MAX_FIR_ORDER];

/**************************************************************/

DoResampleEdit(ei,new_rate)
struct EditInstance *ei;
int new_rate;
{
unsigned long tau,tau_per_samp,frac_org_n;	/** Fixed point **/
unsigned long new_length;
unsigned long new_n,org_n;
int org_rate;
int error;
short *data;
short m;
struct SumData64 news;

new_pointer(PTR_BUSY);
if (AllocSincTab()!=NO_ERROR)
	return(OUT_OF_MEMORY);

#if 0
kprintf("sinc(-4 , .25) %04x%04x\n",SINC(-4,0x100));
kprintf("sinc(0 , 0) %04x%04x\n",SINC(0,0));
kprintf("sinc(1 , .5) %04x%04x\n",SINC(1,0x180));
#endif

newsfp=(struct SampleFilePointer *)SampleOpenInDefPath("CopyBuffer",SP_MODE_NEWFILE,0);
if (newsfp==0) {
	FreeSincTab();
	new_pointer(PTR_NORMAL);
	return(STDERR_CANT_OPEN_SAMPLE);
	}

buflen=StudioBase->defaults.copy_buf_size/2+FIR_ORDER;
orgmem=(short *)AllocMem(buflen*2,0);
if (orgmem==0) {
	SampleClose(newsfp);
	FreeSincTab();
	new_pointer(PTR_NORMAL);
	return(OUT_OF_MEMORY);
	}

newmem=(short *)AllocMem(buflen*2,0);
if (newmem==0) {
	FreeMem(orgmem,buflen*2);
	SampleClose(newsfp);
	FreeSincTab();
	new_pointer(PTR_NORMAL);
	return(OUT_OF_MEMORY);
	}

error=NO_ERROR;

org_rate=ei->sfp->tag.parms.rate;
tau_per_samp=(org_rate<<SINC_SHIFT)/new_rate;
new_length=((double)(ei->end_mark-ei->start_mark+1)/(double)org_rate)*(double)new_rate;
new_n=0;
last_usable_n_inbuf=-1;
newmem_pos=0;

#if 0
kprintf("org_rate %04x%04x\n",org_rate);
kprintf("new_rate %04x%04x\n",new_rate);
kprintf("newlen %04x%04x\n",new_length);
#endif

/** Read first block of data **/

ReadOrgData(ei,1);

/** Build Resampled data by using FIR fitler  **/
/** s(k-tau)=SUM[FIR_TOP->m->FIR_TOP](m-tau)*s(k-m) **/
/** See "Sample Rate Conversion" from Motorola **/

while (new_length--) {

	/** I keep incrementing "new_n" (the new sample's index) **/
	/** and generating the "org_n" + tau.  org_n + tau **/
	/** describes a fraciontal sample position.  tau <= 1 **/
	/** I will then use a filter to create the sample that **/
	/** would be in this fractional poition if it had been **/
	/** digitized with the orginal sample **/
	
	frac_org_n=tau_per_samp*new_n;
	org_n=(int)(frac_org_n>>SINC_SHIFT)+1;
	tau=(org_n<<SINC_SHIFT)-frac_org_n;

	/** Is the org_n we want in the current read buffer? **/
	/** if not, read it in **/

	if (org_n > last_usable_n_inbuf) {
		if ((error=ReadOrgData(ei,org_n))!=NO_ERROR)
			break;
		}

	/** Create 1 new sample by passing through FIR filter **/

	news.result_ls=0;
	news.result_ms=0;
	data=orgmem+org_n-orgmem_start;
	for (m=-FIR_TOP; m <= FIR_TOP; m++) {
		news.op=(SINC(m,tau)*(long)data[-m]);
		Sum64(&news);	/** news.result=news.result+news.op **/
		}
	/** Put new sample into buffer, & write to disk if needed **/
	ShiftResult64(&news);	/** news.result >> 15 and saturate **/
	newmem[newmem_pos++]=news.result_ls;
	if (newmem_pos==buflen)
		if ((error=FlushNewBuffer())!=NO_ERROR)
			break;
	new_n++;
	}

if (error==NO_ERROR)
	error=FlushNewBuffer();
FreeMem(orgmem, buflen*2);
FreeMem(newmem, buflen*2);
CopySampData(&newsfp->tag,&ei->sfp->tag);
newsfp->tag.parms.rate=new_rate;
SampleClose(newsfp);

new_pointer(PTR_NORMAL);
FreeSincTab();
return(error);
}

/**************************************************************/
/** Given an index n (from ei->start_mark) of the orginal data **/
/** return a pointer to a buffer starting at 'n', with at least **/
/** n-FIR_TOP to n+FIR_TOP of valid data **/
/**************************************************************/

int ReadOrgData(ei,n)
struct EditInstance *ei;
long n;
{
long did,len,at;
int error;

if (n>=FIR_TOP) {
	at=n-FIR_TOP+ei->start_mark;
	SampleSeek(ei->sfp, at, SS_OFFSET_BEGINNING);
	len=min(buflen-FIR_TOP, ei->sfp->tag.length-at);
	did=SampleRead(ei->sfp, orgmem, len);
	setmem(orgmem+len, buflen-len, 0);
	if (did!=len)
		error=READ_ERROR;
	else
		error=NO_ERROR;
	}
else {
	setmem(orgmem, FIR_TOP, 0);
	at=n+ei->start_mark;
	SampleSeek(ei->sfp, at, SS_OFFSET_BEGINNING);
	len=min(buflen-FIR_TOP, ei->sfp->tag.length-at);
	did=SampleRead(ei->sfp,orgmem+FIR_TOP, len);
	setmem(orgmem+len+FIR_TOP, buflen-len-FIR_TOP, 0);
	if (did!=len)
		error=READ_ERROR;
	else
		error=NO_ERROR;
	}

last_usable_n_inbuf=n+did-FIR_TOP;
orgmem_start=n-FIR_TOP;
return(error);
}

/**************************************************************/

int FlushNewBuffer()
{
int did,error;

did=SampleWrite(newsfp,newmem, newmem_pos);
if (did!=newmem_pos)
	error=WRITE_ERROR;
else
	error=NO_ERROR;
newmem_pos=0;
return(error);
}

/**************************************************************/

long dsinc(m,tau)
long m,tau;
{
static double pi=3.14159265359;
double xpi;
double rv;
long lrv;

xpi=(double)tau/(double)(SINC_TABLE_SIZE-1);
xpi=pi*((double)m-xpi);
if (xpi==0)
	return(0x8000);
rv=sin(xpi)/xpi;
lrv=(long)(rv*(double)0x8000);
return(lrv);
}

/**************************************************************/

int AllocSincTab()
{
long m,k,j;

for (m=-FIR_TOP; m <= FIR_TOP; m++) {
	k=m+FIR_TOP;
	sinctab[k]=(long *)AllocMem(SINC_TABLE_SIZE*4,0);
	if (sinctab[k]==0) {
		FreeSincTab();
		return(OUT_OF_MEMORY);
		}
	for (j=0; j < SINC_TABLE_SIZE; j++) {
		(sinctab[k])[j]=dsinc(m,j);
/*if (m==1 && j < 180)
		kprintf("m %04x%04x tau %04x%04x Sinc %04x%04x\n",m,j,SINC(m,j));*/
		}
	}
return(NO_ERROR);
}

/**************************************************************/

void FreeSincTab()
{
long m;
long k;

for (m=-FIR_TOP; m <= FIR_TOP; m++) {
	k=m+FIR_TOP;
	if (sinctab[k])
		FreeMem(sinctab[k],SINC_TABLE_SIZE*4);
	}
}

/**************************************************************/
