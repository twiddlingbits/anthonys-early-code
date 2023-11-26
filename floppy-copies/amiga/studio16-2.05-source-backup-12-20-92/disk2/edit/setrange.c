#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

#include "setrange.pwc"

static struct NewButton okbutnguide = 
	{NULL,10,43,44,53,BUTN_COOL,0,0,0,0,0,0,"OK"};

static struct NewButton cancelbutnguide = 
	{&okbutnguide,130,43,186,53,BUTN_COOL,0,0,0,0,0,0,"CANCEL"};

extern struct StudioBase *StudioBase;
extern struct EditInstance EI[EI_MAX];
extern short longest_ei;
void ValRng();
void StrToInt();
void IntToStr();
void ExpandString();

/***********************************************************/

void SetRange()
{
struct Window *window;
struct IntuiMessage *intuimsg;
struct GadMsg *gadmsg;
struct GadgetHeader *gadpressed;
extern struct EditState *thisstate;
int sm,em;
short i;

/** Initilize gadgets to match samp **/

if (EI[longest_ei].marked_samp) {
	IntToStr(EI[longest_ei].start_mark,EI[longest_ei].end_mark);
	}
else {
	if (thisstate->es_flags&ES_SMPTE_UNITS) {
		sprintf(sm_nameSIBuff,"00:00:00:00");
		sprintf(em_nameSIBuff,"00:00:00:00");
		}
	else {
		sprintf(sm_nameSIBuff,"0");
		sprintf(em_nameSIBuff,"0");
		}
	}

/** Open the edit window **/

NewSetRangeWin.Screen = StudioBase->screen;
NewSetRangeWin.LeftEdge=-1;
NewSetRangeWin.TopEdge=-1;
window=(struct Window *)OpenNewWind(&NewSetRangeWin,NULL,NULL,NULL);
if (window==NULL)
	return;

cancelbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.c = &StudioBase->defaults.colors;
okbutnguide.notifyme=(struct MsgPort *)CreatePort(0,0);
cancelbutnguide.notifyme=okbutnguide.notifyme;
create_gadget_chain(window, &cancelbutnguide);

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) {
	Wait((1<<window->UserPort->mp_SigBit)|(1<<okbutnguide.notifyme->mp_SigBit));

	while (intuimsg=(struct IntuiMessage *)GetMsg(window->UserPort)) {
		HandleIntuitMess(window,intuimsg);
		ReplyMsg(intuimsg);
		if (thisstate->es_flags&ES_SMPTE_UNITS) {
			ExpandString(sm_nameSIBuff);
			ExpandString(em_nameSIBuff);
			RefreshGList(&str_em,window,NULL,1);
			RefreshGList(&str_sm,window,NULL,1);
			}
		}

	if (gadmsg=(struct GadMsg *)GetMsg(okbutnguide.notifyme)) {
		gadpressed=gadmsg->g;
		ReplyMsg(gadmsg);
		break;
		}
	}


if (gadpressed==okbutnguide.gad_ptr) {
	StrToInt(&sm,&em);
	for (i=0; i < EI_MAX; i++)
		if (EI[i].flags&EI_IN_USE) {
			EI[i].marked_samp = TRUE;
			EI[i].start_mark=sm;
			EI[i].end_mark=em;
			}
	}

delete_gadget_chain(window);
DeletePort(okbutnguide.notifyme);

CloseWind(window);
}

/***********************************************************/

void ExpandString(str)
char *str;
{
struct TimeCode tc1;
long j;

j=TCstr2int(str);
movmem(&j,&tc1,4);
sprintf(str,"%02d:%02d:%02d:%02d",
	tc1.hours,tc1.minutes,tc1.seconds,tc1.frames);
}

/***********************************************************/

void IntToStr(sm,em)
int sm,em;
{
struct TimeCode tc1,tc2;

if (thisstate->es_flags&ES_SMPTE_UNITS) {
	SampToSmpte(&tc1, sm, TRUE);
	SampToSmpte(&tc2, em, TRUE);
	sprintf(sm_nameSIBuff,"%02d:%02d:%02d:%02d",
		tc1.hours,tc1.minutes,tc1.seconds,tc1.frames);
	sprintf(em_nameSIBuff,"%02d:%02d:%02d:%02d",
		tc2.hours,tc2.minutes,tc2.seconds,tc2.frames);
	}
else {
	sprintf(sm_nameSIBuff,"%d",sm);
	sprintf(em_nameSIBuff,"%d",em);
	}
}

/***********************************************************/

void StrToInt(sm,em)
int *sm,*em;
{
int temp,j;

if (thisstate->es_flags&ES_SMPTE_UNITS) {
	j=TCstr2int(sm_nameSIBuff);
	*sm=SmpteToSamp(&j);
temp=*sm;
	j=TCstr2int(em_nameSIBuff);
	*em=SmpteToSamp(&j);
	}
else {
	sscanf(sm_nameSIBuff,"%d",sm);	
	sscanf(em_nameSIBuff,"%d",em);
	}

ValRng(sm, EI[longest_ei].sfp->tag.length-1);
ValRng(em, EI[longest_ei].sfp->tag.length-1);

if (*sm > *em) {
	temp=*sm;
	*sm=*em;
	*em=temp;
	}
}

/***********************************************************/

void ValRng(val, top)
int *val;
int top;
{
if (*val < 0)
	*val = 0;
if (*val > top)
	*val=top;
}

/***********************************************************/

int SmpteToSamp(tc)
struct TimeCode *tc;
{
float rate,numsamps;
int frames;
int *p1,*p2;
int result;
int ret;

if (EI[longest_ei].sfp->tag.parms.smpte_sampling_rate==0)
	rate=EI[longest_ei].sfp->tag.parms.rate;
else
	rate=EI[longest_ei].sfp->tag.parms.smpte_sampling_rate;

if (rate==0)
	rate=44100;		/** should never occur, but just in case avoid /0 **/

p1=(int *)tc;
p2=(int *)(&EI[longest_ei].sfp->tag.parms.starttimecode);
result=SubTimeCode(*p1,*p2,StudioBase->defaults.smpte_mode);
movmem(&result,tc,sizeof(result));

frames=TC2Frm(result,StudioBase->defaults.smpte_mode);
numsamps=(float)frames*rate;
if (StudioBase->defaults.smpte_mode) {
	ret=(int)(numsamps/(float)StudioBase->defaults.smpte_mode+0.9999);  /** Round up **/
/****
sprintf(bug,"frames %d framesXrate%f  rate %f samps %d\n",frames,numsamps,rate,ret);
kprintf("SETR %s",bug);
*****/
	}
else
	ret=(int)(numsamps/29.97+.9999);

/*if (StudioBase->defaults.smpte_mode) ret=(int)((numsamps+StudioBase->defaults.smpte_mode-1)/StudioBase->defaults.smpte_mode);
else ret=(int)((((float)numsamps)+28.97)/29.97);*/

return(ret);
}

