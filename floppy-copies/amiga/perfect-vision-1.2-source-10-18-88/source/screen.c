#include <exec/types.h>
#include "stdio.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"

/***************************/
/** Open a screen         **/
/***************************/

/* width   == 320 or 640
   height  == 200 or 400
   depth   == 1-5 (number of bitplanes) */

struct Screen *openscreen(width,height,depth,topedge)

int width,height,depth,topedge;
{

struct NewScreen cust;

cust.LeftEdge=0;
cust.TopEdge=topedge;
cust.Width=width;
cust.Height=height;
cust.Depth=depth;
cust.DetailPen=0;
cust.BlockPen=0;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
if (height>=400) cust.ViewModes|=LACE;
cust.Type=SCREENQUIET;
cust.Font=0;
cust.DefaultTitle=0;
cust.Gadgets=0;
cust.CustomBitMap=0;

return(OpenScreen(&cust));
}
