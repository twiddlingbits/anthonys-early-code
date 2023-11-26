#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>

/***************************/
/** Open a screen         **/
/***************************/

/* width   == 320 or 640
   height  == 200 or 400
   depth   == 1-5 (number of bitplanes) */

struct Screen *openscreen(width,height,depth)

int width,height,depth;

{
struct NewScreen cust;
struct Screen *s;
struct Screen *OpenScreen();
int i;
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
cust.DetailPen=5;
cust.BlockPen=15;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
if (height>=400) cust.ViewModes|=LACE;
cust.Type=CUSTOMSCREEN;
cust.Font=&thefont;
cust.DefaultTitle=0;
cust.Gadgets=0;
cust.CustomBitMap=0;

s = OpenScreen(&cust);

if (s!=0) {
   for (i=0; i < 16; i++)
      SetRGB4(&s->ViewPort,i, i, i, i);
   ShowTitle(s, FALSE);
   }
return(s);
}
