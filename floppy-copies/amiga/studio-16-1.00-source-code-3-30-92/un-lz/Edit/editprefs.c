#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"

#include "editprefs.pwc"

extern struct Module *thismod;
extern struct StudioBase *StudioBase;
extern struct TimeCode tc_offset;
extern struct EditInstance *ei;

static struct Window *prefwin;
void EditPrefsClose();

static struct NewButton smpteunits = 
	{NULL,15,15,31,23,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,NULL,1,NULL,NULL,"Time in SMPTE"};

static struct NewButton sampunits = 
	{&smpteunits,15,25,31,33,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,NULL,1,NULL,NULL,"Time in samples"};


struct Window *EditPrefsOpen(state,le,te)
int le,te;
struct StandardModState *state;
{
/** Initilize gadgets to match ModState **/

if (((struct EditState *)thismod->state)->es_flags&ES_SMPTE_UNITS) {
	smpteunits.flags |= BUTN_HIT;
	sampunits.flags &= (~BUTN_HIT);
	}
else {
	sampunits.flags |= BUTN_HIT;
	smpteunits.flags &= (~BUTN_HIT);
	}

/** Open the edit window **/

NewPrefWin.LeftEdge=le-(NewPrefWin.Width+6);
NewPrefWin.TopEdge=te+3;

sprintf(smpte_offset_SIBuff,"%02d:%02d:%02d:%02d",tc_offset.hours,tc_offset.minutes,tc_offset.seconds,tc_offset.frames);

if (prefwin==NULL) {
	prefwin=(struct Window *)OpenNewWind(&NewPrefWin,NULL,HIDEMESSAGE,NULL);
	if (prefwin==NULL)
		return(NULL);
	smpteunits.c = &StudioBase->defaults.colors;
	sampunits.c = &StudioBase->defaults.colors;
	create_gadget_chain(prefwin, &sampunits);
	PrintIText(prefwin->RPort,&IntuiTextList1,0,0);
	}
return(prefwin);
}

void EditPrefsClose()
{
ULONG v,x,y,c,i[12]={0,0,0,0,0,0,0,0,0,0,0,0};

if (prefwin) {
/** copy any changes made **/
	if (smpteunits.gad_ptr->flags&BUTN_HIT)
		((struct EditState *)thismod->state)->es_flags|=ES_SMPTE_UNITS;
	else
		((struct EditState *)thismod->state)->es_flags&=(~ES_SMPTE_UNITS);

/*   sscanf(smpte_offset_SIBuff,"%2d:%2d:%2d:%2d",&i1,&i2,&i3,&i4); */

/********** start tod mod ***********/


y=0;c=0;v=0;
for (x=0;x<13;x++)
	{
	if ((smpte_offset_SIBuff[x]>='0' && smpte_offset_SIBuff[x]<='9')) {y=y*10; y=y+smpte_offset_SIBuff[x]-'0';v++;}
	else {c++; i[c]=y; y=0;v=0;}
	}

if (i[1]>24) i[1]=00;
if (i[2]>59) i[2]=00;
if (i[3]>59) i[3]=00;
if (i[4]>30) i[4]=00;

/********** end tod mod ***********/

   tc_offset.hours=i[1];
   tc_offset.minutes=i[2];
   tc_offset.seconds=i[3];
   tc_offset.frames=i[4];


/** Clean up **/
	delete_gadget_chain(prefwin);
	CloseWind(prefwin);
	prefwin=NULL;
	update_status(ei);
	}
}