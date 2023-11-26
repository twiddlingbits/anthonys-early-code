#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "Clib/Macros.h"
#include "ep.data"
/**********************************/
/* Declarations for CBACK */ 
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_MASTER_PREFS";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
void HidePrefWind();
struct window *DisplayPrefWind();
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
char *FindFileName();
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void Quit();
void handle_studioevents();
void handle_intuievents();
void WindGadSize();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
/*struct Module *hand;*/
struct Module *mod;
struct Window *wind;
struct StandardModState *state;
/**********************************/
/**********************************/
char *OldPath[100];
int DigX[6];
void wdraw_digits();
void UpdateTime();
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void _main(argv)
char *argv;
{
int mask;
InitModule(argv);			/** initalize module **/

while (TRUE)
	{
	mask=0;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();

	}
}
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
static struct NewModule ModData ={NULL,0,sizeof(struct StandardModState),0};

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((  mod=(struct Module *)AddModule(&ModData)  )==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Master Preferences:","Could Not Add Module",0);
	Quit();
	}

if (StudioBase->defaults.screen_viewmodes&LACE) Interbutton.flags|=BUTN_HIT;
else Interbutton.flags&=~BUTN_HIT;

state=mod->state;

se_cmd_select_module		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window		= GetEventID("SE_CMD_HIDE_WINDOW");

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"),mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) HideWind();
if (mod) DeleteModule(mod);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module && (struct Module *)event->arg1==mod)
			if (wind==NULL) DisplayWind();

	/*********************************/
	if(event->type == se_cmd_kill_module && (struct Module *)event->arg1 == mod)
		{
		ReplyMsg(event);
		Quit();
		}

	ReplyMsg(event);
	}
}
/**************************************************************/
/**************************************************************/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
struct Gadget *gad;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	gad=(struct Gadget *)message->IAddress;
	class=message->Class;
	ReplyMsg(message);

	if (class == CLOSEWINDOW)
		{
		if (wind) HideWind();
		return;
		}

	if (class == NEWSIZE)
		WindGadSize();
	if (class == GADGETUP)
		{
		}
	}
}
/**************************************************************/
struct ViewPort *vport;
int R,G,B;
/**************************************************************/
int XXX(x)
int x;
{
int z;
z=16;
while(z!=0 && ((1<<z)>x) )
	z--;
return(z);
}
/**************************************************************/
void DisplayWind()
{
struct Node *node;
struct SmpteSource *source;
char buff[100];
int x,y,flagy,oldflagy;
USHORT clr;
struct FileFilter *ff;
struct Window *OpenNewWind();
vport=&StudioBase->screen->ViewPort;
strcpy(OldPath,StudioBase->defaults.samp_paths);

if ((wind = OpenNewWind(&NewWind,mod->state,NULL,NULL))!=0)  
	{
clr=GetRGB4(vport->ColorMap,0);
R=(clr>>8)&15;G=(clr>>4)&15;B=clr&15;
SliderRed.top_element=R;
SliderGreen.top_element=G;
SliderBlue.top_element=B;

SliderCopy.top_element=XXX(StudioBase->defaults.copy_buf_size/1024);
SliderActive.top_element=XXX(StudioBase->defaults.active_buf_size/512);

if (StudioBase->defaults.flags & WIND_POPTOFRONT)
	CTFbutton.flags|= BUTN_HIT;
else	CTFbutton.flags&= ~BUTN_HIT;

if (StudioBase->defaults.flags & WIND_SHORTTITLEBAR)
	STBbutton.flags |= BUTN_HIT;
else	STBbutton.flags &= ~BUTN_HIT;

if (StudioBase->defaults.flags & SBD_USE_EXTENDED_MEMORY)
	MEMbutton.flags |= BUTN_HIT;
else	MEMbutton.flags &= ~BUTN_HIT;


SMPTE30button.flags&=~BUTN_HIT;
SMPTE25button.flags&=~BUTN_HIT;
SMPTE24button.flags&=~BUTN_HIT;
if (StudioBase->defaults.smpte_mode==30) SMPTE30button.flags|=BUTN_HIT;
if (StudioBase->defaults.smpte_mode==25) SMPTE25button.flags|=BUTN_HIT;
if (StudioBase->defaults.smpte_mode==24) SMPTE24button.flags|=BUTN_HIT;
if (StudioBase->defaults.smpte_mode==0) SMPTEDFbutton.flags|=BUTN_HIT;

oldflagy=0x7FFFFFFF;
for(x=0;x<NUM_DEFAULT_CLRS;x++)
	{
	flagy=0;
	for(y=0;y<8;y++)
		{
		if (Colors[x][y]==GetRGB4(vport->ColorMap,y)) flagy-=10;
		flagy+=abs((Colors[x][y]&0x000f)-(GetRGB4(vport->ColorMap,y)&0x000f));
		flagy+=abs(((Colors[x][y]&0x00f0)-(GetRGB4(vport->ColorMap,y)&0x00f0))>>4);
		flagy+=abs(((Colors[x][y]&0x0f00)-(GetRGB4(vport->ColorMap,y)&0x0f00))>>8);
		}
	if (flagy<oldflagy) {CLRselect.initial_value=x;oldflagy=flagy;}
	}

	WindGadSize();
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	ColorGadList(&CLRselect);
	if (create_gadget_chain(wind,&button1)==0) ;

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->SmpteSources.lh_Head; node->ln_Succ; node = node->ln_Succ)
	{
	source=(struct SmpteSource *)node;
	sprintf(buff,"%s %d",source->name,source->unit);
	add_list_entry(((struct droplist *)(SMPTEselect.gad_ptr))->l,buff,source);
	if (source=StudioBase->CurrentSmpteSource) select_list_entry_by_userdata(((struct droplist *)(SMPTEselect.gad_ptr))->l,source);
	}

for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) 
	{
	ff=(struct FileFilter *)node;
	add_list_entry(((struct droplist *)(FFselect.gad_ptr))->l, ff->name, ff);
	if (!stricmp(ff->name,StudioBase->defaults.file_filter_name)) select_list_entry_by_userdata(((struct droplist *)(FFselect.gad_ptr))->l,ff);
	}

ReleaseSharedSemaphore(&StudioBase->lockstudio);


	}
else telluser("Master Preferences:","Can not open window!",0);
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;

if (wind)
	{
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/**************************************************************/
char *Int2MegStr(x,str)
int x;
char *str;
{
char dec[5];
sprintf(dec,".%03d",((x%1024)*1000)/1024);
x=x/1024;
if (x<999) sprintf(str,"%d",x);
else if (x<999999) sprintf(str,"%d,%03d",x/1000,x-((x/1000)*1000));
else sprintf(str,"%d,%03d,%03d",x/1000000,(x/1000)-((x/1000000)*1000),x-((x/1000)*1000));
strcat(str,dec);
return(str);
}
/**************************************************************/
void WindGadSize()
{
char str[STUDIO16_MAX_PATH_SIZE];
char str2[40];
int size;

if (wind)
	{
	RefreshWind(wind);
	refresh_gadget_chain(wind);

	draw_indent(wind->RPort,9  ,17  ,400  ,88,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	draw_indent(wind->RPort,9+2,17+1,400-2,88-1,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,300-37,39,400-11,88-5,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	S16Text(wind->RPort," INTERFACE ",11,((9+400)/2)-11*4,20);

	draw_indent(wind->RPort,408  ,17  ,527  ,88,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	draw_indent(wind->RPort,408+2,17+1,527-2,88-1,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	S16Text(wind->RPort," SMPTE ",7,((527+408)/2)-7*4,20);

	draw_indent(wind->RPort,9  ,94  ,350  ,148,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	draw_indent(wind->RPort,9+2,94+1,350-2,148-1,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	S16Text(wind->RPort," MEMORY ",8,((9+350)/2)-8*4,94+3);

	draw_indent(wind->RPort,358  ,94  ,527  ,148,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	draw_indent(wind->RPort,358+2,94+1,527-2,148-1,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	S16Text(wind->RPort," RECORD FORMAT ",15,((358+527)/2)-15*4,94+3);

	SetAPen(wind->RPort,StudioBase->defaults.colors.button.draw_unselected);
	Move(wind->RPort,372,123);
	Text(wind->RPort,"Record Path",11);

	sprintf(str,"%s",StudioBase->defaults.samp_paths);
	if (strlen(str)>18) {str[18]=NULL;str[17]='*';}

	SetAPen(wind->RPort,StudioBase->defaults.colors.button.draw_unselected);
	Move(wind->RPort,372,123+10);
	Text(wind->RPort,str,MIN(strlen(str),18));

	size=DiskSpaceAvail(StudioBase->defaults.samp_paths);
	if (size>=0) sprintf(str,"Free:%10sM",Int2MegStr(size,str2));
	else sprintf(str,"Not Mounted");
	Move(wind->RPort,372,123+20);
	Text(wind->RPort,str,16);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
int ClrNo;
struct ColorMap *ClrMap;
USHORT clr;
void ChangeColor(k)
struct button *k;
{
if (k->flags & BUTN_HIT)
	{
	ClrNo=(int)k->user_data;

	clr=GetRGB4(vport->ColorMap,ClrNo);
	R=(clr>>8)&15;G=(clr>>4)&15;B=clr&15;
	((struct slider *)(SliderRed.gad_ptr))->top_element=R; move_knob(SliderRed.gad_ptr);
	((struct slider *)SliderGreen.gad_ptr)->top_element=G; move_knob(SliderGreen.gad_ptr);
	((struct slider *)SliderBlue.gad_ptr)->top_element=B;  move_knob(SliderBlue.gad_ptr);
	}
}
/**************************************************************/
void SmpteSelfunc(k)
struct slider *k;
{
struct list_entry *le;
struct SmpteSource *x;

le=(((struct droplist *)(SMPTEselect.gad_ptr))->l->active_entry);
x=(struct SmpteSource *)le->user_data;
SetSmpteSource(x);
StudioBase->defaults.SmpteSourceUnit=x->unit;
strcpy(StudioBase->defaults.SmpteSourceName,x->name);
}
/**************************************************************/
void NewColorFunc(k)
struct slider *k;
{
int x,z;
USHORT y;
	x=(((struct droplist *)(CLRselect.gad_ptr))->l->current);
	for(z=0;z<8;z++)
		{
		y=Colors[x][z];
		SetRGB4(vport,z,(y>>8)&15,(y>>4)&15,y&15);
		}

	clr=GetRGB4(vport->ColorMap,ClrNo);
	R=(clr>>8)&15;G=(clr>>4)&15;B=clr&15;
	((struct slider *)(SliderRed.gad_ptr))->top_element=R; move_knob(SliderRed.gad_ptr);
	((struct slider *)SliderGreen.gad_ptr)->top_element=G; move_knob(SliderGreen.gad_ptr);
	((struct slider *)SliderBlue.gad_ptr)->top_element=B;  move_knob(SliderBlue.gad_ptr);
}
/**************************************************************/
void NewFFFunc(k)
struct slider *k;
{
struct FileFilter *ff;
ff=(struct FileFilter *)(((struct droplist *)(FFselect.gad_ptr))->l->active_entry->user_data);
strcpy(StudioBase->defaults.file_filter_name,ff->name);
}
/**************************************************************/

void BlueFunc(k)
struct slider *k;
{
char buf[20];

sprintf(buf,"Blue %2d",k->top_element);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

B=k->top_element;
SetRGB4(vport,ClrNo,R,G,B);
}
/**************************************************************/
/**************************************************************/
void GreenFunc(k)
struct slider *k;
{
char buf[20];

sprintf(buf,"Green%2d",k->top_element);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

G=k->top_element;
SetRGB4(vport,ClrNo,R,G,B);
}
/**************************************************************/
/**************************************************************/
void RedFunc(k)
struct slider *k;
{
static char buf[20];

sprintf(buf,"Red  %2d",k->top_element);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
sprintf(buf,"Red  %2d",k->top_element);
Text(k->kw->RPort, buf, 7);

R=k->top_element;
SetRGB4(vport,ClrNo,R,G,B);
}
/**************************************************************/
void CTFfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT) StudioBase->defaults.flags |= WIND_POPTOFRONT;
else StudioBase->defaults.flags &= ~WIND_POPTOFRONT;
}
/**************************************************************/
void MEMfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT) StudioBase->defaults.flags |= SBD_USE_EXTENDED_MEMORY;
else StudioBase->defaults.flags &= ~SBD_USE_EXTENDED_MEMORY;
}
/**************************************************************/
void STBfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT) StudioBase->defaults.flags |= WIND_SHORTTITLEBAR;
else StudioBase->defaults.flags &= ~WIND_SHORTTITLEBAR;
}
/**************************************************************/
void CopyFunc(k)
struct slider *k;
{
char buf[20];

sprintf(buf,"Copy Buffer %4dK",1<<k->top_element);
StudioBase->defaults.copy_buf_size=(1<<k->top_element)*1024;

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 17);

}
/**************************************************************/
void ActiveFunc(k)
struct slider *k;
{
char buf[20];

sprintf(buf,"Channel Buff%4dK",(1<<k->top_element)*4);
StudioBase->defaults.active_buf_size=(1<<k->top_element)*(512);

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 17);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
static struct NewWindow PNewWind = {
   30,
   12,
   PREF_WIND_WIDTH,
   PREF_WIND_HEIGHT,
   BLACK,
   WHITE,
   CLOSEWINDOW,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   0,0,
   "HELP!!!", /** TITLE **/
   0,0,
   104,37,1024,1024,
   CUSTOMSCREEN
   };
struct Window *Pwind=NULL;

static struct NewButton PANICbutton = 
	{NULL,				/* *next (gadget) */
	10,15,190,61,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR,				/* Flags */
	NULL,				/* Call Function */
	NULL,					/* user_data */
	NULL,				/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Panic Button"};					/* *text */

/********************************************************************/
struct Window *DisplayPrefWind(state,le,te)
struct StandardModState *state;
int le,te;
{
struct Window *OpenNewWind();
void HidePrefWind();

if (Pwind==NULL)
	{

	PNewWind.LeftEdge=le-(PNewWind.Width+6);
	PNewWind.TopEdge=te+3;

	if ((  Pwind = OpenNewWind(&PNewWind,mod->state,PREFERENCES,NULL)  )==NULL)  {telluser("Master Preferences:","Error opening custom prefs, Can not open window!",0);return(Pwind);}

	ColorGadList(&PANICbutton);
	if (  create_gadget_chain(Pwind,&PANICbutton)  ==0) {telluser("Master Preferences:","Error opening custom prefs, could not create gadgets",NULL);return(Pwind);}
	}
return(Pwind);
}
/**************************************************************/
void HidePrefWind()
{
struct Window *w;

delete_gadget_chain(Pwind);

if (Pwind)
	{
	w=Pwind;
	Pwind=NULL;
	CloseWind(w);
	}
}
/**************************************************************/

void InterFunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	StudioBase->defaults.screen_viewmodes|=LACE;
else	StudioBase->defaults.screen_viewmodes&=~LACE;
telluser("You must save, quit Studio16, and restart","in order for this function to take effect.",NOTICE_RETURN_NOW);
}
/**************************************************************/
/**************************************************************/
void SmpteFunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT) 
	{
	StudioBase->defaults.smpte_mode=(USHORT)b->user_data;
	}
}
/**************************************************************/
/*****************************************************************/
