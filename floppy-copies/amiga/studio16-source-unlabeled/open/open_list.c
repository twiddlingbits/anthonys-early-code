#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#include "open_list.pwc"

#define MAX_PLAYING_SAMPLES 32

struct Window *open_list_win;
struct list *open_list;
char fname[40],fullpath[120];
struct Module *thismod;
struct PlayingSample *all_ps[MAX_PLAYING_SAMPLES];

extern struct studioBase *studioBase;

void func_open_list();
void handle_studioevents();
void handle_intuievents();
void InitOpenList();
void DoOpenList();
struct Window *OpenWind();
void play();
void FreeMyChans();
void stop_all_playback();

/** events I'm interested in **/

short se_info_sample_created;
short se_info_sample_deleted;
short se_cmd_select_module;
short se_cmd_kill_module;    
short se_cmd_edit_sample;
short se_cmd_play_sample;
short se_cmd_stop_playing;
short se_cmd_stop_all_playback;
short se_cmd_trigger_playback;
short se_info_channel_done;
short se_info_preload_done;
short se_cmd_preload_sample;
short se_info_module_closed;
short se_info_module_opened;

/***********************************************************/

void main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitOpenList();					/** initalize module **/
DoOpenList();						/** open window **/

while (TRUE) {
	if (open_list_win)
		waitmask=(1<<open_list_win->UserPort->mp_SigBit) | (1<<thismod->notifyme->mp_SigBit);
	else
		waitmask=(1<<thismod->notifyme->mp_SigBit);

   mask=Wait(waitmask);
/*printf("mask %x\n",mask);*/
	if (open_list_win)
		if (mask&(1<<open_list_win->UserPort->mp_SigBit))
			handle_intuievents();
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	}
}

/***********************************************************/

void InitOpenList()
{
static struct NewModule open_list_data ={"OpenList",0,sizeof(struct StandardModState)};
int i;

if ((thismod=(struct Module *)AddModule(&open_list_data))==0) {
	closelibraries();
   exit(10);
   }

fname[0]=0;

se_info_sample_created	=GetEventID("SE_INFO_SAMPLE_CREATED");
se_info_sample_deleted	=GetEventID("SE_INFO_SAMPLE_DELETED");
se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_edit_sample		=GetEventID("SE_CMD_EDIT_SAMPLE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_playing		=GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_cmd_trigger_playback	=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_channel_done		=GetEventID("SE_INFO_CHANNEL_DONE");
se_info_preload_done		=GetEventID("SE_INFO_PRELOAD_DONE");
se_cmd_preload_sample	=GetEventID("SE_CMD_PRELOAD_SAMPLE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_info_sample_created, thismod);
NotifyMeOnEvent(se_info_sample_deleted, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_play_sample, thismod);
NotifyMeOnEvent(se_cmd_stop_all_playback, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);
NotifyMeOnEvent(se_info_preload_done, thismod);
NotifyMeOnEvent(se_cmd_trigger_playback, thismod);

/*
for (i=0; i < studioBase->max_registered_ids; i++)
	printf("%s %d\n",studioBase->eventidarray[i],i);
*/
}

/***********************************************************/

void DoOpenList()
{
struct MinNode *node;
struct Disk_Samp *samp;
static struct NewList newopenlist =
			{NULL,10,14,260,80,LIST_COOL|LIST_SORTED,func_open_list,0,0,0,0,0,0};


if (thismod->state->win_h!=0)
	RestoreWindowParms(thismod,&NewWindowStructure1);

open_list_win=OpenWind(&NewWindowStructure1);
if (open_list_win==NULL) {
	printf("can't open window!!\n");
	exit(10);
	}				/** FIX!!!!!!! **/

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);

newopenlist.c=&studioBase->defaults.colors;
open_list = (struct list *)create_gadget(open_list_win, &newopenlist);
if (open_list==0) {
   printf("open_list!!!ERROR!!!!!");
   }

/** Build displayed sample list by searching for sample list **/

ObtainSemaphore(&studioBase->lock);

for (node=studioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   samp=(struct Disk_Samp *)node;
	add_list_entry(open_list, samp->name, samp);
	}

ReleaseSemaphore(&studioBase->lock);

/* printf("open_list.c: studioBase size %d\n",sizeof(struct studioBase)); */

}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct Disk_Samp *samp;
struct PlayingSample *ps;
short i;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_info_sample_created) {
			samp = (struct Disk_Samp *)event->arg1;
			add_list_entry(open_list, samp->name, samp);
			}

		else if (event->type==se_info_sample_deleted) {
			delete_list_entry_by_userdata(open_list, event->arg1);
			}

		else if (event->type==se_cmd_select_module) {
			if ((struct Module *)event->arg1==thismod) {
				if (open_list_win==0) {
					DoOpenList();
					}
				}
			}

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0);
			DeleteModule(thismod);
			if (open_list_win) {
				delete_gadget_chain(open_list_win);  /** delete all gadgets in this window */
				UpdateStandardModState(thismod, open_list_win);
				CloseWindow(open_list_win);
				UpdateStandardModState(thismod, NULL);
				}
			closelibraries();
			exit(0);
			}

		else if (event->type==se_info_preload_done) {
			ps = (struct PlayingSample *)event->arg1;
			if (ps->owner==thismod) {
				if (ps->owner_data) {		/** GET KEY HERE FROM PLAY_SAMPLE SOMEHOW! **/
					ps->key=(struct ChanKey *)ps->owner_data;
					}
				else {
					ps->key = (struct ChanKey *)AllocChan(NULL,-1);
/*if (ps->key)			
	printf("OpenList: AllocChan %d\n",ps->key->chan_num);
else
	printf("OpenList: AllocChan: FAIL\n");*/
					}
				if (ps->key) {
					BroadcastEventParms(se_cmd_trigger_playback,ps,0,0,0,0,0);
					}
				else {
					BroadcastEventParms(se_info_channel_done,ps,0,0,0,0,0);
					}
				}
			}

		else	if (event->type==se_cmd_play_sample) {
			play(event->arg1, event->arg2, event->arg3, event->arg4);
			}

		else	if (event->type==se_info_channel_done) {
			FreeMyChans(event->arg1);
			}

		else	if (event->type==se_cmd_stop_all_playback) {
			stop_all_playback();
			}

		else if (event->type==se_cmd_trigger_playback) {
			for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
				if (all_ps[i]==NULL) {
					all_ps[i]=(struct PlayingSample *)event->arg1;
					break;
					}
				}
			}

	ReplyMsg(event); 
	}
}

/***********************************/
/* Play a digitized sample         */
/***********************************/

void play(samp,start,len,key)
int len,start;
struct Disk_Samp *samp;
struct ChanKey *key;
{
BroadcastEventParms(se_cmd_preload_sample,thismod, samp, start, len, 1);
}

/*******************************************************/
/*** cancel a playing channel 								**/
/*******************************************************/

void stop_all_playback()
{
short i;


for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_ps[i]) {
printf("STOP PLAYING! %x\n",all_ps[i]);
		BroadcastEventParms(se_cmd_stop_playing,all_ps[i],0,0,0,0,0);
		}
	}
}

/***********************************************************/

void FreeMyChans(ps)
struct PlayingSample *ps;
{
struct ChanKey *key;
short i;

for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_ps[i]==ps) {
		all_ps[i]=NULL;
		break;
		}
	}

if (ps->owner==thismod) {		/** Generated by SE_CMD_PLAY_SAMPLE **/
	key = ps->key;
	if (key)
		FreeChan(key);
	}
}

/***********************************************************/

void handle_intuievents()
{
struct IntuiMessage *message;
struct Disk_Samp *samp;
int i;
struct list_entry *le;
struct MinNode *node;
ULONG class;
USHORT code;
short spf;
struct Gadget *g;

while ((message=(struct IntuiMessage *)GetMsg(open_list_win->UserPort))!=0) {

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0);
		delete_gadget_chain(open_list_win);  /** delete all gadgets in this window */
		CloseWindow(open_list_win);
		open_list_win=0;
		return;
		}

	if (class==GADGETUP && g==&edit) {
		for (node=open_list->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
			if (((struct list_entry *)node)->flags & LE_SELECTED) {
				BroadcastEventParms(se_cmd_edit_sample,((struct list_entry *)node)->user_data,0,0,0,0,0);
				((struct list_entry *)node)->flags &= (~LE_SELECTED);
				}
			}
		refresh_list_text(open_list);
   	}

	if (class==GADGETUP && g==&save) {
#if 0
		BroadcastEventParms(se_cmd_save_as_sample,((struct list_entry *)node)->text,0,0,0,0,0);
		node=open_list->entries.mlh_Head;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				samp = (struct Disk_Samp *)le->user_data;
				strcpy(fname,samp->name);
				cancel=get_fname(studioBase->back_window,studioBase->screen,"Save Sample:",fname,studioBase->defaults.samp_io_path);
				if (cancel) {
					strcpy(fullpath,studioBase->defaults.samp_io_path);
					add(fullpath,fname);       /* fname is now path+filename */
				   newpointer(PTR_BUSY);
					save_samp (studioBase->back_window,samp,fullpath,0,samp->length-1);
				   newpointer(PTR_NORMAL);
					}
				}
	      }
#endif
	   }

	if (class==GADGETUP && g==&open) {
	   newpointer(PTR_BUSY);
	   i=load_samp(open_list_win,studioBase->defaults.samp_io_path,&samp);
	   show_error(i);
	   newpointer(PTR_NORMAL);
	   }

	if (class==GADGETUP && g==&PLAYgad) {
		node=open_list->entries.mlh_Head;
		spf=TRUE;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				spf=FALSE;
		      samp = (struct Disk_Samp *)le->user_data;
				BroadcastEventParms(se_cmd_play_sample,samp,0,samp->length,0,0,0);
   	      }
	      }
		for (node=open_list->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ)
			((struct list_entry *)node)->flags &= (~LE_SELECTED);
		refresh_list_text(open_list);
		if (spf) {	
			BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
			}
	   }

	if (class==GADGETUP && g==&close) {
		node=open_list->entries.mlh_Head;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
		      if (askuser("OK to close sample:",le->text,0)) {
	   	      DeleteSample(le->user_data);
					}
	         }
	      }
	   }
	}
}

/***********************************************************/

void func_open_list(l)
struct list *l;
{
struct Module *find_module();
extern struct studioBase *studioBase;
/*struct Module *m;*/

if (l->flags&LIST_NEWSELECT) {
#if 0
   m = find_module("SoundEdit");
   if (m) {
      if (l->current==-1)
         cur=NULL;
       else
         cur = (struct Disk_Samp *)l->active_entry->user_data;
      if (m->window) 
         refresh_edit_win();
      }
#endif
   }

}

