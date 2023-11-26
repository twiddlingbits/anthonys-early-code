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

graphscreen(width,height,depth)
SHORT width,height,depth;
{
struct NewScreen cust;
static struct TextAttr thefont=
   {
   "topaz.font",
   TOPAZ_EIGHTY,
   FS_NORMAL,
   FPF_ROMFONT,
   };

cust.LeftEdge=0;
cust.TopEdge=0;
cust.Width=width;
cust.Height=height;
cust.Depth=depth;
if (depth==1)  {
   cust.DetailPen=1;
   cust.BlockPen=1;
   }
else {
   cust.DetailPen=2;
   cust.BlockPen=3;
   }
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
if (height>=400) cust.ViewModes|=LACE;
cust.Type=CUSTOMSCREEN;
cust.Font=&thefont;
cust.DefaultTitle=0;
cust.Gadgets=0;
cust.CustomBitMap=0;

return((int)OpenScreen(&cust));
}
