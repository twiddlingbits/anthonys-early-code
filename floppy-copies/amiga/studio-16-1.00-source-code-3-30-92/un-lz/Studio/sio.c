#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include "/include/studio16.h"

/**  Studio 16 uses these low level equivilents to AmigaDos **/
/**  when doing file io.  If alternate methods of sample storage **/
/**  are ever implemented, only this file should need modifying **/

/** !!!! Eventually change these to deal in words, not bytes, **/
/** !!!! and add one more lower layer of calls that directly **/
/** !!!! replace AmigaDos calls (PrimativeOpen(), etc.  **/
/** !!!! Change the modes to use SS_mode flags **/

/*******************************************************/

long SOpen(name, mode)
char *name;
long mode;
{
long fp;
long len;
struct SampleFileTag *dummy;

fp=Open(name, mode);
if (fp==0 && mode==MODE_READWRITE)	/** Fix early 1.3 bug that won't create **/
	fp=Open(name, MODE_NEWFILE); /** files in READWRITE mode **/
if (fp) {
	dummy=(struct SampleFileTag *)AllocMem(sizeof(struct SampleFileTag),MEMF_CLEAR);
	dummy->id=TAG_KWIK;
	if (mode==MODE_NEWFILE) {
		SWriteTag(fp, dummy);
		}
	else if (mode==MODE_READWRITE) {
		len=Seek(fp, 0L, OFFSET_END);
		len=Seek(fp, 0L, OFFSET_END);
		if (len==0) 
			SWriteTag(fp, dummy);
		}
	if (dummy)
		FreeMem(dummy, sizeof(struct SampleFileTag));
	}
return(fp);
}

/*******************************************************/

int SClose(fileptr)
long fileptr;
{
int reterr;
extern struct Library *GfxBase;

reterr=Close(fileptr);
if (GfxBase->lib_Version < 36)
	reterr=1;

return(reterr);
}

/*******************************************************/

long SSeek(fileptr, position, mode)
long fileptr;
long position;
long mode;
{
long oldpos;

if (mode==OFFSET_BEGINNING)
	position+=sizeof(struct SampleFileTag);
oldpos=Seek(fileptr, position, mode);
return(oldpos-(long)sizeof(struct SampleFileTag));
}

/*******************************************************/

SRead(fileptr, buffer, length)
long fileptr;
char *buffer;
long length;
{
return(Read(fileptr, buffer, length));
}

/*******************************************************/

SWrite(fileptr, buffer, length)
long fileptr;
char *buffer;
long length;
{
return(Write(fileptr, buffer, length));
}

/*******************************************************/

SWriteTag(fileptr, tag)
long fileptr;
struct SampleFileTag *tag;
{
Seek(fileptr, 0L, OFFSET_BEGINNING);
return(Write(fileptr, tag, sizeof(struct SampleFileTag)));
}

/*******************************************************/

SReadTag(fileptr, tag)
long fileptr;
struct SampleFileTag *tag;
{
Seek(fileptr, 0L, OFFSET_BEGINNING);
return(Read(fileptr, tag, sizeof(struct SampleFileTag)));
}

/*******************************************************/
