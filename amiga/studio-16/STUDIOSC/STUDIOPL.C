#include <stdlib.h>
#include <stdio.h>
#include <proto/exec.h>

#include "exec/types.h"
#include "exec/exec.h"
#include "include/studio16.h"

/** Studio 16 cli play command.  Anthony J. Wood Oct 27, 1991 **
/** This program will play a sound from the SHELL by using Studio16 **/
/** Calls.  The program will also load the AD1012 device driver **/
/** and the DiskIO utility module if they are not currently loaded **/

struct StudioBase *StudioBase;

short GetEventID();

void main(int argc,char *argv[])	
{
	struct ChanKey *key1,*key2;
	struct ActiveSample *as[3];
	struct Region region1,region2;
	int we_are_initing;

	if (argc != 3)
	{
		printf("usage: StudioPlay <samplename1> <samplename2>\n");
		exit(10);
	}

	StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
	if (StudioBase==NULL)
	{
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

	setmem(&region1, sizeof(struct Region), 0);
	strcpy(region1.samp_name,argv[1]);

	setmem(&region2, sizeof(struct Region), 0);
	strcpy(region2.samp_name,argv[2]);
	
/** Activate the sample for playback & preload the data **/
/** AS_AUTO_DATA means that DiskIO will handle ASRead() requests **/

	as[0]=(struct ActiveSample *)ASOpen(&region1, AS_PLAY|AS_AUTO_DATA);
	if (as[0] == NULL)
	{
		printf("Error opening sample 1 for playback!\n");
		if (we_are_initing)
			CloseAllModules(FALSE);
		CloseLibrary((struct Library *)StudioBase);
		exit(10);
	}

	as[1]=(struct ActiveSample *)ASOpen(&region2, AS_PLAY|AS_AUTO_DATA);
	if (as[1] == NULL)
	{
		printf("Error opening sample 2  for playback!\n");
		if (we_are_initing)
			CloseAllModules(FALSE);
		CloseLibrary((struct Library *)StudioBase);
		exit(10);
	}

/** Allocate any free channel for playback on **/

	as[0]->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
	key1=as[0]->key;
	as[1]->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
	key2=as[1]->key;

	as[2] = NULL;

	if ((key1 != NULL) && (key2 != NULL))
		ASTriggerList(as,TRIGGER_LIST_MANUAL_SYNC);
	else
		printf("Can't Allocate first play channel!\n");

/** ASClose() will wait untill the sample is finished playing **/
/** before it frees the buffers, etc. **/

//	rate=as1->region.parms.rate;
	ASClose(as[0]);		/** wait for it to finish & close it **/
	ASClose(as[1]);
	FreeChan(key1);
	FreeChan(key2);

	if (we_are_initing)
		CloseAllModules(FALSE);

	CloseLibrary((struct Library *)StudioBase);
}
