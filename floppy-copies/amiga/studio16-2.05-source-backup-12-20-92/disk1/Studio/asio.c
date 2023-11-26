#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <devices/audio.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include "/include/studio16.h"

/** Need to add: ASPreload,	ASF_NO_PRELOAD **/

extern struct StudioBase *StudioBase;

extern short se_cmd_trigger_record;
extern short se_cmd_trigger_playback;
extern short se_cmd_stop_recording;
extern short se_cmd_stop_playing;
extern short se_info_playback_done;
extern short se_info_record_done;
extern short se_info_preload_done;
extern short se_info_presave_done;
extern short se_info_block_recorded;
extern short se_info_block_played;

struct BigBlock *AllocBigBlock();
void FreeBigBlock();
int FreeActiveSample();
int ASFlush();
void ASWaitStop();
void ASkprint();

/*############################################################
NAME	
	ASOpen
FUNCTION
	Prepares a sample for record or playback.	Returns
	an ActiveSample pointer which you use like a file pointer.
	See studio16.h for the region structure.	You will need to
	fill in the following paramaters:
		myregion.samp_name	Sample name to play/record from/to
		myregion.start			First sample to play/record on top of
		myregion.end			Last sample
		myregion.parms			Sampling rate, etc. to use.
		myregion.user_data	Use for what ever you want

	You must also pass a flags variable. Either AS_PLAY or AS_RECORD
	must be selected.	 You can also select:

	AS_AUTO_DATA	if you want diskio to automatically read/write data
						to disk.	 This flag is usually set.
	AS_AUTO_CLOSE	if you want ASClose() to be called by diskio after
						playback/record completes.	 During play, ASClose()
						will be called automatically after the sound
						finishes playing naturally or by ASStop().
						ASClose() is called during record after ASStop().
	AS_AUTO_FREECHAN	This will cause "FreeChan(as->key)" to be made
							automaticlly upon play/record completion,
							or a call to ASClose().

	ASOpen allocates the ActiveSample struct and the four
	buffers used during record/playback.  If the sample is
	for playback, ASRead() is called four times to prefill the
	buffers.
INPUTS
	struct Region *
	flags (AS_RECORD, AS_PLAY, etc.)
RESULTS
	NULL if an error or ActiveSample *
##############################################################*/

struct ActiveSample *ASOpen(region, flags)
struct Region *region;
unsigned long flags;
{
struct ActiveSample *as;
short i;
struct BigBlock *bb;

as = (struct ActiveSample *)AllocMem(sizeof(struct ActiveSample),MEMF_CLEAR);
if (as==0)
	{
	return(0);
	}

as->region=*region;
NewList(&as->EmptyBlocks);
NewList(&as->FullBlocks);
InitSemaphore(&as->rwlock);

for (i=0; i < 4; i++) {
	bb=AllocBigBlock();
	if (bb) {
		AddTail(&as->EmptyBlocks, bb);
		}
	else {
		FreeActiveSample(as);
		return(0);
		}
	}

as->error=NO_ERROR;
as->flags=flags;
as->skip_amt=0;

if (flags&AS_RECORD) {
	as->sp = (struct SampleFilePointer *)SampleOpen(region->samp_name,SP_MODE_READWRITE,0);  /** WAS MODE_NEWFILE in ver 1.0 **/
	if (as->sp) as->block_align=SampleQueryAlign(as->sp);		/** See ASRead() for docs **/
	}
else {	/** Assume AS_PLAY **/
	as->sp = (struct SampleFilePointer *)SampleOpen(region->samp_name,SP_MODE_OLDFILE,0);
	}

if (as->sp==0) {
	FreeActiveSample(as);
	return(0);
	}

/** Setup default values if NULLS are passed--asking for defaults **/

if (as->region.parms.volume==0 && as->region.parms.rate==0) {
	as->region.parms=as->sp->tag.parms;
	}
if (as->region.start==0 && as->region.end==0) {
	as->region.end=as->sp->tag.length-1;
	}

/** Saftey check **/

if (flags&AS_PLAY && (as->region.start-as->region.end) < 2) {
	FreeActiveSample(as);
	return(0);
	}

/** Seek to beggining of file **/

SampleSeek(as->sp,as->region.start,SS_OFFSET_BEGINNING);			/* start is in samples */
as->next_write_pos=SampleSeek(as->sp, 0L, SS_OFFSET_CURRENT);

if (flags&AS_RECORD) {
	BroadcastEventParms(se_info_presave_done, as,0,0,0,as->region.user_data,0);
	}
else {
	i=ASCountBlocks(as, ASCB_EMPTY_BLOCKS);
	while (i--) {
		if (ASRead(as)!=NO_ERROR)
			break;
		}
	if (as->error==NO_ERROR)
		BroadcastEventParms(se_info_preload_done, as,0,0,0,as->region.user_data,0);
	else {
		FreeActiveSample(as);
		as=NULL;
		}
	}
return(as);
}

/*************************************************************/

struct BigBlock *AllocBigBlock()
{
struct BigBlock *bb;

bb=(struct BigBlock *)AllocMem(sizeof(struct BigBlock),MEMF_CLEAR);
if (bb) {
	bb->alloclen=StudioBase->defaults.active_buf_size;			/** in samples (words) ! **/
	bb->allocdata = (short *)AllocDmaMem(bb->alloclen*2+4);	/** +4 is so we can move **/
	if (bb->allocdata==NULL) {											/** bb->data to a long word boundry **/
		FreeMem(bb, sizeof(struct BigBlock));
		return(NULL);
		}
	}
return(bb);
}

/*************************************************************/

void FreeBigBlock(bb)
struct BigBlock *bb;
{
FreeMem(bb->allocdata, bb->alloclen*2+4);
FreeMem(bb, sizeof(struct BigBlock));
}

/*************************************************************/

int FreeActiveSample(as)
struct ActiveSample *as;
{
struct BigBlock *bb;
struct Node *node;
int reterr;

reterr=NO_ERROR;

if (as->sp)
	reterr=SampleClose(as->sp);

ASFlush(as, 0);	/** make sure all blocks are freed **/

node=as->EmptyBlocks.lh_Head;
while (node->ln_Succ) {
	bb=(struct BigBlock *)node;
	node = node->ln_Succ;
	Remove(bb);
	FreeBigBlock(bb);
	}

SlowFreeMem(as, sizeof(struct ActiveSample));
return(reterr);
}

/*############################################################
NAME	
	ASWaitStop()
FUNCTION
	Waits for the triggered sample to stop playing/recording
INPUTS
	ActiveSample struct
RESULTS
	NONE
##############################################################*/

void ASWaitStop(as)
struct ActiveSample *as;
{
while (as->flags&AS_IS_ACTIVE)	/** Flag cleared in diskio when channel_done received **/
	WaitTOF();
}

/*############################################################
NAME	
	ASClose
FUNCTION
	Frees an ActiveSample struct and its buffers.  Also closes
	the associated file.
INPUTS
	ActiveSample struct
RESULTS
	zero if no error
##############################################################*/


int ASClose(as)
struct ActiveSample *as;
{
int reterr;

/** Make sure sample is not still playing **/

ASWaitStop(as);	/** wait for play/record to finish **/

if (as->sp->flags&SP_MODE_OLDFILE) {
	BroadcastEventParms(se_info_playback_done,0,as->key,as->region.user_data,0,0,0);
	}
else {		/** Flush any unwritten recorded blocks to disk **/
	Forbid();
	if (as->HandlerBlock) {	 /** stoped mid block? **/
		as->HandlerBlock->islen=as->HandlerBlock->xferlen;
		AddTail(&as->FullBlocks, as->HandlerBlock);
		as->HandlerBlock=NULL;
		}
	Permit();
	ASWrite(as);  /** write last block to disk **/
	as->sp->tag.parms=as->region.parms;
	BroadcastEventParms(se_info_record_done,0,as->key,as->region.user_data,0,0,0);
	}

if (as->flags&AS_AUTO_FREECHAN) {
	if (as->key) {
		FreeChan(as->key);
		as->key=NULL;
		}
	}

reterr=FreeActiveSample(as);
return(reterr);
}

/*############################################################
NAME	
	ASRead
FUNCTION
	Reads one "big_block" from disk.	 This is normally called
	by system routines, such as diskio.
INPUTS
	Active Sample struct
RESULTS
	standard error code (NO_ERROR, etc.)
##############################################################*/

int ASRead(as)
struct ActiveSample *as;
{
int blklen;
int readlen,totalread;
long skip;
struct BigBlock *bb;
struct Node *node;

ObtainSemaphore(&as->rwlock);
/*kprintf("ASRead() IN\n");*/
Forbid();
node=as->EmptyBlocks.lh_Head;
if (node->ln_Succ==NULL) { /** Chanage this to alloc another bigblock? **/
	Permit();
	as->error=READ_ERROR;
/*kprintf("ASRead() OUT3\n");*/
	ReleaseSemaphore(&as->rwlock);
	return(as->error);
	}

bb=(struct BigBlock *)node;

/** Does ASSeek() want us to skip some samples? **/

skip=as->skip_amt;	
as->skip_amt=0;		
Permit();

if (skip) {
	SampleSeek(as->sp, skip, SS_OFFSET_CURRENT);
	}
bb->samp_pos=SampleSeek(as->sp, 0L, SS_OFFSET_CURRENT);
/*kprintf("ASRead is at %04x%04x\n",bb->samp_pos);*/
bb->xferlen=0;
bb->flags=0;

if (bb->samp_pos+bb->alloclen > as->region.end) {
	blklen =	 as->region.end - bb->samp_pos+1;
	bb->flags|=BBF_EOF;
/*kprintf("EOF !\n");*/
	}
else { 
	blklen = bb->alloclen;
	}

if (as->error==NO_ERROR) {
	/** Allign buffer boundray on same type as seek position.  **/
	/** for example, if current file positon is 1003, then set **/
	/** data to point to a byte boundry (allocdata+1).  **/
	/** This allows FFS to DMA the bulk of the read to a longword */
	/** alligned address. **/
	bb->data=(short *)((char *)bb->allocdata+SampleQueryAlign(as->sp));
	readlen=SampleRead(as->sp,bb->data,blklen);
/*kprintf("blklen %04x%04x readlen %04x%04x\n",blklen,readlen);*/
/*kprintf("allign %0x%04x samppos %04x%04x\n",SampleQueryAlign(as->sp),bb->samp_pos);*/
	bb->islen=readlen;
	if (readlen!=blklen) {
		as->error=READ_ERROR;
		}

	if (readlen) {
		if ((bb->flags&BBF_EOF) && as->region.loop) {
			bb->flags&=(~BBF_EOF);
			totalread=readlen;
			while (totalread!=bb->alloclen && as->region.loop-- && as->error==NO_ERROR) {
				SampleSeek(as->sp,as->region.start,SS_OFFSET_BEGINNING);
				blklen =	 min(as->region.end-as->region.start+1,bb->alloclen-totalread);
				readlen=SampleRead(as->sp,bb->data+totalread,blklen);
				if (readlen!=blklen)
					as->error=READ_ERROR;
				totalread+=readlen;
				}
			bb->islen=totalread;
			}
		if (as->region.effect) /** Cheap way to do real time effects **/
			(*as->region.effect)(bb->data,bb->islen); /** Used by 'try' echo in edit **/
		Forbid();
		Remove(node);			/** take out of EmptyBlocks **/
		AddTail(&as->FullBlocks, bb);
		Permit();
		}
	else {
/*kprintf("ASRead() OUT2\n");*/
		ReleaseSemaphore(&as->rwlock);
		return(STDERR_DATA_EOF);
		}
	}
/*kprintf("ASRead() OUT1\n");*/
ReleaseSemaphore(&as->rwlock);
return(as->error);
}

/*############################################################
NAME	
	ASWrite
FUNCTION
	Writes one big block of data to disk
INPUTS
	ActiveSample struct
RESULTS
	standard error code like NO_ERROR
##############################################################*/

int ASWrite(as)
struct ActiveSample *as;
{
long writtenlen;
struct BigBlock *bb;
struct Node *node;

/*ObtainSemaphore(&as->rwlock);*/
Forbid();
node=as->FullBlocks.lh_Head;
if (node->ln_Succ==NULL) {
	Permit();
	as->error=WRITE_ERROR;	/**Nothing to write!!!?? **/
/*	ReleaseSemaphore(&as->rwlock);*/
	return(as->error);
	}
Remove(node);
Permit();

bb=(struct BigBlock *)node;

if (as->error==NO_ERROR) {
	writtenlen=SampleWrite(as->sp,bb->data,bb->islen);
	if (writtenlen!=bb->islen) {
		as->error=WRITE_ERROR;
		}
	bb->islen=writtenlen;
	as->next_write_pos=SampleSeek(as->sp, 0L, SS_OFFSET_CURRENT);
	}
else {
	bb->islen=0;
	}

Forbid();
AddTail(&as->EmptyBlocks, bb);
Permit();
/*ReleaseSemaphore(&as->rwlock);*/
return(as->error);
}

/*############################################################
NAME	
	ASTrigger
FUNCTION
	broadcasts se_cmd_trigger_record or se_cmd_trigger_playback
	as required
INPUTS
	ActiveSample struct
RESULTS
	NONE
##############################################################*/

void ASTrigger(as)
struct ActiveSample *as;
{
as->flags |= AS_IS_ACTIVE;
if (as->flags&AS_RECORD)
	BroadcastEventParms(se_cmd_trigger_record,as,NULL,0,0,0,0);
else	/** assume AS_PLAY **/
	BroadcastEventParms(se_cmd_trigger_playback,as,NULL,0,0,0,0);
}
/*############################################################
NAME	
	ASTriggerList
FUNCTION
	broadcasts se_cmd_trigger_record or se_cmd_trigger_playback
	as required
INPUTS
	NULL terminated array of ActiveSample struct pointers
	flags
RESULTS
	NONE
##############################################################*/

void ASTriggerList(as,flags)
struct ActiveSample *as[];
{
int x;
struct AudioHandler *hand=NULL;

x=0;
while(as[x]) as[x++]->flags |= AS_IS_ACTIVE;

hand = (struct AudioHandler *) FindAudioHandler(NULL,-1,0,NULL,-1);
while(hand)
	{
	(*hand->TriggerList)(as,flags);
	hand = (struct AudioHandler *) FindAudioHandler(hand,-1,0,NULL,-1);
	}

/*** ADD STARTPLAYBACK STUFF HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****/

}

/*############################################################
NAME	
	ASStop
FUNCTION
	Broadcasts se_cmd_stop_recording or se_cmd_stop_playback
	as required and waits for the sample to stop play/record.
INPUTS
	ActiveSample struct
RESULTS
	NONE
##############################################################*/

void ASStop(as)
struct ActiveSample *as;
{
if (as->flags&AS_RECORD)
	BroadcastEventParms(se_cmd_stop_recording,as->key,0,0,0,0,0);
else	/** assume AS_PLAY **/
	BroadcastEventParms(se_cmd_stop_playing,as->key,0,0,0,0,0);
ASWaitStop(as);	/** Wait for message to process **/
}

/*############################################################
NAME	
	ASSeek
FUNCTION
	Seeks within a sample "ASOpen()"'ed.  Currently only works
	for AS_PLAY and not AS_RECORD samps.
	Can be called before or after ASTrigger(). However, the
	ASSeek does not take effect until the next Block of data
	is read (i.e, the next ASRead()).  Furthermore, the next
	ASRead() is generally three BigBlock's worth of data behind
	the data currently playing.  For Seeks to occur immediately,
	use:
		numflushed=ASFlush(as,0);
		ASSeek(as, x, AS_XX);
		ASQueueReads(as, numflushed);

	However, this example is reliable if the sample is not yet
	triggered.
INPUTS
	as			ActiveSample *
	numsamps Seek this many samples
	mode		numsamps is relative (ASS_OFFSET_CURRENT) or
				absolute (ASS_OFFET_BEGINNING)
RESULTS
	NONE
##############################################################*/

int ASSeek(as, numsamps, mode)
struct ActiveSample *as;
long numsamps;
long mode;
{
Forbid();
if (mode&ASS_OFFSET_BEGINNING) {
	as->skip_amt=numsamps-SampleSeek(as->sp, 0, SS_OFFSET_CURRENT);
	}
else {
	as->skip_amt+=numsamps;
	}
Permit();
return(0);
}

/*############################################################
NAME	
	ASHandlerQueryPlayData
FUNCTION
	This function is called by audio handlers to access audio
	data.	 Typically, a handler waits for a Trigger message to
	start sound playback.  The handler then starts making calls
	to this function to get the data to play.	 These calls
	occur untill ASHandlerQueryPlayData() returns  STDERR_DATA_EOF.
	Data is available on the the EOF call.	 
INPUTS
	as		an Open ActiveSample
	len	The number of samples the handler wants
RESULTS
	actual_len	This is the number of samples available.	This
					will always match the 'len' except for two cases:
					1. The EOF call
					2. If your len is not evenly divisible into the 
						User's BigBlock size.  If you use 1K for len
						this will never happen.
	data			short *.	 Data buffer of length "actual_len"
RETURNS
	Standard error code: NO_ERROR, STDERR_DATA_NOT_AVAILABLE_YET,
	STDERR_DATA_EOF.	 If "STDERR_DATA_NOT_AVAILABLE_YET" is returned
	the caller should pause (e.g. WaitTOF()) then make the call
	again.
	The error code could be an error such as READ_ERROR.
##############################################################*/

int ASHandlerQueryPlayData(as, len, actual_len, data)
struct ActiveSample *as;
unsigned long len;
unsigned long *actual_len;
short **data;
{
struct BigBlock *bb;
struct Node *node;

/*kprintf(".");*/
Forbid();
if (as->HandlerBlock==NULL) {
	node=as->FullBlocks.lh_Head;
	if (node->ln_Succ==NULL) { 
		Permit();
/*kprintf("1");*/
		return(STDERR_DATA_NOT_AVAILABLE_YET);
		}
/*kprintf("SWAP");*/
	Remove(node);
	bb=(struct BigBlock *)node;
	as->HandlerBlock=bb;
	}
else {
	bb=as->HandlerBlock;
	}
Permit();

*data=&(bb->data[bb->xferlen]);
bb->xferlen+=len;
*actual_len=len;
as->samp_pos=bb->samp_pos;
as->xferlen=bb->xferlen;

if (bb->xferlen>=bb->islen) {
	*actual_len=bb->islen-(bb->xferlen-len);
	Forbid();
	as->HandlerBlock=NULL;
	AddTail(&as->EmptyBlocks, bb);
	Permit();
	if (bb->flags&BBF_EOF) {
/*kprintf("2");*/
		return(STDERR_DATA_EOF);
		}
	else { 
/*kprintf("Block Played!\n");*/
/*kprintf("pos %04x%04x smpte %04x%04x\n",as->position,QuerySmpteTime());*/
/*kprintf("B");*/
		BroadcastEventParms(se_info_block_played, as, 0, 0,0,0,0);
		}
	}
/*kprintf("3");*/
return(as->error);
}

/*############################################################
NAME	
	ASHandlerQueryRecordData()

FUNCTION
	This function is called by audio handlers to access audio
	data buffers.	Typically, a handler waits for a Trigger message to
	start sound record.	The handler then starts making calls
	to this function to get the buffer where record data is stored
	in.  
INPUTS
	as		an Open ActiveSample
	len	The number of samples the handler wants
RESULTS
	actual_len	This is the size of the available buffer.	 This
					will always match the 'len' except for one case:
						If your len is not evenly divisible into the 
						User's BigBlock size.  If you use 1K for len
						this will never happen.
	data			short *.	 Data buffer of length "actual_len"
RETURNS
	Standard error code: NO_ERROR, STDERR_DATA_NOT_AVAILABLE_YET,
	STDERR_DATA_EOF.	 If "STDERR_DATA_NOT_AVAILABLE_YET" is returned
	the caller should pause (e.g. WaitTOF()) then make the call
	again.
##############################################################*/

int ASHandlerQueryRecordData(as, len, actual_len, data)
struct ActiveSample *as;
unsigned long len;
unsigned long *actual_len;
short **data;
{
struct BigBlock *bb;
struct Node *node;

Forbid();
if (as->HandlerBlock==NULL) {
	node=as->EmptyBlocks.lh_Head;
	if (node->ln_Succ==NULL) { 
		Permit();
		return(STDERR_DATA_NOT_AVAILABLE_YET);
		}
	Remove(node);
	bb=(struct BigBlock *)node;
	as->HandlerBlock=bb;
	/** assign buffer so that its memory alignment matches files **/
	bb->data=(short *)((char *)bb->allocdata+as->block_align);
	/** determine alignment of next block **/
	as->block_align=(as->block_align+bb->alloclen)&3;
	Permit();
	bb->xferlen=0;
	bb->flags=0;
	bb->islen=bb->alloclen;
	bb->samp_pos=as->next_write_pos;
	}
else {	
	bb=as->HandlerBlock;
	Permit();
	}

*data=&(bb->data[bb->xferlen]);
bb->xferlen+=len;
*actual_len=len;
as->samp_pos=bb->samp_pos;
as->xferlen=bb->xferlen;

if (bb->xferlen>=bb->islen) {
	*actual_len=bb->islen-(bb->xferlen-len);
	Forbid();
	as->HandlerBlock=NULL;
	AddTail(&as->FullBlocks, bb);
	Permit();
	BroadcastEventParms(se_info_block_recorded,as,0, 0,0,0,0);
	}
return(as->error);
}

/*############################################################
NAME	
	ASFlush
FUNCTION
	Flush all BigBlock full buffers except for the number passed
INPUTS
	keep	Number of BigBlock Buffers to keep.  If greater than
			the number of buffers full, then no buffers are flushed
RESULTS
	The number of buffers flushed
##############################################################*/

int ASFlush(as, keep)
struct ActiveSample *as;
int keep;
{
struct Node *node;
struct BigBlock *bb;
int i;
int flush_count;

/*kprintf("ASFlush() IN\n");*/
flush_count=0;
Forbid();
if (as->HandlerBlock) {
	if (keep==0) {
		bb=as->HandlerBlock;
		as->HandlerBlock=NULL;
		AddTail(&as->EmptyBlocks, bb);
		keep=1;
		flush_count++;
		}
	}
else {
	keep++;	/** if no HandlerBlock, keep an extra FullBlock **/
	}

do {
	i=0;
	for (node=as->FullBlocks.lh_Head; node->ln_Succ; node = node->ln_Succ) {
		bb=(struct BigBlock *)node;
		i++;
		}

	if (i >= keep) {
		flush_count++;
		Remove(bb);
		AddTail(&as->EmptyBlocks, bb);
		}
	} while (i >= keep);
Permit();
/*kprintf("ASFlush() OUT\n");*/
/*kprintf("ASFlush(%x%x)\n",flush_count);*/
return(flush_count);
}

/*############################################################
NAME	
	ASPos
FUNCTION
	For playback, determine the approximate sample number, relative
	to the beginning of the sample that is playing.  For the AD1012
	this is can by off by as much as 6K.
	For record, ASPos returns the approximate number of bytes recorded
	to disk so far.
INPUTS
	as		ActiveSample pointer
RESULTS
	Position in samples
##############################################################*/

unsigned long ASPos(as)
struct ActiveSample *as;
{
unsigned long pos;

/** ASPos() has to handle looping **/

Forbid();
if (as->samp_pos+as->xferlen > as->region.end) {
	pos=as->samp_pos+as->xferlen-as->region.end;
	pos=pos%(as->region.end-as->region.start+1);
	pos=pos+as->region.start;
	}
else {
	pos=as->samp_pos+as->xferlen;
	}
Permit();
return(pos);
}

/*############################################################
NAME	
	ASQueueReads
FUNCTION
	Tell Diskio to call ASRead() a given number of times
INPUTS
	as		ActiveSample pointer
RESULTS
	NONE
##############################################################*/

void ASQueueReads(as, num)
struct ActiveSample *as;
int num;
{
while (num--) {
	BroadcastEventParms(se_info_block_played, as, 0, 0,0,0,0);
	}
}

/*############################################################
NAME	
	ASCountBlocks
FUNCTION
	Counts the number of indicated big blocks.  For example, if
	you have triggered a sound, waited for it to finish, and
	you want to rewind and trigger it again, you could use:

		ASWaitStop(as);
		ASFlush(as, 0);	** probably not needed **
		ASSeek(as, 0, ASS_OFFSET_BEGINNNING);
		k=ASCountBlocks(as, ASCB_EMPTY_BLOCKS);
		while (k--)
			ASRead(as);
		ASTrigger(as);

INPUTS
	as		ActiveSample pointer
	flags Type of blocks to count: 
		ASCB_EMPTY_BLOCKS|ASCB_FULL_BLOCKS|ASCB_INUSE_BLOCKS
RESULTS
	Total blocks counted
##############################################################*/

int ASCountBlocks(as, flags)
struct ActiveSample *as;
unsigned long flags;
{
int k;
struct Node *node;

k=0;
Forbid();

if (flags&ASCB_INUSE_BLOCKS) {
	if (as->HandlerBlock)
		k++;
	}

if (flags&ASCB_EMPTY_BLOCKS) {
	node=as->EmptyBlocks.lh_Head;
	while (node->ln_Succ) {
		node = node->ln_Succ;
		k++;
		}
	}

if (flags&ASCB_FULL_BLOCKS) {
	node=as->FullBlocks.lh_Head;
	while (node->ln_Succ) {
		node = node->ln_Succ;
		k++;
		}
	}
Permit();
return(k);
}

/****************************************************************/

#if 0

void ASkprint(as)
struct ActiveSample *as;
{
int i;
struct Node *node;

Forbid();
kprintf("--------------------------------------\n");
if (as->HandlerBlock)
	kprintf("HB 1\n");
else
	kprintf("HB 0\n");

i=0;
node=as->EmptyBlocks.lh_Head;
while (node->ln_Succ) {
	node = node->ln_Succ;
	i++;
	}
kprintf("EmptyBlocks %04x%04x hex\n",i);

i=0;
node=as->FullBlocks.lh_Head;
while (node->ln_Succ) {
	node = node->ln_Succ;
	i++;
	}
kprintf("FullBlocks %04x%04x hex\n",i);
kprintf("--------------------------------------\n");
Permit();
}

#endif