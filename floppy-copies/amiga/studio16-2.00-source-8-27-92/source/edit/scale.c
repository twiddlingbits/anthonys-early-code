#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"
#include <stdio.h>

#include "scale.pwc"

extern struct StudioBase *StudioBase;
extern short se_cmd_kill_module;

void percent_func();
void DrawRamp();
void DrawRampLine();
void ScaleGraphBuffer();
void InitScaleMemWord();
void BlockScaleMemWord();
void ScaleMemDclips();

static struct NewButton okbutnguide = 
	{NULL,20,48,44,58,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutnguide = 
	{&okbutnguide,304,48,360,58,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

static struct NewSlider slider_start = {
    &cancelbutnguide,10,15,330,27,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    percent_func,0,0,0,0,0,10,1,5,112,-1};

static struct NewSlider slider_end = {
    &slider_start,10,30,330,42,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    percent_func,0,0,0,0,0,10,1,5,112,-1};

static int last_y1;
static int last_y2;
static int ss=0, es=100;

Scale(ei)
struct EditInstance *ei;
{
static char title_text[80];
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;

/** Tell DrawRamp that no draws have been made yet **/

last_y1=-1;
last_y2=-1;

/** Initalize gadgets **/

slider_start.list_size=201;
slider_start.show_size=1;
slider_start.top_element=ss;

slider_end.list_size=201;
slider_end.show_size=1;
slider_end.top_element=es;

/** Open the edit window **/

ScaleWin.Screen = StudioBase->screen;
ScaleWin.LeftEdge=-1;
ScaleWin.TopEdge=-1;

sprintf(title_text,"Scale/Fade '%s'",StrFileName(ei->samp_name));
ScaleWin.Title=title_text;
window=(struct Window *)OpenNewWind(&ScaleWin,NULL,NULL,NULL);
if (window==NULL)
	return(OUT_OF_MEMORY);

cancelbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.c = &StudioBase->defaults.colors;
slider_start.c = &StudioBase->defaults.colors;
slider_end.c = &StudioBase->defaults.colors;
slider_start.gad_ptr = NULL;
slider_end.gad_ptr = NULL;
okbutnguide.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutnguide.notifyme=okbutnguide.notifyme;

create_gadget_chain(window, &slider_end);

/** draw little box for the ramp **/

draw_indent(window->RPort, 335, 15, 370, 42, 
	StudioBase->defaults.colors.list.topleft_3d,
	StudioBase->defaults.colors.list.bottomright_3d);

/** Wait for CloseWindow gadget **/

while (TRUE) {
	Wait((1<<window->UserPort->mp_SigBit)|(1<<okbutnguide.notifyme->mp_SigBit));
	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
		}
	if (gadmsg=(struct GadMsg *)GetMsg(okbutnguide.notifyme)) {
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		break;
		}
	}

/** Clean up **/

ss=((struct slider *)slider_start.gad_ptr)->top_element;
es=((struct slider *)slider_end.gad_ptr)->top_element;

delete_gadget_chain(window);
DeletePort(okbutnguide.notifyme);
CloseWind(window);

/** Scale the graph! **/

if (gadpressed==okbutnguide.gad_ptr)
	return(DoScaleEdit(ei, (ss*512)/100, (es*512)/100));
else
	return(NO_ERROR);
}

/**************************************************************/

void percent_func(k)     /** use -v when compiling !!! **/
struct slider *k;
{
char buf[40];
int i;
int bpen;

i=k->top_element;

if ((struct GadgetHeader *)k==slider_start.gad_ptr)  {
	sprintf(buf,"Start %5d%%",i);
	}
else {
	sprintf(buf,"End   %5d%%",i);
	}

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT)
	bpen=k->c->slider_knob.hitbox_selected;
else
	bpen=k->c->slider_knob.hitbox_unselected;
SetBPen(k->kw->RPort, bpen);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);

/** Check for for case where first slider is being rendered **/
/**	and second slider hasn't been initilitzed yet. **/

if (slider_start.gad_ptr && slider_end.gad_ptr)
	DrawRamp(k->kw->RPort,
		((struct slider *)slider_start.gad_ptr)->top_element,
		((struct slider *)slider_end.gad_ptr)->top_element,
		k->c->gadget_window.draw_selected,
		k->c->gadget_window.hitbox_unselected);
}

/**************************************************************/

void DrawRamp(rp, start, end, forecolor, backcolor)
struct RastPort *rp;
int start,end,forecolor,backcolor;
{
int x1,x2;
int y1,y2;

x1=338;
x2=367;
y1=40-start*23/200;
y2=40-end*23/200;
if (last_y1 != -1)	/** erase last line **/
	DrawRampLine(rp, x1, x2, last_y1, last_y2, backcolor);
DrawRampLine(rp, x1, x2, y1, y2, forecolor);	/** draw new line **/
last_y1=y1;
last_y2=y2;
}

void DrawRampLine(rp, x1, x2, y1, y2, color)
struct RastPort *rp;
{
SetDrMd(rp, JAM1);
SetAPen(rp, color);
Move(rp,x1,y1);
Draw(rp,x2,y2);
}

/**************************************************************/

DoScaleEdit(ei, start_scale, end_scale)
struct EditInstance *ei;
int start_scale, end_scale;
{
short *mem;
long buflen;
long dolen;
long thislen;
int error;

buflen=StudioBase->defaults.copy_buf_size/2;
mem=(short *)AllocMem(buflen*2, 0);
if (mem==0) {
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);
dolen=ei->end_mark-ei->start_mark+1;
ScaleMemDclips(ei,ei->sfp->tag.dclips,ei->start_mark,start_scale, end_scale, dolen/ei->gb_scale);
SampleSeek(ei->sfp, ei->start_mark, SS_OFFSET_BEGINNING);
error=NO_ERROR;
InitScaleMemWord(start_scale, end_scale, dolen);

do {
	thislen=min(dolen, buflen);
	if (SampleRead(ei->sfp, mem, thislen)!=thislen)
		error=READ_ERROR;

	BlockScaleMemWord(mem,thislen);

	SampleSeek(ei->sfp, -thislen, SS_OFFSET_CURRENT);
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

/**************************************************************/

static int bump;
static int addval;
static int div;
static int bc;
static int current_scale;

/** scale's are fixed point, with unity being 512 **/
/** eg, 256 would scale by 50% **/

void InitScaleMemWord(start_scale, end_scale, len)
int end_scale, start_scale;
{
div=end_scale-start_scale+1;
if (div==0)
	div=1;
bump = len/div;
if (bump < 0) {
	bump=-bump;
	addval=-1;
	}
else {
	addval=1;
	}
bc=bump;

current_scale=start_scale;
}

/**************************************************************/

void BlockScaleMemWord(mem,blocklen)
short *mem;
int blocklen;
{
int index;
int val;

index=0;

while (blocklen--) {
	val = ((int)mem[index]*current_scale)>>9; /** /512 **/
	if (val > 32767) val=32767;
	if (val < -32767) val=-32767;
	mem[index] = val;
	index++;
	bc--;
	if (bc<=0) {
		bc=bump;
		current_scale+=addval;
		}
	}
}

/**************************************************************/

void ScaleMemDclips(ei,dclips,sm,start_scale, end_scale, len)
struct EditInstance *ei;
struct SampleDataClip *dclips;
int sm;	/** start_mark **/
int end_scale, start_scale, len;
{
int index;
int bump;
int bc;
int addval;
int val;
short dclipindex;
char *mem;
int filepos;
int div;

div=end_scale-start_scale+1;
if (div==0)
	div=1;
bump = len/div;
if (bump < 0) {
	bump=-bump;
	addval=-1;
	}
else {
	addval=1;
	}
bc=bump;

mem = ei->org_graph_buffer;
MapEditedToData2(dclips,sm,&filepos,&dclipindex);
index = filepos/ei->gb_scale;

while (len--) {
	val = ((int)mem[index]*start_scale)>>9; /** /512 **/
	if (val > 127) val=127;
	if (val < -127) val=-127;
	mem[index] = val;
	index++;
	filepos=filepos+ei->gb_scale;
	if (filepos > dclips[dclipindex].end) {
		dclipindex++;
		filepos=dclips[dclipindex].start;
		index = filepos/ei->gb_scale;
		}
	bc--;
	if (bc<=0) {
		bc=bump;
		start_scale+=addval;
		}
	}
}
