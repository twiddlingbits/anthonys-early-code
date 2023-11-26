#include <exec/types.h>
#include "stdio.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"
#include "desk.h"

/***************************/
/** Open a screen         **/
/***************************/

/* width   == 320 or 640
   height  == 200 or 400
   depth   == 1-5 (number of bitplanes) */

openscreen(width,height,depth)
SHORT width,height,depth;
{
struct NewScreen cust;
struct Screen *s;
static struct TextAttr thefont=
   {
   "topaz.font",
   TOPAZ_EIGHTY,
   FS_NORMAL,
   FPF_ROMFONT,
   };

if (width > 800) {
   s = OpenA2024Screen();
   }

else {
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
      cust.DetailPen=WHITE;
      cust.BlockPen=GRAY1;
      }
   cust.ViewModes=0;
   if (width>320) cust.ViewModes=HIRES;
   if (height>=400) cust.ViewModes|=LACE;
   cust.Type=CUSTOMSCREEN;
   cust.Font=&thefont;
   #if GERMAN
   cust.DefaultTitle="City Desk 2.0 © 1987 & 1988 MicroSearch.  Entwickelt von  SunRize Industries";
   #else
   cust.DefaultTitle="City Desk 2.0 © 1987 & 1988 MicroSearch.  Written By SunRize Industries";
   #endif
   cust.Gadgets=0;
   cust.CustomBitMap=0;

   s = OpenScreen(&cust);
   }

if (s!=0) {
   SetRGB4(&s->ViewPort, 3, 0, 0, 0);        /** black **/
   SetRGB4(&s->ViewPort, 2, 5, 5, 5);        /** Gray **/
   SetRGB4(&s->ViewPort, 1, 10, 10, 10);        /** Gray **/
   SetRGB4(&s->ViewPort, 0, 15, 15, 15);     /** white **/
   }
return(s);

}
