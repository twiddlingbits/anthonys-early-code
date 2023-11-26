/** lc -v gadlib.c **/

#include <exec/types.h>
#include <exec/exec.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/gadlib.h"

static int gadlib_seg;
static struct MsgPort *gaddfltport;

/**************************************************************/

load_play()
{
struct MsgPort *CreatePort();
struct MsgPort *CreateProc();

gadlib_seg = LoadSeg("gadlib.exe");
if (gadlib_seg==0) {
   printf("Can't load gadlib.exe\n");
   return(CANT_OPEN_FILE);
   }

gaddfltport=CreateProc("GadlibProc",0,gadlib_seg,8000);
if (gaddfltport==0) {
   UnLoadSeg(gadlib_seg);
   printf("Can't create gadlibproc!\n");
   return(OUT_OF_MEMORY);
   }
}

/**************************************************************/

void unload_play()
{

/* printf("unload play\n"); */

if (gaddfltport)
   DeleteTask((int)gaddfltport - sizeof(struct Task));
if (gadlib_seg)
   UnLoadSeg(gadlib_seg);
}
