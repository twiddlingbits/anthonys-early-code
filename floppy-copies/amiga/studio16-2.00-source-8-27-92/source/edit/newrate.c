#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"
#include <stdio.h>

#include "newrate.pwc"

extern struct StudioBase *StudioBase;
extern short se_cmd_kill_module;

void num_func();

static struct NewButton okbutnguide = 
	{NULL,20,60,44,70,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutnguide = 
	{&okbutnguide,304,60,360,70,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

static struct NewSlider slider_rate = {
    &cancelbutnguide,10,15,370,27,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,0,0,0,0,10,1,5,112,-1};

static struct NewSlider slider_vol = {
    &slider_rate,10,30,370,42,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,0,0,0,0,10,1,5,112,-1};

static struct NewSlider slider_pan = {
    &slider_vol,10,45,370,57,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR|HIDE_GADGET,
    num_func,0,0,0,0,0,10,1,5,112,-1};

static struct NewSlider slider_freq = {
    &slider_pan,10,45,370,57,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR|HIDE_GADGET,
    num_func,0,0,0,0,0,10,1,5,112,-1};

struct Module 		*Hmod;
struct AudioHandler 	*hand;
extern struct StandardModState *state;


void NewRate(ei)
struct EditInstance *ei;
{
static char title_text[80];
struct SampleInfo *parms;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
struct Window *window;
int i;
int GetHandler();
void ForgetHandler();

/** Initilize gadgets to match Samp **/

if(GetHandler()) return;

parms=&ei->sfp->tag.parms;

slider_rate.list_size=hand->number_of_rate_settings;
slider_rate.show_size=1;
slider_rate.top_element=max(0,(*hand->FindBestRate)(parms->rate));

if (parms->filter3db==0)
	parms->filter3db=parms->rate/2;

if (hand->flags&AH_FILTER_ADJ) {
	slider_freq.show_size=1;
	slider_freq.list_size=hand->number_of_filter_settings;
	slider_freq.top_element=(*hand->FindBestFilter)(parms->filter3db);
	slider_freq.flags&= (~HIDE_GADGET);
	}
else {		/** This code assumes that if there is no filter, the card has a pan. **/
	if (ei->sfp->ff->flags&FF_PAN) {
		slider_pan.list_size=201;
		slider_pan.show_size=1;
		i=parms->pan/32;
		if (i<0) i=0;
		if (i>200) i=200;
		slider_pan.top_element=i;
		slider_pan.flags&= (~HIDE_GADGET);
		}
	else {
		telluser("Edit Warning:","This file format doesn't support 'pans'.",0);	
		}
	}

slider_vol.list_size=47;
slider_vol.show_size=1;
i=parms->volume/32-100+40;
if (i<0) i=0;
if (i>46) i=46;
slider_vol.top_element=i;

/** Open the edit window **/

sprintf(title_text,"Default Sample Parameters '%s'",StrFileName(ei->samp_name));
NewRateWin.Title=title_text;
NewRateWin.Screen = StudioBase->screen;
NewRateWin.LeftEdge=-1;
NewRateWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&NewRateWin,NULL,NULL,NULL);
if (window==NULL)
	return;

slider_pan.c = &StudioBase->defaults.colors;
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

	i=(*hand->RateValueFunc)(((struct slider *)slider_rate.gad_ptr)->top_element);
	parms->rate=i;

	i=(hand->FilterValueFunc)(((struct slider *)slider_freq.gad_ptr)->top_element);
	parms->filter3db=i;

	i=((struct slider *)slider_vol.gad_ptr)->top_element;
	i=(i-40)*32+ZERO_DB;
	parms->volume=i;

	i=((struct slider *)slider_pan.gad_ptr)->top_element;
	i=i*32;
	parms->pan=i;
	}

/** Clean up **/

delete_gadget_chain(window);
DeletePort(okbutnguide.notifyme);
CloseWind(window);
ForgetHandler();
}

/**************************************************************/

void num_func(k)     /** use -v when compiling !!! **/
struct slider *k;
{
char buf[40];
int i;

if ((struct GadgetHeader *)k==slider_rate.gad_ptr)  {
	i=(*hand->RateValueFunc)(k->top_element);
	sprintf(buf,"Rate %5d  ",i);
	}
else if ((struct GadgetHeader *)k==slider_vol.gad_ptr)  {
	i=k->top_element-40;
	if (i!=0)
		sprintf(buf,"Vol %+2d dB   ",i);
	else
		sprintf(buf,"Vol %2d dB   ",i);
	}
else if ((struct GadgetHeader *)k==slider_pan.gad_ptr)  {
	i=k->top_element;
	if (i==0) 
		strcpy(buf,"Full Left   ");
	else if (i==100)
		strcpy(buf,"Pan L+R     ");
	else if (i==200)
		strcpy(buf,"Full Right  ");
	else
		sprintf(buf,"Pan %3d     ",i);
	}
else {
	i=(hand->FilterValueFunc)(k->top_element);
	sprintf(buf,"Filter %5d",i);
	}

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void ForgetHandler()
{
hand=NULL;
Hmod=NULL;
}
/**************************************************************/
int GetHandler()
{
struct AudioHandler *FindAudioHandler();

if (state->hand==NULL) 
	{
	hand=FindAudioHandler(NULL,-1,0,NULL,-1);
	if (hand)
		{
		state->hand=hand;
		state->handler_name=hand->name;
		state->handler_class_id =hand->class_id;
		}
	else {telluser("EDIT:","Could not find a Handler.",NULL);return(-1);}
	}
else	hand=state->hand;

Hmod=hand->handler_mod;
return(NULL);
}
/**************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
