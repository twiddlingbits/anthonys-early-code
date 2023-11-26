#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include <stdio.h>

#include "newrate.pwc"

extern struct StudioBase *StudioBase;
extern short se_cmd_kill_module;

void num_func();

static struct NewButton okbutnguide = 
	{NULL,20,60,44,70,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutnguide = 
	{&okbutnguide,304,60,360,70,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

static struct NewSlider slider_vol = {
    &cancelbutnguide,10,45,370,57,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,0,0,0,0,10,1,5,112,-1};

static struct NewSlider slider_rate = {
    &slider_vol,10,15,370,27,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,0,0,0,0,10,1,5,112,-1};

static struct NewSlider slider_freq = {
    &slider_rate,10,30,370,42,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,0,0,0,0,10,1,5,112,-1};

void NewRate(samp)
struct Disk_Samp *samp;
{
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
struct Window *window;
int i;

/** Initilize gadgets to match Samp **/

slider_rate.list_size=600;
slider_rate.show_size=1;
slider_rate.top_element=max(0,660-5000000/samp->parms.rate);

if (samp->parms.filter3db==0)
	samp->parms.filter3db=samp->parms.rate/2;

slider_freq.list_size=15;
slider_freq.show_size=1;
slider_freq.top_element=max(0,15-100000/samp->parms.filter3db);

slider_vol.list_size=47;
slider_vol.show_size=1;
i=samp->parms.volume/32-100+40;
if (i<0) i=0;
if (i>46) i=46;
slider_vol.top_element=i;

/** Open the edit window **/

NewRateWin.Screen = StudioBase->screen;
NewRateWin.LeftEdge=-1;
NewRateWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&NewRateWin,NULL,NULL,NULL);
if (window==NULL)
	return;

slider_vol.c = &StudioBase->defaults.colors;
slider_freq.c = &StudioBase->defaults.colors;
slider_rate.c = &StudioBase->defaults.colors;
cancelbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutnguide.notifyme=okbutnguide.notifyme;
create_gadget_chain(window, &slider_freq);

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

if (gadpressed==okbutnguide.gad_ptr) {

	/** copy any changes made **/

	i=((struct slider *)slider_rate.gad_ptr)->top_element;
	i = 5000000/(660-i);
	samp->parms.rate=i;

	i=((struct slider *)slider_freq.gad_ptr)->top_element;
	i = 100000/(15-i);
	samp->parms.filter3db=i;

	i=((struct slider *)slider_vol.gad_ptr)->top_element;
	i=(i-40)*32+ZERO_DB;
	samp->parms.volume=i;
	}

/** Clean up **/

delete_gadget_chain(window);
DeletePort(okbutnguide.notifyme);
CloseWind(window);
}

/**************************************************************/

void num_func(k)     /** use -v when compiling !!! **/
struct slider *k;
{
char buf[40];
int i;

if ((struct GadgetHeader *)k==slider_rate.gad_ptr)  {
	i=k->top_element;
   i = 5000000/(660-i);
	sprintf(buf,"Rate %5d  ",i);
	}
else if ((struct GadgetHeader *)k==slider_vol.gad_ptr)  {
	i=k->top_element-40;
	if (i!=0)
		sprintf(buf,"Vol %+2d dB   ",i);
	else
		sprintf(buf,"Vol %2d dB   ",i);
	}
else {
	i=k->top_element;
	i = 100000/(15-i);
	if (i > 99999) i=99999;
	sprintf(buf,"Filter %5d",i);
	}

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);
}

