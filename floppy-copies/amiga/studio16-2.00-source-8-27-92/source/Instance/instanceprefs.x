#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "instance.h"

#include "instanceprefs.pwc"

extern struct Module *thismod;
extern short se_cmd_refresh_instance_list;
extern struct StudioBase *StudioBase;
static struct Window *prefwin;
void InstancePrefsClose();

static struct NewButton util = 
	{NULL,15,15,31,23,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,NULL,1,NULL,NULL,"Show Utilities"};

static struct NewButton apps = 
	{&util,15,25,31,33,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,NULL,1,NULL,NULL,"Show Applications"};


struct Window *InstancePrefsOpen(state,le,te)
int le,te;
struct StandardModState *state;
{
/** Initilize gadgets to match ModState **/

if (((struct InstanceModState *)thismod->state)->utility_mode) {
	util.flags |= BUTN_HIT;
	apps.flags &= (~BUTN_HIT);
	}
else {
	apps.flags |= BUTN_HIT;
	util.flags &= (~BUTN_HIT);
	}

/** Open the edit window **/

	NewPrefWin.LeftEdge=le-(NewPrefWin.Width+6);
	NewPrefWin.TopEdge=te+3;

if (prefwin==NULL) {
	prefwin=(struct Window *)OpenNewWind(&NewPrefWin,NULL,HIDEMESSAGE,NULL);
	if (prefwin==NULL)
		return(NULL);
	util.c = &StudioBase->defaults.colors;
	apps.c = &StudioBase->defaults.colors;
	create_gadget_chain(prefwin, &apps);
	}
return(prefwin);
}

void InstancePrefsClose()
{
if (prefwin) {
/** copy any changes made **/
	if (util.gad_ptr->flags&BUTN_HIT)
		((struct InstanceModState *)thismod->state)->utility_mode = TRUE;
	else
		((struct InstanceModState *)thismod->state)->utility_mode = FALSE;
/** Clean up **/
	delete_gadget_chain(prefwin);
	CloseWind(prefwin);
	prefwin=NULL;
	BroadcastEventParms(se_cmd_refresh_instance_list,0,0,0,0,0,0); 
	}
}