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
void ExpandString();

int OffS,OffE;
/***********************************************************/

BOOL SetRange()
{
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
BOOL ret=FALSE;

/** Initilize gadgets to match samp **/

sprintf(sm_nameSIBuff,"00:00:00:00");
sprintf(em_nameSIBuff,"23:59:59:00");

/** Open the window **/

NewSetRangeWin.Screen = StudioBase->screen;
NewSetRangeWin.LeftEdge=-1;
NewSetRangeWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&NewSetRangeWin,NULL,NULL,NULL);
if (window==NULL) return(FALSE);

cancelbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutnguide.notifyme=okbutnguide.notifyme;
create_gadget_chain(window, &cancelbutnguide);

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) 
	{
	Wait((1<<window->UserPort->mp_SigBit)|(1<<okbutnguide.notifyme->mp_SigBit));

	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) 
		{
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
			{
			ExpandString(sm_nameSIBuff);
			ExpandString(em_nameSIBuff);
			RefreshGList(&str_em,window,NULL,1);
			RefreshGList(&str_sm,window,NULL,1);
			}
		}

	if (gadmsg=(struct GadMsg *)GetMsg(okbutnguide.notifyme)) 
		{
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		break;
		}
	}


if (gadpressed==okbutnguide.gad_ptr) 
	{
	ret=TRUE;
	OffS=TCstr2int(sm_nameSIBuff);
	OffE=TCstr2int(em_nameSIBuff);
	}

delete_gadget_chain(window);
DeletePort(okbutnguide.notifyme);

CloseWind(window);
return(ret);
}

/***********************************************************/

void ExpandString(str)
char *str;
{
struct TimeCode tc1;
long j;

j=TCstr2int(str);
movmem(&j,&tc1,4);
sprintf(str,"%02d:%02d:%02d:%02d",
	tc1.hours,tc1.minutes,tc1.seconds,tc1.frames);
}

/***********************************************************/

