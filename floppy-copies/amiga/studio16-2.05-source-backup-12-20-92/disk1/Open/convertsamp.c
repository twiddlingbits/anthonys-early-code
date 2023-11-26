#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "open.h"

struct FileFilter *AskFileFormat();
struct Window *QuickWindow();
void CopySampData();

extern struct StudioBase *StudioBase;
char convertpath[STUDIO16_MAX_PATH_SIZE];

int ConvertSample(l,le,path)
struct list *l;
struct list_entry *le;
char *path;
{
int retval;
struct FileFilter *ff;
char name[50];
char pathplusname[STUDIO16_MAX_PATH_SIZE];

ff = AskFileFormat();
if (ff==NULL)
	return(TRUE);

strcpy(name, StrFileName(le->text));
retval=FileRequest("Save Converted Sample As:",convertpath,name,FR_SINGLE_SELECT);
if (retval==0) return(TRUE);
strcpy(pathplusname, convertpath);
StrPathFileCat(pathplusname, name);

new_pointer(PTR_BUSY);
retval=DoConvertSamp(le->text,pathplusname,ff);
new_pointer(PTR_NORMAL);
show_error(retval);
return(retval==NO_ERROR);
}

/************************************************************/

int DoConvertSamp(from,to,ff)
char *from, *to;			/** complete paths **/
{
int error;
struct SampleFilePointer *wsfp,*rsfp;

rsfp=(struct SampleFilePointer *)SampleOpen(from,SP_MODE_OLDFILE,0);
if (rsfp==0)
	return(STDERR_CANT_CREATE_SAMPLE);

wsfp=(struct SampleFilePointer *)SampleOpen(to,SP_MODE_NEWFILE,ff);
if (wsfp==0) {
	SampleClose(rsfp);
	return(STDERR_CANT_CREATE_SAMPLE);
	}

error=SampleCopy(wsfp,rsfp,rsfp->tag.length);
CopySampData(&wsfp->tag,&rsfp->tag);
SampleClose(rsfp);
SampleClose(wsfp);
return(error);
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
long flags;

port = (struct MsgPort *)CreatePort(0,0);
ffbutn.notifyme=port;
ffbutn2.notifyme=port;
fflist.notifyme=port;
ffbutn.c=&StudioBase->defaults.colors;
ffbutn2.c=&StudioBase->defaults.colors;
fflist.c=&StudioBase->defaults.colors;

if ((w=(struct Window *)OpenNewWind(&NewWind,NULL,NULL,NULL))==0) 
	{telluser("OpenList:","Could Not Open Save Window",NOTICE_RETURN_NOW);return(NULL);}

create_gadget_chain(w,&ffbutn);

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   ff=(struct FileFilter *)node;
	if (ff->flags&FF_WRITE)
		add_list_entry(fflist.gad_ptr, ff->name, ff);
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);
handle_new_select(fflist.gad_ptr,get_list_entry_ptr(fflist.gad_ptr, 0),0);

while (TRUE) {
	WaitPort(port);
	msg=(struct GadMsg *)GetMsg(port);
	g = msg->g;
	flags=msg->flags;
	ReplyMsg(msg);
	if (g==ffbutn2.gad_ptr || g==ffbutn.gad_ptr)
		break;
	if (flags&LIST_DOUBLESELECT) {
		g=ffbutn.gad_ptr;	/** OK **/
		break;
		}
	}

if (g==ffbutn2.gad_ptr) {
	ff=NULL;
	}
else {
	if (((struct list *)(fflist.gad_ptr))->active_entry)
		ff = (struct FileFilter *)(((struct list *)(fflist.gad_ptr))->active_entry->user_data);
	else
		ff=NULL;
	}

ffbutn.notifyme=NULL;
ffbutn2.notifyme=NULL;
fflist.notifyme=NULL;
hide_gadget(fflist.gad_ptr);
delete_gadget_chain(w);
CloseWind(w);
DeletePort(port); 
return(ff);
}

/************************************************************/

char *AskPathToRemove()
{
static struct NewList rempathlist =
			{NULL,10,16,WIN_WIDTH-11,WIN_HEIGHT-20,LIST_COOL|LIST_SINGLESELECT,NULL,0,0,0,0,0,0,0,0};
static struct NewButton cancel = {
   &rempathlist,(WIN_WIDTH>>1)+3,WIN_HEIGHT-16,WIN_WIDTH-11,WIN_HEIGHT-6,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"CANCEL"};

static struct NewButton okay = {
   &cancel,10,WIN_HEIGHT-16,(WIN_WIDTH>>1)-3,WIN_HEIGHT-6,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"OK"};

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
   "Select Path:",
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };
char *rempath;
struct MsgPort *port;
struct Window *w;
struct GadMsg *msg;
struct GadgetHeader *g;
long flags;
extern struct OLPath olpath[OL_MAX_PATHS];
short i;

port = (struct MsgPort *)CreatePort(0,0);
okay.notifyme=port;
cancel.notifyme=port;
rempathlist.notifyme=port;
okay.c=&StudioBase->defaults.colors;
cancel.c=&StudioBase->defaults.colors;
rempathlist.c=&StudioBase->defaults.colors;

if ((w=(struct Window *)OpenNewWind(&NewWind,NULL,NULL,NULL))==0) 
	{telluser("OpenList:","Could Not Window",NOTICE_RETURN_NOW);return(NULL);}

create_gadget_chain(w,&okay);

for (i=0; i < OL_MAX_PATHS; i++)
	if (olpath[i].state!=OL_EMPTY)
		add_list_entry(rempathlist.gad_ptr, olpath[i].path,olpath[i].path);

while (TRUE) {
	WaitPort(port);
	msg=(struct GadMsg *)GetMsg(port);
	g = msg->g;
	flags=msg->flags;
	ReplyMsg(msg);
	if (g==cancel.gad_ptr || g==okay.gad_ptr)
		break;
	if (flags&LIST_DOUBLESELECT) {
		g=okay.gad_ptr;	/** OK **/
		break;
		}
	}

if (g==cancel.gad_ptr) {
	rempath=NULL;
	}
else {
	if (((struct list *)(rempathlist.gad_ptr))->active_entry)
		rempath = (char *)(((struct list *)(rempathlist.gad_ptr))->active_entry->user_data);
	else
		rempath=NULL;
	}

okay.notifyme=NULL;
cancel.notifyme=NULL;
rempathlist.notifyme=NULL;
hide_gadget(rempathlist.gad_ptr);
delete_gadget_chain(w);
CloseWind(w);
DeletePort(port); 
return(rempath);
}

/************************************************************/

void CopySampData(wtag,rtag)
struct SampleFileTag *rtag,*wtag;
{
wtag->parms=rtag->parms;
}

