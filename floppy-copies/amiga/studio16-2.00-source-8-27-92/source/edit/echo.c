#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"
#include <stdio.h>

#define MAX_CIRC_SIZE 25000	/** In Words! Buffer is 2X this **/

static struct NewWindow EchoWin = {
	-1,-1,	/* window XY origin relative to TopLeft of screen */
	360,76,	/* window width and height */
	0,1,	/* detail and block pens */
	NULL,	/* IDCMP flags */
	WINDOWDRAG+NOCAREREFRESH,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	80,50,	/* minimum width and height */
	-1,-1,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

extern struct StudioBase *StudioBase;
extern long zoom_scale;		/* samples per pixel */
extern short se_cmd_play_sample;
extern short se_cmd_stop_all_playback;

void EchoGraphBuffer();
void EchoMemWord();
void EchoMemDclips();
void level_func();
void GetEchoBlockParms();
void TryEcho();
void InitCircBuffer();

static short *cbuf;			/** buffer memory **/
long circbuf;					/** buffer index **/
struct EditInstance *echoei;


static struct NewButton okbutn = 
	{NULL,20,62,44,72,BUTN_COOL,0,0,0,0,0,0,"Do"};

static struct NewButton trybutn = 
	{&okbutn,52,62,92,72,BUTN_COOL,0,0,0,0,0,0,"Try"};

static struct NewButton stopbutn = 
	{&trybutn,100,62,140,72,BUTN_COOL,0,0,0,0,0,0,"Stop"};

static struct NewButton cancelbutn = 
	{&stopbutn,280,62,340,72,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

static struct NewSlider slider_feedback = {
    &cancelbutn,10,15,350,27,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    level_func,0,0,0,0,0,10,1,5,120,-1};

static struct NewSlider slider_delay = {
    &slider_feedback,10,30,350,42,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    level_func,0,0,0,0,0,10,1,5,120,-1};

static struct NewSlider slider_dlevel = {
    &slider_delay,10,45,350,57,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    level_func,0,0,0,0,0,10,1,5,120,-1};

int DoEchoEdit(ei)
struct EditInstance *ei;
{
static char title_text[80];
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
short *temp;
char *sampname[2];
/** Initilize local global variable **/

echoei=ei;
sampname[1]=0;		/** mark end of list **/

/** Initalize gadgets **/

slider_feedback.list_size=101;
slider_feedback.show_size=1;
slider_feedback.top_element=50;

slider_dlevel.list_size=101;
slider_dlevel.show_size=1;
slider_dlevel.top_element=100;

slider_delay.list_size=MAX_CIRC_SIZE;
slider_delay.show_size=1;
slider_delay.top_element=200;

/** Allocate the Echo buffer **/

cbuf=(short *)AllocMem(MAX_CIRC_SIZE*2, 0);
if (cbuf==0)
	return(OUT_OF_MEMORY);

/** Open the edit window **/

EchoWin.Screen = StudioBase->screen;
EchoWin.LeftEdge=-1;
EchoWin.TopEdge=-1;
sprintf(title_text,"Echo Range in '%s'",StrFileName(ei->samp_name));
EchoWin.Title=title_text;
window=(struct Window *)OpenNewWind(&EchoWin,NULL,NULL,NULL);
if (window==NULL) {
	FreeMem(cbuf,MAX_CIRC_SIZE*2);	
	cbuf=0;
	return(OUT_OF_MEMORY);
	}

cancelbutn.c = &StudioBase->defaults.colors;
okbutn.c = &StudioBase->defaults.colors;
trybutn.c = &StudioBase->defaults.colors;
stopbutn.c = &StudioBase->defaults.colors;
slider_feedback.c = &StudioBase->defaults.colors;
slider_dlevel.c = &StudioBase->defaults.colors;
slider_delay.c = &StudioBase->defaults.colors;
slider_feedback.gad_ptr = NULL;
slider_dlevel.gad_ptr = NULL;
slider_delay.gad_ptr = NULL;
okbutn.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutn.notifyme=okbutn.notifyme;
trybutn.notifyme=okbutn.notifyme;
stopbutn.notifyme=okbutn.notifyme;

create_gadget_chain(window, &slider_dlevel);

/** Wait for CloseWindow gadget **/

while (TRUE) {
	Wait((1<<window->UserPort->mp_SigBit)|(1<<okbutn.notifyme->mp_SigBit));
	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
		}
	if (gadmsg=(struct GadMsg *)GetMsg(okbutn.notifyme)) {
		BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		if (gadpressed==trybutn.gad_ptr) {
			InitCircBuffer();
			sampname[0]=echoei->samp_name;
			BroadcastEventParms(se_cmd_play_sample,sampname,echoei->start_mark,echoei->end_mark-echoei->start_mark+1,0,TryEcho,0);
			}
		else if (gadpressed==okbutn.gad_ptr) {
			if (askuser("Are you sure you want to Echo range?","(There is no undo.)",0)) {
				show_error(EchoRange());
				break;
				}
			}
		else if (gadpressed==cancelbutn.gad_ptr) {
			break;
			}
		}
	}

/** Clean up **/

temp=cbuf;
cbuf=0;			/** Tell TryEcho() to stop **/
delete_gadget_chain(window);
DeletePort(okbutn.notifyme);
CloseWind(window);
WaitTOF();WaitTOF();WaitTOF();		/** Super Icky way to make sure stop playback is done **/
FreeMem(temp,MAX_CIRC_SIZE*2);		/** remove when we have Sync. StopPlayBack() **/
return(NO_ERROR);
}

/**************************************************************/

void level_func(k) 
struct slider *k;
{
static char buf[40];
int i;
int bpen;
int len;

i=k->top_element;

if ((struct GadgetHeader *)k==slider_delay.gad_ptr)  {
	sprintf(buf,"Delay%7.2fms",(double)((i+1)*1000)/(double)echoei->sfp->tag.parms.rate);
	len=14;
	}
else if ((struct GadgetHeader *)k==slider_feedback.gad_ptr)  {
	sprintf(buf,"Feedback %3d%%",i);
	len=13;
	}
else {
	sprintf(buf,"DLevel %3d%%",i);
	len=11;
	}

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT)
	bpen=k->c->slider_knob.hitbox_selected;
else
	bpen=k->c->slider_knob.hitbox_unselected;
SetBPen(k->kw->RPort, bpen);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, len);
}

/**************************************************************/

void __asm EchoBlock(register __a0 short *,register __d3 long,
							register __a1 short *,register __d0 long,
							register __d1 short,register __d2 short);

EchoRange()
{
short *mem;
long buflen;
long dolen;
long thislen;
int error;
long dlevel;
long feedback;
long csize;


buflen=StudioBase->defaults.copy_buf_size/2;
mem=(short *)AllocMem(buflen*2, 0);
if (mem==0) {
	return(OUT_OF_MEMORY);
	}

GetEchoBlockParms(&dlevel,&feedback,&csize);
InitCircBuffer();
new_pointer(PTR_BUSY);
dolen=echoei->end_mark-echoei->start_mark+1;
SampleSeek(echoei->sfp, echoei->start_mark, SS_OFFSET_BEGINNING);
error=NO_ERROR;

do {
	thislen=min(dolen, buflen);
	if (SampleRead(echoei->sfp, mem, thislen)!=thislen)
		error=READ_ERROR;

	EchoBlock(cbuf,csize,mem,thislen,dlevel,feedback);

	SampleSeek(echoei->sfp, -thislen, SS_OFFSET_CURRENT);
	if (SampleWrite(echoei->sfp, mem, thislen)!=thislen)
		error=WRITE_ERROR;
	dolen -= thislen;
	} while (dolen && error==NO_ERROR);

FreeMem(mem, buflen*2);
if (zoom_scale==1) {
	BuildEIRP(echoei); 
	DrawAllRP();
	}
new_pointer(PTR_NORMAL);
return(error);
}

/**************************************************************/

void TryEcho(mem, memsize)
short *mem;
long memsize;
{
long dlevel, feedback, csize;
static long last_csize;
long oldpri;

if (cbuf && memsize) {
	oldpri=SetTaskPri(FindTask(0),0);		/** Drop pri so gadlib sliders don't jerk **/	
	GetEchoBlockParms(&dlevel,&feedback,&csize);
	if (last_csize!=csize) {
		last_csize=csize;
		InitCircBuffer();
		}
	EchoBlock(cbuf,csize,mem,memsize,dlevel,feedback);
	SetTaskPri(FindTask(0),oldpri);	
	}
}

/**************************************************************/

void GetEchoBlockParms(long *dlevel,long *feedback,long *csize)
{
*dlevel=(((struct slider *)(slider_dlevel.gad_ptr))->top_element*0x3FFF)/100,
*feedback=(((struct slider *)(slider_feedback.gad_ptr))->top_element*0x3FFF)/100,
*csize=(((struct slider *)(slider_delay.gad_ptr))->top_element+1)*2;
}

/**************************************************************/

void InitCircBuffer()
{
circbuf=0;
setmem(cbuf,MAX_CIRC_SIZE*2, 0);
}
