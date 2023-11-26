#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"

struct RastPort *rec_rp;
unsigned short *plane;

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
if (depth==1)
   cust.BlockPen=1;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
if (depth==1)
   cust.Type=CUSTOMSCREEN;
else
   cust.Type=CUSTOMSCREEN|SCREENBEHIND;
cust.Font=&thefont;
cust.DefaultTitle="Perfect Sound 3.2 Copyright 1986, 1989 Anthony J. Wood";
if (depth==1)
   cust.DefaultTitle=0;
cust.Gadgets=0;
cust.CustomBitMap=0;

screen=OpenScreen(&cust);
if (depth==1 && screen!=0) {
   SetRGB4(&screen->ViewPort,0,0,0,0);
   SetRGB4(&screen->ViewPort,1,15,4,4);
   ShowTitle(screen,FALSE);
   SetAPen(&screen->RastPort,0);
   SetDrMd(&screen->RastPort,JAM1);
   rec_rp = &screen->RastPort;
   plane=rec_rp->BitMap->Planes[0]+40*REC_ZERO;
   RectFill(rec_rp,0,0,319,12);
   SetAPen(&screen->RastPort,1);
   SetDrMd(&screen->RastPort,JAM1);
   }
return((int)screen);
}


