#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include "/include/studio16.h"

/**  Studio 16 uses these low level equivilents to AmigaDos **/
/**  when doing file io.  If alternate methods of sample storage **/
/**  are ever implemented, only this file should need modifying **/

/*******************************************************/

long SOpen(name, mode)
char *name;
long mode;
{
long fp;

fp=Open(name, mode);
if (fp==0 && mode==MODE_READWRITE)	/** Fix early 1.3 bug that won't create **/
	fp=Open(name, MODE_NEWFILE); /** files in READWRITE mode **/
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
return(Seek(fileptr, position, mode));
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

/*******************************************************
	Query file position allignment, 	returns:

	0:	long word aligned
	1:	long word aligned + 1
	2:	long word aligned +2 (word aligned, non long word boundry)
	3:	long word alligned + 3
*******************************************************/

SQueryAlign(fileptr)
long fileptr;
{
return(Seek(fileptr, 0L, OFFSET_CURRENT)&0x3);
}
