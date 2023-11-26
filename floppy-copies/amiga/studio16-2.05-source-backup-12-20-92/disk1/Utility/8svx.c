#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <stdio.h>
#include "/include/iff8svx.h"
#include "/include/studio16.h"

/** FileFilter for 8 bit IFF 8SVX **/

struct FileFilter *InitSVX();
void UnInitSVX();

long SVXFFOpen();
long SVXFFRead();
long SVXFFWrite();
long SVXFFSeek();
long SVXFFClose();
long SVXFFReadTag();
long SVXFFWriteTag();
long SVXFFQueryAlign();
void SkipRest();
void __asm ByteToWord(register __a0 short *,register __a1 char *, 
					  register __d0 long len);
void __asm WordToByte(register __a0 char *,register __a1 short *,
					  register __d0 long len);

static struct FileFilter *ff;

/*** Structures ****/


struct ChunkPos {
	long chan;
	long vhdr;
	long body;
	};

struct SVXHandle {
	long fp;		/** SOpen pointer **/
	struct ChunkPos ChunkPos;
	};

/**********************************************************************/

struct FileFilter *InitSVX()
{
static struct NewFileFilter nff = {"IFF_8SVX",
											FF_READ | FF_WRITE,16,-5,
											SVXFFOpen,SVXFFRead,SVXFFWrite,SVXFFSeek,
											SVXFFClose,SVXFFReadTag,SVXFFWriteTag,
											SVXFFQueryAlign,NULL}; /** No QueryOptions() **/

ff = (struct FileFilter *)AddFileFilter(&nff);
return(ff);
}

/**********************************************************************/

void UnInitSVX()
{
DeleteFileFilter(ff);
}

/**********************************************************************/
long SVXFFOpen(char *path, long mode)
{
struct SVXHandle *kh;
long len,dummy;
int succ;

/*kprintf("SVX Open '%s'\n",path);*/
kh=(struct SVXHandle *)AllocMem(sizeof(struct SVXHandle), MEMF_CLEAR);
if (kh==NULL)
	return(NULL);

kh->fp=SOpen(path, mode);
if (kh->fp==NULL) {
	FreeMem(kh, sizeof(struct SVXHandle));
	return(NULL);
	}

if (mode==MODE_OLDFILE || mode==MODE_READWRITE) {
	len=SRead(kh->fp,&dummy,sizeof(dummy));
	if (mode==MODE_READWRITE && len==0) {	/** new file mode READ/WRITE **/
		succ=SVXWriteNewForm(kh,StrFileName(path));
		}
	else { /** Make sure this file is the kind of format I understand **/
		succ=SVXReadNewForm(kh);
		}
	}
else {	/** MODE_NEWFILE **/
	succ=SVXWriteNewForm(kh,StrFileName(path));
	}

if (!succ) {
	SClose(kh->fp);
	FreeMem(kh, sizeof(struct SVXHandle));
	kh=NULL;
	}

return((long)kh);
}

/**********************************************************************/

SVXWriteNewForm(kh,sampname)
struct SVXHandle *kh;
char *sampname;
{
long pos=0;
long namelen;
static char form[]="FORM....8SVX";
static char anno[]="ANNO\0\0\0\060Recorded with Studio 16 from SunRize Industries.";
static char vhdr[]="VHDR\0\0\0\024++++++++++++++++++++";
static char chan[]="CHAN\0\0\0\004++++";
static char body[]="BODY....";
static char name[]="NAME";

if (WriteChunk(kh->fp,form,sizeof(form)-1,&pos)) {
	if (WriteChunk(kh->fp,anno,sizeof(anno)-1,&pos)) {
		if (WriteChunk(kh->fp,name,sizeof(name)-1,&pos)) {
			namelen=strlen(sampname);
			WriteChunk(kh->fp,&namelen,4,&pos);
			if (namelen&1) namelen++;	/** pad odd length with zero **/
			WriteChunk(kh->fp,sampname,namelen,&pos);
			kh->ChunkPos.vhdr=pos+8;
			if (WriteChunk(kh->fp,vhdr,sizeof(vhdr)-1,&pos)) {
				kh->ChunkPos.chan=pos+8;
				if (WriteChunk(kh->fp,chan,sizeof(chan)-1,&pos)) {
					kh->ChunkPos.body=pos+8;
					if (WriteChunk(kh->fp,body,sizeof(body)-1,&pos))
						return(TRUE);
					}
				}
			}
		}
	}
return(FALSE);
}

/**********************************************************************/

WriteChunk(fp, ptr, size,pos)
long fp;
char *ptr;
long size;
long *pos;
{
long len;

len=Write(fp, ptr, size);
*pos=*pos+len;
return(len==size);
}

/**********************************************************************/

SVXReadNewForm(kh)
struct SVXHandle *kh;
{
long k,x,pos=0;

Seek(kh->fp, 0, OFFSET_BEGINNING);
if (4!=Read(kh->fp,&k,4))
	return(FALSE);
if (k!=FORM)
	return(FALSE);

if(4!=Read(kh->fp,(char *)&x,4)) return(FALSE);	 /* skip length */
if(4!=Read(kh->fp,(char *)&x,4)) return(FALSE);	 /* get form type */
if (x!=SVX) {
	return(FALSE);
	}

pos=pos+12;

do	{
	if(4!=Read(kh->fp,(char *)&k,4)) return(FALSE);	 /* get chunk type */
	pos=pos+8;
	switch(k)
		{
		case CHAN:
			kh->ChunkPos.chan=pos;
			if(4!=Read(kh->fp,(char *)&x,4)) return(FALSE);
			SkipRest(kh->fp,x,&pos);
			break;

		case VHDR:
			kh->ChunkPos.vhdr=pos;
			if(4!=Read(kh->fp,(char *)&x,4)) return(FALSE);
			SkipRest(kh->fp,x,&pos);
			break;

		case BODY:
			kh->ChunkPos.body=pos;
			break;

		default:
			if(4!=Read(kh->fp,(char *)&x,4)) return(FALSE);
			SkipRest(kh->fp,x,&pos);
			break;
		}
	} while(k!=BODY);

return(TRUE);
}

/**********************************************************************/

void SkipRest(fp,len,pos)
long fp,len,*pos;
{
Seek(fp,len,OFFSET_CURRENT);  /* seek past it */
if ((len&1)==1) {
	Seek(fp,1L,OFFSET_CURRENT);  /* skip padding if odd */
	*pos=*pos+1;
	}
*pos=*pos+len;
}

/**********************************************************************/

long SVXFFRead(struct SVXHandle *kh, short *buffer, long len)
{
long x;
char *rb;

x=0;
if (len) {
	rb=(char *)AllocDmaMem(len);
	if (rb) {
		x=SRead(kh->fp, rb, len);
		ByteToWord(buffer,rb,len);
		FreeMem(rb, len);
		}
	}
/*kprintf("8SVX: try to read %04x%04x, read %04x%04x\n",len,x);*/
return(x);
}

/**********************************************************************/

long SVXFFWrite(struct SVXHandle *kh, short *buffer, long len)
{
long x;
char *rb;

x=0;
if (len) {
	rb=(char *)AllocDmaMem(len);
	if (rb) {
		WordToByte(rb,buffer,len);
		x=SWrite(kh->fp, rb, len);
		FreeMem(rb, len);
		}
	}
/*kprintf("8SVX: try to write %04x%04x, write %04x%04x\n",len,x);*/
return(x);
}

/**********************************************************************/

long SVXFFSeek(struct SVXHandle *kh, long pos, long mode)
{
long oldpos;

if (mode==OFFSET_BEGINNING)
	pos+=kh->ChunkPos.body;
oldpos=SSeek(kh->fp, pos, mode);
oldpos=oldpos-kh->ChunkPos.body;
if (oldpos < 0)
	oldpos=0;
/*kprintf("8svx Seek returns %04x%04x\n",oldpos);*/
return(oldpos);
}

/**********************************************************************/

long SVXFFClose(struct SVXHandle *kh)
{
long fp;

fp=kh->fp;
FreeMem(kh, sizeof(struct SVXHandle));
return(SClose(fp));
}

/**********************************************************************/

long SVXFFReadTag(struct SVXHandle *kh, struct SampleFileTag *filltag)
{
struct Voice8Header vhdr;
long len,formlen;
long chan;

setmem(filltag, sizeof(struct SampleFileTag), 0);

/*** Get length of FORM ***/

SSeek(kh->fp, 4, OFFSET_BEGINNING);
len=SRead(kh->fp, &formlen, sizeof(formlen));
if (len!=sizeof(formlen))
	return(NULL);

/*** Determine Pan info ***/

if (kh->ChunkPos.chan) {
	SSeek(kh->fp, kh->ChunkPos.chan, OFFSET_BEGINNING);
	len=SRead(kh->fp, &chan, sizeof(chan));
	if (len!=sizeof(chan))
		return(NULL);
	if (chan==2) /** LEFT **/
		filltag->parms.pan=(0<<5);
	else
		filltag->parms.pan=(200<<5);
	}
else {	/** No CHAN chunk **/
	filltag->parms.pan=ZERO_DB;
	}

/****** Read VHDR for sampling rate, etc. *****/

SSeek(kh->fp, kh->ChunkPos.vhdr, OFFSET_BEGINNING);
len=SRead(kh->fp, &vhdr, sizeof(vhdr));
if (len!=sizeof(vhdr))
	return(NULL);

filltag->parms.rate=vhdr.samplesPerSec;
filltag->parms.filter3db=vhdr.samplesPerSec/2;
filltag->parms.volume=ZERO_DB;	/** !!!!! USE vhdr.Volume !!!!**/

/***** Calculate length ******/

filltag->length=(formlen+8)-kh->ChunkPos.body;
filltag->org_length=filltag->length;

/** Initilized one clip containing entire sample **/

filltag->dclips[0].end=filltag->length-1;
return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

long SVXFFWriteTag(struct SVXHandle *kh, struct SampleFileTag *filltag)
{
struct Voice8Header vhdr;
long len,chantype;

setmem(&vhdr, sizeof(vhdr),0);
vhdr.samplesPerSec=filltag->parms.rate;
vhdr.Volume=Unity;	/** USE parms.volume!!!!! **/
vhdr.oneShotHiSamples=filltag->length;
vhdr.repeatHiSamples=0;
vhdr.samplesPerHiCycle=0;
vhdr.ctOctave=1;
vhdr.sCompression=0;	/** no compression **/

SSeek(kh->fp, kh->ChunkPos.vhdr, OFFSET_BEGINNING);
len=SWrite(kh->fp, &vhdr, sizeof(vhdr));
if (len!=sizeof(vhdr))
	return(NULL);

/** Update BODY length **/

SSeek(kh->fp, kh->ChunkPos.body-4, OFFSET_BEGINNING);
SWrite(kh->fp, &filltag->length, 4);

SSeek(kh->fp, 0, OFFSET_END);
len=SSeek(kh->fp, 0, OFFSET_END);

/** Update CHAN type **/

if (kh->ChunkPos.chan) {
	if (filltag->parms.pan < ZERO_DB)
		chantype=2;			/** LEFT **/
	else
		chantype=4;			/** RIGHT **/
	SSeek(kh->fp, kh->ChunkPos.chan, OFFSET_BEGINNING);
	SWrite(kh->fp, &chantype, 4);
	}

/** Update FORM length **/

SSeek(kh->fp, 4, OFFSET_BEGINNING);
len=filltag->length+kh->ChunkPos.body-8;
if (SWrite(kh->fp, &len, 4)!=4)
	return(NULL);

return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

static long SVXFFQueryAlign(struct SVXHandle *kh)
{
return(SQueryAlign(kh->fp));
}

/**********************************************************************/
