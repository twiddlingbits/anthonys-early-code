#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

static struct TextAttr TOPAZ80 = {
	(STRPTR)"topaz.font",
	TOPAZ_EIGHTY,0,0
};

static UBYTE UNDOBUFFER[13];

static UBYTE freqSIBuff[13];
static struct StringInfo freqSInfo = {
	freqSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

static struct Gadget str_freq = {
	NULL,	/* next gadget */
	95,39,	/* origin XY of hit box relative to window TopLeft */
	96,10,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&freqSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

static struct IntuiText IText3 = {
	WHITE,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	5,40,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Wave Freq:",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

static UBYTE timeSIBuff[13];
static struct StringInfo timeSInfo = {
	timeSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

static struct Gadget str_time = {
	NULL,	/* next gadget */
	95,27,	/* origin XY of hit box relative to window TopLeft */
	96,10,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&timeSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

static struct IntuiText IText2 = {
	WHITE,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	5,28,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Seconds:",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

static UBYTE rateSIBuff[13];
static struct StringInfo rateSInfo = {
	rateSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

static struct Gadget str_rate = {
	&str_time,	/* next gadget */
	95,15,	/* origin XY of hit box relative to window TopLeft */
	96,10,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&rateSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

static struct IntuiText IText1 = {
	WHITE,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	5,16,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Samp Rate:",	/* pointer to text */
	&IText2	/* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow NewGenReqWin = {
	-1,-1,	/* window XY origin relative to TopLeft of screen */
	196,67,	/* window width and height */
	0,1,	/* detail and block pens */
	NULL,	/* IDCMP flags */
	WINDOWDRAG+NOCAREREFRESH,	/* other window flags */
	&str_rate,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	80,50,	/* minimum width and height */
	-1,-1,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};


static struct NewButton okbutn = 
	{NULL,10,53,44,63,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutn = 
	{&okbutn,130,53,186,63,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

extern struct StudioBase *StudioBase;

int GenReq(ei,title,rateval,timeval,freqval)
struct EditInstance *ei;
char *title;
long *rateval;
double *timeval,*freqval;
{
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;

/** Initilize String Gadgets **/

sprintf(rateSIBuff,"%d",*rateval);
sprintf(timeSIBuff,"%5.1f",*timeval);
if (freqval) {
	sprintf(freqSIBuff,"%7.1f",*freqval);
	str_time.NextGadget=&str_freq;
	IText2.NextText=&IText3;
	}
else {
	str_time.NextGadget=NULL;
	IText2.NextText=NULL;
	}

/** Open the edit window **/

NewGenReqWin.Screen = StudioBase->screen;
NewGenReqWin.LeftEdge=-1;
NewGenReqWin.TopEdge=-1;
NewGenReqWin.Title=title;
window=(struct Window *)OpenNewWind(&NewGenReqWin,NULL,NULL,NULL);
if (window==NULL)
	return(FALSE);

cancelbutn.c = &StudioBase->defaults.colors;
okbutn.c = &StudioBase->defaults.colors;
okbutn.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutn.notifyme=okbutn.notifyme;
create_gadget_chain(window, &cancelbutn);

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) {
	Wait((1<<window->UserPort->mp_SigBit)|(1<<okbutn.notifyme->mp_SigBit));
	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
		}
	if (gadmsg=(struct GadMsg *)GetMsg(okbutn.notifyme)) {
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		break;
		}
	}

delete_gadget_chain(window);
DeletePort(okbutn.notifyme);
CloseWind(window);

sscanf(rateSIBuff,"%d",rateval);
sscanf(timeSIBuff,"%lf",timeval);
if (freqval)
	sscanf(freqSIBuff,"%lf",freqval);

return(gadpressed==okbutn.gad_ptr);
}
