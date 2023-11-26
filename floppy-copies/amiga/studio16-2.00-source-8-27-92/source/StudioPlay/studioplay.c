#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

/** Studio 16 cli play command.  Anthony J. Wood Oct 27, 1991 **
/** This program will play a sound from the SHELL by using Studio16 **/
/** Calls.  The program will also load the AD1012 device driver **/
/** and the DiskIO utility module if they are not currently loaded **/

struct StudioBase *StudioBase;

short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
struct ChanKey *key;
struct ActiveSample *as;
struct Region myregion;
int rate,we_are_initing;

if (argc!=2) {
	printf("usage: StudioPlay <samplename>\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't open studio.library\n");
	exit(10);
	}

/** If this is the first time studio.library is opened, we **/
/** need to tell studio.library to go find all the working **/
/** samples in the audio: directory and assign studio16: **/

if (StudioBase->LibNode.lib_OpenCnt==1)
	we_are_initing=TRUE; 
else
	we_are_initing=FALSE;

InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE|ISUE_LOAD_DRIVERS,0,0);

/** ASOpen() etc. use the Region struct to define what part of the **/
/** sample we should play.  Define a region covering the entire sample **/
/** can be done by setting it to zero.  ASOpen() will fill end,etc. **/
setmem(&myregion, sizeof(struct Region), 0);
strcpy(myregion.samp_name,argv[1]);
	
/** Activate the sample for playback & preload the data **/
/** AS_AUTO_DATA means that DiskIO will handle ASRead() requests **/

as=(struct ActiveSample *)ASOpen(&myregion, AS_PLAY|AS_AUTO_DATA);
if (as==0) {
	printf("Error opening sample for playback!\n");
	if (we_are_initing)
		CloseAllModules(FALSE);
	CloseLibrary(StudioBase);
	exit(10);
	}

/** Allocate any free channel for playback on **/

as->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
key=as->key;
if (key)
	ASTrigger(as);		/** start playback **/
else
	printf("Can't Allocate a play channel!\n");

/** ASClose() will wait untill the sample is finished playing **/
/** before it frees the buffers, etc. **/

rate=as->region.parms.rate;
ASClose(as);		/** wait for it to finish & close it **/
Delay((50*6144)/rate);	/** AD1012Handler bug (wait for card buffers to empty) **/
FreeChan(key);
if (we_are_initing)
	CloseAllModules(FALSE);
CloseLibrary(StudioBase);
}
