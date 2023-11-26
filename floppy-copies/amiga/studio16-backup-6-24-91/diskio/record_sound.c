#include "/include/psound.h"
#include "intuition/intuition.h"

static struct NewWindow RecNW = {
   320,
   100,
   330,
   25,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG,
   0,0,
   NULL, /** TITLE **/
   0,0,
   154,67,640,400,
   CUSTOMSCREEN
   };

void HideWind();
void DisplayWind();

static struct Window *wind;

static int recording=FALSE;
struct Disk_Samp *recording_samp;

extern short se_cmd_record_sample;
extern short se_cmd_stop_recording;
extern short se_cmd_begin_recording;
extern short se_cmd_stop_recording;
extern short se_info_record_data;

extern struct studioBase *studioBase;

/************************************************************************/
record_sound(name, rate)
char *name;
int rate;
{
struct StudioEvent *GetMsg();
int i;

	recording = TRUE;
	i=CreateSample(name, 0, &recording_samp);

	if (i!=NO_ERROR) 
		{
		recording=FALSE;
		return(i);
		}

	recording_samp->rate=rate;
	recording_samp->type=RIGHT;
	WriteLockSamp(recording_samp);

	BroadcastEventParms(se_cmd_begin_recording, 3, recording_samp->rate, studioBase->bigblock[0][3], studioBase->bigblock[1][3], BIG_BLOCK_SIZE,0); 

return(NO_ERROR);
}
/************************************************************************/
void stop_record_sound()
{
BroadcastEventParms(se_cmd_stop_recording, 3 ,0, 0, 0, 0, 0);
}
/************************************************************************/

void msg_record_data(msgtorep)
struct StudioEvent *msgtorep;
{
extern struct list *open_list;

if (recording)
	{
	QuickWrite(recording_samp, msgtorep->arg1, msgtorep->arg2);
	if (msgtorep->flags & LAST_BLOCK) {
		recording=FALSE;
		WriteUnLockSamp(recording_samp);
		}
	}
}
/**********************************************************************/
void DisplayWind()
{
struct Window *OpenWindow();

RecNW.Screen = studioBase->screen;

if ((wind = OpenWindow(&RecNW))==0)  wind=NULL;
}
/*****************************************************************/

void HideWind()
{
if (wind)
	{
	CloseWindow(wind);
	wind=NULL;
	}
}

/**************************************************************/
