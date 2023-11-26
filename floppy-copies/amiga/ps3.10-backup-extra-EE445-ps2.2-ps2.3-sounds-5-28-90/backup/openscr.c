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
cust.Height=200;
cust.Depth=depth;
cust.DetailPen=1;
cust.BlockPen=3;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
cust.Type=CUSTOMSCREEN|SCREENBEHIND;
cust.Font=&thefont;
cust.DefaultTitle="Perfect Sound 3.0 Copyright 1986, 1989 Anthony J. Wood";
cust.Gadgets=0;
cust.CustomBitMap=0;

screen=OpenScreen(&cust);
return((int)screen);
}


