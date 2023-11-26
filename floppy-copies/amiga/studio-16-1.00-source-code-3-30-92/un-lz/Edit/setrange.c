#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

#include "setrange.pwc"

static struct NewButton okbutnguide = 
	{NULL,10,43,44,53,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutnguide = 
	{&okbutnguide,130,43,186,53,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

extern struct StudioBase *StudioBase;
void ValRng();

void SetRange(ei)
struct EditInstance *ei;
{
struct Window *window;
int temp;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
extern struct EditState *thisstate;
struct TimeCode tc1,tc2;
int a,b,c,d;

/** Initilize gadgets to match samp **/

if (ei->marked_samp) {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		SampToSmpte(&tc1, ei->start_mark, TRUE);
		SampToSmpte(&tc2, ei->end_mark, TRUE);
		sprintf(sm_nameSIBuff,"%02d:%02d:%02d:%02d",
			tc1.hours,tc1.minutes,tc1.seconds,tc1.frames);
		sprintf(em_nameSIBuff,"%02d:%02d:%02d:%02d",
			tc2.hours,tc2.minutes,tc2.seconds,tc2.frames);
		}
	else {
		sprintf(sm_nameSIBuff,"%d",ei->start_mark);
		sprintf(em_nameSIBuff,"%d",ei->end_mark);
		}
	}
else {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(sm_nameSIBuff,"00:00:00:00");
		sprintf(em_nameSIBuff,"00:00:00:00");
		}
	else {
		sprintf(sm_nameSIBuff,"0");
		sprintf(em_nameSIBuff,"0");
		}
	}

/** Open the edit window **/

NewSetRangeWin.Screen = StudioBase->screen;
NewSetRangeWin.LeftEdge=-1;
NewSetRangeWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&NewSetRangeWin,NULL,NULL,NULL);
if (window==NULL)
	return;

cancelbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutnguide.notifyme=okbutnguide.notifyme;
create_gadget_chain(window, &cancelbutnguide);

PrintIText(window->RPort,&IntuiTextList1,0,0);

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


	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sscanf(sm_nameSIBuff,"%d:%d:%d:%d",&a,&b,&c,&d);
		ei->start_mark=SmpteToSamp(a,b,c,d);
		sscanf(em_nameSIBuff,"%d:%d:%d:%d",&a,&b,&c,&d);
		ei->end_mark=SmpteToSamp(a,b,c,d);
		}
	else {
		sscanf(sm_nameSIBuff,"%d",&ei->start_mark);	
		sscanf(em_nameSIBuff,"%d",&ei->end_mark);
		}

	ei->marked_samp = ei->cur;

	ValRng(&ei->start_mark, ei->cur->length-1);
	ValRng(&ei->end_mark, ei->cur->length-1);

	if (ei->start_mark > ei->end_mark) {
		temp=ei->start_mark;
		ei->start_mark=ei->end_mark;
		ei->end_mark=temp;
		}
	}

delete_gadget_chain(window);
DeletePort(okbutnguide.notifyme);

CloseWind(window);
}

void ValRng(val, top)
int *val;
int top;
{
if (*val < 0)
	*val = 0;
if (*val > top)
	*val=top;
}