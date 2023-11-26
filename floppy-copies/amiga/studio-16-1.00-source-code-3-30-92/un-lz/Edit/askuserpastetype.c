#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"
#include <stdio.h>

#include "askuserpastetype.pwc"

extern struct StudioBase *StudioBase;


static struct NewButton insertbutn = 
	{NULL,15,20,95,32,BUTN_COOL,0,0,0,0,0,0,"Insert"};

static struct NewButton overwritebutn = 
	{&insertbutn,15,36,95,48,BUTN_COOL,0,0,0,0,0,0,"Replace"};

static struct NewButton cancelbutn = 
	{&overwritebutn,15,52,95,64,BUTN_COOL,0,0,0,0,0,0,"Cancel"};


int AskUserPasteType()
{
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;

/** Open the edit window **/

AskWin.Screen = StudioBase->screen;
AskWin.LeftEdge=-1;
AskWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&AskWin,NULL,NULL,NULL);
if (window==NULL)
	return(PT_CANCEL);

insertbutn.c = &StudioBase->defaults.colors;
cancelbutn.c = &StudioBase->defaults.colors;
overwritebutn.c = &StudioBase->defaults.colors;

insertbutn.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutn.notifyme=insertbutn.notifyme;
overwritebutn.notifyme=insertbutn.notifyme;

create_gadget_chain(window, &cancelbutn);

/** Wait butn click **/

while (TRUE) {
	Wait((1<<window->UserPort->mp_SigBit)|(1<<insertbutn.notifyme->mp_SigBit));
	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
		}
	if (gadmsg=(struct GadMsg *)GetMsg(insertbutn.notifyme)) {
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		break;
		}
	}

delete_gadget_chain(window);
DeletePort(insertbutn.notifyme);
CloseWind(window);
if (gadpressed==insertbutn.gad_ptr)
	return(PT_INSERT);
else if (gadpressed==overwritebutn.gad_ptr)
	return(PT_OVERWRITE);
else
	return(PT_CANCEL);
}
