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
struct Disk_Samp *samp;
struct ActiveSample *as;
struct Region myregion;
struct Module *han;
struct Module *diskio;

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

InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_ATTACH_WORKING_FILES|ISUE_LOAD_STUDIO16BASE,0,0);

/** See if the requested sample is a current studio 16 **/
/** working sample **/

samp = (struct Disk_Samp *)FindSample(argv[1],-1);
if (samp==0) {
	printf("'%s' is not an open Studio 16 sample.\n",argv[1]);
	CloseLibrary(StudioBase);
	exit(10);
	}

/** If the required device driver and utilities are not resident **/
/** load them off disk **/

han=NULL;
diskio=NULL;

if (FindModule("AD1012Handler")==NULL) {
	han=(struct Module *)LoadModule("AD1012Handler");
	if (han)	 {/** Wait until AD1012Handler is listening for all messages **/
		WaitUntilListening(GetEventID("SE_CMD_TRIGGER_PLAYBACK"),han);
			}
	else {
		printf("Can't find module 'AD1012Handler'.\nCheck that studio16: is assigned\n");
		CloseLibrary(StudioBase);
		exit(10);
		}
	}

if (FindModule("DiskIO")==NULL) {
	diskio=(struct Module *)LoadModule("DiskIO");
	if (diskio) {
		WaitUntilListening(GetEventID("SE_CMD_UNLOAD_SAMPLE"),diskio);
		}
	else {
		printf("Can't find module 'AD1012Handler'.\nCheck that studio16: is assigned\n");
		CloseLibrary(StudioBase);
		exit(10);
		}
	}

/** ASOpen() etc. use the Region struct to define what part of the **/
/** sample we should play.  Define a region covering the entire sample **/

myregion.samp=samp;
myregion.start=0;
myregion.end=samp->length-1;
myregion.owner=NULL;
myregion.user_data=NULL;
myregion.parms=samp->parms;

/** Activate the sample for playback & preload the data **/
/** AS_AUTO_DATA means that DiskIO will handle ASRead() requests **/

as=(struct ActiveSample *)ASOpen(&myregion, AS_PLAY|AS_AUTO_DATA);
if (as==0) {
	printf("Error opening sample for playback!\n");
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

ASClose(as);		/** wait for it to finish & close it **/
FreeChan(key);

/** If we loaded AD1012Handler or DiskIO, then unload them **/

if (diskio)
	SendMsg(diskio, GetEventID("SE_CMD_KILL_MODULE"),diskio,0,0,0,0,0);

if (han) {
	/** Before unloading handler, let AD1012's onboard 4K buffer empty **/
	Delay((50*4096)/samp->parms.rate);	
	SendMsg(han, GetEventID("SE_CMD_KILL_MODULE"),han,0,0,0,0,0);
	}

CloseLibrary(StudioBase);
}
