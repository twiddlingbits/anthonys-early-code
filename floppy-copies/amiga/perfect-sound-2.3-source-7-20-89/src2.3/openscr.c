#include "exec/types.h"
#include "intuition/intuition.h"

/***************************/
/** Open a screen         **/
/***************************/

openscr(width,depth)
SHORT width,depth;
{
struct NewScreen cust;
struct Screen *screen;
static struct TextAttr thefont= {

   "topaz.font",
   TOPAZ_SIXTY,
   FS_NORMAL,
   FPF_ROMFONT,
   };

cust.LeftEdge=0;
cust.TopEdge=199;
cust.Width=width;
cust.Height=200;
cust.Depth=depth;
cust.DetailPen=2;
cust.BlockPen=3;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
cust.Type=CUSTOMSCREEN;
cust.Font=&thefont;
cust.DefaultTitle=" this is te";
cust.Gadgets=0;
cust.CustomBitMap=0;

screen=OpenScreen(&cust);
return((int)screen);
}


