#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include <devices/timer.h>
#include "trans.data"
#include "trans.h"
#include "clib/macros.h"
/**********************************/
/* Declarations for CBACK */
long _stack = 16000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_TRANSPORT";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
int NumTracks=1;
int NumRecTracks=1;
int plock=NULL;
char tbn[100];
/**********************************/
void RecordStats();
void PlaySamples();
void InformEndPlayback();
void my_new_gadget_size();
void InformStartPlayback();
struct Window *OpenNewWind();
void OpenSampleParmsWindow();
void OpenRecordParmsWindow();
struct ActiveSample *RecSam();
struct ActiveSample *PlaySam();
struct AudioHandler *ItemData();
struct MenuItem *BuildHandlerMenu();
struct AudioHandler *FindAudioHandler();
/**********************************/
struct Task *task,*FindTask();
int RecSize=0;
/**********************************/
extern BOOL SPEXIT;
extern BOOL RPEXIT;
extern struct Window  *rpwind;
extern struct Window  *spwind;
/**********************************/
int GetHandler();
void ForgetHandler();
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
short se_cmd_pause_playback;
short se_info_playback_done;
short se_info_record_done;
short se_info_sample_created;
short se_info_sample_deleted;
short se_cmd_stop_all_playback;
short se_cmd_stop_recording;
short se_info_block_recorded;
short se_info_channel_done;
short se_info_sample_path_changed;
/**********************************/
/**********************************/
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
void handle_gadlibevents();
void WindGadSize();
BOOL FileExists();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod;
struct Window 		*wind;
struct StandardModState *state;
struct AudioHandler 	*hand;
struct MsgPort *gadlibport;
/**********************************/
/**********************************/
short Playing=0;
short Recording=0;
int DiskSpaceAvailable;
/**************************************************************/
struct Region region[17];
struct ActiveSample *as[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct ChanKey *AllocChan();
#define MAX_CHANS	16
/**************************************************************/
/**************************************************************/
long sw_abort_sigbit=-1;
long sw_trigger_sigbit=-1;
long sw_trigger_sigmask;
long sw_end_sigbit=-1;
long sw_end_sigmask;
/**************************************************************/
/**************************************************************/
void TCFunc();
static struct NewSmpteSink NewSMPTEsnk = {0,NULL,TCFunc,NULL};
static struct SmpteSink *SMPTEsnk;
int StartTC=0x00000100;
int EndTC=0x01000000;
/**************************************************************/
/**************************************************************/
void _main(cmd)
char *cmd;
{
int mask;
void StopTIO();
InitModule(cmd);			/** initalize module **/

while (TRUE)
	{
	mask=NULL;
	if (gadlibport) mask|=(1<<gadlibport->mp_SigBit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit) | sw_end_sigmask;
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	if (wind)
		{ 
		if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
		if(mask&sw_end_sigmask && Pflags&SMPTE_STOP)  StopTIO();
		}
	if (gadlibport) if (mask & (1<<gadlibport->mp_SigBit)) handle_gadlibevents();
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
static struct NewModule ModData ={NULL,NULL,sizeof(struct StandardModState)};

task=FindTask(NULL);
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);
if ((mod=(struct Module *)AddModule(&ModData))==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Transport:","Could Not Add Module",0);
	Quit();
	}

state=mod->state;

se_cmd_select_module 		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window		= GetEventID("SE_CMD_HIDE_WINDOW");
se_info_playback_done		= GetEventID("SE_INFO_PLAYBACK_DONE");
se_info_record_done		= GetEventID("SE_INFO_RECORD_DONE");
se_info_sample_created		= GetEventID("SE_INFO_SAMPLE_CREATED");
se_info_sample_deleted		= GetEventID("SE_INFO_SAMPLE_DELETED");
se_cmd_pause_playback		= GetEventID("SE_CMD_PAUSE_PLAYBACK");
se_cmd_stop_all_playback	= GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_cmd_stop_recording		= GetEventID("SE_CMD_STOP_RECORDING");
se_info_block_recorded		= GetEventID("SE_INFO_BLOCK_RECORDED");
se_info_sample_path_changed	= GetEventID("SE_INFO_SAMPLE_PATH_CHANGED");
se_info_channel_done		= GetEventID("SE_INFO_CHANNEL_DONE");
NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);  
NotifyMeOnEvent(se_cmd_kill_module, mod);

if ((sw_abort_sigbit=AllocSignal(-1))==-1) {telluser("Transport:","Cant get SigBit",NULL);Quit();}
if ((sw_trigger_sigbit=AllocSignal(-1))==-1) {telluser("Transport:","Cant get SigBit",NULL);Quit();}
sw_trigger_sigmask=1<<sw_trigger_sigbit;
if ((sw_end_sigbit=AllocSignal(-1))==-1) {telluser("Transport:","Cant get SigBit",NULL);Quit();}
sw_end_sigmask=1<<sw_end_sigbit;
BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) HideWind();
if (mod) DeleteModule(mod);
if (sw_abort_sigbit!=-1) FreeSignal(sw_abort_sigbit);
if (sw_trigger_sigbit!=-1) FreeSignal(sw_trigger_sigbit);
if (sw_end_sigbit!=-1) FreeSignal(sw_end_sigbit);
closelibraries();
exit(0);
}
/**************************************************************/
void DisplayWind()
{
int x;
struct AudioHandler *hnd=NULL;
struct Menu *mnu=NULL;

x=0;
hnd = FindAudioHandler(NULL,-1,0,NULL,-1);
while(hnd)
	{
	x++;
	hnd = FindAudioHandler(hnd,-1,0,NULL,-1);
	}

if (GetHandler()!=-1)
{
if (x>1) 
	{
	Titles[2].FirstItem=BuildHandlerMenu(NULL,NULL);
	CheckData(Titles[2].FirstItem,hand);
	Titles[1].NextMenu=&Titles[2];
	mnu=Titles;
	}
else
	{
	Titles[1].NextMenu=NULL;
	mnu=Titles;
	}

NumTracks=0;
for(x=0;x<hand->number_of_channels;x++)
	{
	if (CK_CHAN_PLAY & hand->channel[x].flags) NumTracks++;
	}
if (NumTracks>MAX_CHANS) NumTracks=MAX_CHANS;

NumRecTracks=0;
for(x=0;x<hand->number_of_channels;x++)
	{
	if (CK_CHAN_RECORD & hand->channel[x].flags) NumRecTracks++;
	}

if (mod->state->flags & MS_STATE_INITILIZED)
	{
	NewWind.LeftEdge=state->win_le;
	NewWind.TopEdge=state->win_te;
/*	NewWind.Width=state->win_w;*/
	}
NewWind.Height=50+(((NumTracks+1)/2)*14)+(NumRecTracks*12);

if ((wind = OpenNewWind(&NewWind,mod->state,DONTMOVEWINDOW,mnu))!=0)  
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);


	Pflags=mod->state->user_data[0];
	if (Pflags&RECORD_OUTPUT) {Items[1].Flags|=CHECKED;Items[0].Flags&=~CHECKED;}
	else {Items[0].Flags|=CHECKED;Items[1].Flags&=~CHECKED;}

	if (Pflags&SMPTE_START) {Items[4].Flags|=CHECKED;Items[3].Flags&=~CHECKED;}
	else {Items[3].Flags|=CHECKED;Items[4].Flags&=~CHECKED;}

	if (Pflags&AUTO_STOP) {Items[8].Flags|=CHECKED;Items[7].Flags&=~CHECKED;Items[6].Flags&=~CHECKED;}
	else if (Pflags&SMPTE_STOP) {Items[7].Flags|=CHECKED;Items[6].Flags&=~CHECKED;Items[8].Flags&=~CHECKED;}
	else {Items[6].Flags|=CHECKED;Items[7].Flags&=~CHECKED;Items[8].Flags&=~CHECKED;}

	if (Pflags&MANUAL_VOLUME) {Items[10].Flags|=CHECKED;Items[11].Flags&=~CHECKED;}
	else {Items[11].Flags|=CHECKED;Items[10].Flags&=~CHECKED;}

	if (Pflags&MANUAL_RATE) {Items[13].Flags|=CHECKED;Items[14].Flags&=~CHECKED;}
	else {Items[14].Flags|=CHECKED;Items[13].Flags&=~CHECKED;}

	ColorGadList(&PlayList[15]);
	ColorGadList(&Rec1But);
	ColorGadList(&Rec2But);

	gadlibport = (struct MsgPort *)CreatePort(0,0);
	PlayBut.notifyme=gadlibport;
	PlayList[0].notifyme=gadlibport;
	PlayList[1].notifyme=gadlibport;
	PlayList[2].notifyme=gadlibport;
	PlayList[3].notifyme=gadlibport;
	PlayList[4].notifyme=gadlibport;
	PlayList[5].notifyme=gadlibport;
	PlayList[6].notifyme=gadlibport;
	PlayList[7].notifyme=gadlibport;
	PlayList[8].notifyme=gadlibport;
	PlayList[9].notifyme=gadlibport;
	PlayList[10].notifyme=gadlibport;
	PlayList[11].notifyme=gadlibport;
	PlayList[12].notifyme=gadlibport;
	PlayList[13].notifyme=gadlibport;
	PlayList[14].notifyme=gadlibport;
	PlayList[15].notifyme=gadlibport;

	SMPTEsnk=(struct SmpteSink *)AddSmpteSink(&NewSMPTEsnk);

	WindGadSize();

 	NotifyMeOnEvent(se_info_sample_created,mod);
	NotifyMeOnEvent(se_info_sample_deleted,mod);
	NotifyMeOnEvent(se_info_record_done, mod);
	NotifyMeOnEvent(se_info_playback_done,mod);
	NotifyMeOnEvent(se_info_block_recorded,mod);
	NotifyMeOnEvent(se_info_sample_path_changed,mod);
	NotifyMeOnEvent(se_info_channel_done,mod);

	}
else telluser("Transport:","Can not open window!",0);
}
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;

if (wind)
	{
	IgnoreEvent(se_info_record_done,mod);
	IgnoreEvent(se_info_playback_done,mod);
	IgnoreEvent(se_info_sample_created,mod);
	IgnoreEvent(se_info_sample_deleted,mod);
	IgnoreEvent(se_info_block_recorded, mod);
	IgnoreEvent(se_info_sample_path_changed,mod);
	IgnoreEvent(se_info_channel_done,mod);

	DeleteSmpteSink(SMPTEsnk);

	DeletePort(gadlibport);	gadlibport=0;

	ForgetHandler();

	SPEXIT=TRUE;
	RPEXIT=TRUE;

	if (rpwind) ActivateWindow(rpwind);
	if (spwind) ActivateWindow(spwind);

	while (plock) WaitTOF(); /* This is for the task Transport Launces */

	if (Titles[2].FirstItem) FreeHandlerMenu(Titles[2].FirstItem);

	RemoveGList(wind,&FileNameGad,1);
	RemoveGList(wind,&FileNameGad2,1);

	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/**************************************************************/
/**************************************************************/
void my_new_gadget_size(ngh,x1,y1,x2,y2)
struct NewGadgetHeader *ngh;
short x1,y1,x2,y2;
{
ngh->box_x1=x1;
ngh->box_x2=x2;
ngh->box_y1=y1;
ngh->box_y2=y2;
/*if (ngh->gad_ptr)
	{
	new_gadget_size(ngh->gad_ptr,x1,y1,x2,y2);
	refresh_gadget(ngh->gad_ptr);
	}*/
}
/**************************************************************/
/**************************************************************/
void WindGadSize()
{
int w,h,l,x;
char str[50];

if (wind)
	{
	RefreshWind(wind);
	delete_gadget_chain(wind);
	w=wind->Width;
	h=wind->Height;
/*
*/
/*	for(x=1;x<9999999999;x++)
*/	draw_indent(wind->RPort,  8,15,wind->Width-9,wind->Height-5,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
/*	while (x!=0) 
*/	draw_indent(wind->RPort,10,16,w-11,18+(((NumTracks+1)/2)*14),StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
/*	printf("U Jerk!");  
*/	draw_indent(wind->RPort,10,19+(((NumTracks+1)/2)*14),w-11,h-21,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
/*	goto hell;
*/	draw_indent(wind->RPort,10,h-20,w-11,h-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
/*
*/
/************/
for(x=0;x<NumTracks;x++)
	{
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
	if (x >= (NumTracks>>1))
		{
		if(x<9) sprintf(str,"#%d",x+1);
		else sprintf(str,"%d",x+1);
		Move(wind->RPort,w/2+1,26+14*(x-(NumTracks>>1)));Text(wind->RPort,str,2);
		my_new_gadget_size(&PlayList[x],w/2+18,18+14*(x-(NumTracks>>1)),w-15,30+14*(x-(NumTracks>>1)));
		}
	else
		{
		if(x<9) sprintf(str,"#%d",x+1);
		else sprintf(str,"%d",x+1);
		Move(wind->RPort,14,26+14*x);Text(wind->RPort,str,2);
		my_new_gadget_size(&PlayList[x],16+15,18+14*x,w/2-2,30+14*x);
		}
	}
/************/

	RemoveGList(wind,&FileNameGad,1);
	RemoveGList(wind,&FileNameGad2,1);

	if (NumRecTracks>0)
		{
		if (NumRecTracks==1) 
			{
			FileNameGad.TopEdge=h-39;
			strcpy(RecFileName,"Untitled");
			Rec1But.box_y1=h-41;
			Rec1But.box_y2=h-41+10;
			}
		else
			{
			FileNameGad.TopEdge=h-51;
			strcpy(RecFileName,"Untitled_L");
			Rec1But.box_y1=h-53;
			Rec1But.box_y2=h-53+10;
			}
		FileNameGad.Width=w-79+24;
		AddGList(wind,&FileNameGad,NULL,1,NULL);
		RefreshGList(&FileNameGad,wind,NULL,1);
		draw_indent(wind->RPort,FileNameGad.LeftEdge-2,FileNameGad.TopEdge-2,FileNameGad.LeftEdge+FileNameGad.Width,FileNameGad.TopEdge+8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
		SetAPen(wind->RPort,0);
		SetBPen(wind->RPort,0);
		Move(wind->RPort,FileNameGad.LeftEdge,FileNameGad.TopEdge-1);
		Draw(wind->RPort,FileNameGad.LeftEdge+FileNameGad.Width-2,FileNameGad.TopEdge-1);

		create_gadget(wind,&Rec1But);
		}
	if (NumRecTracks>1)
		{
		strcpy(RecFileName2,"Untitled_R");
		FileNameGad2.TopEdge=h-39;
		FileNameGad2.Width=w-79+24;
		AddGList(wind,&FileNameGad2,NULL,1,NULL);
		RefreshGList(&FileNameGad2,wind,NULL,1);
		draw_indent(wind->RPort,FileNameGad2.LeftEdge-2,FileNameGad2.TopEdge-2,FileNameGad2.LeftEdge+FileNameGad2.Width,FileNameGad2.TopEdge+8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
		SetAPen(wind->RPort,0);
		SetBPen(wind->RPort,0);
		Move(wind->RPort,FileNameGad2.LeftEdge,FileNameGad2.TopEdge-1);
		Draw(wind->RPort,FileNameGad2.LeftEdge+FileNameGad2.Width-2,FileNameGad2.TopEdge-1);
		Rec2But.box_y1=h-41;
		Rec2But.box_y2=h-41+10;
		create_gadget(wind,&Rec2But);
		}

/************/
	l=(w-24)/4;

	my_new_gadget_size(&StopBut,12+l*3,h-19,12+l*4-1,h-7);
	my_new_gadget_size(&PauseBut,12+l*2,h-19,12+l*3-1,h-7);
	my_new_gadget_size(&PlayBut,12+l*1,h-19,12+l*2-1,h-7);
	my_new_gadget_size(&RecBut,12+l*0,h-19,12+l*1-1,h-7);
/************/
	if (create_gadget_chain(wind,&PlayList[NumTracks-1])==0)  {telluser("Transport:","Could not create gadgets!",0); Quit();}
	hide_gadget(PauseBut.gad_ptr);

	DiskSpaceAvailable=SampleSpaceAvail(StudioBase->defaults.samp_paths);
	RecordStats();

	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void Check4End()
{
int x;
if (Recording<0) telluser("Transport Bug!","Record Counter Negitive",NULL);
if (Playing<0) telluser("Transport Bug!","Play Counter Negitive",NULL);
if (Recording) if (Playing==Recording) if (RecBut.gad_ptr->flags&BUTN_HIT)
	{
	if(Pflags&AUTO_STOP)
		{
		WaitTOF();WaitTOF();WaitTOF();
		WaitTOF();WaitTOF();WaitTOF();
		WaitTOF();WaitTOF();WaitTOF();
		WaitTOF();WaitTOF();WaitTOF();
		for(x=0;x<MAX_CHANS;x++)
			{
			if (as[x]) 
				{
				if (as[x]->key->flags&CK_CHAN_RECORD)
					{
					ASStop(as[x]);
					}
				}
			}
		RecSize=0;
		}
	}

if (Playing==0)
	{
	StopBut.gad_ptr->flags|=BUTN_HIT;
	refresh_gadget(StopBut.gad_ptr);

	if (PauseBut.gad_ptr->flags&BUTN_HIT) BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,0,0,0,0);
	PauseBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(PauseBut.gad_ptr);
	hide_gadget(PauseBut.gad_ptr);

	show_gadget(PlayBut.gad_ptr);
	PlayBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(PlayBut.gad_ptr);

	show_gadget(RecBut.gad_ptr);
	DiskSpaceAvailable=SampleSpaceAvail(StudioBase->defaults.samp_paths);
	RecordStats();
	if (Titles[1].NextMenu) Titles[1].NextMenu->Flags|=MENUENABLED;
	}
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;
int x;


while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_info_channel_done)
		{
		for(x=0;x<MAX_CHANS;x++)
			{
			if (as[x])
				{
				if (as[x]->key==(struct ChanKey *)event->arg2)
					{
					if (Pflags&SMPTE_START) as[x]->region.parms.starttimecode=StartTC;
					ASClose(as[x]);
					if (as[x]->key->flags&CK_CHAN_RECORD)
						{
						if (as[x]->region.parms.filter3db==-2)
							{
							if (DeleteSample(as[x]->region.samp_name)!=NO_ERROR)
								telluser("TRANSPORT:","Sample in use! Can't Delete it",0);
							}
						FreeChan(as[x]->key);
						as[x]=NULL;
						Playing--;
						Recording--;
						}
					else
						{
						FreeChan(as[x]->key);
						as[x]=NULL;
						Playing--;
						break;
						}
					}
				}
			}
		}
	/*********************************/
	if(event->type == se_info_record_done)
		{
/*			for(x=0;x<MAX_CHANS;x++)
				if (as[x]) 
					if (as[x]->key==(struct ChanKey *)event->arg2)
						{
						if (as[x]->region.parms.filter3db==-2)
							{
							if (DeleteSample(as[x]->region.samp_name)!=NO_ERROR)
								telluser("TRANSPORT:","Sample in use! Can't Delete it",0);
							}
						FreeChan(as[x]->key);
						as[x]=NULL;
						Playing--;
						Recording--;
						}*/
			Check4End();
		}
	/*********************************/
	if(event->type == se_info_playback_done)
		{
/*			for(x=0;x<MAX_CHANS;x++)
				{
				if (as[x]) 
					{
					if (as[x]->key==(struct ChanKey *)event->arg2)
						{
						FreeChan(as[x]->key);
						as[x]=NULL;
						Playing--;
						break;
						}
					}
				}*/
			Check4End();
		}
	/*********************************/
	if(event->type == se_info_sample_deleted)
		{
		DiskSpaceAvailable=SampleSpaceAvail(StudioBase->defaults.samp_paths);
		RecordStats();
		}
	/*********************************/
	if(event->type == se_info_sample_created)
		{
		DiskSpaceAvailable=SampleSpaceAvail(StudioBase->defaults.samp_paths);
		RecordStats();
		}
	/*********************************/
	if(event->type == se_info_sample_path_changed)
		{
		DiskSpaceAvailable=SampleSpaceAvail(StudioBase->defaults.samp_paths);
		RecordStats();
		}

	/*********************************/
	if (event->type == se_info_block_recorded)
		{
		RecSize=0;
		for(x=0;x<MAX_CHANS;x++)
			{
			if(as[x]) if(as[x]->key->flags&CK_CHAN_RECORD) RecSize+=((ASPos(as[x])*2)/1024);
			}

		if (Recording)
		if(RecSize>DiskSpaceAvailable)
			{
			for(x=0;x<MAX_CHANS;x++)
				{
				if(as[x]) if(as[x]->key->flags&CK_CHAN_RECORD)
					{
					ASStop(as[x]);
					}
				RecSize=0;
				}
			RecordStats();
			telluser("Record:","Disk Full!",NOTICE_RETURN_NOW);
			}
		else RecordStats();
		}
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
struct Screen *s=StudioBase->screen;
struct IntuiMessage *message;
ULONG class;
ULONG code;
int x,t;
struct MenuItem *FindCheckedItem(),*mi;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	code=message->Code;
	ReplyMsg(message);

	if(class== MENUPICK)
		{
		if (Titles[1].NextMenu)
			{
			if (hand!=ItemData(FindCheckedItem(Titles[2].FirstItem)))
				{
				hand=ItemData(FindCheckedItem(Titles[2].FirstItem));
				state->hand=hand;
				sprintf(tbn,"Transport - %s",hand->name);
				WindTitleChange(wind,tbn);

				NumTracks=0;
				for(x=0;x<hand->number_of_channels;x++)
					{
					if (CK_CHAN_PLAY & hand->channel[x].flags) NumTracks++;
					}
				if (NumTracks>MAX_CHANS) NumTracks=MAX_CHANS;

				NumRecTracks=0;
				for(x=0;x<hand->number_of_channels;x++)
					{
					if (CK_CHAN_RECORD & hand->channel[x].flags) NumRecTracks++;
					}

				NewWind.Height=50+(((NumTracks+1)/2)*14)+(NumRecTracks*12);

				if (s->Height < (wind->TopEdge+NewWind.Height))
					MoveWindow(wind,0,s->Height - (NewWind.Height+wind->TopEdge));

				SizeWindow(wind,0,NewWind.Height-wind->Height);
				}
			}
		if (MENUNUM(code)==FIRST_MENU+1)
			{
			x=ITEMNUM(code);
			if (x==0) for(x=0;x<MAX_CHANS;x++) 
				{
				if (PlayList[x].gad_ptr) 
					{
					set_box_text(PlayList[x].gad_ptr,NULL);
					refresh_gadget(PlayList[x].gad_ptr);
					}
				}
			if (x==1) OpenSampleParmsWindow();
			if (x==2) OpenRecordParmsWindow();
			}
/*		if (MENUNUM(code)==FIRST_MENU)*/
			{
			Pflags=0;
			t=0;
			mi=Titles[0].FirstItem;
			while (mi)
				{
				if (mi->Flags&CHECKED)
					{
					if (t==1) Pflags|=RECORD_OUTPUT;
					if (t==4) Pflags|=SMPTE_START;
					if (t==7) Pflags|=SMPTE_STOP;
					if (t==8) Pflags|=AUTO_STOP;
					if (t==10) Pflags|=MANUAL_VOLUME;
					if (t==13) Pflags|=MANUAL_RATE;
					}
				t++;
				mi=mi->NextItem;
				}
			}
/*		kprintf("Pflags:%04x%04x\n",Pflags);*/
		mod->state->user_data[0]=Pflags;
		}
	if (class == NEWSIZE)
		WindGadSize();

	if (class == CLOSEWINDOW)
		{
		if (Playing) 
			{
			telluser("Transport:","Can not close window until sounds stop playing",NOTICE_RETURN_NOW);
			((struct WindowParms *)(wind->UserData))->Flags2 &= ~NOMESSAGE;
			}
		else {HideWind();return;}
		}
	}
}
/**************************************************************/
/**************************************************************/
void handle_gadlibevents()
{
struct GadMsg *msg;
struct GadgetHeader *g;
ULONG flags;

while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) 
	{
	g = msg->g;
	flags=msg->flags;

	if (g==PlayBut.gad_ptr) 
		{
		if (flags & BUTN_HIT) PlaySamples();
		}
	if (msg->type==GADMSG_DRAG_AND_DROP)
		{
		set_box_text(msg->g,msg->text);
		refresh_gadget(msg->g);
		}
	ReplyMsg(msg);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/* \  /								 \  / */
/*  XX				Playback			  XX  */
/* /  \								 /  \ */
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void StopTIO()
{
int x;

if (Playing)
	{
	BroadcastEventParms(se_cmd_pause_playback, hand, TRUE,0,0,0,0);
	for(x=0;x<NumTracks;x++) if (as[x]) ASStop(as[x]);
	RecSize=0;
	BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,FLUSH_PLAY_BUFFERS,0,0,0);
	}
}
/**************************************************************/
/**************************************************************/
void STOPfunc(b)
struct button *b;
{
int x;
if (b->flags&BUTN_HIT)
	{
	if (Playing)
		{
		BroadcastEventParms(se_cmd_pause_playback, hand, TRUE,0,0,0,0);
		for(x=0;x<MAX_CHANS;x++) if (as[x]) ASStop(as[x]);
		BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,FLUSH_PLAY_BUFFERS,0,0,0);
		}
	RecSize=0;
	}
else if (!(PlayBut.gad_ptr->flags & BUTN_HIT))
	{
	StopBut.gad_ptr->flags|=BUTN_HIT;
	refresh_gadget(StopBut.gad_ptr);
	}
}
/*****************************************************************/
/*****************************************************************/
void PAUSEfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT) BroadcastEventParms(se_cmd_pause_playback, hand, TRUE,0,0,0,0);

else BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,0,0,0,0);
}
/*****************************************************************/
/*****************************************************************/
void RECfunc(b)
struct button *b;
{
int x;


if (b->flags&BUTN_HIT) 
	{
	if (!stricmp(hand->name,&"AD1012"))
		{
		hide_gadget(PlayList[3].gad_ptr);
		set_box_text(PlayList[3].gad_ptr,NULL);
		refresh_gadget(PlayList[3].gad_ptr);

		SetDrMd(wind->RPort,JAM1);
		SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
		RectFill(wind->RPort, PlayList[3].box_x1,PlayList[3].box_y1,PlayList[3].box_x2,PlayList[3].box_y2);
		}
	}
else
	{
	for(x=0;x<MAX_CHANS;x++)
		{
		if (Playing) if(as[x]) if(as[x]->key->flags&CK_CHAN_RECORD) ASStop(as[x]);
		}
	if (!stricmp(hand->name,&"AD1012"))
		{
		show_gadget(PlayList[3].gad_ptr);
		refresh_gadget(PlayList[3].gad_ptr);
		}
	RecSize=0;
	}
}
/*****************************************************************/
/*****************************************************************/
int ANrate;
/**************************************************************/
/**************************************************************/
void PlaySamples()
{
char  *samp;
int x,y;
void OpenSmpteWaitWindow();
void CloseSmpteWaitWindow();
long mask=0;

GlobSampRate=hand->current_sampling_rate;
LastPlayRate=hand->current_sampling_rate;
ANrate=0;

for(x=0;x<NumTracks;x++) {if (as[x]!=NULL) {telluser("Transport:","Channels Active, Aborting",NULL);return;}}

new_pointer(PTR_BUSY);
x=0;
for(y=0;y<NumTracks;y++)
	{
	samp=((struct box *)(PlayList[y].gad_ptr))->text;
	if (samp)
		{
		as[x]=PlaySam(samp,x,y);
		if (as[x]) 
			{
			x++;
			Playing++;
			}
		}
	}

if (RecBut.gad_ptr->flags&BUTN_HIT)
	{
	if(Rec1But.gad_ptr->flags&BUTN_HIT)
		{
		if (NumRecTracks==1) as[x]=RecSam(RecFileName,x,NULL);
		else as[x]=RecSam(RecFileName,x,CK_CHAN_LEFT);
		if (as[x]) {x++;Playing++;Recording++;}
		}
	if (NumRecTracks>1)
		if(Rec2But.gad_ptr->flags&BUTN_HIT)
			{
			as[x]=RecSam(RecFileName2,x,CK_CHAN_RIGHT);
			if (as[x]) {x++;Playing++;Recording++;}
			}
	}

if (x)
	{
	if (Titles[1].NextMenu) Titles[1].NextMenu->Flags&=~MENUENABLED;
	if (!(Pflags&MANUAL_RATE) && ANrate)
		{
		BroadcastEventParmsLessOne(GetEventID("SE_CMD_SET_SAMPLING_RATE"),mod,hand,ANrate,0, 0, 0,0);
		}

	BroadcastEventParms(se_cmd_pause_playback, hand, TRUE,0,0,0,0);
	ASTriggerList(as,TRIGGER_LIST_MANUAL_SYNC);

	new_pointer(PTR_NORMAL);

	if (Pflags&SMPTE_START)
		{
		OpenSmpteWaitWindow(StartTC,EndTC);
		SetSignal(0,(1<<sw_abort_sigbit)|(1<<sw_trigger_sigbit));
		mask=Wait((1<<sw_abort_sigbit)|(1<<sw_trigger_sigbit));

		if (mask&(1<<sw_abort_sigbit)) 
			{
			for(x=0;x<NumTracks;x++) 
				if (as[x]) 
					{
					if(!(as[x]->key->flags&CK_CHAN_RECORD))
						ASStop(as[x]);
					}
			}
		}

	BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,0,0,0,0);

	if (Pflags&SMPTE_START) CloseSmpteWaitWindow();

	if (mask&(1<<sw_abort_sigbit)) 
		{
		for(x=0;x<NumTracks;x++) 
			if (as[x]) 
				{
				if(as[x]->key->flags&CK_CHAN_RECORD)
					{
/*					strcpy(buld,as[x]->region.samp_name);*/
					ASStop(as[x]);
					as[x]->region.parms.filter3db=-2;
					}
				}
		RecSize=0;
		}

	show_gadget(PauseBut.gad_ptr);
	hide_gadget(PlayBut.gad_ptr);
	StopBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(StopBut.gad_ptr);

	if (!(as[Playing-1]->key->flags&CK_CHAN_RECORD))
		{
		RecBut.gad_ptr->flags&=~BUTN_HIT;
		refresh_gadget(RecBut.gad_ptr);
		hide_gadget(RecBut.gad_ptr);
		}
	}
else
	{
	new_pointer(PTR_NORMAL);
	PlayBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(PlayBut.gad_ptr);

	StopBut.gad_ptr->flags|=BUTN_HIT;
	refresh_gadget(StopBut.gad_ptr);

	PauseBut.gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(PauseBut.gad_ptr);
	}

}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ActiveSample *PlaySam(samp,x,num)
char *samp;
int x;
{
int ASFLAG=0;
static char str[STUDIO16_MAX_PATH_SIZE+30];

if (samp)
	{
	if (FindRegion(samp,&region[x]) != NO_ERROR)
		{
		sprintf(str,"Can't find sample/region '%s'",samp);
		telluser("Transport:",str,NOTICE_RETURN_NOW);
		return(NULL);
		}

	if (Pflags&MANUAL_RATE) region[x].parms.rate=GlobSampRate;
	else
		{
		if (!ANrate) ANrate=region[x].parms.rate;
		else region[x].parms.rate=ANrate;
		}
	if (Pflags&MANUAL_VOLUME) ASFLAG=AS_IGNORE_DEFAULT_VOLUME;

	if ((as[x]=(struct ActiveSample *)ASOpen(&region[x], AS_PLAY|AS_AUTO_DATA/*|AS_AUTO_FREECHAN*/|ASFLAG))!=0)
		{
		as[x]->key=(struct ChanKey *)AllocChan(hand,num,CK_CHAN_PLAY);
		if (as[x]->key==NULL)
			{
			ASClose(as[x]);
			sprintf(str,"Can't Allocate play channel #%d",x);
			telluser("Transport:",str,NOTICE_RETURN_NOW);
			return(NULL);
			}
		}
	else 
		{
		sprintf(str,"Can't open disk sample '%s'",region[x].samp_name);
		telluser("Transport:",str,NOTICE_RETURN_NOW);
		return(NULL);
		}
	}
return(as[x]);
}
/*****************************************************************/
char sn[200];
/*****************************************************************/
struct ActiveSample *RecSam(name,x,flags)
char *name;
int x,flags;
{
int y=1;
static char str[STUDIO16_MAX_PATH_SIZE+40];

if (DiskSpaceAvailable<1) 
	{
	telluser("Transport:","Not enough disk space available to record");
	return(NULL);
	}

name=(char *)StrFileName(name);
sprintf(sn,"%s",name);
while(FindSample(sn,-1))
	{
	y++;
	sprintf(sn,"%s_#%d",name,y);
	}

strcpy(region[x].samp_name,StudioBase->defaults.samp_paths);
add(region[x].samp_name,sn);
region[x].start=0;
region[x].end=0;
region[x].user_data=NULL;
region[x].parms.filter3db=hand->current_filter;
region[x].parms.volume=3200;
region[x].parms.pan=ZERO_DB;
region[x].parms.flags=0;
if (flags&CK_CHAN_LEFT) region[x].parms.pan=0;
if (flags&CK_CHAN_RIGHT) region[x].parms.pan=200<<5;
if (Pflags&MANUAL_RATE) region[x].parms.rate=GlobSampRate;
else region[x].parms.rate=LastPlayRate;

if ((as[x]=(struct ActiveSample *)ASOpen(&region[x], AS_RECORD|AS_AUTO_DATA))!=0)
	{
	if (Pflags&RECORD_OUTPUT)
		{as[x]->region.parms.flags|=SI_TAP_OUTPUT;as[x]->key=(struct ChanKey *)AllocChan(hand,-1,CK_CHAN_RECORD|CK_TAP_OUTPUT|flags);}
	else 	{as[x]->region.parms.flags|=SI_TAP_INPUT;as[x]->key=(struct ChanKey *)AllocChan(hand,-1,CK_CHAN_RECORD|CK_TAP_INPUT|flags);}
	if (as[x]->key==0)
		{
		ASClose(as[x]);
		telluser("Transport:","Can't Allocate a record channel!",NOTICE_RETURN_NOW);
		DeleteSample(region[x].samp_name);
		return(NULL);
		}
	}
else {sprintf(str,"Can't open record sample '%s'",region[x].samp_name);telluser("Transport:",str,NOTICE_RETURN_NOW);return(NULL);}


return(as[x]);
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
void ForgetHandler()
{
hand=NULL;
}
/*****************************************************************/
/*****************************************************************/
int GetHandler()
{

if (state->hand==NULL) 
	{
	hand=FindAudioHandler(NULL,-1,0,NULL,-1);
	if (hand)
		{
		state->hand=hand;
		state->handler_name=hand->name;
		state->handler_class_id =hand->class_id;
		}
	else {telluser("Transport:","Could not find a Handler.",NULL);return(-1);}
	}
else	hand=state->hand;
return(NULL);
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
void RecordStats()
{
int q,w,e,r;
char str[80];

	{
	e=(DiskSpaceAvailable-RecSize)/1024;
	r=(((DiskSpaceAvailable-RecSize)-(e*1024))*1000)/1024;
	q=(RecSize)/1024;
	w=((RecSize-(q*1024))*1000)/1024;

	if (e<0 || r<0) {e=0;r=0;}

	sprintf(str,"Size:%3d.%03dM    Free:%4d.%03dM",q,w,e,r);
	LockGadLibRender();
	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	SetDrMd(wind->RPort,JAM2);
	Move(wind->RPort,15,wind->Height-23);
	Text(wind->RPort, str,31);
	UnLockGadLibRender();
	}
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/************************************************************/

void add_dir(l,path)
struct list *l;
char *path;
{
struct FileLock      *fkey;
static struct FileInfoBlock XXX,*dinfo=&XXX;
short len;

if (! (fkey=(struct FileLock *)Lock(path,ACCESS_READ)) ) 
	{
	return;
	}

if ( ! Examine(fkey, dinfo) ) 
	{
	UnLock(fkey);
	return;
	}

if ( dinfo->fib_DirEntryType < 0L ) 
	{
	UnLock(fkey);
	return;
	}

while( ExNext( fkey, dinfo ) )
	{
	if (dinfo->fib_EntryType<0) 
		{
		len=strlen(dinfo->fib_FileName);
		if (len < 6) 
			{
			add_list_entry(l, dinfo->fib_FileName, NULL);
			}
		else
			{
			if (stricmp(&dinfo->fib_FileName[len-6],".graph"))
				add_list_entry(l, dinfo->fib_FileName, NULL);
			}
		}
	}

UnLock(fkey);
}

/***********************************************************/
BOOL FileExists(name)
char *name;
{
long lock;
char file[200];
int x;

strcpy(file,name);
x=strlen(file);

while (x>0 && file[x]!=REGION_DELIMITER_CHAR && file[x]!=':' && file[x]!='/') x--;
if (file[x]==REGION_DELIMITER_CHAR) file[x]=0;

if (lock=Lock(file,ACCESS_READ)) 
	{
	UnLock(lock);
	return(TRUE);
	}
return(FALSE);
}
/***********************************************************/
/***********************************************************/
