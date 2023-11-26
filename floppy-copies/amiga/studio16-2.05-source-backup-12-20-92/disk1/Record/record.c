#include <exec/types.h>
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "/include/clr.h"
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include "Record.data"
/**********************************/
/*ADD WARNIG IF INPUT VOULEM IS NOT ZERO DBS OR LESS*/
/**********************************/
#define Clock 10000000
/**********************************/
/**********************************/
long _stack = 8000;
char *_procname = "STUDIO_RECORDER";
long _priority = 0;
long _BackGroundIO = 0;
/**********************************/
/**********************************/
struct ChanKey *AllocChan();
struct ChanKey *mon_key=NULL;
struct ChanKey *RecChan=NULL;
/**********************************/
char tbn[100];
/**********************************/
void ColorGadList();
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
int istc=0;
struct TimeCode *stc=(struct TimeCode *)&istc;
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_cmd_set_input_gain;
short se_cmd_set_cutoff_freq;
short se_cmd_set_sampling_rate;
short se_cmd_set_channel_volume;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_presave_sample;
short se_info_presave_done;
short se_cmd_stop_recording;
short se_info_channel_done;
short se_info_record_done;
short se_info_block_recorded;
/**********************************/
int GadRef();
/**********************************/
char TempSampName[120] =  {"Untitled",0};
int TempSize=0;
int GlobSampRate=44100;
int GlobFilterCut=20000;
int DiskSpaceAvailable;
static UBYTE	AutoFilt=0;
/**********************************/
int Recording=0;
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
static void WinGadSize();
/**********************************/
/**********************************/
void NotifyRecording();
void NotifyEndRecording();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod;
struct Window 		*wind;
struct RecordModState *state;
struct AudioHandler 	*hand;
struct Window *Rwind;
/**********************************/
/**********************************/
struct Task *task,*FindTask();
struct MsgPort *gadlibport=0;
char sn[200];
/**********************************/
int NUMGADS=5;
/**********************************/
void RecordStats();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;
InitModule(argv);

while (TRUE)
	{
	mask=0;
	if (gadlibport) mask|=(1<<gadlibport->mp_SigBit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask); 
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	if (gadlibport) if (mask & (1<<gadlibport->mp_SigBit)) handle_gadlibevents();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct Region region[16];
struct ActiveSample *as[16]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
struct ChanKey *RecKey[16]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_gadlibevents()
{
void RecSam();
struct GadMsg *msg;
struct GadgetHeader *g;
ULONG flags;
void PlaySamples();
int x;

while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) 
	{
	g = msg->g;
	flags=msg->flags;
	ReplyMsg(msg);

	if (g==RECORDbutton.gad_ptr) 
		{
		if (flags & BUTN_HIT) 
			{
			hide_gadget(RECORDbutton.gad_ptr);
			RecSam(sn);
			}
/*		else
			{
			x=0;
			while(as[x]) {ASStop(as[x]);as[x++]=NULL;}
			x=0;
			while(RecKey[x]) {FreeChan(RecKey[x]);RecKey[x++]=NULL;}
			NotifyEndRecording();
			}*/
		}
	if (g==STOPbutton.gad_ptr) 
		{
			x=0;
			while(as[x]) {ASStop(as[x]);as[x++]=NULL;}
			x=0;
			while(RecKey[x]) {FreeChan(RecKey[x]);RecKey[x++]=NULL;}
			NotifyEndRecording();
			RECORDbutton.gad_ptr->flags&=~BUTN_HIT;
			show_gadget(RECORDbutton.gad_ptr);
			refresh_gadget(RECORDbutton.gad_ptr);
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
char fn[100];
void RecSam(file)
char *file;
{
char *FindSample();
int y=0,x,flg=0,compath;
struct MenuItem *FindCheckedItem(),*mi;
struct ChanKey *ItemData(),*key,*AllocChanByKey();
char *StrFileName();
char *name;
char path[200];
long fp;
struct FileLock      *fkey;

name=StrFileName(file);
compath=strlen(file)-strlen(name);

if (compath) {strcpy(path,file);path[((int)name)-((int)file)]=NULL;}
else strcpy(path,StudioBase->defaults.samp_paths);
if (! (fkey=(struct FileLock *)Lock(path,ACCESS_READ)) ) 
	{
	telluser("Record:","Does the record path exist?",0);
	RECORDbutton.gad_ptr->flags&=~BUTN_HIT;
	show_gadget(RECORDbutton.gad_ptr);
	refresh_gadget(RECORDbutton.gad_ptr);
	return;
	}
UnLock(fkey);

DiskSpaceAvailable=SampleSpaceAvail(path);

if (DiskSpaceAvailable<1) 
	{
	telluser("Record:","Not enough disk space available");
	RECORDbutton.gad_ptr->flags&=~BUTN_HIT;
	show_gadget(RECORDbutton.gad_ptr);
	refresh_gadget(RECORDbutton.gad_ptr);
	return;
	}

StrPathFileCat(path,name);

x=0;
if (Titles[0].FirstItem) mi=FindCheckedItem(Titles[0].FirstItem);
else mi=(struct MenuItem *)RecChan;

while(mi && x<16)
	{
	if (Titles[0].FirstItem) key=AllocChanByKey(ItemData(mi));
	else {key=AllocChanByKey(RecChan);mi=NULL;}
	if (key)
		{
		if (key->flags&CK_CHAN_LEFT) sprintf(fn,"%s_L",path);
		else if (key->flags&CK_CHAN_RIGHT) sprintf(fn,"%s_R",path);
		else sprintf(fn,"%s",path);

		y=1;
		while((fp=Open(fn,MODE_OLDFILE)))
			{
			Close(fp);
			y++;
			if (key->flags&CK_CHAN_LEFT) sprintf(fn,"%s_#%d_L",path,y);
			else if (key->flags&CK_CHAN_RIGHT) sprintf(fn,"%s_#%d_R",path,y);
			else sprintf(fn,"%s_#%d",path,y);
			}

		strcpy(region[x].samp_name,fn);
		region[x].region_name[0]=0;
		region[x].start=0;
		region[x].end=0;
		region[x].user_data=NULL;
		region[x].parms.rate=key->handler->current_sampling_rate;
		region[x].parms.filter3db=key->handler->current_filter;
		region[x].parms.volume=3200;
		if (key->flags&CK_CHAN_LEFT) region[x].parms.pan=(0<<5);
		else if (key->flags&CK_CHAN_RIGHT) region[x].parms.pan=(200<<5);
		else region[x].parms.pan=(100<<5);

		if ((as[x]=(struct ActiveSample *)ASOpen(&region[x], AS_RECORD|AS_AUTO_CLOSE|AS_AUTO_DATA))!=NULL)
			{
			as[x]->key= key;
			RecKey[x]=as[x]->key;
			x++;
			}
		else 
			{
			telluser("Record:","Can not open one record sample, check the path",NOTICE_RETURN_NOW);
			if(key) FreeChan(key);
			flg++;
			}
		}
	else 
		{
		telluser("Record:","Could not get one ChanKey");
		flg++;
		}

	if (mi)
		{
		mi=mi->NextItem;
		mi=FindCheckedItem(mi);
		}
	}

if (x==0)
	{
	if (flg==0) telluser("Record:","Nothing to Record. Select a Record Channel!",NULL);
	RECORDbutton.gad_ptr->flags&=~BUTN_HIT;
	show_gadget(RECORDbutton.gad_ptr);
	refresh_gadget(RECORDbutton.gad_ptr);
	return;
	}

Recording=x;

BroadcastEventParms(GetEventID("SE_CMD_TRIGGER_RECORD"),as[0],as[1],0,0,0,0);
/*ASTriggerList(as);*/
NotifyRecording(x);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule(name)
char *name;
{
char *FindFileName();
struct AudioHandler *FindAudioHandler();
static struct NewModule ModData ={NULL,0,sizeof(struct RecordModState)};

task=FindTask(NULL);

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Record:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

state=(struct RecordModState *)mod->state;
if (!(state->sms.flags & MS_STATE_INITILIZED)) 
	{
	state->menucksum=0;
	state->itemnum[0]=1;
	state->itemnum[1]=-1;
	}

se_cmd_select_module		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_set_input_gain		= GetEventID("SE_CMD_SET_INPUT_GAIN");
se_cmd_set_cutoff_freq		= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
se_cmd_set_sampling_rate	= GetEventID("SE_CMD_SET_SAMPLING_RATE");
se_info_channel_done		= GetEventID("SE_INFO_CHANNEL_DONE");
se_info_record_done		= GetEventID("SE_INFO_RECORD_DONE");
se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_info_block_recorded		= GetEventID("SE_INFO_BLOCK_RECORDED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

/** Tell studio.library that we are initilitzed **/
BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;
int x;
struct ActiveSample *ps;
void WindGadSize();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module && ((struct Module *)event->arg1==mod))
		{
		if (wind==NULL)	DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
		{
		if (((struct Module *)event->arg1 == mod) || (hand ? ((struct Module *)event->arg1 == hand->handler_mod) : FALSE))
			{
			ReplyMsg(event);
			if (wind) HideWind();
			DeleteModule(mod);
			closelibraries();
			exit(0);
			}
		else {
			}
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_channel_volume)
		{
		if(mon_key==(struct ChanKey *)event->arg1)
			if(event->arg2)
				if(!(MONITORbutton.gad_ptr->flags & BUTN_HIT)){MONITORbutton.gad_ptr->flags|=BUTN_HIT; refresh_button(MONITORbutton.gad_ptr);}
				else ;
			else
				{MONITORbutton.gad_ptr->flags&=~BUTN_HIT; refresh_button(MONITORbutton.gad_ptr);}
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_sampling_rate && event->arg1==(int)(hand))
		{
		x=(*hand->FindBestRate)(hand->current_sampling_rate);
		if (x!=RATEknob->top_element)
			{
			RATEknob->top_element=x;
			move_knob(RATEknob);
			}
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_cutoff_freq && event->arg1==(int)(hand))
		{
		if (hand->flags&AH_FILTER_ADJ)
			{
			x=(*hand->FindBestFilter)(hand->current_filter);
			if (FREQknob->top_element!=x)
				{
				FREQknob->top_element=x;
				move_knob(FREQknob);
				}
			}
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_input_gain && event->arg1==(int)(hand))
		{
		x=(*hand->FindBestGain)(hand->current_input_gain);
		if (x!=GAINknob->top_element)
			{
			GAINknob->top_element=x;
			move_knob(GAINknob);
			}
		}

	/*********************************/
	if (event->type == se_info_block_recorded && as[0])
		{
		ps=(struct ActiveSample *)event->arg1;
		istc=ps->region.parms.starttimecode;
		x=0;TempSize=0;
		while(as[x]) TempSize+=((ASPos(as[x++])*2)/1024);
		if(TempSize>DiskSpaceAvailable)
			{
			x=0;
			while(as[x]) {ASStop(as[x]);as[x++]=NULL;}
			x=0;
			while(RecKey[x]) {FreeChan(RecKey[x]);RecKey[x++]=NULL;}

			RECORDbutton.gad_ptr->flags&=~BUTN_HIT;
			show_gadget(RECORDbutton.gad_ptr);
			refresh_gadget(RECORDbutton.gad_ptr);
			NotifyEndRecording();
			telluser("Record:","Disk Full!",NULL);
			}
		else RecordStats();
		}

	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
void WindGadSize();
struct MenuItem *FindCheckedItem(),*mi;
int t;
/*struct AudioHandler *ItemData();*/ /* SAS C is moranic */

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	ReplyMsg(message);

	if(class== CLOSEWINDOW)
		{
		HideWind();
		return;
		}
	if(class== MENUPICK)
		{
		if (Titles[0].FirstItem)
			{
			mi=Titles[0].FirstItem;
			t=0;
			while ((mi=FindCheckedItem(mi)) && t<16)
				{
				state->itemnum[t]=ItemNum(mi);
				t++;
				mi=mi->NextItem;
				}
			state->itemnum[t]=-1;
			}
/***/
		if (Titles[1].FirstItem) 
			{
			mi=FindCheckedItem(Titles[1].FirstItem);
			if(hand!=(struct AudioHandler *)ItemData(mi))
				{
				hand=(struct AudioHandler *)ItemData(mi);
				state->sms.hand=hand;

				if((mon_key=AllocChan(hand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) telluser("Record:","COULD NOT GET MONITOR KEY",0);
				if(mon_key->parms.volume)	MONITORbutton.gad_ptr->flags|=BUTN_HIT;
				else	MONITORbutton.gad_ptr->flags&=~BUTN_HIT;

				if (hand->flags&AH_FILTER_ADJ)
					{
					FREQknob->list_size=hand->number_of_filter_settings;
					FREQknob->top_element=(*hand->FindBestFilter)(hand->current_filter);
					}

				RATEknob->list_size=hand->number_of_rate_settings;
				RATEknob->top_element=(*hand->FindBestRate)(hand->current_sampling_rate);

				GAINknob->list_size=hand->number_of_gain_settings;
				GAINknob->top_element=(*hand->FindBestGain)(hand->current_input_gain);

				sprintf(tbn,"Recorder - %s - %s",hand->name,TempSampName);
				WindTitleChange(wind,tbn);
				WindGadSize();
				}
			}
		}
	if(class== NEWSIZE)
		{
		if (wind) 
			{
			WindGadSize();
			}
		}
	}
}
/*****************************************************************/
/*****************************************************************/
void DisplayWind()
{
int x,w,s,i;
struct MenuItem *BuildChannelMenu();
struct MenuItem *BuildHandlerMenu();
struct ChanKey *ItemData();
int t,rc,cksum;
struct AudioHandler *hnd=NULL;
struct Menu *mnu;

t=0;
hnd = (struct AudioHandler *) FindAudioHandler(NULL,-1,0,NULL,-1);
while(hnd)
	{
	t++;
	hnd = (struct AudioHandler *) FindAudioHandler(hnd,-1,0,NULL,-1);
	}

if (t==1)
	{
	rc=0;
	hnd = (struct AudioHandler *) FindAudioHandler(NULL,-1,0,NULL,-1);
	for(x=0;x<hnd->number_of_channels;x++)
		{
		if (CK_CHAN_RECORD & hnd->channel[x].flags) {rc++;RecChan=&hnd->channel[x];}
		}
	}
else	rc=t;

if (t>1)
	{
	Titles[1].FirstItem=BuildHandlerMenu(NULL,NULL);
	Titles[0].NextMenu=&Titles[1];
	Titles[0].FirstItem=BuildChannelMenu(CK_CHAN_RECORD,MENU_MULTI_SELECT);
	}
else
	{
	Titles[1].FirstItem=NULL;
	Titles[0].NextMenu=NULL;
	if (rc>1)
		{
		Titles[0].FirstItem=BuildChannelMenu(CK_CHAN_RECORD,MENU_MULTI_SELECT);
		}
	else
		{
		Titles[0].FirstItem=NULL;
		}
	}

if (!GadRef())
{
if(rc>1) mnu=&Titles[0];
else mnu=NULL;
	if (wind = OpenNewWind(&Record_NewWindow,mod->state,NULL,mnu))
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

	if (Titles[1].FirstItem) CheckData(Titles[1].FirstItem,hand);

	if (Titles[0].FirstItem)
		{
		cksum=MenuCheckSum(Titles[0].FirstItem);
		if (cksum==state->menucksum || state->menucksum==0)
			{
			t=0;
			while (state->itemnum[t]!=-1)
				{
				CheckNum(Titles[0].FirstItem,state->itemnum[t]);
				t++;
				}
			}
		else
			{
			state->itemnum[1]=1;
			state->itemnum[0]=-1;
			CheckNum(Titles[0].FirstItem,1);
			}
		state->menucksum=cksum;
		}

	RATEgad.box_x2=wind->Width-11;
	GAINgad.box_x1=10;

	if (hand->flags&AH_FILTER_ADJ)
		{
		FREQgad.box_x2=wind->Width-11;
		FREQgad.box_x1=((wind->Width-20)/2)+3;
		GAINgad.box_x2=((wind->Width-20)/2)-3;
		NUMGADS=5;
		}
	else
		{
		FREQgad.flags|=HIDE_GADGET;
		GAINgad.box_x2=wind->Width-11;
		AUTOFILTbutton.flags|=HIDE_GADGET;
		NUMGADS=4;
		}

	s=((wind->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;
	for(i=0;i<NUMGADS;i++)
		{
		but[i].box_x1=(x>>8);
		but[i].box_x2=(x+w)>>8; x=x+w+(5<<8);
		but[i].box_y1=wind->Height-16;
		but[i].box_y2=wind->Height-6;
		}

	gadlibport = (struct MsgPort *)CreatePort(0,0);
	RECORDbutton.notifyme=gadlibport;
	STOPbutton.notifyme=gadlibport;

	ColorGadList(&MONITORbutton);
	if (create_gadget_chain(wind,&MONITORbutton)) ;
	RATEknob = (struct slider *)RATEgad.gad_ptr;
	GAINknob = (struct slider *)GAINgad.gad_ptr;
	FREQknob = (struct slider *)FREQgad.gad_ptr;

	NotifyMeOnEvent(se_cmd_set_input_gain, mod);
	NotifyMeOnEvent(se_cmd_set_cutoff_freq, mod);
	NotifyMeOnEvent(se_cmd_set_sampling_rate, mod);
	NotifyMeOnEvent(se_cmd_set_channel_volume, mod);
	NotifyMeOnEvent(se_info_block_recorded, mod);
	sprintf(sn,"%s",TempSampName);

	sprintf(tbn,"Recorder - %s - %s",hand->name,sn);
	WindTitleChange(wind,tbn);
	}
}
else telluser("Record:","Could not Open Window",0);
}
/*****************************************************************/
void HideWind()
{
struct Window *w;
int x;

for(x=0;x<16;x++) if(RecKey[x]) {telluser("Record:","Could Not Close Window, Channels Active",NULL);return;}

if (wind)
	{
	if (Titles[0].FirstItem) FreeChannelMenu(Titles[0].FirstItem);
	if (Titles[1].FirstItem) FreeHandlerMenu(Titles[1].FirstItem);

	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);

	IgnoreEvent(se_cmd_set_input_gain, mod);
	IgnoreEvent(se_cmd_set_cutoff_freq, mod);
	IgnoreEvent(se_cmd_set_sampling_rate, mod);
	IgnoreEvent(se_cmd_set_channel_volume, mod);
	IgnoreEvent(se_info_block_recorded, mod);

	DeletePort(gadlibport);	gadlibport=0;

	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
int GadRef()
{
/***/
	if (state->sms.hand==NULL)
		{
		if (hand=FindAudioHandler(NULL,-1,0,NULL,-1))
			{
			state->sms.hand=hand;
			state->sms.handler_name=hand->name;
			state->sms.handler_class_id =hand->class_id;
			}
		else {telluser("Record:","Could not find a handler!",NULL);return(-1);}
		}
	else	hand=state->sms.hand;
/***/

	if (hand->flags&AH_FILTER_ADJ)
		{
		FREQgad.list_size=hand->number_of_filter_settings;
		FREQgad.top_element=(*hand->FindBestFilter)(hand->current_filter);
		}

	RATEgad.list_size=hand->number_of_rate_settings;
	RATEgad.top_element=(*hand->FindBestRate)(hand->current_sampling_rate);

	GAINgad.list_size=hand->number_of_gain_settings;
	GAINgad.top_element=(*hand->FindBestGain)(hand->current_input_gain);

	if((mon_key=AllocChan(hand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) telluser("Record:","COULD NOT GET MONITOR KEY",0);

	if(mon_key->parms.volume)	MONITORbutton.flags|=BUTN_HIT;
	else	MONITORbutton.flags&=~BUTN_HIT;

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
static void GAINfunc(k)
struct slider *k;
{
char buf[20];
static int KFLAG=0;
static int VOL=0;
int gain;

gain= (*hand->GainValueFunc)(k->top_element);

sprintf(buf,"Gain%3ddB",(gain>>5)-100);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 9);

if (KFLAG!=(k->flags&BUTN_HIT)) 
	{
	if (!KFLAG) 
		{
		VOL=mon_key->parms.volume;

		BroadcastEventParms(se_cmd_set_channel_volume,mon_key,3200,0,0,0);
		}
	else	BroadcastEventParms(se_cmd_set_channel_volume,mon_key,VOL,0,0,0);
	KFLAG=(k->flags&BUTN_HIT);
	}
if (FindTask(NULL)!=task) BroadcastEventParmsLessOne(se_cmd_set_input_gain,mod,hand,gain, 0, 0, 0, 0);
}

/*****************************************************************/
static void FREQfunc(k)
struct slider *k;
{
char buf[20];
int	freq;

if (FindTask(NULL)!=task)
	{
	freq=(hand->FilterValueFunc)(k->top_element);
	BroadcastEventParmsLessOne(se_cmd_set_cutoff_freq,mod,hand,freq, 0, 0, 0,0);
	}
else
	{
	k->top_element=(*hand->FindBestFilter)(hand->current_filter);
	k->list_size=hand->number_of_filter_settings;
	freq=(hand->FilterValueFunc)(k->top_element);
	}

sprintf(buf,"Filter %5d",freq);
GlobFilterCut=freq;

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);
}
/*****************************************************************/
static void RATEfunc(k)
struct slider *k;
{
char buf[20];
int	rate;

rate= (*hand->RateValueFunc)(k->top_element);
GlobSampRate=rate;
sprintf(buf,"Rate %5d",rate);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

if (FindTask(NULL)!=task) 
	{
	BroadcastEventParmsLessOne(se_cmd_set_sampling_rate,mod,hand,rate,0, 0, 0,0);
	if (AutoFilt)	BroadcastEventParms(se_cmd_set_cutoff_freq,hand,rate/2,0, 0, 0,0);
	}
}
/*****************************************************************/
static void MONITORfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,mon_key,0x0C80,0,0,0);
else
	BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,mon_key,0x0000,0,0,0);
}
/*****************************************************************/
/*****************************************************************/
static void FILEfunc(b)
struct button *b; 
{
FILEbut = b;
newgetstr(TempSampName,"Sample Name?",8*40);
xor_button(b);
sprintf(sn,"%s",TempSampName);
sprintf(tbn,"Recorder - %s - %s",hand->name,sn);
WindTitleChange(wind,tbn);
RecordStats();
}
/*****************************************************************/
static void AUTOFILTfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	{
	BroadcastEventParms(se_cmd_set_cutoff_freq,hand,GlobSampRate/2, 0, 0, 0,0);
	AutoFilt=1;
	}
else	AutoFilt=0;
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
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct NewWindow RecordingNW = {
   (640-200)>>1,
   (200-12)>>1,
   200-21,
   14+8*3+4+9,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|SMART_REFRESH|WINDOWDRAG,
   0,0,
   "-- RECORDING --", /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };

/*****************************************************************/

void NotifyRecording(x)
int x;
{
struct Window *OpenNewWind();

if (!Rwind)
	{
	RecordingNW.Height=42+x*9;
	Rwind=OpenNewWind(&RecordingNW,NULL,NULL,NULL);
	istc=0;
	TempSize=0;
	RecordStats();
	}
}

void NotifyEndRecording()
{
if (Rwind) CloseWind(Rwind);
Rwind=NULL;
}
/*****************************************************************/
void RecordStats()
{
char str[100];
int q,w;
static flag=0;


LockGadLibRender();
if (Rwind)
	{
	SetAPen(Rwind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetBPen(Rwind->RPort,StudioBase->defaults.colors.gadget_window.background);
	SetDrMd(Rwind->RPort, JAM2);

	if (flag) 
		if(!(StudioBase->flags&SB_SMPTE_ON)) 
			{
			istc=0xFFFFFFFF;
			sprintf(str,"StartTime NA",stc->hours,stc->minutes,stc->seconds,stc->frames);
			flag=0;
			Move(Rwind->RPort, 5,Rwind->Height-4);
			Text(Rwind->RPort, str,strlen(str));
			}
		else 
			{
			sprintf(str,"StartTime %02d:%02d:%02d:%02d",stc->hours,stc->minutes,stc->seconds,stc->frames);
			flag=0;
			Move(Rwind->RPort, 5,Rwind->Height-4);
			Text(Rwind->RPort, str,strlen(str));
			}

	if (istc==0) {flag=1;}
	if (flag) 
		{
		q=(Rwind->Height-42)/9;
		for(w=0;w<q;w++)
			{
			sprintf(str,"%d) %-18s",w+1,StrFileName(as[w]->region.samp_name));
			str[21]=NULL;
			Move(Rwind->RPort, 5,20+w*9);
			Text(Rwind->RPort, str,strlen(str));
			}
		}

	q=TempSize/1024;
	w=((TempSize-(q*1024))*1000)/1024;
	sprintf(str,"Size:%4d.%03dM",q,w);
	Move(Rwind->RPort, 5,Rwind->Height-(51-29));
	Text(Rwind->RPort, str,strlen(str));

	q=(DiskSpaceAvailable-TempSize)/1024;
	w=(((DiskSpaceAvailable-TempSize)-(q*1024))*1000)/1024;
	sprintf(str,"Left:%4d.%03dM",q,w);
	Move(Rwind->RPort, 5,Rwind->Height-(51-38));
	Text(Rwind->RPort, str,strlen(str));
	}
UnLockGadLibRender();
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
void WindGadSize()
{
int x,w,s,i;

RefreshWind(wind);

new_gadget_size(RATEgad.gad_ptr,10,32, wind->Width-11,44);
if (hand->flags&AH_FILTER_ADJ)
	{
	show_gadget(FREQknob);
	new_gadget_size(FREQgad.gad_ptr,((wind->Width-20)/2)+3,16, wind->Width-11,28);
	new_gadget_size(GAINgad.gad_ptr,10,16,((wind->Width-20)/2)-3,28);
	show_gadget(AUTOFILTbutton.gad_ptr);
	NUMGADS=5;
	}
else
	{
	hide_gadget(FREQknob);
	new_gadget_size(GAINgad.gad_ptr,10,16,wind->Width-11,28);
	hide_gadget(AUTOFILTbutton.gad_ptr);
	NUMGADS=4;
	}

s=((wind->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
w=(s/NUMGADS);
x=10<<8;

for(i=0;i<NUMGADS;i++)
	{
	new_gadget_size(but[i].gad_ptr,(x>>8),wind->Height-16,(x+w)>>8,wind->Height-6);
	refresh_gadget(but[i].gad_ptr);
	x=x+w+(5<<8);
	}

refresh_gadget_chain(wind);
}
