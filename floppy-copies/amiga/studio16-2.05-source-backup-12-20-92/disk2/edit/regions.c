#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <libraries/dos.h>
#include "/include/gadlib.h"
#include "/include/soundedit.h"
#include "/include/studio16.h"

static struct NewWindow NewWindowStructure2 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   175,75,   /* window width and height */
   BLACK,WHITE,   /* detail and block pens */
   CLOSEWINDOW|NEWSIZE,   /* IDCMP flags */
   WINDOWSIZING|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Editor Regions",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   110,60,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

/** Gadgets for region display window ***/

struct NewList regionlist=
	{NULL,1,1,1,1,LIST_COOL|LIST_SORTED|MANUAL_XOR|LIST_DRAGABLE|LIST_SINGLESELECT|TEXT_ONLY_REGION_NAME|HIDE_GADGET,0,0,0,0,0,0,0};

extern struct Window *region_win;
extern struct Window *edit_win;
extern struct EditInstance EI[EI_MAX];
extern short longest_ei;
extern unsigned long regionwin_sigbit;
extern struct StudioBase *StudioBase;
extern struct MsgPort *gadlib_port;
extern struct Menu RTitles[];
extern struct MenuItem Items[];

void CloseRegionWin();
void SizeAllRegionLists();
void BuildRegionList();
void RefreshWinImagery();
char *FindActiveText();

/***********************************************************/

void OpenRegionWin()
{
short i;

if ((region_win=(struct Window *)OpenNewWind(&NewWindowStructure2,NULL,NULL,RTitles))==NULL)
	{
	telluser("Can't open region window",0,0);
	return;
	}

regionlist.c=&StudioBase->defaults.colors;
regionlist.notifyme=gadlib_port;
regionwin_sigbit=(1<<region_win->UserPort->mp_SigBit);

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		EI[i].rl=(struct list *)create_gadget(region_win,&regionlist);
		BuildRegionList(&EI[i]);
		}

SizeAllRegionLists();

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE)
		show_gadget(EI[i].rl);
RefreshWinImagery();
}

/***********************************************************/

void CloseRegionWin()
{
if (region_win) {
	delete_gadget_chain(region_win);
	CloseWind(region_win);
	region_win=NULL;
	regionwin_sigbit=0;
	}
}

/***********************************************************/

void handle_regionwinevents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code,menunum,itemnum,subnum;
char *text;
char name[40];
short i;

while ((message=(struct IntuiMessage *)GetMsg(region_win->UserPort))!=0) {
	HandleIntuitMess(region_win,message);
	class=message->Class;
	code=message->Code;
	ReplyMsg(message);

	switch (class) {
	case CLOSEWINDOW:
		CloseRegionWin();
		Items[45].Flags&=(~CHECKED);
		return;

	case NEWSIZE:
		RefreshWind(region_win);	/** Todd's windfunc.c **/
		SizeAllRegionLists();
		RefreshWinImagery();
	   break;

	case MENUPICK:
		menunum=MENUNUM(code);
		itemnum=ITEMNUM(code);
		subnum=SUBNUM(code);
		if (menunum==2) {			/** Editor (project) **/
			switch (itemnum) {
				case 0:	/* add region */
					if (rangeset(&EI[longest_ei])) {
						name[0]=0;
						newgetstr(name,"Enter Region Name:",8*25);
						if (name[0])
							for (i=0; i < EI_MAX; i++)
								if (EI[i].flags&EI_IN_USE)
									show_error(EditCreateRegion(&EI[i],name));
						}
					break;
					
				case 1:	/* delete region */
					text=FindActiveText();
					if (text) {				
						for (i=0; i < EI_MAX; i++)
							if (EI[i].flags&EI_IN_USE)
								show_error(EditDeleteRegion(&EI[i],text));
						}
					else {
						telluser("Edit:","No Region selected.",0);
						}
					break;

				case 2:	/* update region */
					if (rangeset(&EI[longest_ei])) {
						text=FindActiveText();
						if (text) {				
							for (i=0; i < EI_MAX; i++)
								if (EI[i].flags&EI_IN_USE)
									show_error(EditChangeRegion(&EI[i],text));
							}
						else {
							telluser("Edit:","No Region selected.",0);
							}
						}
					break;

				}
			}
		}
	}
}

/********************************************************/

void SizeAllRegionLists()
{
short i;
short width;
short x;

x=6;
width = (region_win->Width-12)/CountEI();

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		new_gadget_size(EI[i].rl,
			x,										/* x1 */
			24,									/* y1 */
			x+width - 6,						/* x2 */
			region_win->Height - 6);		/* y2 */
			x=x+width;
		}
}

/********************************************************/

void RefreshWinImagery()
{
short i,len,width,x;
char *str;

SetDrMd(region_win->RPort, JAM1);
SetAPen(region_win->RPort, StudioBase->defaults.colors.gadget_window.bottomright_3d);

x=6;
width = (region_win->Width-12)/CountEI();

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		Move(region_win->RPort, x, 21);
		x=x+width;
		str=(char *)StrFileName(EI[i].samp_name);
		len=min(strlen(str),(width-6)/8);
		Text(region_win->RPort,str,len);
		}
refresh_gadget_chain(region_win);
}

/********************************************************/

void handle_region_gadgets(event)
struct GadMsg *event;
{
struct Region region;
int error;
char *text;
short i;
short text_ei;
/** Only one entry in all lists can be active at a time **/
/** Handle mutual exclude between lists **/

if (((struct list *)(event->g))->active_entry) {
	text=((struct list *)(event->g))->active_entry->text;
		for (i=0; i<EI_MAX; i++)
			if (EI[i].flags&EI_IN_USE)
				if (EI[i].rl==(struct list *)event->g)
					text_ei=i;
				else
					if (EI[i].rl->active_entry) {
						EI[i].rl->active_entry->flags &= (~LE_SELECTED);
						EI[i].rl->active_entry=NULL;
						EI[i].rl->current=-1;
						refresh_list_text(EI[i].rl);
						}
	}
else {
	return;
	}

if (text) {
	text=(char *)StrRegionName(text);
		if ((error=FindRegionBySfp(EI[text_ei].sfp,text,&region))==NO_ERROR) {
		for (i=0; i<EI_MAX; i++)
			if (EI[i].flags&EI_IN_USE) {
				EI[i].start_mark=region.start;
				EI[i].end_mark=region.end;
				EI[i].marked_samp = TRUE;
				}
		DrawAllRP();
		update_status();
		UpdateLoopingRange();
		}
	show_error(error);
	}
}

/********************************************************/

char *FindActiveText()
{
short i;
static char copy[STUDIO16_MAX_PATH_SIZE];

for (i=0; i<EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE)
		if (EI[i].rl->active_entry) {
			strcpy(copy,(char *)StrRegionName(EI[i].rl->active_entry->text));
			return(copy);
			}
return(NULL);
}

/********************************************************/

int EditCreateRegion(ei,region_name)
struct EditInstance *ei;
char *region_name;
{
struct Region region;
int error;
char comprn[STUDIO16_MAX_PATH_SIZE];

setmem(&region, sizeof(struct Region), 0);

strcpy(region.region_name, region_name);
strcpy(region.samp_name, ei->samp_name);
region.start=ei->start_mark;
region.end=ei->end_mark;
region.parms=ei->sfp->tag.parms;

if ((error=AddRegionBySfp(ei->sfp, &region))==NO_ERROR)	{/** Studio library call **/
	sprintf(comprn,"%s%c%s",region.samp_name,REGION_DELIMITER_CHAR,region.region_name);
	if (region_win)
		add_list_entry(ei->rl, comprn, 0);
	SampleWriteTag(ei->sfp);
	}
return(error);
}

/********************************************************/

int EditDeleteRegion(ei,region_name)
struct EditInstance *ei;
char *region_name;
{
char comprn[STUDIO16_MAX_PATH_SIZE];
int error;

if ((error=DeleteRegionBySfp(ei->sfp, region_name))==NO_ERROR) {	/** Studio library call **/
	if (region_win) {
		sprintf(comprn,"%s%c%s",ei->samp_name,REGION_DELIMITER_CHAR,region_name);
		delete_list_entry_by_text(ei->rl, comprn);
		}
	SampleWriteTag(ei->sfp);
	}
return(error);
}

/********************************************************/

int EditChangeRegion(ei,region_name)
struct EditInstance *ei;
char *region_name;
{
struct Region region;
int error;

setmem(&region, sizeof(struct Region), 0);
if ((error=FindRegionBySfp(ei->sfp,region_name,&region))==NO_ERROR) {
	if ((error=DeleteRegionBySfp(ei->sfp, region_name))==NO_ERROR) {	/** Studio library call **/
		region.start=ei->start_mark;
		region.end=ei->end_mark;	
		error=AddRegionBySfp(ei->sfp, &region);
		SampleWriteTag(ei->sfp);
		}
	}
return(error);
}

/********************************************************/

void BuildRegionList(ei)
struct EditInstance *ei;
{
int i;
char comprn[STUDIO16_MAX_PATH_SIZE];

for (i=0; i < SFT_NUM_REGIONS; i++)
	if (ei->sfp->tag.dregions[i].region_name[0]!=NULL) {
		sprintf(comprn,"%s%c%s",ei->sfp->samp_name,REGION_DELIMITER_CHAR,ei->sfp->tag.dregions[i].region_name);
		add_list_entry(ei->rl, comprn, 0);
		}
}

