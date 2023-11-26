#include "exec/types.h"
#include <exec/exec.h>
#include "exec/lists.h"
#include "libraries/dos.h"
#include "prog:include/studio16.h"

extern struct StudioBase *StudioBase;
/**************************************************************/
/**************************************************************/
int DiskSpaceAvail(vol)
char *vol;
{
struct InfoData diskinfo;
struct FileLock      *lock;
int x=0;

if (!strnicmp(vol,"ram:",4))
	return(RamSpaceAvail());

lock=(struct FileLock *)Lock(vol,ACCESS_READ);
if (lock)
	{
	if(Info(lock,&diskinfo))
		{
		if (diskinfo.id_BytesPerBlock<=1024)
			x=(diskinfo.id_NumBlocks-diskinfo.id_NumBlocksUsed)/(1024/diskinfo.id_BytesPerBlock);
		else
			x=(diskinfo.id_NumBlocks-diskinfo.id_NumBlocksUsed)*(diskinfo.id_BytesPerBlock/1024);
		}
	return(x);
	}
else
	{
	return(-1);
	}
}
/**************************************************************/
/*****************************************************************/
int RamSpaceAvail()
{
int x;
int RamSpaceAvail();
x=(AvailMem(MEMF_FAST))/1024;
return(x);
}
/*****************************************************************/
/**************************************************************/
int SampleSpaceAvail(vol)
char *vol;
{
int space;
if (!strnicmp(vol,"ram:",4))
	{
	space=RamSpaceAvail();
	space=space-((StudioBase->defaults.active_buf_size*8)/1024);
	space=space-(StudioBase->defaults.record_safety_size/1024);
	space=(space*69)/70;
	}
else
	{
	space=DiskSpaceAvail(vol);
	space=space-((StudioBase->defaults.active_buf_size*8)/1024);
	space=space-(StudioBase->defaults.record_safety_size/1024);
	space=(space*69)/70;
	}
return(space);
}
/**************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
