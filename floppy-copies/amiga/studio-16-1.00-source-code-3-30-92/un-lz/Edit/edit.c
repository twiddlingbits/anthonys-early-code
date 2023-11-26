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

long _stack = 10000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_EDIT";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

/** My default edit NewWindow struct **/

static struct NewWindow NewWindowStructure1 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   BLACK,WHITE,   /* detail and block pens */
   MOUSEMOVE|MENUPICK|MOUSEBUTTONS|RAWKEY|CLOSEWINDOW|NEWSIZE,   /* IDCMP flags */
   WINDOWSIZING|WINDOWDRAG|WINDOWDEPTH|REPORTMOUSE|ACTIVATE|WINDOWCLOSE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Edit",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   200,100,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct SampleFilePointer *SampleOpen();
struct SampleFilePointer *SampleCreateRevision();
short SampleGetOne();

void handle_freehand_events();
void zoom_func();
void hscroll_func();
void nondest_func();
void update_status();
void CompLine();
void AllocGraphBuffer();
void AllocOrgGraphBuffer();
void FreeOrgGraphBuffer();
void hide_nondest();
void show_dest();
void refresh_dest();
void hide_dest();
void show_nondest();
void refresh_nondest();
void DoFreehand();
void OpenFreehand();
void StartFreehand();
void EndFreehand();
void CloseFreehand();
void SampToSmpte();

/***** ROW 4B ******/


struct NewButton dadd = {
   NULL,GRAPH_X+318,0,GRAPH_X+368,0,BUTN_COOL|MANUAL_XOR|HIDE_GADGET,0,0,0,0,0,0,"Add"};

struct NewButton dpaste = {
   &dadd,GRAPH_X+264,0,GRAPH_X+313,0,BUTN_COOL|MANUAL_XOR|HIDE_GADGET,0,0,0,0,0,0,"Paste"};

struct NewButton dcopy = {
   &dpaste,GRAPH_X+211,0,GRAPH_X+259,0,BUTN_COOL|MANUAL_XOR|HIDE_GADGET,0,0,0,0,0,0,"Copy"};

struct NewButton compress = {
   &dcopy,GRAPH_X+104,0,GRAPH_X+206,0,BUTN_COOL|MANUAL_XOR|HIDE_GADGET,0,0,0,0,0,0,"Compress"};

/***** ROW 4 ******/

struct NewButton keep = {
   &compress,GRAPH_X+318,0,GRAPH_X+368,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Keep"};

struct NewButton delete = {
   &keep,GRAPH_X+264,0,GRAPH_X+313,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Erase"};

struct NewButton paste = {
   &delete,GRAPH_X+211,0,GRAPH_X+259,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Paste"};

struct NewButton copy = {
   &paste,GRAPH_X+157,0,GRAPH_X+206,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Copy"};

struct NewButton cut = {
   &copy,GRAPH_X+104,0,GRAPH_X+152,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Cut"};

struct NewButton nondest = {
   &cut,GRAPH_X-2,0,GRAPH_X+99,0,BUTN_COOL|MANUAL_XOR|BUTN_LOOP,nondest_func,0,0,0,0,0,"Non Dest"};


/***** ROW 3 ******/


struct NewButton begin = {
   &nondest,GRAPH_X+318,0,GRAPH_X+368,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Begin"};

struct NewButton flip = {
   &begin,GRAPH_X+264,0,GRAPH_X+313,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Flip"};

struct NewButton scale = {
   &flip,GRAPH_X+211,0,GRAPH_X+259,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Scale"};

struct NewButton freehand = {
   &scale,GRAPH_X+104,0,GRAPH_X+206,0,BUTN_COOL|MANUAL_XOR|BUTN_TOGGLE,0,0,0,0,0,0,"Freehand"};

struct NewButton silence = {
   &freehand,GRAPH_X-2,0,GRAPH_X+99,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Add Silence"};

/***** ROW 2 ******/


struct NewButton undo = {
   &silence,GRAPH_X+318,0,GRAPH_X+368,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Undo"};


struct NewButton rate = {
   &undo,GRAPH_X+211,0,GRAPH_X+313,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Set Rate"};

struct NewButton range = {
   &rate,GRAPH_X+104,0,GRAPH_X+206,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Set Range"};


struct NewButton showrange = {
   &range,GRAPH_X-2,0,GRAPH_X+99,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Show Range"};


/***** ROW 1 ******/

struct NewButton stop = {
   &showrange,GRAPH_X+318,0,GRAPH_X+368,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Stop"};

struct NewButton playwave = {
   &stop,GRAPH_X+211,0,GRAPH_X+313,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Play Wave"};

struct NewButton playrange = {
   &playwave,GRAPH_X+104,0,GRAPH_X+206,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Play Range"};

struct NewButton zoombutn = {
   &playrange,GRAPH_X-2,0,GRAPH_X+99,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,0};

struct NewSlider zoom = {
    &zoombutn,GRAPH_X-2,0,GRAPH_X+99,0,HIDE_GADGET|SLIDER_COOL|FREE_HORIZ|MANUAL_XOR|KNOB_TODDISH,
    zoom_func,0,0,0,0,0,0,0,0,12,-1};

struct NewSlider hscroll= {
    &zoom,GRAPH_X-2,0,0,0,AUTO_KNOB|SLIDER_COOL|FREE_HORIZ|MANUAL_XOR|KNOB_TODDISH|SLIDER_ARROWS,
    hscroll_func,0,0,0,0,0,0,0,0,-1,-1};


/** variables global to this module **/

static int freehand_size;
static int freehand_gs;
static short freehand_buf[1024];
static last_mousex;
static short dragging;
int new_zoom;
int new_graph_start;
unsigned long update_graph_sigbit;
unsigned long update_graph_allocbit;
unsigned long close_freehand_sigbit;
unsigned long close_freehand_allocbit;
struct EditInstance editparms;
struct EditInstance *ei;
unsigned long edit_sigbit=0;
struct Module *thismod;
struct EditState *thisstate;
struct Task *tc_edit;
struct MsgPort *gadlib_port;
char zoombutn_text[20];
short se_cmd_kill_module;
short se_cmd_trigger_playback;
short se_info_channel_done;
short se_cmd_edit_sample;
short se_cmd_play_sample;
short se_cmd_stop_all_playback;
short se_info_sample_edited;
short se_info_sample_deleted;
struct TimeCode tc_offset;
struct MsgPort *timerport;
struct timerequest *timermsg;
short WaitingForTimer=FALSE;

int cur;		/*** DELETE ME!!!!! **/

/** This modules function delclerations **/

int start_dragging();
void set_new_em();
int in_graph();
void mix_samples();
void draw_marked();
void delete_sample();
struct Disk_Samp *get_sample_ptr();
struct Module *find_module();
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
void InitGraphBuffer();
struct EditInstance *InitEditInstance();
void FreeGraphBuffer();
long CopyGraphData();
void start_timer();
void stop_timer();
void handle_timer_events();

/** function's used in this module but defined externally **/


/** variables used in the module but defined externally **/

extern struct StudioBase *StudioBase;

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long studio_sigbit;
unsigned long gadlib_sigbit;
unsigned long timer_sigbit;

openlibraries();
StandardEnviroment();	/** opens studio16 customscreen **/
InitEdit();					/** initalize module **/

studio_sigbit=(1<<thismod->notifyme->mp_SigBit);
gadlib_sigbit=(1<<gadlib_port->mp_SigBit);
timer_sigbit=(1<<timerport->mp_SigBit);

while (TRUE) {
   mask=Wait(edit_sigbit|update_graph_sigbit|studio_sigbit|gadlib_sigbit|timer_sigbit|close_freehand_sigbit);
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
	}
}

/***********************************************************/

void InitEdit()
{
static struct NewModule edit_data ={"Edit",MS_UTILITY_MODULE|MS_KEEP_RESIDENT,sizeof(struct EditState),0};
struct NewGadgetHeader *ng;
struct Window *EditPrefsOpen();
void EditPrefsClose();

if ((thismod=(struct Module *)AddModule(&edit_data))==0) {
	closelibraries();
   exit(10);
   }

thismod->state->flags|=MS_CUSTOM_PREFS;
thismod->state->pref_name="Edit Options";
thismod->state->pref_display_func=EditPrefsOpen;
thismod->state->pref_hide_func=EditPrefsClose;

thisstate=(struct EditState *)thismod->state;

se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_trigger_playback	=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_channel_done		=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_edit_sample		=GetEventID("SE_CMD_EDIT_SAMPLE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_info_sample_edited	=GetEventID("SE_INFO_SAMPLE_EDITED");
se_info_sample_deleted	=GetEventID("SE_INFO_SAMPLE_DELETED");

NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_edit_sample, thismod);

update_graph_allocbit=AllocSignal(-1);
update_graph_sigbit=1<<update_graph_allocbit;
close_freehand_allocbit=AllocSignal(-1);
close_freehand_sigbit=1<<close_freehand_allocbit;
tc_edit = (struct Task *)FindTask(0);

gadlib_port = (struct MsgPort *)CreatePort(0,0);	/**!!! ERROR CHECK? **/ 
for (ng=(struct NewGadgetHeader *)&hscroll; ng; ng=ng->next)
	ng->c = &StudioBase->defaults.colors;

timerport = (struct MsgPort *) CreatePort( 0, 0 );
if (timerport) {
	timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
	OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );
	}

zoom.notifyme=gadlib_port;
zoombutn.notifyme=gadlib_port;
freehand.notifyme=gadlib_port;
dadd.notifyme=gadlib_port;
dpaste.notifyme=gadlib_port;
dcopy.notifyme=gadlib_port;
compress.notifyme=gadlib_port;
keep.notifyme=gadlib_port;
delete.notifyme=gadlib_port;
paste.notifyme=gadlib_port;
copy.notifyme=gadlib_port;
cut.notifyme=gadlib_port;
begin.notifyme=gadlib_port;
flip.notifyme=gadlib_port;
scale.notifyme=gadlib_port;
freehand.notifyme=gadlib_port;
silence.notifyme=gadlib_port;
undo.notifyme=gadlib_port;
rate.notifyme=gadlib_port;
range.notifyme=gadlib_port;
showrange.notifyme=gadlib_port;
stop.notifyme=gadlib_port;
playwave.notifyme=gadlib_port;
playrange.notifyme=gadlib_port;
zoom.notifyme=gadlib_port;
}


/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
		if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
		   FreeSignal(update_graph_allocbit);
			DeleteModule(thismod);
			close_edit();
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

		else if (event->type==se_cmd_edit_sample) {
			open_edit(event->arg1);
			}

		else if (event->type==se_info_sample_deleted) {
			if (editparms.edit_win!=NULL) {	/** make sure ei is valid **/
				if ((struct Disk_Samp *)event->arg1==ei->cur) {
					ei->cur=NULL;
					close_edit();
					}
				}
			}

		else if (event->type==se_cmd_trigger_playback) {
			if (ei) {
				if (ei->edit_win) {
					if (((struct ActiveSample *)event->arg1)->region.samp==ei->cur) {	
						stop_timer();
						ei->as=(struct ActiveSample *)event->arg1;
						start_timer();
						}
					}
				}
			}

		else if (event->type==se_info_channel_done) {
			if (ei) {
				if (ei->edit_win) {
					if (ei->as==(struct ActiveSample *)event->arg1) {
						stop_timer();
						CompLine(ei, ei->line_pos);	/** erase **/
						ei->line_pos=0xFFFFFFFF;	/** No line drawn **/
						ei->as=NULL;
						}
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
	graph_end=ei->graph_start+ei->graph_width*ei->zoom_scale;
	if (pos >= ei->graph_start && pos < graph_end) {
		LockGadLibRender();
		ei->edit_win->RPort->Mask = 1;
		pos = (pos - ei->graph_start)/ei->zoom_scale+GRAPH_X;
		SetDrMd(ei->edit_win->RPort, COMPLEMENT);
		Move (ei->edit_win->RPort, pos, GRAPH_Y1);
		Draw (ei->edit_win->RPort, pos, GRAPH_Y1+ei->graph_height-1);
		ei->edit_win->RPort->Mask = 0xFF;
		UnLockGadLibRender();
		}
	}	
}


/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;
struct button *b;

while ((event=(struct GadMsg *)GetMsg(gadlib_port))!=0) {
	if (event->type==GADMSG_RELEASED && event->g==zoom.gad_ptr) {
		hide_gadget(zoom.gad_ptr);
		show_gadget(zoombutn.gad_ptr);
		refresh_gadget(zoombutn.gad_ptr);
		}

	if (event->type==GADMSG_ACTIVE && event->g==zoombutn.gad_ptr) {
		b=(struct button *)event->g;
		hide_gadget(b);
		show_gadget(zoom.gad_ptr);
		refresh_gadget(zoom.gad_ptr);
		if (freehand.gad_ptr->flags&BUTN_HIT) {
			xor_button(freehand.gad_ptr);
			CloseFreehand();
			}
		}

	if (event->type==GADMSG_ACTIVE && event->g==playrange.gad_ptr) {
		if (rangeset())
			BroadcastEventParms(se_cmd_play_sample,ei->cur,ei->start_mark,ei->end_mark-ei->start_mark+1,0,0,0);
		}
			
	if (event->type==GADMSG_ACTIVE && event->g==playwave.gad_ptr) {
		BroadcastEventParms(se_cmd_play_sample,ei->cur,0,ei->cur->length-1,0,0,0);
		}
			
	if (event->type==GADMSG_ACTIVE && event->g==stop.gad_ptr) {
		BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
		}

	if (event->type==GADMSG_ACTIVE && event->g==cut.gad_ptr) {
		if (rangeset())
			if (okreq("Cut range"))  {
				show_error(DoNonDestCut());
				BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
				}
			}

	if (event->type==GADMSG_ACTIVE && event->g==copy.gad_ptr) {
		if (rangeset())
			if (okreq("Copy range. (Does not alter sample 'CopyBuffer')")) {
				show_error(DoNonDestCopy());
				}
		}

	if (event->type==GADMSG_ACTIVE && event->g==delete.gad_ptr) {
		if (rangeset())
			if (okreq("Erase range")) {
				show_error(DoNonDestDelete());
				BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
				}
		}

	if (event->type==GADMSG_ACTIVE && event->g==keep.gad_ptr) {
		if (rangeset())
			if (okreq("erase all except marked range")) {
				show_error(DoNonDestKeep());
				BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
				}
		}

	if (event->type==GADMSG_ACTIVE && event->g==rate.gad_ptr) {
		NewRate(ei->cur);
		update_status(ei);
		}
			
	if (event->type==GADMSG_ACTIVE && event->g==paste.gad_ptr) {
		if (rangeset()) {
				show_error(DoNonDestPaste());
				BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
				}
		}

	if (event->type==GADMSG_ACTIVE && event->g==dpaste.gad_ptr) {
		if (rangeset()) {
			show_error(DoPasteEdit());
			BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
			}
		}

	if (event->type==GADMSG_ACTIVE && event->g==dadd.gad_ptr) {
		if (okreq("Append (add) CopyBuffer to end of this sample")) {
			show_error(DoAddEdit());
			BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
			}
		}

	if (event->type==GADMSG_ACTIVE && event->g==compress.gad_ptr) {
		if (okreq("Compress: make 'nondestructive' edits permanent?"))
			show_error(DoCompressEdit());
		}

	if (event->type==GADMSG_ACTIVE && event->g==dcopy.gad_ptr) {
		if (rangeset())
			if (okreq("Copy the range to CopyBuffer"))
				show_error(DoCopyEdit());
		}

	if (event->type==GADMSG_ACTIVE && event->g==range.gad_ptr) {
		SetRange(ei);
		ei->mark_drawn=TRUE;
	   ei->old_x1 = sampletomousenoclip(matchscale(ei->start_mark));
	   ei->old_x2 = sampletomousenoclip(matchscale(ei->end_mark));
		graph(ei); 
		update_status(ei);
		}

	if (event->type==GADMSG_ACTIVE && event->g==silence.gad_ptr) {
		show_error(AddSilence(ei->cur->parms.rate));	/** 1 second worth **/
		}

	if (event->type==GADMSG_ACTIVE && event->g==flip.gad_ptr) {
		if (rangeset())
			if (okreq("Reverse Range")) {
				show_error(DoFlipEdit());
				BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
				}
		}

	if (event->type==GADMSG_ACTIVE && event->g==scale.gad_ptr) {
		if (rangeset())
			Scale(ei);
		}

	if (event->type==GADMSG_ACTIVE && event->g==begin.gad_ptr) {
		if (okreq("Remove all non destructive edits and begin anew")) {
			show_error(DoNonDestNoClips());
			BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
			}
		}

	if (event->type==GADMSG_ACTIVE && event->g==showrange.gad_ptr) {
		if (rangeset()) {
			((struct slider *)zoom.gad_ptr)->top_element 
				= (ei->end_mark-ei->start_mark+1)/ei->graph_width;
			ei->flags |=EI_CENTER_RANGE;
			move_knob(zoom.gad_ptr);		/** Also calls zoom_func() **/
			}
		}

	if (event->type==GADMSG_ACTIVE && event->g==undo.gad_ptr) {
		show_error(DoNonDestUndo());
		BroadcastEventParms(se_info_sample_edited,ei->cur,0,0,0,0,0);
		}

	if (event->type==GADMSG_ACTIVE && event->g==freehand.gad_ptr) {
		b=(struct button *)event->g;
		if (b->flags&BUTN_HIT) {
			new_pointer(PTR_BUSY);
			SetNewZoom(1);
			if (new_zoom!=ei->zoom_scale) {
				ei->flags |= EI_REFRESH_HSCROLL;
				handle_graph_events();
				}
			hide_gadget(zoom.gad_ptr);	/** slider is now inacurate, so hide it if visible **/
			show_gadget(zoombutn.gad_ptr);
			refresh_gadget(zoombutn.gad_ptr);
			OpenFreehand();
			new_pointer(PTR_NORMAL);
			}
		else {
			CloseFreehand();
			}
		}

	ReplyMsg(event);		/** need because of exit() **/
	}
}

/************************************************************/

rangeset()
{
if (ei->marked_samp)
	return(TRUE);

telluser("No range marked",0,0);
return(FALSE);
}


/************************************************************/

AddSilence(len)
{
short *mem;
struct SampleFilePointer *sp;
int error;

error=OUT_OF_MEMORY;
mem=(short *)AllocMem(len*2, MEMF_CLEAR);
if (mem) {
	new_pointer(PTR_BUSY);
	error=STDERR_CANT_GET_READ_LOCK;
	sp=SampleOpen(ei->cur,SP_MODE_READWRITE);
	if (sp) {
		error=NO_ERROR;
		SampleSeek(sp, 0L, SS_OFFSET_END);
		SampleWrite(sp, mem, len);
		SampleClose(sp);
		GraphAppend(mem, len/ei->gb_scale);
		RedrawGraphBuffer(ei->edit_win);
		}
	FreeMem(mem, len*2);
	new_pointer(PTR_NORMAL);
	}
return(error);
}

/************************************************************/

GraphAppend(src, len)
char *src;
int len;
{
char *newbuf;
int newsize;

newsize=ei->org_gb_size+len;
newbuf=(char *)AllocMem(newsize, 0);
if (newbuf) {
	movmem(ei->org_graph_buffer,newbuf, ei->org_gb_size);
	movmem(src, newbuf+ei->org_gb_size, len);
	FreeGraphBuffer(ei);
	ei->org_gb_size=newsize;
	ei->org_graph_buffer=newbuf;
	}
return(NO_ERROR);
}

/************************************************************/

DoCopyEdit()
{
struct Disk_Samp *newsamp;
int error;
struct SampleFilePointer *rsp;
struct SampleFilePointer *wsp;

newsamp=(struct Disk_Samp *)FindSample("CopyBuffer",-1);
if (newsamp==ei->cur)
	return(STDERR_COPYBUFFER_IN_USE);
if (newsamp) 
 	DeleteSample(newsamp);
if ((error=CreateSample("CopyBuffer",0,&newsamp))==NO_ERROR) {
	if ((error=GetLocks(ei->cur,newsamp, &rsp, &wsp))==NO_ERROR) {
		new_pointer(PTR_BUSY);
		SampleSeek(rsp, ei->start_mark, SS_OFFSET_BEGINNING);
		SampleCopy(wsp,rsp,ei->end_mark-ei->start_mark+1);
		ReleaseLocks(wsp,rsp);
		CopySampData(ei->cur,newsamp);
		new_pointer(PTR_NORMAL);
		}
	else {
		DeleteSample(newsamp);	/** Never used it! **/
		}
	}
return(error);
}

/************************************************************/

DoPasteEdit()
{
struct Disk_Samp *copysamp;
struct Disk_Samp *newsamp;
int error;
struct SampleFilePointer *rsp1;
struct SampleFilePointer *rsp2;
struct SampleFilePointer *wsp;

copysamp=(struct Disk_Samp *)FindSample("CopyBuffer",-1);
if (copysamp==0) {
	return(STDERR_CANT_FIND_COPYBUFFER);
	}

error=AskUserPasteType();

if (error==PT_CANCEL)
	return(NO_ERROR);

if ((wsp=SampleCreateRevision(ei->cur))==0) {
	return(STDERR_CANT_GET_WRITE_LOCK);
	}

newsamp=wsp->disksamp;

if ((rsp1=SampleOpen(copysamp,SP_MODE_OLDFILE))==0) {
	SampleClose(wsp);	
	DeleteSample(newsamp);
	return(STDERR_CANT_GET_READ_LOCK);
	}


if ((rsp2=SampleOpen(ei->cur,SP_MODE_OLDFILE))==0) {
	SampleClose(wsp);
	DeleteSample(newsamp);
	SampleClose(rsp1);	
	return(STDERR_CANT_GET_READ_LOCK);
	}


new_pointer(PTR_BUSY);
if (error==PT_OVERWRITE) {
	if (SampleCopy(wsp,rsp2,ei->start_mark)==NO_ERROR)
		if (SampleCopy(wsp,rsp1,copysamp->length)==NO_ERROR) {
			SampleSeek(rsp2, ei->end_mark+1, SS_OFFSET_BEGINNING);
			SampleCopy(wsp,rsp2,ei->cur->length-ei->end_mark-1);
			}
	}
else {	/** must be PT_INSERT **/
	if (SampleCopy(wsp,rsp2,ei->start_mark)==NO_ERROR)
		if (SampleCopy(wsp,rsp1,copysamp->length)==NO_ERROR)
			SampleCopy(wsp,rsp2,ei->cur->length-ei->start_mark);
	}


error=NO_ERROR;
SampleClose(wsp);	/** Need to delete sampe struct here!? **/
SampleClose(rsp1);	
SampleClose(rsp2);	


if (DeleteWorkingFile(ei->cur))	/** take this line out to support undo levels **/
	DeleteSample(ei->cur);

ei->cur=newsamp;		/** close_edit references ei->cur **/
close_edit();
open_edit(newsamp);

new_pointer(PTR_NORMAL);
return(error);
}

/************************************************************/

DoAddEdit()
{
struct Disk_Samp *copysamp;
int error;
struct SampleFilePointer *rsp;
struct SampleFilePointer *wsp;

copysamp=(struct Disk_Samp *)FindSample("CopyBuffer",-1);
if (copysamp==0) {
	return(STDERR_CANT_FIND_COPYBUFFER);
	} 

if ((error=GetLocks(copysamp, ei->cur, &rsp, &wsp))==NO_ERROR) {
	new_pointer(PTR_BUSY);
	SampleSeek(wsp, 0, SS_OFFSET_END);
	SampleCopy(wsp,rsp,copysamp->length);
	ReleaseLocks(wsp,rsp);

	close_edit();
	open_edit(ei->cur);

	new_pointer(PTR_NORMAL);
	}
return(error);
}

/************************************************************/

DoCompressEdit()
{
struct SampleFilePointer *rsp;
struct SampleFilePointer *wsp;
struct Disk_Samp *newsamp;

rsp=SampleOpen(ei->cur, SP_MODE_OLDFILE);
if (rsp==0) {
	return(STDERR_CANT_GET_READ_LOCK);
	}

wsp=(struct SampleFilePointer *)SampleCreateRevision(ei->cur);
if (wsp==0) {
	SampleClose(rsp);
	return(STDERR_CANT_CREATE_SAMPLE);
	}

new_pointer(PTR_BUSY);
newsamp=wsp->disksamp;
SampleCopy(wsp,rsp,rsp->disksamp->length);
CopySampData(ei->cur,wsp->disksamp);
SampleClose(rsp);
SampleClose(wsp);
if (DeleteWorkingFile(ei->cur))	/** take this line out to support undo levels **/
	DeleteSample(ei->cur);

ei->cur=newsamp;		/** close_edit references ei->cur **/
close_edit();
open_edit(newsamp);

new_pointer(PTR_NORMAL);
return(NO_ERROR);
}

/************************************************************/

DoFlipEdit()
{
struct SampleFilePointer *wsp;
short *amem;
short *bmem;
int buflen;
int fliplen;
int astart, bend;
short temp;
int thislen;
int i;

wsp=SampleOpen(ei->cur, SP_MODE_READWRITE);
if (wsp==0) {
	return(STDERR_CANT_OPEN_SAMPLE);
	}
buflen=StudioBase->defaults.copy_buf_size/2;
amem=(short *)AllocMem(buflen*2, 0);
if (amem==0) {
	SampleClose(wsp);
	return(OUT_OF_MEMORY);
	}

bmem=(short *)AllocMem(buflen*2, 0);
if (bmem==0) {
	FreeMem(amem, buflen*2);
	SampleClose(wsp);
	return(OUT_OF_MEMORY);
	}

new_pointer(PTR_BUSY);

astart=ei->start_mark;
bend=ei->end_mark;
fliplen=max((bend-astart+1)/2,1);	/** 1 so no crashes **/

do {
	thislen= min(fliplen, buflen);
	SampleSeek(wsp, astart, SS_OFFSET_BEGINNING);
	SampleRead(wsp, amem, thislen);
	SampleSeek(wsp, bend-thislen+1, SS_OFFSET_BEGINNING);
	SampleRead(wsp, bmem, thislen);
	for (i=0; i < thislen; i++) {
		temp=amem[i];
		amem[i]=bmem[thislen-i-1];
		bmem[thislen-i-1]=temp;
		}
	SampleSeek(wsp, astart, SS_OFFSET_BEGINNING);
	SampleWrite(wsp, amem, thislen);
	SampleSeek(wsp, bend-thislen+1, SS_OFFSET_BEGINNING);
	SampleWrite(wsp, bmem, thislen);
	fliplen -= thislen;
	astart += thislen;
	bend -= thislen;
	} while (fliplen);

FreeMem(amem, buflen*2);
FreeMem(bmem, buflen*2);
SampleClose(wsp);
close_edit();
open_edit(ei->cur);
new_pointer(PTR_NORMAL);
return(NO_ERROR);
}

#if 0
/************************************************************/

long CopyGraphData(src_ptr,src_size,src_scale,src_sm,src_em,ei,offset)
char *src_ptr;
long src_size;
long src_sm,src_em;
struct EditInstance *ei;
int offset;
{
long dest_scale;
long fixed;
long index;
char *dest_ptr;
long len,k;

if (src_sm > src_em)	
	return(offset);

dest_scale = ei->gb_scale;

src_ptr = src_ptr+src_sm/src_scale;
dest_ptr = ei->graph_buffer+offset;
len = (src_em-src_sm+1)/dest_scale;
len = min(len, ei->gb_size-offset);
fixed = (dest_scale<<16)/src_scale;
index = 0;

for (k=len; k; k--) {
	*dest_ptr++ = src_ptr[index>>16];
	index=index+fixed;
	}
return(offset+len);
}
#endif
/************************************************************/

GetLocks(rs,ws,rsp,wsp)
struct Disk_Samp *rs,*ws;
struct SampleFilePointer **rsp,**wsp;
{
if ((*wsp=SampleOpen(ws, SP_MODE_READWRITE))==0) {
	return(STDERR_CANT_GET_WRITE_LOCK);
	}

if ((*rsp=SampleOpen(rs,SP_MODE_OLDFILE))==0) {
	SampleClose(wsp);
	return(STDERR_CANT_GET_READ_LOCK);
	}
return(NO_ERROR);
}

/************************************************************/

void CopySampData(rs,ws)
struct Disk_Samp *rs,*ws;
{
struct SampleFilePointer *sp;

ws->parms=rs->parms;
ws->type=rs->type;
sp = SampleOpen(ws, SP_MODE_READWRITE);
if (sp)	/** SampleClose calls SampleWriteTag() if in READWRITE mode **/
	SampleClose(sp);
}

/************************************************************/

void ReleaseLocks(wsp,rsp)
struct SampleFilePointer *rsp,*wsp;
{
SampleClose(wsp);
SampleClose(rsp);
}

/************************************************************/

okreq(str)
char *str;
{
#if 0
if (thisstate->es_flags&ES_DESTRUCTIVE_EDITS)
	return(askuser("OK to do DESTRUCTIVE edit:",str,0));
else
	return(askuser("OK to do non-destructive edit:",str,0));
#endif
return(askuser("OK to do edit:",str,0));
}

/************************************************************/

void open_edit(samp)
struct Disk_Samp *samp;
{
/****
if (thisstate->es_flags&ES_IN_USE) {
	telluser("All loaded edit modules in use!",0,0);
	return;
	}
*****/
thisstate->es_flags |= ES_IN_USE;

NotifyMeOnEvent(se_info_sample_deleted, thismod);
NotifyMeOnEvent(se_cmd_trigger_playback, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);

/** If no dclips exist for this disk sample, create one **/
/** covering the entire sample **/

if (samp->dclips[0].end==0) {
	samp->dclips[0].end=samp->length-1;
	telluser("Bug Detected by Edit!","No dclips set!",0);
	}

if (samp->org_length==0) {		/** Just in case someone didn't set it **/
	samp->org_length=samp->length;
	telluser("Bug Detected by Edit!","No org_length set!",0);
	}

if (samp->length==0) {		/** Just in case someone didn't set it **/
	samp->length=samp->org_length;
	telluser("Bug Detected by Edit!","No length set!",0);
	}

setmem(&editparms, sizeof(editparms), 0);
ei=InitEditInstance(samp);

/** Round MaxWidth **/

NewWindowStructure1.MaxWidth = (samp->length+(editparms.max_zoom-1))/editparms.max_zoom+GRAPH_X+GRAPH_X;
if ((editparms.edit_win=OpenNewWind(&NewWindowStructure1,thismod->state,PREFERENCES,NULL))==NULL)
	{
	telluser("Can't open edit window",0,0);
	thisstate->es_flags &= (~ES_IN_USE);
	return;
	}
/** Set graph_width now because NewWindow.Width can be changed **/
/** by calls to OpenWind() **/

ei->graph_width = ei->edit_win->Width - GRAPH_X - GRAPH_X;
ei->graph_height = ei->edit_win->Height - GRAPH_Y1 - GRAPH_Y2;

calc_edit_sigbit();	/** changes when window changes **/

hscroll.top_element = 0;
hscroll.list_size = samp->length;
hscroll.show_size = ei->graph_width*ei->zoom_scale;
zoom.top_element = new_zoom-1;
zoom.list_size = ei->max_zoom;
zoom.show_size = 1;

/** Setup buffer that stores waveform summery to graph from **/

InitGraphBuffer(ei);			/** pick size & allocate memory **/
BuildDivTab(ei);

if (!AttemptReadGraphBuffer(ei))
	FillGraphBuffer(ei);			/** fill org_graph_buffer with data **/
else
	DeleteGraphFile(ei);		/** Remove since it may change **/
GenEditGraphBuffer(ei);		/** fill graph_buffer with data **/

EditSetGadsBox();	/* set box_x1, etc. */

create_gadget_chain(ei->edit_win,&hscroll);

sprintf(zoombutn_text,"ZM 1:%d",ei->zoom_scale);
((struct button *)(zoombutn.gad_ptr))->text=zoombutn_text;

/** Initilize double buffer rastport and bitmaps **/

if (InitRenderBuffer(ei)!=NO_ERROR) {
	telluser("Can't alloocate display buffers!",0,0);
	return;
	}

/** Draw the graph & gadgets at current zoom setting **/

refresh_edit_win(ei->edit_win);
update_status(ei);
FillUndoBuffer();	/** Set undobuffer to current state **/
thisstate->es_flags ^= ES_DESTRUCTIVE_EDITS;	/** Toggle flag **/
nondest_func(nondest.gad_ptr);  /** Toggle flag & update buttons */
}

/***********************************************************/

struct EditInstance *InitEditInstance(samp)
struct Disk_Samp *samp;
{
struct EditInstance *ei;
int denom;

ei=&editparms;

/** editparms.max_zoom = samp->length/(StudioBase->screen->Width-GRAPH_X-GRAPH_X)+1; WAS 1/24/92 **/

/** max_zoom is in samples/pixel.  Typically, the sound won't be **/
/** be evenly divisable.  That is, each pixel represents max_zoom **/
/** number of samples.  However, the last pixel will typically **/
/** represent less than max_zoom pixals.  We want to round **/
/** max_zoom such that includes that last fractional pixel **/

denom=StudioBase->screen->Width-GRAPH_X-GRAPH_X;
editparms.max_zoom = (samp->length+(denom-1))/denom;

editparms.inserting  = FALSE;        /** NOT USING THIS ANY MORE!! **/
editparms.mark_drawn = FALSE;
editparms.cur=samp;
editparms.line_pos=0xFFFFFFFF;	/** No line drawn **/
/** zero trashed variables **/

ei->old_x1=0;
ei->old_x2=0;
ei->start_mark=0;
ei->end_mark=0;
ei->insert_mark=0;
ei->marked_samp=0;
ei->insert_samp=0;
ei->flags=0;

/** Setup gadgets **/

ei->zoom_scale = ei->max_zoom;	
new_zoom = ei->zoom_scale;
ei->graph_start = 0;
new_graph_start = ei->graph_start;


ei->rp.BitMap = &ei->bm;

return(ei);
}

/***********************************************************/

void InitGraphBuffer(ei)
struct EditInstance *ei;
{
FreeGraphBuffer(ei);
ei->gb_scale = ei->cur->org_length/DEF_GBSIZE+1;	/** buffer:sample ratio **/

AllocOrgGraphBuffer(ei);
AllocGraphBuffer(ei);
}

void AllocGraphBuffer(ei)
struct EditInstance *ei;
{
ei->gb_size = ei->cur->length/ei->gb_scale;	/** PUT DEF_GBSIZE in edit state!!!! **/
ei->graph_buffer = (char *)AllocMem(ei->gb_size, MEMF_CLEAR);
}

void AllocOrgGraphBuffer(ei)
struct EditInstance *ei;
{
ei->org_gb_size = ei->cur->org_length/ei->gb_scale;
ei->org_graph_buffer = (char *)AllocMem(ei->org_gb_size, MEMF_CLEAR);
}


/***********************************************************/

void FreeGraphBuffer(ei)
struct EditInstance *ei;
{
if (ei->graph_buffer) {
	FreeMem(ei->graph_buffer, ei->gb_size);
	ei->graph_buffer=NULL;
	}

FreeOrgGraphBuffer(ei);
}

void FreeOrgGraphBuffer(ei)
struct EditInstance *ei;
{
if (ei->org_graph_buffer) {
	FreeMem(ei->org_graph_buffer, ei->org_gb_size);
	ei->org_graph_buffer=NULL;
	}
}


/***********************************************************/

int InitRenderBuffer(ei)
struct EditInstance *ei;
{
if (ei->bm.Planes[0]) {
	FreeMem(ei->bm.Planes[0], ei->plane_size);
	ei->bm.Planes[0]=0;
	}

InitBitMap(&ei->bm,1L,ei->graph_width,ei->graph_height);	/** depth, width, height of bitmap **/

ei->plane_size=RASSIZE(ei->graph_width,ei->graph_height);
ei->bm.Planes[0]=(PLANEPTR)AllocMem(ei->plane_size,MEMF_CHIP);
if (ei->bm.Planes[0]==NULL)
	return(OUT_OF_MEMORY);

InitRastPort(&ei->rp);
ei->rp.BitMap = &ei->bm;
ei->rp.Mask = 1;

return(NO_ERROR);
}

/************************************************************/

void FreeRenderBuffer(ei)
struct EditInstance *ei;
{
if (ei->bm.Planes[0]) {
	FreeMem(ei->bm.Planes[0], ei->plane_size);
	ei->bm.Planes[0]=0;
	}
}
/************************************************************/

void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;
SHORT mousex,mousey;
struct Gadget *gad;
int temp;
struct Window *aw;
short festep;

aw = ei->edit_win;

while ((message=(struct IntuiMessage *)GetMsg(aw->UserPort))!=0) {
	HandleIntuitMess(aw,message);
	class=message->Class;
	code=message->Code;
	gad=(struct Gadget *)message->IAddress;
	mousex=message->MouseX;
	mousey=message->MouseY;
	ReplyMsg(message);

reswitch:

	switch (class) {

	case CLOSEWINDOW:
		if (editparms.edit_win==aw) {
			WriteGraphBuffer(ei);
			close_edit();
			}
		return;

	case NEWSIZE:
		RefreshWind(ei->edit_win);	/** Todd's windfunc.c **/
	   ei->graph_width = ei->edit_win->Width - GRAPH_X - GRAPH_X;
	   ei->graph_height = ei->edit_win->Height - GRAPH_Y1 - GRAPH_Y2;
		if (InitRenderBuffer(ei)!=NO_ERROR) {
			telluser("Can't alloocate display buffers!",0,0);
			}
	   refresh_edit_win(ei->edit_win);
		update_status(ei);
		if (ei->flags&EI_SET_MAX_WIDTH) {
			WindowLimits(ei->edit_win,0,0,ei->edit_win->Width,0);
			ei->flags &= (~EI_SET_MAX_WIDTH);
			}
	   break;

	case MOUSEMOVE:
		if (ei->flags&EI_DRAWING) {  /** Free hand edit mode? **/
			if (mousex > last_mousex) festep=1;else festep=-1;
			if (mousex==last_mousex) festep=0;
			do {
				last_mousex +=festep;
				DoFreehand(last_mousex, mousey);
				} while (mousex!=last_mousex);
			break;
			}
	   if (dragging) {
		/** clip pointer position to inside graph **/
			if (mousex > GRAPH_X+ei->graph_width-1)
				mousex=GRAPH_X+ei->graph_width-1;
			if (mousex < GRAPH_X)
				mousex=GRAPH_X;

         set_new_em(mousex);
	      draw_marked(ei->edit_win);
			update_status(ei);
	      }

	/** Through away extra mouse moves **/

	   while(message=(struct IntuiMessage *)GetMsg(ei->edit_win->UserPort)) {
	      class=message->Class;
			code=message->Code;
			gad=(struct Gadget *)message->IAddress;
			mousex=message->MouseX;
			mousey=message->MouseY;
			ReplyMsg(message);
			if (class!=MOUSEMOVE)
				goto reswitch;
			}
			break;


	case MOUSEBUTTONS:
	   if (code!=SELECTDOWN) {
	      if (dragging) {
   	      dragging=FALSE;
	         if (ei->start_mark > ei->end_mark) {     /** swap **/
   	         temp = ei->start_mark;
      	      ei->start_mark = ei->end_mark;
					ei->end_mark=temp;
	            }

			/** Round end_mark up.  Since each pixel defines many **/
			/** samples, the end_mark should include all samples **/
			/** in it's pixel **/
			
				ei->end_mark=fullmousetosample(sampletomousenoclip(ei->end_mark));

				update_status(ei);
	         }
			else {
				if (ei->flags&EI_DRAWING && ei->sp)
					EndFreehand();
				}
	      break;
	      }

	   if (in_graph(mousex, mousey) && ei->cur!=NULL) {
			if (ei->flags&EI_FREEHAND) {
				StartFreehand(mousex, mousey);
				break;
				}
	      dragging = TRUE;
         if (ei->marked_samp==ei->cur && closeto (mousex, ei->end_mark)) {
            set_new_em(mousex);
  	         draw_marked(ei->edit_win);
            }
         else if (ei->marked_samp==ei->cur && closeto (mousex, ei->start_mark)) {
            ei->start_mark = ei->end_mark;
            set_new_em(mousex);
            draw_marked(ei->edit_win);
            }
         else {
            ei->start_mark = start_dragging(mousex);
            set_new_em(mousex);
     	      ei->marked_samp = ei->cur;
        	   }
			update_status(ei);
	      }
   	   break;

	case RAWKEY:
	   if (code==64) {      /** space bar cancels **/
   	   }
	   break;
		}
	}		/** END of Intuition event loop **/
return;
}

/************************************************************/

void close_edit()
{
struct SampleFilePointer *sp;

if (editparms.edit_win) {
	IgnoreEvent(se_info_sample_deleted, thismod);
	IgnoreEvent(se_cmd_trigger_playback, thismod);
	IgnoreEvent(se_info_channel_done, thismod);

	((struct button *)(zoombutn.gad_ptr))->text=NULL;
	delete_gadget_chain(editparms.edit_win);
	FreeGraphBuffer(ei);
	CloseWind(editparms.edit_win);
	editparms.edit_win=NULL;
	edit_sigbit=0;
	FreeRenderBuffer(ei);
	thisstate->es_flags &= (~ES_IN_USE);
	if (ei->cur) {
		sp = SampleOpen(ei->cur, SP_MODE_READWRITE);
		if (sp)	/** SampleClose calls SampleWriteTag() if in READWRITE mode **/
			SampleClose(sp);
		}
	}
}

/************************************************************/

void calc_edit_sigbit()
{
edit_sigbit=0;
edit_sigbit |= (1<<editparms.edit_win->UserPort->mp_SigBit);
}

/************************************************************/

void OpenFreehand()
{
new_pointer(PTR_BUSY);
ei->sp=SampleOpen(ei->cur, SP_MODE_READWRITE);
if (ei->sp==0) {
	ei->flags &= ~EI_FREEHAND;
	ei->flags &= ~EI_DRAWING;
	telluser("EDIT:","Can't open sample file!",0);
	return;
	}
ei->flags |= EI_FREEHAND;
freehand_gs=ei->graph_start;
SampleSeek(ei->sp, freehand_gs, SS_OFFSET_BEGINNING);
freehand_size=SampleRead(ei->sp, freehand_buf, sizeof(freehand_buf)/2);
new_pointer(PTR_NORMAL);
}

/************************************************************/

void StartFreehand(mousex, mousey)

{

/** If user has moved display, update freehand buffer **/
if (freehand_gs!=ei->graph_start) {
	new_pointer(PTR_BUSY);
	SampleSeek(ei->sp, freehand_gs, SS_OFFSET_BEGINNING);
	SampleWrite(ei->sp, freehand_buf, freehand_size);

	freehand_gs=ei->graph_start;
	SampleSeek(ei->sp, freehand_gs, SS_OFFSET_BEGINNING);
	freehand_size=SampleRead(ei->sp, freehand_buf, sizeof(freehand_buf)/2);
	new_pointer(PTR_NORMAL);
	}

ei->flags|=EI_DRAWING;
last_mousex=mousex;
DoFreehand(mousex, mousey);
}

/************************************************************/
/** THIS ROUTINE HAS A BUG IN THAT IT DOESN'T UPDATE ei->org_graph **/

void CloseFreehand()
{
new_pointer(PTR_BUSY);
SampleSeek(ei->sp, freehand_gs, SS_OFFSET_BEGINNING);
SampleWrite(ei->sp, freehand_buf, freehand_size);
SampleClose(ei->sp);
ei->flags &= ~EI_DRAWING;
ei->flags &= ~EI_FREEHAND;
ei->sp=0;
graph(ei); 
new_pointer(PTR_NORMAL);
}

/************************************************************/

void EndFreehand()
{
ei->flags &= ~EI_DRAWING;
}

/************************************************************/

void DoFreehand(mousex, mousey)
{
int y;
extern unsigned short div_tab[128];
short val;

if (mousex <=GRAPH_X)
	return;
if (mousex >= (GRAPH_X+ei->graph_width-1))
	return;
if (mousey < GRAPH_Y1)
	mousey=GRAPH_Y1;
if (mousey >= (GRAPH_Y1+ei->graph_height))
	mousey=GRAPH_Y1+ei->graph_height-1;

/** erase old points **/
SetDrMd(ei->edit_win->RPort, JAM1);
SetAPen(ei->edit_win->RPort, BROWN);	/** USE STUDIO PENS !!!! **/
Move (ei->edit_win->RPort, mousex, GRAPH_Y1);
Draw (ei->edit_win->RPort, mousex, GRAPH_Y1+ei->graph_height-1);

Move (ei->edit_win->RPort, mousex+1, GRAPH_Y1);
Draw (ei->edit_win->RPort, mousex+1, GRAPH_Y1+ei->graph_height-1);
Move (ei->edit_win->RPort, mousex-1, GRAPH_Y1);
Draw (ei->edit_win->RPort, mousex-1, GRAPH_Y1+ei->graph_height-1);

/** Draw new point **/

ei->edit_win->RPort->Mask = 0x01;
SetAPen(ei->edit_win->RPort, 1);

y = GRAPH_Y1+ei->graph_height/2+
	 GMAP(freehand_buf[mousex - GRAPH_X - 1]);
val = (32767L*(int)(mousey-GRAPH_Y1-ei->graph_height/2))/(ei->graph_height/2);
freehand_buf[mousex - GRAPH_X]=val;  /**!!!! WHY IS THIS +1 NEEDED!!!!!!!!!**/
Move (ei->edit_win->RPort, mousex-1, y);
Draw (ei->edit_win->RPort, mousex, mousey);
y = GRAPH_Y1+ei->graph_height/2+
	 GMAP(freehand_buf[mousex - GRAPH_X + 1]);
Draw(ei->edit_win->RPort, mousex+1, y);
ei->edit_win->RPort->Mask = 0xFF;
}

/************************************************************/

/** given the pointer position, update the global variable "marker" **/

int start_dragging(mousex)

int mousex;
{
int marker;    /** sample where dragging starts **/

LockGadLibRender();
ei->edit_win->RPort->Mask = 0x01;
SetDrMd(ei->edit_win->RPort, COMPLEMENT);
SetDrMd(&ei->rp, COMPLEMENT);

if (ei->mark_drawn) {
	if (ei->old_x2>=GRAPH_X && ei->old_x1 <= (GRAPH_X+ei->graph_width-1)) {
		if (ei->old_x1 < GRAPH_X)
			ei->old_x1=GRAPH_X;
		if (ei->old_x2 > (GRAPH_X+ei->graph_width-1))
			ei->old_x2 = GRAPH_X+ei->graph_width-1;
	   RectFill(ei->edit_win->RPort, ei->old_x1, GRAPH_Y1, ei->old_x2, GRAPH_Y1+ei->graph_height-1);
	   RectFill(&ei->rp, ei->old_x1-GRAPH_X, 0, ei->old_x2-GRAPH_X, ei->graph_height-1);
		}
	}

ei->mark_drawn=TRUE;    /** erase old marked range **/
ei->old_x1 = ei->old_x2 = mousex;

marker = mousetosample(mousex);
Move (ei->edit_win->RPort, mousex, GRAPH_Y1);
Draw (ei->edit_win->RPort, mousex, GRAPH_Y1+ei->graph_height-1);
Move (&ei->rp, mousex-GRAPH_X, 0);
Draw (&ei->rp, mousex-GRAPH_X, ei->graph_height-1);
ei->edit_win->RPort->Mask = 0xFF;
UnLockGadLibRender();
return(marker);
}

/*****************************************************/

void draw_marked(Window)

struct Window *Window;

{
int x1,x2;
int dbx1,dbx2;
int temp;
/*char bug[80];*/
int edgemoving;

SetDrMd(ei->edit_win->RPort, COMPLEMENT);
SetDrMd(&ei->rp, COMPLEMENT);

x1 = sampletomouse(matchscale(ei->start_mark));
x2 = sampletomouse(matchscale(ei->end_mark));

/** Currently, ei->end_mark is always the edge of the range that is chaning **/

edgemoving=x2;

if (x2 < x1) {    /** swap **/
   temp = x1;
   x1 = x2;
   x2 = temp;
   }

dbx1 = x1-GRAPH_X;		/** double buffer **/
dbx2 = x2-GRAPH_X;

if (ei->old_x1==x1 && ei->old_x2==x2 && ei->mark_drawn)
   return;

LockGadLibRender();
ei->edit_win->RPort->Mask = 0x01;

/** As user moves the mouse, just rectfill the changes **/

if (ei->mark_drawn) {
	   if (x1 < ei->old_x1) {
   	   RectFill(ei->edit_win->RPort, x1, GRAPH_Y1, ei->old_x1-1, GRAPH_Y1+ei->graph_height-1);
   	   RectFill(&ei->rp, dbx1, 0, ei->old_x1-1-GRAPH_X, ei->graph_height-1);
			}
	   if (x1 > max(ei->old_x1,GRAPH_X)) {
   	   RectFill(ei->edit_win->RPort, ei->old_x1, GRAPH_Y1, x1-1, GRAPH_Y1+ei->graph_height-1);
   	   RectFill(&ei->rp, ei->old_x1-GRAPH_X, 0, dbx1-1, ei->graph_height-1);
			}
	   if (x2 > ei->old_x2) {
   	   RectFill(ei->edit_win->RPort, ei->old_x2+1, GRAPH_Y1, x2, GRAPH_Y1+ei->graph_height-1);
   	   RectFill(&ei->rp, ei->old_x2+1-GRAPH_X, 0, dbx2, ei->graph_height-1);
			}
	   if (x2 < min(ei->old_x2,GRAPH_X+ei->graph_width-1)) {
   	   RectFill(ei->edit_win->RPort, x2+1, GRAPH_Y1, min(ei->old_x2,GRAPH_X+ei->graph_width-1), GRAPH_Y1+ei->graph_height-1);
   	   RectFill(&ei->rp, dbx2+1, 0, min(ei->old_x2-GRAPH_X,ei->graph_width-1), ei->graph_height-1);
			}
	   }
else {
   ei->mark_drawn=TRUE;
   RectFill(ei->edit_win->RPort, x1, GRAPH_Y1, x2, GRAPH_Y1+ei->graph_height-1);
   RectFill(&ei->rp, dbx1, 0, dbx2, ei->graph_height-1);
	ei->old_x1 = x1;
	ei->old_x2 = x2;
   }

ei->edit_win->RPort->Mask = 0xFF;
UnLockGadLibRender();


if (edgemoving==x1) {
	ei->old_x1 = x1;
	if (ei->old_x2 < (GRAPH_X+ei->graph_width-1))
		ei->old_x2 = x2;
	}
else {
	ei->old_x2 = x2;
	if (ei->old_x1 >= GRAPH_X)	/** AUGHH!! x1 is clipped to graph, old_x1 isnt!! **/
		ei->old_x1 = x1;			/** Don't update with cliped value!! **/
	}

#if 0
if (ei->old_x1 >= GRAPH_X)	/** AUGHH!! x1 is clipped to graph, old_x1 isnt!! **/
	ei->old_x1 = x1;			/** Don't update with cliped value!! **/
if (ei->old_x2 < (GRAPH_X+ei->graph_width-1))
	ei->old_x2 = x2;
#endif
}

/*****************************************************/

int in_graph(x, y)

int x, y;

{
return (x>=GRAPH_X && x<(GRAPH_X+ei->graph_width) && y>GRAPH_Y1 && y<(GRAPH_Y1+ei->graph_height));
}

/*****************************************************/

void set_new_em(mousex)
int mousex;
{
ei->end_mark = mousetosample(mousex);

if (ei->end_mark >= ei->start_mark)	/** round last pixel up **/
	ei->end_mark=fullmousetosample(mousex);
	
else
	ei->start_mark=fullmousetosample(sampletomousenoclip(ei->start_mark));

if (ei->end_mark >= ei->cur->length)
   ei->end_mark = ei->cur->length-1;
if (ei->end_mark < 0)
   ei->end_mark = 0;
}

/*****************************************************/

buffertosample(mousex)
int mousex;
{
return(ei->graph_start+mousex*ei->zoom_scale);
}

/***************************************************************/
/** this one returns a value that starts at the beginning of  **/
/** the pixel.  Samples represented by the pixel arn't counted */
/***************************************************************/

mousetosample(mousex)
int mousex;
{
int retval;

retval=ei->graph_start+(mousex - GRAPH_X)*ei->zoom_scale;
if (retval >= ei->cur->length)
	retval=ei->cur->length-1; /** end of sample probably ends **/
return(retval);		/** in the middle of the last pixel **/
}

/***************************************************************/
/** this one returns a value that includes the samples inside **/
/** the pixel 															  	  **/
/***************************************************************/

fullmousetosample(mousex)
int mousex;
{
int retval;

retval=ei->graph_start+(mousex - GRAPH_X+1)*ei->zoom_scale-1;
if (retval >= ei->cur->length)
	retval=ei->cur->length-1; /** end of sample probably ends **/
return(retval);		/** in the middle of the last pixel **/
}

/*****************************************************/

sampletomouse(index)
int index;
{
if (index < ei->graph_start)
	return(GRAPH_X);
if (index >= (ei->graph_start+ei->graph_width*ei->zoom_scale))
	return(GRAPH_X+ei->graph_width-1);
return(sampletomousenoclip(index));
}

/*****************************************************/

sampletomousenoclip(index)
int index;
{
return(GRAPH_X+(index - ei->graph_start)/ei->zoom_scale);
}

/*****************************************************/

sampletobuffer(index)
int index;
{
if (index < ei->graph_start)
	return(0);
if (index > (ei->graph_start+ei->graph_width*ei->zoom_scale))
	return(ei->graph_width-1);
return(sampletobuffernoclip(index));
}

/*****************************************************/

sampletobuffernoclip(index)
int index;
{
return((index - ei->graph_start)/ei->zoom_scale);
}

/*****************************************************/

matchscale(index)
{/** round up! **/
return(((index+ei->zoom_scale-1)/ei->zoom_scale)*ei->zoom_scale);
}

/*****************************************************/

closeto(mousex, mark)
int mousex, mark;
{
int i;

i = mousex - sampletomouse(mark);

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
int middle;

if (ei->flags&EI_REFRESH_HSCROLL) {
	ei->flags &= (~EI_REFRESH_HSCROLL);
	if (ei->flags&EI_CENTER_RANGE) 
		middle = ei->start_mark+(ei->end_mark-ei->start_mark)/2;
	else
		middle = ei->graph_start+(ei->graph_width*ei->zoom_scale)/2;
	ei->flags &= (~EI_CENTER_RANGE);
	ei->zoom_scale = new_zoom;
	ei->graph_start = matchscale(middle-(ei->graph_width*ei->zoom_scale)/2);
	((struct slider *)(hscroll.gad_ptr))->show_size = ei->graph_width*ei->zoom_scale;
	((struct slider *)(hscroll.gad_ptr))->top_element=ei->graph_start;
	new_gadget_size(hscroll.gad_ptr,
		GRAPH_X-2,											/* x1 */
		ei->edit_win->Height - GRAPH_Y2 + 2,		/* y1 */
		ei->edit_win->Width - GRAPH_X + 1,			/* x2 */
		ei->edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H);	/* y2 */
	ei->graph_start = matchscale(((struct slider *)hscroll.gad_ptr)->top_element);
   ei->old_x1 = sampletomousenoclip(matchscale(ei->start_mark));
   ei->old_x2 = sampletomousenoclip(matchscale(ei->end_mark));
	refresh_gadget(hscroll.gad_ptr);
	graph(ei); 
	sprintf(zoombutn_text,"ZM 1:%d",new_zoom);
	if (!(zoombutn.gad_ptr->flags&HIDE_GADGET))
		refresh_gadget(zoombutn.gad_ptr);
	}
else {			/* round new_graph_start to a multiple of zoom_scale **/
	move_graph(ei,new_graph_start);
	}
update_status(ei);
}
		
/************************************************************/

void handle_timer_events()
{
if (GetMsg(timerport)) {
	WaitingForTimer=FALSE;
	if (ei) {
		if (ei->edit_win) {
			if (ei->as) {
				CompLine(ei, ei->line_pos);
				ei->line_pos = max(0,ASPos(ei->as)-5120);	/** Try & adjust of ad1012's internal buffers **/
				CompLine(ei, ei->line_pos);
				start_timer();
				}
			}
		}	
	}
}

/************************************************************/

void start_timer()
{
if (timerport) {
	timermsg->tr_node.io_Command = TR_ADDREQUEST;
	timermsg->tr_time.tv_secs=0;
	timermsg->tr_time.tv_micro=200000;
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

void zoom_func(s)
struct slider *s;
{
new_zoom = s->top_element+1;
if (new_zoom!=ei->zoom_scale) {
	ei->flags |= EI_REFRESH_HSCROLL;
	Signal(tc_edit, update_graph_sigbit);	
	}
if (freehand.gad_ptr->flags&BUTN_HIT) {
	Signal(tc_edit, close_freehand_sigbit);	
	}
}

/************************************************************/

void handle_freehand_events()
{
if (freehand.gad_ptr->flags&BUTN_HIT) {
	xor_button(freehand.gad_ptr);
	CloseFreehand(); 
	}	
}

/************************************************************/

void hscroll_func(s)
struct slider *s;
{
new_graph_start = s->top_element;
if (new_graph_start!=ei->graph_start) {
	Signal(tc_edit, update_graph_sigbit);	
	}
}

/************************************************************/

void nondest_func(b)
struct button *b;
{
SetDrMd(b->bw->RPort, JAM2);
SetAPen(b->bw->RPort,b->c->button.draw_unselected);
SetBPen(b->bw->RPort, b->c->button.hitbox_unselected);	
Move(b->bw->RPort, b->box_x1+19, b->box_y1+8);
if (thisstate->es_flags&ES_DESTRUCTIVE_EDITS) {
	thisstate->es_flags &= (~ES_DESTRUCTIVE_EDITS);
	hide_dest();
	show_nondest();
	refresh_nondest();
	SetAPen(b->bw->RPort, BROWN);	/** USE STUDIO PENS !!!! **/
	RectFill(b->bw->RPort,cut.box_x2+1,cut.box_y1,copy.box_x1-1,copy.box_y2);
	}
else {
	thisstate->es_flags |= ES_DESTRUCTIVE_EDITS;
	Text(b->bw->RPort, "Dest    ",8);
	hide_nondest();
	show_dest();
	refresh_dest();
	}
}

void hide_dest()
{
hide_gadget(dadd.gad_ptr);
hide_gadget(dpaste.gad_ptr);
hide_gadget(dcopy.gad_ptr);
hide_gadget(compress.gad_ptr);
}

void hide_nondest()
{
hide_gadget(keep.gad_ptr);
hide_gadget(delete.gad_ptr);
hide_gadget(paste.gad_ptr);
hide_gadget(copy.gad_ptr);
hide_gadget(cut.gad_ptr);
}

void show_dest()
{
show_gadget(dadd.gad_ptr);
show_gadget(dpaste.gad_ptr);
show_gadget(dcopy.gad_ptr);
show_gadget(compress.gad_ptr);
}

void show_nondest()
{
show_gadget(keep.gad_ptr);
show_gadget(delete.gad_ptr);
show_gadget(paste.gad_ptr);
show_gadget(copy.gad_ptr);
show_gadget(cut.gad_ptr);
}

void refresh_dest()
{
refresh_gadget(dadd.gad_ptr);
refresh_gadget(dpaste.gad_ptr);
refresh_gadget(dcopy.gad_ptr);
refresh_gadget(compress.gad_ptr);
}

void refresh_nondest()
{
refresh_gadget(keep.gad_ptr);
refresh_gadget(delete.gad_ptr);
refresh_gadget(paste.gad_ptr);
refresh_gadget(copy.gad_ptr);
refresh_gadget(cut.gad_ptr);
}

/************************************************************/

void update_status(ei)
struct EditInstance *ei;
{
char str[50];
char buf[50];
short len;
short x,y;
long width;
unsigned long sm, em;
struct TimeCode tc1,tc2,tc3;
struct TimeCode d1,d2,d3;

LockGadLibRender();

/** start & end marks are sometimes swapped for rendering **/
/** convience.  So I'm making sure that the smallest is always **/
/** the start mark, end the larges is always the end mark **/

if (ei->start_mark <= ei->end_mark) {
	sm=ei->start_mark;
	em=ei->end_mark;
	}
else {
	em=ei->start_mark;
	sm=ei->end_mark;
	}

width=ei->graph_width*ei->zoom_scale;
if (ei->graph_start+width > ei->cur->length)
	width=ei->cur->length-ei->graph_start;

if (thisstate->es_flags&ES_SMPTE_UNITS) {
	SampToSmpte(&tc1, sm, TRUE);
	SampToSmpte(&tc2, em, TRUE);
	SampToSmpte(&tc3, em-sm+1, FALSE);

	SampToSmpte(&d1, ei->graph_start,TRUE);
	SampToSmpte(&d2, ei->graph_start+width-1, TRUE);
	SampToSmpte(&d3, width,FALSE);
	}

SetDrMd(ei->edit_win->RPort,JAM2);
SetAPen(ei->edit_win->RPort,StudioBase->defaults.colors.button.draw_unselected);
SetBPen(ei->edit_win->RPort, BROWN);	/** USE STUDIO PENS !!!! **/

/**********************************************************/
/** Build Text for first line - start of range and graph ***/
/**********************************************************/

if (ei->marked_samp==NULL) {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str,"Start --:--:--:-- %02d:%02d:%02d:%02d",
			d1.hours,d1.minutes,d1.seconds,d1.frames);
		}
	else {
		sprintf(str,"Start    ---    %9d",ei->graph_start);
		}
	}
else {	/*** Range Set **/
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str,"Start %02d:%02d:%02d:%02d %02d:%02d:%02d:%02d",
			tc1.hours,tc1.minutes,tc1.seconds,tc1.frames,
			d1.hours,d1.minutes,d1.seconds,d1.frames);
		}
	else {
		sprintf(str,"Start %9d %9d",sm,ei->graph_start);
		}
	}

setmem(buf, sizeof(buf), ' ');
strncpy(buf, str, strlen(str));
x = stop.gad_ptr->box_x2+10;
y = stop.gad_ptr->box_y1+8;
len = min(sizeof(buf), (ei->edit_win->Width-x-4)/8);
Move(ei->edit_win->RPort, x, y);
Text(ei->edit_win->RPort,buf,len);

/**********************************************************/
/** Build Text for second line - end of range and graph ***/
/**********************************************************/

if (ei->marked_samp==NULL) {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str,"End   --:--:--:-- %02d:%02d:%02d:%02d",
			d2.hours,d2.minutes,d2.seconds,d2.frames);
		}
	else {
		sprintf(str,"End      ---    %9d",ei->graph_start+width-1);
		}
	}
else {	/*** Range Set **/
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str,"End   %02d:%02d:%02d:%02d %02d:%02d:%02d:%02d",
			tc2.hours,tc2.minutes,tc2.seconds,tc2.frames,
			d2.hours,d2.minutes,d2.seconds,d2.frames);
		}
	else {
		sprintf(str,"End   %9d %9d",em,ei->graph_start+width-1);
		}
	}

setmem(buf, sizeof(buf), ' ');
strncpy(buf, str, strlen(str));
len = min(sizeof(buf), (ei->edit_win->Width-x-4)/8);
Move(ei->edit_win->RPort, x, y+10);
Text(ei->edit_win->RPort,buf,len);

/**********************************************************/
/** Build Text for third line - length of range and graph ***/
/**********************************************************/

if (ei->marked_samp==NULL) {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str,"Len   --:--:--:-- %02d:%02d:%02d:%02d",
			d3.hours,d3.minutes,d3.seconds,d3.frames);
		}
	else {
		sprintf(str,"Len      ---    %9d",width);
		}
	}
else {	/*** Range Set **/
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(str,"Len   %02d:%02d:%02d:%02d %02d:%02d:%02d:%02d",
			tc3.hours,tc3.minutes,tc3.seconds,tc3.frames,
			d3.hours,d3.minutes,d3.seconds,d3.frames);
		}
	else {
		sprintf(str,"Len   %9d %9d",em-sm+1,width);
		}
	}

setmem(buf, sizeof(buf), ' ');
strncpy(buf, str, strlen(str));
len = min(sizeof(buf), (ei->edit_win->Width-x-4)/8);
Move(ei->edit_win->RPort, x, y+20);
Text(ei->edit_win->RPort,buf,len);

/**********************************************************/
/** Generate Line 4 text ***/
/**********************************************************/

#if 0
sprintf(str,"Sample Len: %d (%dK)",ei->cur->length,ei->cur->length/512);
setmem(buf, sizeof(buf), ' ');
strncpy(buf, str, strlen(str));
len = min(sizeof(buf), (ei->edit_win->Width-x-4)/8);
Move(ei->edit_win->RPort, x, y+20);
Text(ei->edit_win->RPort,buf,len);
#endif

sprintf(str,"File Size: %d (%dK)",ei->cur->org_length,ei->cur->org_length/512);
setmem(buf, sizeof(buf), ' ');
strncpy(buf, str, strlen(str));
len = min(sizeof(buf), (ei->edit_win->Width-x-4)/8);
Move(ei->edit_win->RPort, x, y+30);
Text(ei->edit_win->RPort,buf,len);

sprintf(str,"Playback Rate: %d samp/sec",ei->cur->parms.rate);
setmem(buf, sizeof(buf), ' ');
strncpy(buf, str, strlen(str));
len = min(sizeof(buf), (ei->edit_win->Width-x-12)/8);
Move(ei->edit_win->RPort, x, y+40);
Text(ei->edit_win->RPort,buf,len);

UnLockGadLibRender();
}

/************************************************************/

AttemptReadGraphBuffer(ei)
struct EditInstance *ei;
{
long fp;
long len;
char buf[100];

GraphFilePath(ei->cur->name,buf,sizeof(buf));

fp = Open(buf, MODE_OLDFILE);
if (fp==0)
	return(FALSE);

new_pointer(PTR_BUSY);
len=Seek(fp, 0L, OFFSET_END);
len=Seek(fp, 0L, OFFSET_BEGINNING);
if (len!=ei->org_gb_size) {
	new_pointer(PTR_NORMAL);
	telluser("EDIT:","Regenerating graph buffer file.",0);
	Close(fp);
	return(FALSE);
	}
Read(fp,ei->org_graph_buffer, len);
Close(fp);
new_pointer(PTR_NORMAL);
return(TRUE);
}

/************************************************************/

WriteGraphBuffer(ei)
struct EditInstance *ei;
{
long fp;
char buf[100];

GraphFilePath(ei->cur->name,buf,sizeof(buf));

fp = Open(buf, MODE_NEWFILE);
if (fp==0)
	return(FALSE);

new_pointer(PTR_BUSY);
Write(fp,ei->org_graph_buffer, ei->org_gb_size);
Close(fp);
new_pointer(PTR_NORMAL);
return(TRUE);
}

/************************************************************/

DeleteGraphFile(ei)
struct EditInstance *ei;
{
char buf[100];

GraphFilePath(ei->cur->name,buf,sizeof(buf));

return(DeleteFile(buf));
}

/***********************************************************/

int GraphFilePath(name,buf,size)
char *name;
char *buf;
int size;
{
char temp[40];

sprintf(temp,"%s.graph",name);

if (strlen(temp)+strlen(StudioBase->defaults.temp_files_path) > (size-3)) {
	return(OUT_OF_MEMORY);
	}

strcpy(buf,StudioBase->defaults.temp_files_path);
add(buf,temp);    /** append filename to path **/
return(NO_ERROR);
}

/***********************************************************/

void SampToSmpte(tc, numsamps, useoffset)
struct TimeCode *tc;
int numsamps;
int useoffset;
{
int rate;
static int lot;
int result;
int *p1,*p2;
int zero=0;

if (ei->cur->parms.smpte_sampling_rate==0)
	rate=ei->cur->parms.rate;
else
	rate=(int)ei->cur->parms.smpte_sampling_rate;

lot=(numsamps*StudioBase->defaults.smpte_mode)/rate;
tc->frames=lot % StudioBase->defaults.smpte_mode; lot=lot/StudioBase->defaults.smpte_mode;
tc->seconds=lot % 60; lot=lot/60;
tc->minutes=lot % 60; lot=lot/60;
tc->hours=lot % 24;

p1=(int *)tc;
if (useoffset)
	p2=(int *)(&tc_offset);
else
	p2=(int *)(&zero);

result=AddTimeCode(*p1,*p2,StudioBase->defaults.smpte_mode);
movmem(&result,tc,sizeof(result));
}

/***********************************************************/

int SmpteToSamp(a,b,c,d)
int a,b,c,d;
{
int rate;
int frames;
struct TimeCode tc;
int *p1,*p2;
int result;
int numsamps;

if (ei->cur->parms.smpte_sampling_rate==0)
	rate=ei->cur->parms.rate;
else
	rate=(int)ei->cur->parms.smpte_sampling_rate;

tc.hours=a;
tc.minutes=b;
tc.seconds=c;
tc.frames=d;

p1=(int *)&tc;
p2=(int *)(&tc_offset);
result=SubTimeCode(*p1,*p2,StudioBase->defaults.smpte_mode);
movmem(&result,&tc,sizeof(result));

a=tc.hours;
b=tc.minutes;
c=tc.seconds;
d=tc.frames;
frames=d+StudioBase->defaults.smpte_mode*(c+b*60+a*3600);
numsamps=frames*rate;
return((int)((numsamps+StudioBase->defaults.smpte_mode-1)/StudioBase->defaults.smpte_mode));  /** Round up **/
}

