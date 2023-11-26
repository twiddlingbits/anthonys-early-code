#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

static struct TextAttr TOPAZ80 = {
	(STRPTR)"topaz.font",
	TOPAZ_EIGHTY,0,0
};

static UBYTE UNDOBUFFER[13];

static UBYTE rangeSIBuff[13];
static struct StringInfo rangeSInfo = {
	rangeSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

static struct Gadget str_range = {
	NULL,	/* next gadget */
	95,30,	/* origin XY of hit box relative to window TopLeft */
	96,10,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&rangeSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

static struct IntuiText IText2 = {
	WHITE,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	5,30,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Rng Start:",	/* pointer to text */
	NULL	/* next IntuiText structure */
};


static UBYTE sampSIBuff[13];
static struct StringInfo sampSInfo = {
	sampSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

static struct Gadget str_samp = {
	&str_range,	/* next gadget */
	95,15,	/* origin XY of hit box relative to window TopLeft */
	96,10,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&sampSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

static struct IntuiText IText1 = {
	WHITE,0,JAM1,	/* front and back text pens, drawmode and fill byte */
	5,16,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Samp Start:",	/* pointer to text */
	&IText2	/* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow NewDisplayOffsetWin = {
	-1,-1,	/* window XY origin relative to TopLeft of screen */
	196,57,	/* window width and height */
	0,1,	/* detail and block pens */
	GADGETUP,	/* IDCMP flags */
	WINDOWDRAG+NOCAREREFRESH,	/* other window flags */
	&str_samp,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	"Enter Display Offset:",	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	80,50,	/* minimum width and height */
	-1,-1,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};


static struct NewButton okbutn = 
	{NULL,10,43,44,53,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutn = 
	{&okbutn,130,43,186,53,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

static struct TimeCode *samptc;

extern struct StudioBase *StudioBase;
extern struct EditInstance EI[EI_MAX];
extern short longest_ei;

void AsciiToTC();
void UpdateGadgetText();

void DisplayOffset(ei)
struct EditInstance *ei;
{
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
extern struct EditState *thisstate;
struct TimeCode tc,tc2,org_offset;
long j;
int i;
struct Gadget *gad;


if (!(thisstate->es_flags&ES_SMPTE_UNITS)) {
	telluser("Edit:","Set Display Offset requires Display units to be in SMPTE.",0);
	return;
	}

/** Remember current offset in case user presses cancel **/

samptc=(struct TimeCode *)&EI[longest_ei].sfp->tag.parms.starttimecode;
org_offset=*samptc;

/** Initilize gadgets to match samp **/

UpdateGadgetText(ei);

/** Open the edit window **/

NewDisplayOffsetWin.Screen = StudioBase->screen;
NewDisplayOffsetWin.LeftEdge=-1;
NewDisplayOffsetWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&NewDisplayOffsetWin,NULL,NULL,NULL);
if (window==NULL)
	return;

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
		gad=(struct Gadget *)intuimsg->IAddress;
		ReplyMsg(intuimsg);
		if (gad==&str_samp) {
			AsciiToTC(sampSIBuff, samptc);
			UpdateGadgetText(ei);
			RefreshGList(&str_range,window,NULL,1);
			RefreshGList(&str_samp,window,NULL,1);
			}
		else if (gad==&str_range) {
			AsciiToTC(rangeSIBuff, &tc);
			SampToSmpte(&tc2, ei->start_mark, FALSE);
			j=SubTimeCode(TCToInt(&tc),TCToInt(&tc2),StudioBase->defaults.smpte_mode);
			movmem(&j,samptc,sizeof(struct TimeCode));
			UpdateGadgetText(ei);
			RefreshGList(&str_samp,window,NULL,1);
			RefreshGList(&str_range,window,NULL,1);
			}
		}
	if (gadmsg=(struct GadMsg *)GetMsg(okbutn.notifyme)) {
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		break;
		}
	}

	/** copy any changes made **/
if (gadpressed==okbutn.gad_ptr) {
	AsciiToTC(sampSIBuff, samptc);
	i=TCToInt(samptc);
	for (j=0; j < EI_MAX; j++)
		if (EI[j].flags&EI_IN_USE)
			EI[j].sfp->tag.parms.starttimecode=i;
	}
else {
	*samptc=org_offset;	
	}

delete_gadget_chain(window);
DeletePort(okbutn.notifyme);

CloseWind(window);
}

/***********************************************************/

void AsciiToTC(str,tc)
char *str;
int *tc;
{
*tc=TCstr2int(str);
}

/***********************************************************/

void UpdateGadgetText(ei)
struct EditInstance *ei;
{
struct TimeCode tc;

sprintf(sampSIBuff,"%02d:%02d:%02d:%02d",samptc->hours,samptc->minutes,samptc->seconds,samptc->frames);
if (ei->marked_samp) {
	SampToSmpte(&tc, ei->start_mark, TRUE);
	sprintf(rangeSIBuff,"%02d:%02d:%02d:%02d",tc.hours,tc.minutes,tc.seconds,tc.frames);
	}
else {
	strcpy(rangeSIBuff,"No Range");
	}

}

/***********************************************************/

TCToInt(tc)
struct TimeCode *tc;
{
int l;

movmem(tc,&l,sizeof(tc));
return(l);
}
