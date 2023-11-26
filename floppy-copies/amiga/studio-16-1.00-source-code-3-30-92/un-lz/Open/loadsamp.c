#include "/include/studio16.h"

/********************************/
/*  Load a new sample from disk */
/********************************/

load_samp (Window,path)

struct Window *Window;
char *path;
{
extern struct StudioBase *StudioBase;
char name[80];
int cancel,x;

name[0]=0;

cancel=FileRequest("Load Sample:",path,name,FR_SINGLE_SELECT);
/*cancel=get_fname(Window,StudioBase->screen,"Select file to load:",name,path);*/
if (cancel==0)	return(NO_ERROR);

x=stclen(path); if(path[x-1]=='/') path[x-1]=0;

if (IsRenamePossible(path,name,FALSE)) {
	if (IsQuickFileType(path,name)) {
		if (UseOriginal()) {
			return(QuickLoadSamp(path,name,name));
			}
		}
	}

new_pointer(PTR_BUSY);
cancel=LoadSamp(path,name,name);
new_pointer(PTR_NORMAL);
return(cancel);
}

/********************************************************/

UseOriginal()
{
struct GadMsg *msg;
struct GadgetHeader *gad;
struct Window *window;
struct MsgPort *port;
static struct NewButton usecopy = {
   NULL,142,47,245,63,BUTN_COOL,0,0,0,0,0,0,"Use Copy"};

static struct NewButton useoriginal = {
   &usecopy,15,47,120,63,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Use Original"};


static struct IntuiText IText5 = {
	3,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	12,25,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"to work on a copy?",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

static struct IntuiText IText4 = {
	3,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	12,15,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"directly, or do you prefer ",	/* pointer to text */
	&IText5	/* next IntuiText structure */
};

static struct IntuiText IText3 = {
	3,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	12,5,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Do you want to edit the file",	/* pointer to text */
	&IText4	/* next IntuiText structure */
};

#define IntuiTextList1 IText3

static struct NewWindow NewWindowStructure1 = {
	155,70,	/* window XY origin relative to TopLeft of screen */
	266,72,	/* window width and height */
	0,1,	/* detail and block pens */
	NULL,	/* IDCMP flags */
	BORDERLESS+NOCAREREFRESH+ACTIVATE,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	640,200,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

NewWindowStructure1.Screen=StudioBase->screen;
window = (struct Window *)OpenWindow(&NewWindowStructure1);
if (window==NULL)
	return(FALSE);

SetAPen(window->RPort, StudioBase->defaults.colors.button.hitbox_unselected);	/** ADD A NEW COLOR TYPE?? **/
RectFill(window->RPort, 0,0, window->Width-1, window->Height-1);
draw_indent(window->RPort, 0, 0, window->Width-1,window->Height-1, StudioBase->defaults.colors.gadget_window.topleft_3d, StudioBase->defaults.colors.gadget_window.bottomright_3d);
PrintIText(window->RPort,&IntuiTextList1,0,0);

port = (struct MsgPort *)CreatePort(0,0);
if (port==NULL) {
	CloseWindow(window);
	return(FALSE);
	}

usecopy.notifyme=port;
useoriginal.notifyme=port;
usecopy.c=&StudioBase->defaults.colors;
useoriginal.c=&StudioBase->defaults.colors;

create_gadget_chain(window, &useoriginal);

Wait(1<<port->mp_SigBit); 

msg=(struct GadMsg *)GetMsg(port);
gad = msg->g;
ReplyMsg(msg);
delete_gadget_chain(window);
CloseWindow(window);
DeletePort(port);
if (gad==useoriginal.gad_ptr)
	return(TRUE);
else
	return(FALSE);
}

/********************************************************/

IsQuickFileType(path,name)
char *path,*name;
{
int fp;
int namelen;
long id;
char *temppath;

id=0;
namelen=strlen(path)+strlen(name)+2;
temppath = (char *)AllocMem(namelen,0);
if (temppath==0)
	return(FALSE);
strcpy(temppath, path);
add(temppath, name);

fp=Open(temppath, MODE_OLDFILE);
if (fp) {
	Read(fp,&id,4);
	Close(fp);
	}
FreeMem(temppath,namelen);
return(id==TAG_KWIK);
}
