#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"

#include "editinstance.pwc"

extern struct StudioBase *StudioBase;
extern short se_cmd_kill_module;
extern short se_info_instance_name_changed;

void EditInstance(state)
struct StandardModState *state;
{
static struct NewButton resident = 
	{NULL,15,45,31,53,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR,
	0,0,NULL,0,NULL,NULL,0};
struct Window *window;
char *newname;
int FLG=0;
struct IntuiMessage *message;

/** Initilize gadgets to match ModState **/

strcpy(instance_nameSIBuff, state->instance_name);
if (state->flags&MS_KEEP_RESIDENT)
	resident.flags |= BUTN_HIT;
else
	resident.flags &= (~BUTN_HIT);

/** Open the edit window **/

NewInstWin.Screen = StudioBase->screen;
window=(struct Window *)OpenNewWind(&NewInstWin,NULL,NULL,NULL);
if (window==NULL)
	return;

PrintIText(window->RPort,&IntuiTextList1,0,0);
Move(window->RPort,114,23);
Text(window->RPort, state->module_name,strlen(state->module_name));

resident.c = &StudioBase->defaults.colors;
create_gadget_chain(window, &resident);

/** Wait for CloseWindow gadget **/
while(FLG==0)
{
WaitPort(window->UserPort);
message=(struct IntuiMessage *)GetMsg(window->UserPort);
HandleIntuitMess(window,message);
if (message->Class==CLOSEWINDOW) FLG=1;
ReplyMsg(message);
}
/** copy any changes made **/

if (resident.gad_ptr->flags&BUTN_HIT)
	state->flags |= MS_KEEP_RESIDENT;
else
	state->flags &= (~MS_KEEP_RESIDENT);


newname = (char *)AllocMem(strlen(instance_nameSIBuff)+1,0);
if (newname) {
	strcpy(newname, instance_nameSIBuff);
	FreeMem(state->instance_name,strlen(state->instance_name)+1);
	state->instance_name=newname;
	}

/** load or unload module's if needed **/

if (state->attached_module==NULL && state->flags&MS_KEEP_RESIDENT) {
	state->flags|=MS_NEED_MODULE;	/** mark so module will pick it when launched **/
	LoadModule(state->module_name);
	}

if (state->attached_module!=NULL)
	if (!(state->flags&MS_KEEP_RESIDENT))
		if (!(state->flags&MS_WINDOW_OPEN)) 
			BroadcastEventParms(se_cmd_kill_module,state->attached_module,0,0,0,0,0);

/** Clean up **/
delete_gadget_chain(window);
CloseWind(window);
BroadcastEventParms(se_info_instance_name_changed,state,0,0,0,0,0);
}