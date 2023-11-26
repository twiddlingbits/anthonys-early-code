#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <libraries/dos.h>
#include <devices/timer.h>
#include "/include/gadlib.h"
#include "/include/soundedit.h"
#include "/include/studio16.h"

/**** CBACK.o defines *****/

long _stack = 8000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_EDITOR";	/* The name of the task to create */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O   */

/** My default edit NewWindow struct **/

static struct NewWindow NewWindowStructure1 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   BLACK,WHITE,   /* detail and block pens */
   MOUSEMOVE|MENUPICK|MOUSEBUTTONS|RAWKEY|CLOSEWINDOW|NEWSIZE,   /* IDCMP flags */
   WINDOWSIZING|WINDOWDRAG|WINDOWDEPTH|REPORTMOUSE|ACTIVATE|WINDOWCLOSE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Editor",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   200,100,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

/** This modules function delclerations **/

short SampleGetOne();
int DoNonDestCut();
int DoNonDestCopy();
int DoNonDestCopy();
int DoNonDestPaste();
int DoNonDestKeep();
int DoNonDestDelete();
int DoNonDestUndo();
int DoNonDestNoClips();
int DoCutEdit();
int DoCopyEdit();
int DoCopyAsEdit();
int DoPasteEdit();
int DoKeepEdit();
int DoCompressEdit();
int DoFlipEdit();
int DoZeroEdit();
int DoInvertEdit();
int DoFFTEdit();
int DoEchoEdit();
int Scale();
int DoNormalizeEdit();
int Resample();
int DoDeleteEdit();
int RecalcGraphHeight();
void HandleMouseDrag(SHORT,SHORT);
void NewGhost();
void UnNewGhost();
void DrawEditGraphBuffers();
void PutUpdateText();
void PlayAllSamps();
void DoNew();
void DoOpen();
void ProcessEdit();
void FreeRenderBuffer();
void start_dragging();
void close_samp();
void RecalcEILongest();
void RegenGraph();
void EditNewSamp();
void handle_freehand_events();
void zoom_in_func();
void zoom_out_func();
void do_zoom_or_scroll();
void do_zoom();
void hscroll_func();
void update_status();
void CompLine();
void AllocGraphBuffer();
void AllocOrgGraphBuffer();
void FreeOrgGraphBuffer();
void DoFreehand();
void OpenFreehand();
void StartFreehand();
void EndFreehand();
void CloseFreehand();
void SampToSmpte();
void PadFreeMem();
unsigned long PadAllocMem();
struct SampleFilePointer *SampleCreateRevision();
struct SampleFilePointer *SampleOpenInDefPath();
struct SampleFilePointer *SampleOpen();
char *FindSample();
void VerifySampleIntegrity();
void set_new_em();
int in_graph();
void draw_marked();
void close_edit();
void handle_studioevents();
void open_edit();
void handle_intuievents();
void calc_edit_sigbit();
void InitEdit();
void handle_gadlibevents();
struct Window *OpenNewWind();
void handle_graph_events();
void ReleaseLocks();
void CopySampData();
void CopySampDataExceptRegions();
void InitGraphBuffer();
struct EditInstance *InitEditInstance();
void FreeGraphBuffer();
long CopyGraphData();
void start_timer();
void stop_timer();
void handle_timer_events();
void UpdateLoopingRange();

/***** buttons ******/

struct NewButton zmout = {
   NULL,0,0,0,0,BUTN_COOL|MANUAL_XOR,zoom_out_func,0,0,0,0,0,"Zm Out"};

struct NewButton zmin = {
   &zmout,0,0,0,0,BUTN_COOL|MANUAL_XOR,zoom_in_func,0,0,0,0,0,"Zm In"};

struct NewButton showrange = {
   &zmin,0,0,0,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Show Range"};

struct NewButton rangeall = {
   &showrange,0,0,0,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Range All"};

struct NewButton showall = {
   &rangeall,0,0,0,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Show All"};

struct NewButton stop = {
   &showall,0,0,0,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Stop"};

struct NewButton playrange = {
   &stop,0,0,0,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Play Rng"};

struct NewSlider hscroll= {
    &playrange,0,0,0,0,AUTO_KNOB|SLIDER_COOL|FREE_HORIZ|MANUAL_XOR|KNOB_TODDISH|SLIDER_ARROWS,
    hscroll_func,0,0,0,0,0,0,0,0,-1,-1};

/*** Variables global to all files ***/

struct Window *edit_win;
struct Window *region_win;
long zoom_scale;		/* samples per pixel */
long max_zoom;			/* largest zoom that will fit in this window */
short longest_ei;
long edit_flags;

/** variables global to this module **/

static int freehand_size;
static int freehand_gs;
struct EditInstance *freehand_ei;
static short freehand_buf[1024];
static last_mousex;
static short dragging;
long new_zoom;
int new_graph_start;
unsigned long regionwin_sigbit;
unsigned long update_graph_sigbit;
unsigned long update_graph_allocbit;
unsigned long close_freehand_sigbit;
unsigned long close_freehand_allocbit;
unsigned long edit_sigbit=0;
struct Module *thismod;
struct EditState *thisstate;
struct Task *tc_edit;
struct MsgPort *gadlib_port;
short se_cmd_kill_module;
short se_cmd_trigger_playback;
short se_cmd_trigger_timecode;
short se_info_channel_done;
short se_cmd_edit_sample;
short se_cmd_play_sample;
short se_cmd_stop_all_playback;
short se_info_sample_edited;
short se_info_sample_deleted;
short se_cmd_select_module;
short se_info_module_closed;
short se_info_module_opened;

struct MsgPort *timerport;
struct timerequest *timermsg;
short WaitingForTimer=FALSE;
short disable_gadgets=TRUE;

struct EditInstance EI[EI_MAX];

/** function's used in this module but defined externally **/

struct MenuItem *ItemAddress();

/** variables used in the module but defined externally **/

extern struct Menu Titles[];
extern struct MenuItem Items[];
extern struct MenuItem SubItems[];
extern struct StudioBase *StudioBase;

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long studio_sigbit;
unsigned long gadlib_sigbit;
unsigned long timer_sigbit;

openlibraries();

MarkStack();			/*** DEBUG!!!! REMOVE!!!**/

StandardEnviroment();	/** opens studio16 customscreen **/
InitEdit();					/** initalize module **/

studio_sigbit=(1<<thismod->notifyme->mp_SigBit);
gadlib_sigbit=(1<<gadlib_port->mp_SigBit);
timer_sigbit=(1<<timerport->mp_SigBit);

while (TRUE) {
   mask=Wait(edit_sigbit|update_graph_sigbit|studio_sigbit|gadlib_sigbit|timer_sigbit|close_freehand_sigbit|regionwin_sigbit);
	if (mask & edit_sigbit)
		handle_intuievents();
	if (mask & studio_sigbit)
		handle_studioevents();
	if (mask & gadlib_sigbit)
		handle_gadlibevents();
	if (mask & update_graph_sigbit)
		handle_graph_events();
	if (mask & timer_sigbit)
		handle_timer_events();
	if (mask & close_freehand_sigbit)
		handle_freehand_events();
	if (mask & regionwin_sigbit)
		handle_regionwinevents();
	}
}

/***********************************************************/

struct StandardModState *state;
void InitEdit()
{
static struct NewModule edit_data ={"Editor",0,sizeof(struct EditState),0};
struct NewGadgetHeader *ng;
int i;

if ((thismod=(struct Module *)AddModule(&edit_data))==0) {
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	closelibraries();
   exit(10);
   }
state=thismod->state;

thisstate=(struct EditState *)thismod->state;
if (!(state->flags & MS_STATE_INITILIZED))
	{
	thisstate->copy_path[0]=0;
	thisstate->es_flags|=ES_SMPTE_UNITS;
	thisstate->es_flags|=ES_OKAY_REQ;
	}

se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_trigger_timecode	=GetEventID("SE_CMD_TRIGGER_TIMECODE");
se_cmd_trigger_playback	=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_channel_done		=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_edit_sample		=GetEventID("SE_CMD_EDIT_SAMPLE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_info_sample_edited	=GetEventID("SE_INFO_SAMPLE_EDITED");
se_info_sample_deleted	=GetEventID("SE_INFO_SAMPLE_DELETED");
se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_cmd_select_module, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_edit_sample, thismod);

update_graph_allocbit=AllocSignal(-1);
update_graph_sigbit=1<<update_graph_allocbit;
close_freehand_allocbit=AllocSignal(-1);
close_freehand_sigbit=1<<close_freehand_allocbit;
tc_edit = (struct Task *)FindTask(0);

gadlib_port = (struct MsgPort *)CreatePort(0,0);	/**!!! ERROR CHECK? **/ 
for (ng=(struct NewGadgetHeader *)&hscroll; ng; ng=ng->next) {
	ng->c = &StudioBase->defaults.colors;
	ng->notifyme=gadlib_port;
	}

timerport = (struct MsgPort *) CreatePort( 0, 0 );
if (timerport) {
	timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
	OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );
	}

/*** Zero all EditInstances so they are not in use **/

for (i=0; i < EI_MAX; i++)
	setmem(&EI[i], sizeof(EI[i]), 0);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);
}


/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
long adoslock;
short i;
short turnofftimer;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
		if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			DoNew();					/** Free EI resources in use **/
			close_edit();			/** close window **/
		   FreeSignal(update_graph_allocbit);
			DeleteModule(thismod);
FindMaxStackSize();		/** !!!!!!!! DEBUG !!!!!!!!!!! **/
			closelibraries();
			DeletePort(gadlib_port);
			stop_timer();
			if (timerport) {
				DeletePort(timerport);
				CloseDevice( (struct IORequest *) timermsg );
				DeleteExtIO( (struct IORequest *) timermsg );
				}
			exit(0);
			}

		else if (event->type==se_cmd_select_module && (struct Module *)event->arg1==thismod) {
			if (edit_win==NULL)
				open_edit();
			}

		else if (event->type==se_cmd_edit_sample) {
			if (edit_win==NULL)
				open_edit();
			EditNewSamp(event->arg1);
			}

		else if (event->type==se_info_sample_deleted) {
			for (i=0; i < EI_MAX; i++) {
				if (EI[i].flags&EI_IN_USE) {
					if (stricmp(event->arg1,EI[i].samp_name)==0) {	/** Ignore messages generated by edit DoPaste() **/
						if (adoslock=Lock(EI[i].samp_name,ACCESS_READ)) {
							UnLock(adoslock);
							}
						else {
							CloseRegionWin();
							close_samp(&EI[i]);
							refresh_edit_win(edit_win);
							}
						}
					}
				}

			if (Items[45].Flags&CHECKED && region_win==NULL)
				if (CountEI()==0)
					Items[45].Flags &= (~CHECKED);
				else
					OpenRegionWin();
			}

		else if (event->type==se_cmd_trigger_playback || event->type==se_cmd_trigger_timecode) {
			if (edit_win) {
				for (i=0; i < EI_MAX; i++) {
					if (EI[i].flags&EI_IN_USE) {
						if (stricmp(((struct ActiveSample *)event->arg1)->region.samp_name,EI[i].samp_name)==0) {	
							stop_timer();
							EI[i].as=(struct ActiveSample *)event->arg1;
							start_timer();
							}
						}
					}
				}
			}

		else if (event->type==se_info_channel_done) {
			if (edit_win) {
				turnofftimer=TRUE;
				for (i=0; i < EI_MAX; i++) {
					if (EI[i].flags&EI_IN_USE) {
						if (EI[i].as==(struct ActiveSample *)event->arg1) {
							CompLine(&EI[i], EI[i].line_pos);	/** erase **/
							EI[i].line_pos=0xFFFFFFFF;	/** No line drawn **/
							EI[i].as=NULL;
							}
						if (EI[i].as)
							turnofftimer=FALSE;
						}
					}
					if (turnofftimer) {
						stop_timer();
						}
				}
			}

	ReplyMsg(event);
	}
}

/***********************************************************/

void CompLine(ei, pos)

struct EditInstance *ei;
unsigned long pos;
{
int graph_end;

if (pos != 0xFFFFFFFF) {
	graph_end=ei->graph_start+ei->graph_width*zoom_scale;
	if (pos >= ei->graph_start && pos < graph_end) {
		LockGadLibRender();
		LockLayer(0,edit_win->WLayer);
		edit_win->RPort->Mask = 4;
		pos = (pos - ei->graph_start)/zoom_scale+GRAPH_X;
		SetDrMd(edit_win->RPort, COMPLEMENT);
		Move (edit_win->RPort, pos, ei->graph_y);
		Draw (edit_win->RPort, pos, ei->graph_y+ei->graph_height-1);
		edit_win->RPort->Mask = 0xFF;
		UnlockLayer(edit_win->WLayer);
		UnLockGadLibRender();
		}
	}	
}


/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;
short i;

while ((event=(struct GadMsg *)GetMsg(gadlib_port))!=0) {

	if (disable_gadgets) {
		if (event->g->flags&GAD_BUTTON)
			DisplayBeep(StudioBase->screen);
		}

	else if (event->g->w==region_win) {
		handle_region_gadgets(event);
		}

	else if (event->type==GADMSG_ACTIVE) {
		if (event->g==playrange.gad_ptr) {
			if (rangeset(&EI[longest_ei])) {
				PlayAllSamps(EI[longest_ei].start_mark,EI[longest_ei].end_mark-EI[longest_ei].start_mark+1);
				if (FindLoopCt())
					edit_flags|=EDIT_RANGE_LOOPING;
				}
			}

		else if (event->g==stop.gad_ptr) {
			edit_flags&=(~EDIT_RANGE_LOOPING);
			BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
			}

		else if (event->g==showall.gad_ptr) {
			new_zoom=max_zoom;
			do_zoom();
			}

		else if (event->g==rangeall.gad_ptr) {
			for (i=0; i<EI_MAX; i++)
				if (EI[i].flags&EI_IN_USE) {
					EI[i].start_mark=0;
					EI[i].end_mark=EI[i].sfp->tag.length-1;
					EI[i].marked_samp = TRUE;
					}
			if (max_zoom!=zoom_scale) {
				new_zoom=max_zoom;
				do_zoom();
				}
			else {
				DrawAllRP();
				update_status();
				}
			UpdateLoopingRange();
			}

		else if (event->g==showrange.gad_ptr) {
			if (rangeset(&EI[longest_ei])) {
				new_zoom=(EI[longest_ei].end_mark-EI[longest_ei].start_mark+1)/EI[longest_ei].graph_width;
				edit_flags |=EDIT_CENTER_RANGE;
				do_zoom_or_scroll();
				}
			}
		}
	ReplyMsg(event);
	}
}

/************************************************************/

rangeset(ei)
struct EditInstance *ei;
{
if (ei->marked_samp)
	return(TRUE);

telluser("No range marked",0,0);
return(FALSE);
}

/************************************************************/

void CopySampData(wtag,rtag)
struct SampleFileTag *rtag,*wtag;
{
int i;

wtag->parms=rtag->parms;
for (i=0; i < SFT_NUM_REGIONS; i++)
	wtag->dregions[i]=rtag->dregions[i];
}

/************************************************************/

void CopySampDataExceptRegions(wtag,rtag)
struct SampleFileTag *rtag,*wtag;
{
wtag->parms=rtag->parms;
}

/************************************************************/

void open_edit()
{
short i;

thisstate->es_flags |= ES_IN_USE;
NotifyMeOnEvent(se_info_sample_deleted, thismod);
NotifyMeOnEvent(se_cmd_trigger_playback, thismod);
NotifyMeOnEvent(se_cmd_trigger_timecode, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);

/******* Set any CHECKMARKS needed *******/

if (thisstate->es_flags&ES_OKAY_REQ)
	Items[43].Flags|=CHECKED;
else
	Items[43].Flags&=(~CHECKED);

if (thisstate->es_flags&ES_SMPTE_UNITS)
	Items[44].Flags&=(~CHECKED);
else
	Items[44].Flags|=CHECKED;

if (thisstate->es_flags&ES_FAST_GRAPH)
	Items[47].Flags|=CHECKED;
else
	Items[47].Flags&=(~CHECKED);

for (i=0; i < 5; i++)
	SubItems[6+i].Flags&=(~CHECKED);

SubItems[6+thisstate->grid].Flags|=CHECKED;

/***** Ghost Menu Items not available unless a sample is loaded **/

NewGhost();

/****** Open the Edit Window ******/

if ((edit_win=OpenNewWind(&NewWindowStructure1,thismod->state,NULL,Titles))==NULL)
	{
	telluser("Can't open edit window",0,0);
	thisstate->es_flags &= (~ES_IN_USE);
	return;
	}

calc_edit_sigbit();	/** changes when window changes **/

hscroll.top_element = 0;
hscroll.list_size = 1;
hscroll.show_size = 1;

/** Setup buffer that stores waveform summery to graph from **/

EditSetGadsBox();	/* set box_x1, etc. */

create_gadget_chain(edit_win,&hscroll);

/** Draw the graph & gadgets at current zoom setting **/

/** zero trashed variables **/

edit_flags=0;

refresh_edit_win(edit_win);

if (thisstate->es_flags&ES_SHOW_REGIONS)
	OpenRegionWin();

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);
}

/***********************************************************/

void EditNewSamp(samp)
char *samp;
{
struct EditInstance *ei;
short i;
ei=InitEditInstance(samp);
if (ei==0) {
	telluser("Edit:","Can't edit sample. Maximum of 8 samples can be edited simultaneously.",0);
	return;
	}

VerifySampleIntegrity(ei);
RecalcEILongest();
max_zoom = EI[longest_ei].sfp->tag.length/DRAWWIDTH+1;

/** Setup gadgets **/

if (CountEI()==1) {		/** first sample added? **/
	UnNewGhost();
	zoom_scale = max_zoom;	
	new_zoom = zoom_scale;
	ei->graph_start = 0;
	}

if (RecalcGraphHeight()!=NO_ERROR) {
	SampleClose(ei->sfp);
	return;
	}

ClearAllGraphs();		/** Rectfill all graph drawings **/
for (i=0; i<EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE && ei!=&EI[i]) {
		BuildEIRP(&EI[i]);
		}
DrawAllRP();

/** Build Graph buffer **/

RegenGraph(ei, TRUE);
update_status();
FillUndoBuffer(ei);	/** Set undobuffer to current state **/

/** update horiz. scroll size in case new sample is now largest **/

((struct slider *)(hscroll.gad_ptr))->list_size = EI[longest_ei].sfp->tag.length;
((struct slider *)(hscroll.gad_ptr))->show_size = EI[longest_ei].graph_width*zoom_scale;
((struct slider *)(hscroll.gad_ptr))->top_element=EI[longest_ei].graph_start;
new_gadget_size(hscroll.gad_ptr,
	GRAPH_X-2,												/* x1 */
	edit_win->Height - GRAPH_Y2 + 2,					/* y1 */
	edit_win->Width - GRAPH_X + 1,					/* x2 */
	edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H);/* y2 */
refresh_gadget(hscroll.gad_ptr);

/** Update window title to show this sample **/

RefreshEditTitle();
}

/***********************************************************/

struct EditInstance *InitEditInstance(samp)
char *samp;
{
char buf[STUDIO16_MAX_PATH_SIZE];
struct EditInstance *ei;
int count;

strcpy(buf,samp);		/** in case passed ei->samp_name **/
samp=buf;

count=CountEI();
if (count==EI_MAX)
	return(NULL);

ei=&EI[count];
setmem(ei, sizeof(struct EditInstance), 0);
ei->flags=EI_IN_USE;

ei->sfp=SampleOpen(samp,SP_MODE_READWRITE,0);
if (ei->sfp==0) {
	telluser("Edit:","Can't open sample",0);
	return(0);
	}

strcpy(ei->samp_name,samp);
ei->line_pos=0xFFFFFFFF;	/** No line drawn **/
ei->rp.BitMap = &ei->bm;

return(ei);
}

/***********************************************************/

void VerifySampleIntegrity(ei)
struct EditInstance *ei;
{
/** If no dclips exist for this disk sample, create one **/
/** covering the entire sample **/

if (ei->sfp->tag.dclips[0].end==0) {
	ei->sfp->tag.dclips[0].end=ei->sfp->tag.length-1;
	telluser("Internal error detected by Edit:","No dclips set.",0);
	}

if (ei->sfp->tag.org_length==0) {		/** Just in case someone didn't set it **/
	ei->sfp->tag.org_length=ei->sfp->tag.length;
	telluser("Internal error detected by Edit:","No org_length set.",0);
	}

if (ei->sfp->tag.length==0) {		/** Just in case someone didn't set it **/
	ei->sfp->tag.length=ei->sfp->tag.org_length;
	telluser("Internal error detected by Edit:","No length set.",0);
	}

if (ei->sfp->tag.parms.rate==0) {
	ei->sfp->tag.parms.rate=44100;
	telluser("Internal error detected by Edit:","No sampling rate set.",0);
	}
}

/************************************************************/

void handle_intuievents()
{
struct IntuiMessage *message;
USHORT code,menunum,itemnum,subnum;
ULONG class;
SHORT mousex,mousey;
struct Gadget *gad;
int temp;
short i;
static long grab_start,grab_end;
long presm;

while ((message=(struct IntuiMessage *)GetMsg(edit_win->UserPort))!=0) {
	class=message->Class;
	code=message->Code;
	gad=(struct Gadget *)message->IAddress;
	mousex=message->MouseX;
	mousey=message->MouseY;
	HandleIntuitMess(edit_win,message);
	ReplyMsg(message);

	if (class==MOUSEMOVE) { 	/** Throw away extra mouse moves **/
	   while(message=(struct IntuiMessage *)GetMsg(edit_win->UserPort)) {
	      class=message->Class;
			code=message->Code;
			gad=(struct Gadget *)message->IAddress;
			mousex=message->MouseX;
			mousey=message->MouseY;
			if (class!=MOUSEMOVE) {
				break;
				}
			else {
				HandleIntuitMess(edit_win,message);
				ReplyMsg(message);
				}
			}
		}

	/** Update draging status on any event **/
	HandleMouseDrag(mousex,mousey);

	switch (class) {

	case CLOSEWINDOW:
		DoNew();
		close_edit();
		return;

	case NEWSIZE:
		RefreshWind(edit_win);	/** Todd's windfunc.c **/
		if (CountEI() > 0) {
			RecalcGraphHeight();	/** ei->graph_height, etc. **/
			RethinkZoom();
			}
	   refresh_edit_win(edit_win);
	   break;

	case MOUSEBUTTONS:
	   if (code==SELECTUP) {
	      if (dragging) {
   	      dragging=FALSE;
				edit_win->Flags &= (~RMBTRAP);
				for (i=0; i < EI_MAX; i++)
					if (EI[i].flags&EI_IN_USE) {
			         if (EI[i].start_mark > EI[i].end_mark) {     /** swap **/
   			         temp = EI[i].start_mark;
      	   		   EI[i].start_mark = EI[i].end_mark;
							EI[i].end_mark=temp;
	   		         }

					/** Round end_mark up.  Since each pixel defines many **/
					/** samples, the end_mark should include all samples **/
					/** in it's pixel **/
			
						EI[i].end_mark=fullmousetosample(&EI[i],sampletomousenoclip(&EI[i],EI[i].end_mark));
	   		      }
				update_status();
				UpdateLoopingRange();
				}
			else {
				if (edit_flags&EDIT_DRAWING)
					EndFreehand();
				}
	      }

		else if (code==SELECTDOWN) {
			if (in_graph(mousex, mousey) && EI[longest_ei].flags&EI_IN_USE) {
				if (edit_flags&EDIT_FREEHAND) {
					StartFreehand(mousex, mousey);
					break;
					}
		      dragging = TRUE;
				edit_win->Flags |= RMBTRAP;	/** Intuition doesn't send LMB when menus shown **/

            if (EI[longest_ei].marked_samp && closeto (&EI[longest_ei], mousex, EI[longest_ei].end_mark)) {
               set_new_em(mousex);
  	  	         draw_marked(edit_win);
               }
            else if (EI[longest_ei].marked_samp && closeto (&EI[longest_ei], mousex, EI[longest_ei].start_mark)) {
					for (i=0; i < EI_MAX; i++)
						if (EI[i].flags&EI_IN_USE)
               		EI[i].start_mark = EI[i].end_mark;
               set_new_em(mousex);
               draw_marked(edit_win);
               }
            else {
               start_dragging(mousex);
           	   }
				update_status();
		      }
			}
		break;

	case MENUPICK:
		while(code!=MENUNULL) {
			menunum=MENUNUM(code);
			itemnum=ITEMNUM(code);
			subnum=SUBNUM(code);
			if (menunum==2) {			/** Editor (project) **/
				switch (itemnum) {
					case 0:	/* new */
						DoNew();
						refresh_edit_win(edit_win);
						break;

					case 1:	/* Open */
						DoOpen();
						break;

					case 3:	/* Play All */
						PlayAllSamps(0,0);
						break;

					case 4:	/* Play Range */
						if (rangeset(&EI[longest_ei])) {
							PlayAllSamps(EI[longest_ei].start_mark,EI[longest_ei].end_mark-EI[longest_ei].start_mark+1);
							if (FindLoopCt())
								edit_flags|=EDIT_RANGE_LOOPING;
							}
						break;

					case 5:	/* Play Display */
						PlayAllSamps(EI[longest_ei].graph_start,min(EI[longest_ei].graph_width*zoom_scale,EI[longest_ei].sfp->tag.length));
						break;

					case 6:	/* Play To Start */
						if (rangeset(&EI[longest_ei])) {
							if (EI[longest_ei].start_mark > 10) {
								presm=max(0,EI[longest_ei].start_mark-EI[longest_ei].sfp->tag.parms.rate);
								PlayAllSamps(presm,EI[longest_ei].start_mark-presm);
								}
							}
						break;

					case 7:	/* Stop Playing */
						BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
						break;
					}
				}

			else if (menunum==3) {			/** Edit **/
				switch (itemnum) {
					case 1:	/* cut */
						ProcessEdit(PE_NONDEST,DoNonDestCut,0,0);
						break;

					case 2:	/* copy */
						ProcessEdit(PE_NONDEST,DoNonDestCopy,0,0);
						break;

					case 3:	/* paste */
						if (subnum==0)
							ProcessEdit(PE_NONDEST|PE_PARM2,DoNonDestPaste,PT_INSERT_START,0);
						else if (subnum==1)
							ProcessEdit(PE_NONDEST|PE_PARM2,DoNonDestPaste,PT_INSERT_END,0);
						else
							ProcessEdit(PE_NONDEST|PE_PARM2,DoNonDestPaste,PT_OVERWRITE,0);
						break;

					case 4:	/* keep */
						ProcessEdit(PE_NONDEST,DoNonDestKeep,0,0);
						break;

					case 5:	/* delete */
						ProcessEdit(PE_NONDEST,DoNonDestDelete,0,0);
						break;

					case 6:	/* undo last */
						ProcessEdit(PE_NONDEST|PE_NO_RANGE_CHECK,DoNonDestUndo,0,0);
						break;

					case 7:	/* undo all */
						ProcessEdit(PE_NONDEST|PE_NO_RANGE_CHECK,DoNonDestNoClips,0,0);
						break;

					case 8:	/* make permanent */
						ProcessEdit(PE_DEST|PE_NO_RANGE_CHECK,DoCompressEdit,0,"Compress: make 'nondestructive' edits permanent?");
						break;

					case 10:	/* dest cut */
						ProcessEdit(PE_DEST,DoCutEdit,0,"Destructive Cut");
						break;

					case 11:	/* dest copy */
						ProcessEdit(PE_DEST|PE_PARM2,DoCopyEdit,0,"Copy range to CopyBuffer");
						break;

					case 12:	/* dest copy As..*/
						ProcessEdit(PE_DEST,DoCopyAsEdit,0,0);
						break;

					case 13:	/* dest paste */
						if (subnum==0)
							ProcessEdit(PE_DEST|PE_PARM2,DoPasteEdit,PT_INSERT_START,"Destructive Paste");
						else if (subnum==1)
							ProcessEdit(PE_DEST|PE_PARM2,DoPasteEdit,PT_INSERT_END,"Destructive Paste");
						else
							ProcessEdit(PE_DEST|PE_PARM2,DoPasteEdit,PT_OVERWRITE,"Destructive Paste");
						break;

					case 14:	/* dest keep */
						ProcessEdit(PE_DEST,DoKeepEdit,0,"Destructive Keep");
						break;

					case 15:	/* dest Delete */
						ProcessEdit(PE_DEST,DoDeleteEdit,0,"Destructive Delete");
						break;

					}
				}

			else if (menunum==4) {	/** effects **/
				switch (itemnum) {
					case 0:	/* echo */
						ProcessEdit(PE_DEST,DoEchoEdit,0,0);
						break;

					case 1:	/* FFT */
						ProcessEdit(PE_DEST,DoFFTEdit,0,0);
						break;

					case 2:	/* invert */
						ProcessEdit(PE_DEST,DoInvertEdit,0,"Invert Range");
						break;

					case 3:	/* Normalize */
						ProcessEdit(PE_DEST,DoNormalizeEdit,0,0);
						break;

					case 4:	/* reverse */
						ProcessEdit(PE_DEST,DoFlipEdit,0,"Reverse Range");
						break;

					case 5:	/* scale */
						ProcessEdit(PE_DEST,Scale,0,0);
						break;

					case 6:	/* zero */
						ProcessEdit(PE_DEST,DoZeroEdit,0,"Zero Range");
						break;

					case 8:	/* resample */
						ProcessEdit(PE_DEST,Resample,0,0);
						break;

					case 9:	/* Add Silence */
						show_error(DoGenSilence(&EI[longest_ei]));
						break;

					case 10:	/* Gen sine */
						show_error(DoGenSine(&EI[longest_ei]));
						break;
					}
				}

			else if (menunum==5) {	/** Options **/
				switch (itemnum) {
					case 0:	/* Set Range */
						SetRange();
						DrawAllRP();
						update_status();
						break;

					case 1:	/* Set Start Mark */
						if (EI[longest_ei].as) {
							grab_start = ASPos(EI[longest_ei].as);
							}
						break;

					case 2:	/* Set End Mark */
						if (EI[longest_ei].as) {
							grab_end = ASPos(EI[longest_ei].as);
							if (grab_start < grab_end)
								for (i=0; i < EI_MAX; i++)
									if (EI[i].flags&EI_IN_USE) {
										EI[i].start_mark=grab_start;
										EI[i].end_mark=grab_end;
										EI[i].marked_samp = TRUE;
										}
							DrawAllRP();
							update_status();
							}
						break;

					case 3:	/* Set sample parms */
						for (i=0; i < EI_MAX; i++)
							if (EI[i].flags&EI_IN_USE) {
								NewRate(&EI[i]);
								SampleWriteTag(EI[i].sfp);
								}
							update_status();
						break;

					case 4:	/* set display offset */
						DisplayOffset(&EI[longest_ei]);
						update_status();
						break;

					case 6:	/* loop */
						/** (Items[41].Flags&CHECKED) **/
						break;

					case 7:	/* freehand draw */
						if (IsFreehandMenuOn()) {
							new_pointer(PTR_BUSY);
							new_zoom=1;
							if (new_zoom!=zoom_scale) {
								edit_flags |= EDIT_REFRESH_HSCROLL;
								handle_graph_events();
								}
							OpenFreehand();
							new_pointer(PTR_NORMAL);
							}
						else {
							CloseFreehand();
							}
						break;

					case 8:	/* ok requesters */
						if (Items[43].Flags&CHECKED)
							thisstate->es_flags|=ES_OKAY_REQ;
						else
							thisstate->es_flags&=(~ES_OKAY_REQ);
						break;

					case 9:	/* units samples */
						if (Items[44].Flags&CHECKED)
							thisstate->es_flags&=(~ES_SMPTE_UNITS);
						else
							thisstate->es_flags|=ES_SMPTE_UNITS;
						update_status();
						break;
		
					case 10: /* show regions */
						if (region_win==NULL)
							OpenRegionWin();
						else
							CloseRegionWin();
						break;

					case 11: /* grid */
						thisstate->grid=subnum;
						for (i=0; i<EI_MAX; i++)
							if (EI[i].flags&EI_IN_USE)
								BuildEIRP(&EI[i]);
						DrawAllRP();
						break;

					case 12:	/* fast graph */
						if (Items[47].Flags&CHECKED)
							thisstate->es_flags|=ES_FAST_GRAPH;
						else
							thisstate->es_flags&=(~ES_FAST_GRAPH);
						DrawEditGraphBuffers();
						break;

					}
				}
			code=ItemAddress(edit_win->MenuStrip,code)->NextSelect;
			}
		break;		/** End MenuPick **/

#if 0
	case RAWKEY:
	   if (code==64) {      /** space bar cancels **/
   	   }
	   break;
#endif
		}
	}		/** END of IDCMP event loop **/
}

/************************************************************/

void HandleMouseDrag(mousex,mousey)
SHORT mousex,mousey;
{
short festep;

if (edit_flags&EDIT_DRAWING) {  /** Free hand edit mode? **/
	if (mousex > last_mousex) festep=1;else festep=-1;
	if (mousex==last_mousex) festep=0;
	do {
		last_mousex +=festep;
		DoFreehand(last_mousex, mousey);
		} while (mousex!=last_mousex);
	return;
	}

if (dragging) {
/** clip pointer position to inside graph **/
	if (mousex > GRAPH_X+DRAWWIDTH-1)
		mousex=GRAPH_X+DRAWWIDTH-1;
	if (mousex < GRAPH_X)
		mousex=GRAPH_X;
	set_new_em(mousex);
	draw_marked(edit_win);
	update_status();
	}
}

/************************************************************/

void ProcessEdit(flags,func,parm2,okstr)
long flags;
int (*func)();
long parm2;
char *okstr;
{
short i;
int error;
long len;

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE)
		if (CheckLock(&EI[i]))
			if (PeRangeCheck(&EI[i],flags))
				if (PeNonDestEditOkay(flags))
					if (PeOkReq(flags,okstr)) {
						len=EI[i].sfp->tag.length;
						if (flags&PE_PARM2)
							error=(*func)(&EI[i],parm2);
						else
							error=(*func)(&EI[i]);
						show_error(error);

						if (len!=EI[i].sfp->tag.length)
							if (IsRegionSet(EI[i].sfp))
								telluser("Edit Warning:","This edit may have altered a region",0);

						BroadcastEventParms(se_info_sample_edited,EI[i].samp_name,0,0,0,0,0);
						if (error!=NO_ERROR)
							break;
					}

}

/************************************************************/

int IsRegionSet(sfp)
struct SampleFilePointer *sfp;
{
int i;

if (sfp->ff->flags&FF_REGIONS) {
	for (i=0; i < SFT_NUM_REGIONS; i++)
		if (sfp->tag.dregions[i].region_name[0])
			return(TRUE);
		}
return(FALSE);
}

/************************************************************/

PeRangeCheck(ei,flags)
struct EditInstance *ei;
long flags;
{
if (flags&PE_NO_RANGE_CHECK)
	return(TRUE);
else
	return(rangeset(ei));
}

/************************************************************/

PeOkReq(flags,okstr)
long flags;
char *okstr;
{
if (okstr==NULL)
	return(TRUE);

if (thisstate->es_flags&ES_OKAY_REQ) {
	if (PE_NONDEST&flags)
		return(askuser("OK to do non-destructive edit:",okstr,0));
	else
		return(askuser("OK to do DESTRUCTIVE edit:",okstr,0));
	}
else {
	return(TRUE);
	}
}

/************************************************************/

PeNonDestEditOkay(flags)
long flags;
{
char buf[80];

if (!(flags&PE_NONDEST))
	return(TRUE);

if (!(EI[longest_ei].sfp->ff->flags&FF_DCLIPS)) {
	sprintf(buf,"File format '%s' does not support non destructive edits.",EI[longest_ei].sfp->ff->name);
	telluser("Edit:",buf,0);
	return(FALSE);
	}
return(TRUE);
}

/************************************************************/

void close_samp(ei)
struct EditInstance *ei;
{
WriteGraphBuffer(ei);
FreeGraphBuffer(ei);
FreeRenderBuffer(ei);
if (ei->sfp) {
	SampleClose(ei->sfp);
	}
setmem(ei, sizeof(struct EditInstance), 0);
RecalcEILongest();
if (CountEI()==0) {
	NewGhost();
	}
else {
	RecalcGraphHeight();
	RethinkZoom();
	}
}

/************************************************************/

void close_edit()
{
if (region_win)
	CloseRegionWin();

if (edit_win) {
	IgnoreEvent(se_info_sample_deleted, thismod);
	IgnoreEvent(se_cmd_trigger_playback, thismod);
	IgnoreEvent(se_cmd_trigger_timecode, thismod);
	IgnoreEvent(se_info_channel_done, thismod);

	delete_gadget_chain(edit_win);
	stop.gad_ptr=NULL;	/** for update_status() **/
	CloseWind(edit_win);
	edit_win=NULL;
	edit_sigbit=0;
	thisstate->es_flags &= (~ES_IN_USE);
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
	}
}

/************************************************************/

void calc_edit_sigbit()
{
edit_sigbit=0;
edit_sigbit |= (1<<edit_win->UserPort->mp_SigBit);
}

/************************************************************/

void OpenFreehand()
{
new_pointer(PTR_BUSY);
edit_flags |= EDIT_FREEHAND;
/*SampleSeek(EI[longest_ei].sfp, freehand_gs, SS_OFFSET_BEGINNING);*/
freehand_gs=0;
freehand_ei=0;
new_pointer(PTR_NORMAL);
}

/************************************************************/

void StartFreehand(mousex, mousey)
{
struct EditInstance *newei;
short i;

/** Determine which ei was clicked **/
newei=NULL;
for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		if (mousey >= EI[i].graph_y && mousey < (EI[i].graph_y+EI[i].graph_height)) {
			newei=&EI[i];
			break;
			}
		}

if (newei==NULL) {
	telluser("EDIT:","Internal Error Alpha.  Contact SunRize.",0);
	return;
	}

/** update freehand buffer id new ei, or scrolled display **/
if ((freehand_ei ? (freehand_gs!=freehand_ei->graph_start) : TRUE) || newei!=freehand_ei) {
	new_pointer(PTR_BUSY);
	if (freehand_ei) {
		SampleSeek(freehand_ei->sfp, freehand_gs, SS_OFFSET_BEGINNING);
		SampleWrite(freehand_ei->sfp, freehand_buf, freehand_size);
		}
	freehand_ei=newei;
	freehand_gs=freehand_ei->graph_start;
	SampleSeek(freehand_ei->sfp, freehand_gs, SS_OFFSET_BEGINNING);
	freehand_size=SampleRead(freehand_ei->sfp, freehand_buf, sizeof(freehand_buf)/2);
	new_pointer(PTR_NORMAL);
	}

last_mousex=mousex;
edit_flags |= EDIT_DRAWING;
DoFreehand(mousex, mousey);
/** check case where sample doesn't fill entire window **/
if (mousex >= (GRAPH_X+freehand_ei->graph_width-1))
	telluser("Edit:","Can't freehand here, sample too short",0);
}

/************************************************************/
/** THIS ROUTINE HAS A BUG IN THAT IT DOESN'T UPDATE EI[0].org_graph **/

void CloseFreehand()
{
new_pointer(PTR_BUSY);
edit_flags &= ~EDIT_DRAWING;
edit_flags &= ~EDIT_FREEHAND;
if (freehand_ei) {
	SampleSeek(freehand_ei->sfp, freehand_gs, SS_OFFSET_BEGINNING);
	SampleWrite(freehand_ei->sfp, freehand_buf, freehand_size);
	BuildEIRP(freehand_ei); 
	}
DrawAllRP();
new_pointer(PTR_NORMAL);
}

/************************************************************/

void EndFreehand()
{
edit_flags &= ~EDIT_DRAWING;
}

/************************************************************/

void DoFreehand(mousex, mousey)
{
int y;
short val,i;

if (mousex <=GRAPH_X)
	return;
if (mousex >= (GRAPH_X+freehand_ei->graph_width-1))
	return;
if (mousey < freehand_ei->graph_y)
	mousey=freehand_ei->graph_y;
if (mousey >= (freehand_ei->graph_y+freehand_ei->graph_height))
	mousey=freehand_ei->graph_y+freehand_ei->graph_height-1;

/** erase old points **/
SetDrMd(edit_win->RPort, JAM1);
SetAPen(edit_win->RPort, StudioBase->defaults.colors.list.hitbox_unselected);

for (i=-1; i <=1; i++) {
	Move (edit_win->RPort, mousex+i, freehand_ei->graph_y);
	Draw (edit_win->RPort, mousex+i, freehand_ei->graph_y+freehand_ei->graph_height-1);
	}

/** Draw new point **/

edit_win->RPort->Mask = 0x04;
SetAPen(edit_win->RPort, 4);

/** redraw center line ***/

WritePixel(edit_win->RPort, mousex+1,freehand_ei->graph_y+freehand_ei->graph_height/2);
WritePixel(edit_win->RPort, mousex,freehand_ei->graph_y+freehand_ei->graph_height/2);
WritePixel(edit_win->RPort, mousex-1,freehand_ei->graph_y+freehand_ei->graph_height/2);

/** Draw new sample & connect dots with neighbors **/

y = freehand_ei->graph_y+freehand_ei->graph_height/2+
	 GMAP(freehand_ei,freehand_buf[mousex - GRAPH_X - 1]);
val = (32767L*(int)(mousey-freehand_ei->graph_y-freehand_ei->graph_height/2))/(freehand_ei->graph_height/2);
freehand_buf[mousex - GRAPH_X]=val;  /**!!!! WHY IS THIS +1 NEEDED!!!!!!!!!**/
Move (edit_win->RPort, mousex-1, y);
Draw (edit_win->RPort, mousex, mousey);
y = freehand_ei->graph_y+freehand_ei->graph_height/2+
	 GMAP(freehand_ei,freehand_buf[mousex - GRAPH_X + 1]);
MyDraw(edit_win->RPort, mousex+1, y);
edit_win->RPort->Mask = 0xFF;
}

/************************************************************/

void start_dragging(mousex)

int mousex;
{
short i;

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		EI[i].marked_samp = TRUE;
		EI[i].start_mark=mousetosample(&EI[i],mousex);
		}
set_new_em(mousex);
DrawAllRP();
}

/*****************************************************/

void draw_marked(Window)
struct Window *Window;
{
DrawAllRP();
return;
}

/*****************************************************/

int in_graph(x, y)

int x, y;

{
return (x>=GRAPH_X && x<(GRAPH_X+EI[longest_ei].graph_width) && y>GRAPH_Y1 && y<(edit_win->Height - GRAPH_Y2));
}

/*****************************************************/

void set_new_em(mousex)
int mousex;
{
short i;

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		EI[i].end_mark = mousetosample(&EI[i],mousex);

		if (EI[i].end_mark >= EI[i].start_mark)	/** round last pixel up **/
			EI[i].end_mark=fullmousetosample(&EI[i],mousex);
		else
			EI[i].start_mark=fullmousetosample(&EI[i],sampletomousenoclip(&EI[i],EI[i].start_mark));

		if (EI[i].end_mark >= EI[i].sfp->tag.length)
		   EI[i].end_mark = EI[i].sfp->tag.length-1;
		if (EI[i].end_mark < 0)
		   EI[i].end_mark = 0;
		}
}

/*****************************************************/

buffertosample(ei,mousex)
struct EditInstance *ei;
int mousex;
{
return(ei->graph_start+mousex*zoom_scale);
}

/***************************************************************/
/** this one returns a value that starts at the beginning of  **/
/** the pixel.  Samples represented by the pixel arn't counted */
/***************************************************************/

mousetosample(ei,mousex)
struct EditInstance *ei;
int mousex;
{
int retval;

retval=ei->graph_start+(mousex - GRAPH_X)*zoom_scale;
if (retval >= ei->sfp->tag.length)
	retval=ei->sfp->tag.length-1; /** end of sample probably ends **/
return(retval);		/** in the middle of the last pixel **/
}

/***************************************************************/
/** this one returns a value that includes the samples inside **/
/** the pixel 															  	  **/
/***************************************************************/

fullmousetosample(ei,mousex)
struct EditInstance *ei;
int mousex;
{
int retval;

retval=ei->graph_start+(mousex - GRAPH_X+1)*zoom_scale-1;
if (retval >= ei->sfp->tag.length)
	retval=ei->sfp->tag.length-1; /** end of sample probably ends **/
return(retval);		/** in the middle of the last pixel **/
}

/*****************************************************/

sampletomouse(ei,index)
struct EditInstance *ei;
int index;
{
if (index < ei->graph_start)
	return(GRAPH_X);
if (index >= (ei->graph_start+ei->graph_width*zoom_scale))
	return(GRAPH_X+ei->graph_width-1);
return(sampletomousenoclip(ei,index));
}

/*****************************************************/

sampletomousenoclip(ei,index)
struct EditInstance *ei;
int index;
{
return(GRAPH_X+(index - ei->graph_start)/zoom_scale);
}

/*****************************************************/

sampletobuffer(ei,index)
struct EditInstance *ei;
int index;
{
if (index < ei->graph_start)
	return(0);
if (index > (ei->graph_start+EI[longest_ei].graph_width*zoom_scale))
	return(EI[longest_ei].graph_width-1);
return(sampletobuffernoclip(ei,index));
}

/*****************************************************/

sampletobuffernoclip(ei,index)
struct EditInstance *ei;
int index;
{
return((index - ei->graph_start)/zoom_scale);
}

/*****************************************************/

/* The following makes index always fall on a zoom scale
 * boundry.  When sampling the graph_buffer, we get
 * aliasing.  And the pattern displayed depends upon the
 * the starting point (index).  If I don't always start
 * on a multiple of the "sampling_rate" (zoom_scale), I'll
 * get different patterns for different index's.  This looks very
 * bad when scrolling a repative wave (like a sine wave)
 */

matchscale(index)
{
if (zoom_scale)
	return(((index+zoom_scale-1)/zoom_scale)*zoom_scale);
else
	return(index);
}

/*****************************************************/

closeto(ei,mousex, mark)
struct EditInstance *ei;
int mousex, mark;
{
int i;

i = mousex - sampletomouse(ei,mark);

if (i < 0)
   i = -i;
if (i < 10)
   return (TRUE);
else
   return(FALSE);
}

/************************************************************/

void handle_graph_events()
{
int snatch_newgs;
short i;

if (edit_flags&EDIT_REFRESH_HSCROLL) {
	edit_flags &= (~EDIT_REFRESH_HSCROLL);
	new_pointer(PTR_BUSY);
	DrawEditGraphBuffers();
	new_pointer(PTR_NORMAL);
	}
else {			/* round new_graph_start to a multiple of zoom_scale **/
	snatch_newgs=new_graph_start;	/** can change while scrolling **/
	for (i=0; i < EI_MAX; i++)
		if (EI[i].flags&EI_IN_USE) {
			ScrollEIRP(&EI[i],snatch_newgs);
			}
	DrawAllRP();
	}
update_status();
}
		
/************************************************************/

void handle_timer_events()
{
short i;

if (GetMsg(timerport)) {
	WaitingForTimer=FALSE;
	if (edit_win) {
		for (i=0; i < EI_MAX; i++) {
			if (EI[i].flags&EI_IN_USE) {
				if (EI[i].as) {
					CompLine(&EI[i], EI[i].line_pos);
	/*				EI[i].line_pos = max(0,ASPos(EI[i].as)-5120);	*//** Try & adjust of ad1012's internal buffers **/
					EI[i].line_pos = ASPos(EI[i].as);
					CompLine(&EI[i], EI[i].line_pos);
					}
				}
			}
		start_timer();
		}	
	}
}

/************************************************************/

void start_timer()
{
if (timerport) {
	timermsg->tr_node.io_Command = TR_ADDREQUEST;
	timermsg->tr_time.tv_secs=0;
	timermsg->tr_time.tv_micro=25000;
	SendIO((struct IORequest *) timermsg );
	WaitingForTimer=TRUE;
	}
}

/************************************************************/

void stop_timer()
{
if (WaitingForTimer) {
	AbortIO(timermsg);
	WaitIO(timermsg);
	WaitingForTimer=FALSE;
	}
}

/************************************************************/

void zoom_out_func(b)
struct button *b;
{
if (!disable_gadgets) {
	new_zoom = new_zoom * 2;
	do_zoom();
	}
}

/************************************************************/

void zoom_in_func(b)
struct button *b;
{
if (!disable_gadgets) {
	new_zoom = new_zoom/2;
	do_zoom();
	}
}

/************************************************************/

void do_zoom()
{
if (new_zoom < 1)
	new_zoom=1;
if (new_zoom > max_zoom)
	new_zoom=max_zoom;
if (new_zoom!=zoom_scale) {
	edit_flags |= EDIT_REFRESH_HSCROLL;
	Signal(tc_edit, update_graph_sigbit);	
	}

if (IsFreehandMenuOn) {
	Signal(tc_edit, close_freehand_sigbit);	
	}
}

/************************************************************/

void do_zoom_or_scroll()
{
if (new_zoom < 1)
	new_zoom=1;
if (new_zoom > max_zoom)
	new_zoom=max_zoom;

edit_flags |= EDIT_REFRESH_HSCROLL;
Signal(tc_edit, update_graph_sigbit);	

if (IsFreehandMenuOn) {
	Signal(tc_edit, close_freehand_sigbit);	
	}
}

/************************************************************/

IsFreehandMenuOn()
{
return(Items[42].Flags&CHECKED);
}


/************************************************************/

void handle_freehand_events()
{
if (IsFreehandMenuOn()) {
	Items[42].Flags&=(~CHECKED);
	CloseFreehand(); 
	}	
}

/************************************************************/

void hscroll_func(s)
struct slider *s;
{
if (!disable_gadgets) {
	new_graph_start = matchscale(s->top_element);
	if (new_graph_start!=EI[longest_ei].graph_start) {
		Signal(tc_edit, update_graph_sigbit);
		}
	}
}

/************************************************************/

void update_status()
{
struct EditInstance *ei;
char str_sm[16];
char str_em[16];
char str_len[18];
char str_gs[16];
char str_ge[16];
char str_big[64];
int y;
long width;
unsigned long sm, em;
struct TimeCode tc1,tc2,tc3;
struct TimeCode d1,d2,d3;

if (edit_win->Width < 550)			/** too small to draw status? **/
	return;

ei=&EI[longest_ei];

if (!(ei->flags&EI_IN_USE)) {	/** no samples loaded? **/
	LockGadLibRender();		/** clear status line **/
	SetDrMd(edit_win->RPort,JAM1);
	SetAPen(edit_win->RPort,StudioBase->defaults.colors.gadget_window.background);	
	RectFill(edit_win->RPort, GRAPH_X, 12, GRAPH_X+DRAWWIDTH-1,GRAPH_Y1-2);
	UnLockGadLibRender();
	return;
	}

/** start & end marks are sometimes swapped for rendering **/
/** convience.  So I'm making sure that the smallest is always **/
/** the start mark, end the largest is always the end mark **/

if (ei->start_mark <= ei->end_mark) {
	sm=ei->start_mark;
	em=ei->end_mark;
	}
else {
	em=ei->start_mark;
	sm=ei->end_mark;
	}

width=ei->graph_width*zoom_scale;
if (ei->graph_start+width > ei->sfp->tag.length)
	width=ei->sfp->tag.length-ei->graph_start;

if (thisstate->es_flags&ES_SMPTE_UNITS) {
	SampToSmpte(&tc1, sm, TRUE);
	SampToSmpte(&tc2, em, TRUE);
	SampToSmpte(&tc3, em-sm+1, FALSE);

	SampToSmpte(&d1, ei->graph_start,TRUE);
	SampToSmpte(&d2, ei->graph_start+width-1, TRUE);
	SampToSmpte(&d3, width,FALSE);
	}

if (!EI[longest_ei].marked_samp) {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str_sm,"--:--:--:--");
		sprintf(str_gs,"%02d:%02d:%02d:%02d",d1.hours,d1.minutes,d1.seconds,d1.frames);
		sprintf(str_em,"--:--:--:--");
		sprintf(str_ge,"%02d:%02d:%02d:%02d",d2.hours,d2.minutes,d2.seconds,d2.frames);
		sprintf(str_len,"(--:--:--:--)");	/** range len **/
		}
	else {
		sprintf(str_sm,"     --     ");
		sprintf(str_gs,"%-12d",ei->graph_start);
		sprintf(str_em,"    ---    ");
		sprintf(str_ge,"%12d",ei->graph_start+width-1);
		sprintf(str_len,"(    --    )");
		}
	}
else {	/*** Range Set **/
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str_sm,"%02d:%02d:%02d:%02d",tc1.hours,tc1.minutes,tc1.seconds,tc1.frames);
		sprintf(str_gs,"%02d:%02d:%02d:%02d",d1.hours,d1.minutes,d1.seconds,d1.frames);
		sprintf(str_em,"%02d:%02d:%02d:%02d",tc2.hours,tc2.minutes,tc2.seconds,tc2.frames);
		sprintf(str_ge,"%02d:%02d:%02d:%02d",d2.hours,d2.minutes,d2.seconds,d2.frames);
		sprintf(str_len,"(%02d:%02d:%02d:%02d)",tc3.hours,tc3.minutes,tc3.seconds,tc3.frames);
		}
	else {
		sprintf(str_sm,"%12d",sm);
		sprintf(str_gs,"%-12d",ei->graph_start);
		sprintf(str_em,"%-12d",em);
		sprintf(str_ge,"%12d",ei->graph_start+width-1);
		sprintf(str_len,"(%12d)",em-sm+1);
		}
	}

y=GRAPH_Y1-4;

LockGadLibRender();
SetDrMd(edit_win->RPort,JAM2);
SetAPen(edit_win->RPort,StudioBase->defaults.colors.button.draw_unselected);
SetBPen(edit_win->RPort,StudioBase->defaults.colors.gadget_window.background);
PutUpdateText(GRAPH_X,y,str_gs);
PutUpdateText(GRAPH_X+DRAWWIDTH-strlen(str_ge)*8,y,str_ge);
sprintf(str_big,"%s  %s  %s",str_sm,str_len,str_em);
PutUpdateText(GRAPH_X+DRAWWIDTH/2-strlen(str_big)*4,y,str_big);
UnLockGadLibRender();
}

/************************************************************/

void PutUpdateText(x,y,str)
int x,y;
char *str;
{
short len;

len = strlen(str);
Move(edit_win->RPort, x, y);
Text(edit_win->RPort,str,len);
}

/***********************************************************/
/***********************************************************/

void SampToSmpte(tc, numsamps, useoffset)
struct TimeCode *tc;
int numsamps;
int useoffset;
{
float rate,smpte_mode;
int frames;
int result;
int p1,*p2;
int zero=0;

if (EI[longest_ei].sfp->tag.parms.smpte_sampling_rate==0)
	rate=EI[longest_ei].sfp->tag.parms.rate;
else
	rate=EI[longest_ei].sfp->tag.parms.smpte_sampling_rate;

if (rate==0)
	rate=44100.0;		/** should never occur, but just in case avoid /0 **/

if (StudioBase->defaults.smpte_mode) smpte_mode=(float)StudioBase->defaults.smpte_mode;
else smpte_mode=29.97;

frames=(int)((((float)numsamps)*smpte_mode)/rate);
p1=Frm2TC(frames,StudioBase->defaults.smpte_mode);
/****
sprintf(bug,"frames %d framesXrate%f rate %f samps %d\n",frames,frames*rate,rate,numsamps);
kprintf("EDIT %s",bug);
****/

if (useoffset)
	p2=(int *)(&EI[longest_ei].sfp->tag.parms.starttimecode);
else
	p2=(int *)(&zero);

result=AddTimeCode(p1,*p2,StudioBase->defaults.smpte_mode);
movmem(&result,tc,sizeof(result));
}

/***********************************************************/

CheckLock(ei)
{
if (DoCheckLock(ei)) {
	return(TRUE);
	}
else {
	telluser("EDIT:  Can't do edit because","sample is in use by another module or is playing.",0);
	return(FALSE);
	}
}

/***********************************************************/

/** Check that the current sample has no read or write locks **/
/** on it.  This is not a good method, and should be replaced **/
/** by a SampleLock() call! **/
/** returns: TRUE if there are no read/write locks on EI[0].samp_name **/


DoCheckLock(ei)
struct EditInstance *ei;
{
int retval;
long adoslock;

SampleClose(ei->sfp);
adoslock=Lock(ei->samp_name,ACCESS_WRITE);
if (adoslock) {
	retval=TRUE;
	UnLock(adoslock);
	}
else {
	retval=FALSE;
	}
ei->sfp=SampleOpen(ei->samp_name,SP_MODE_READWRITE,0);
if (ei->sfp==0) {
	telluser("Edit:","Fatal error trying to reopen sample!",0);
	retval=FALSE;
	}
return(retval);
}

/*********************************************************/
/** Allocates requested memory with 8 bytes in front of **/
/** and 8 bytes after the allocation request.  This is  **/
/** cheap insurance agains a MungWall hit I got once and**/
/** can't reproduce **/
/*********************************************************/

unsigned long PadAllocMem(size,type)
unsigned long size,type;
{
unsigned long ptr;

ptr=AllocMem(size+16, type);
if (ptr)
	return(ptr+8);
else
	return(NULL);
}

void PadFreeMem(ptr, size)
unsigned long ptr;
unsigned long size;
{
FreeMem(ptr-8, size+16);
}

/********************************************************/
/** PUT THIS IN STUDIO.LIBRARY!!!!!		***/

struct SampleFilePointer *SampleOpenInDefPath(samp_name,mode,ff)
char *samp_name;
long mode;
APTR ff;
{
char *str;
struct SampleFilePointer *sfp;

str=(char *)AllocCompletePath(StudioBase->defaults.samp_paths, samp_name);
sfp=SampleOpen(str,mode,ff);
FreeMem(str,strlen(str)+1);
return(sfp);
}

/********************************************************/

struct SampleFilePointer *SampleCreateRevision(samp_name,ff)
char *samp_name;
struct FileFilter *ff;
{
char buf[STUDIO16_MAX_PATH_SIZE];

strcpy(buf,samp_name);
strcat(buf,".edit");
return(SampleOpen(buf,SP_MODE_NEWFILE,ff));
}

/********************************************************/

UseRevisedSample(ei)
struct EditInstance *ei;
{
char tmpname[STUDIO16_MAX_PATH_SIZE];
int error;

DeleteSample(ei->samp_name);
strcpy(tmpname,ei->samp_name);
strcat(tmpname,".edit");
error=RenameSample(tmpname,0,ei->samp_name,0);
ei->sfp=SampleOpen(ei->samp_name,SP_MODE_READWRITE,0);
if (ei->sfp==0) {
	telluser("Edit:","Fatal error trying to reopen sample in rename!",0);
	error=STDERR_CANT_GET_READ_LOCK;
	}
return(error);
}

/********************************************************/

CountEI()
{
short i,k;

k=0;
for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE)
		k++;
return((int)k);
}

/********************************************************/

void RecalcEILongest()
{
short i;
int longest;

longest=0;
longest_ei=0;

for (i=0; i<EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		if (EI[i].sfp->tag.length > longest) {
			longest=EI[i].sfp->tag.length;
			longest_ei=i;
			}
		}
}

/********************************************************/

int RecalcGraphHeight()
{
short i;
int hsum;
int error;

hsum=0;

for (i=0; i<EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
   	EI[i].graph_height = (edit_win->Height - GRAPH_Y1 - GRAPH_Y2)/CountEI();
   	EI[i].graph_y=hsum+GRAPH_Y1;
		hsum=hsum+EI[i].graph_height;
		BuildDivTab(&EI[i]);

		/** Must Reallocate new double buffers of correct height **/

		if ((error=InitRenderBuffer(&EI[i]))!=NO_ERROR) {
			telluser("Can't alloocate display buffers!",0,0);
			return(error);
			}
		}
return(NO_ERROR);
}

/********************************************************/

void DoNew()
{
short i;

for (i=0; i<EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE)
		close_samp(&EI[i]);
}

/********************************************************/

void DoOpen()
{
int temp;
char name[40];
char path[STUDIO16_MAX_PATH_SIZE];

name[0]=0;
strcpy(path,StudioBase->defaults.samp_paths);
temp=FileRequest("Select File to Edit:",path,name,FR_SINGLE_SELECT);
if (temp) {
	CloseRegionWin();
	StrPathFileCat(path,name);
	EditNewSamp(path);
	if (Items[45].Flags&CHECKED && region_win==NULL)
		OpenRegionWin();
	}
}

/********************************************************/

void PlayAllSamps(start,len)
long start;
long len;
{
struct Region region;
struct ChanKey *key;
struct ActiveSample *as[16];
struct ActiveSample *ASOpen();
short count;
short i;


count=0;
for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) 
		if (EI[i].sfp->tag.length-5 >= start) {
			len=min(len,EI[i].sfp->tag.length-start);
			as[count]=NULL;
			key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
			if (key==0) {
				while (count--)
					ASClose(as[count]);	
				telluser("Edit:","Can't Allocate a play channel!",0);
				return;
				}
			setmem(&region,sizeof(struct Region),0);
			strcpy(region.samp_name, EI[i].samp_name);
/*			region.effect=callback;*/
			region.loop=FindLoopCt();
			if (len==0) {
				region.start=0;	/** 0,0 tells ASOpen() to use defaults **/
				region.end=0;
				}
			else {
				region.start=start;
				region.end=start+len-1;
				}

			/** Since region.tag is all NULL (we never initilized it) **/
			/** ASOpen() will pull the default parameters from the sample **/

			if ((as[count]=ASOpen(&region, AS_PLAY|AS_AUTO_DATA|AS_AUTO_CLOSE|AS_AUTO_FREECHAN))==0) {
				FreeChan(key);
				while (count--)
					ASClose(as[count]);	
				telluser("Edit:","Can't Play Sample. Sample may be in use, or Amiga low on RAM.",NULL);
				return;
				}

			as[count]->key=key;
			count++;
			}
if (count) {
	as[count]=0;	/** indicate last entry **/
	ASTriggerList(as,0);
	}
}

/********************************************************/

FindLoopCt()
{
if (Items[41].Flags&CHECKED)
	return(-1);
else
	return(0);
}

/********************************************************/

void NewGhost()
{
int i;

for (i=0; i < 48; i++)
	Items[i].Flags&=~ITEMENABLED;

Items[1].Flags|=ITEMENABLED;		/* open */
Items[8].Flags|=ITEMENABLED;		/* Non Destructive */
Items[17].Flags|=ITEMENABLED;		/* Destructive */
Items[31].Flags|=ITEMENABLED;		/* ····· */
Items[33].Flags|=ITEMENABLED;		/* gen silence */
Items[34].Flags|=ITEMENABLED;		/* gen sinewave */
Items[40].Flags|=ITEMENABLED;		/* ····· */
Items[43].Flags|=ITEMENABLED;		/* ok reqesters */
Items[44].Flags|=ITEMENABLED;		/* units in samples */
Items[46].Flags|=ITEMENABLED;		/* grid */
disable_gadgets=TRUE;
}

/********************************************************/

void UnNewGhost()
{
int i;

for (i=0; i < 48; i++)
	Items[i].Flags|=ITEMENABLED;
disable_gadgets=FALSE;
}

/********************************************************/

void UpdateLoopingRange()
{
if (edit_flags&EDIT_RANGE_LOOPING) {
	if (EI[longest_ei].as==NULL) {		/** Not playing ? **/
		edit_flags&=(~EDIT_RANGE_LOOPING);
		}
	else {
		BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
		PlayAllSamps(EI[longest_ei].start_mark,EI[longest_ei].end_mark-EI[longest_ei].start_mark+1);
		}
	}
}

/********************************************************/
