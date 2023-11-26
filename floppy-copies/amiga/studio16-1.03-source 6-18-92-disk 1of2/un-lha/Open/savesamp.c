#include <exec/types.h>
#include <intuition/intuition.h>

#include "/include/studio16.h"

struct FileFilter *AskFileFormat();
struct Window *QuickWindow();

/********************************/
/*  Save a new sample to   disk */
/********************************/

save_samp (Window,path,defname,samp)

struct Window *Window;
char *path,*defname;
struct Disk_Samp *samp;
{
extern struct StudioBase *StudioBase;
struct FileFilter *ff;
char name[50];
int cancel,x;

ff = AskFileFormat();
if ((int)ff==-1) return(NO_ERROR);

strcpy(name, defname);
cancel=FileRequest("OpenList Save Sample:",path,name,FR_SINGLE_SELECT);
if (cancel==0) return(NO_ERROR);

x=stclen(path); if(path[x-1]=='/') path[x-1]=0;

if (ff) {		/** Make a copy of the file **/
	new_pointer(PTR_BUSY);
	cancel=SaveSamp(path,name,samp,ff);
	new_pointer(PTR_NORMAL);
	}
else {			/** simpy rename working file **/
	cancel=QuickSaveSamp(path,name,samp);
	}
return(cancel);
}

/************************************************************/

#define WIN_HEIGHT 92
#define WIN_WIDTH 240

struct FileFilter *AskFileFormat()
{
static struct NewList fflist =
			{NULL,10,16,WIN_WIDTH-11,WIN_HEIGHT-20,LIST_COOL|LIST_SINGLESELECT,NULL,0,0,0,0,0,0,0,0};
static struct NewButton ffbutn2 = {
   &fflist,(WIN_WIDTH>>1)+3,WIN_HEIGHT-16,WIN_WIDTH-11,WIN_HEIGHT-6,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"CANCEL"};

static struct NewButton ffbutn = {
   &ffbutn2,10,WIN_HEIGHT-16,(WIN_WIDTH>>1)-3,WIN_HEIGHT-6,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"OK"};

struct NewWindow NewWind = {
   -1,
   -1,
   WIN_WIDTH,
   WIN_HEIGHT,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH,
   0,0,
   "Select File Format:",
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };

struct Node *node;
struct FileFilter *ff;
struct MsgPort *port;
struct Window *w;

struct GadMsg *msg;
struct GadgetHeader *g;

port = (struct MsgPort *)CreatePort(0,0);
ffbutn.notifyme=port;
ffbutn2.notifyme=port;
ffbutn.c=&StudioBase->defaults.colors;
ffbutn2.c=&StudioBase->defaults.colors;
fflist.c=&StudioBase->defaults.colors;

if ((w=(struct Window *)OpenNewWind(&NewWind,NULL,NULL,NULL))==0) 
	{telluser("OpenList:","Could Not Open Save Window",NOTICE_RETURN_NOW);return((struct FileFilter *)-1);}

create_gadget_chain(w,&ffbutn);

add_list_entry(fflist.gad_ptr, "QUICK (rename temp file)", 0);
handle_new_select(fflist.gad_ptr,get_list_entry_ptr(fflist.gad_ptr, 0),0);

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   ff=(struct FileFilter *)node;
	if (ff->flags&FF_SAVE_FILTER)
		add_list_entry(fflist.gad_ptr, ff->name, ff);
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);

WaitPort(port);
msg=(struct GadMsg *)GetMsg(port);
g = msg->g;
ReplyMsg(msg);
if (g==ffbutn2.gad_ptr) ff=(struct FileFilter *)-1;
else
	if (((struct list *)(fflist.gad_ptr))->active_entry)
		ff = (struct FileFilter *)(((struct list *)(fflist.gad_ptr))->active_entry->user_data);
	else	ff=NULL;

ffbutn.notifyme=NULL;
ffbutn2.notifyme=NULL;
hide_gadget(fflist.gad_ptr);
delete_gadget_chain(w);
CloseWind(w);
DeletePort(port); 
return(ff);
}

