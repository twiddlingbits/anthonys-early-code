#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/rastport.h"
#include "graphics/gfx.h"

/****************************************/
/** Moves all space invaders an offset **/
/****************************************/


moveall(window,x,y,h,w,xoff,yoff)

struct Window *window;
int x;       /** X pos of current invader block **/
int y;       /** Y pos of current invader block **/
int h;       /** Height of space invaders block **/
int w;       /** Width of space invaders block **/
int xoff,yoff;  /** Amount to move invaders **/

{

#if 0
struct BitMap bitmap;
struct RastPort trp;
UBYTE *tbm;
int i;

tbm = (UBYTE *)AllocMem(RASSIZE(w,h)*2,MEMF_CHIP|MEMF_CLEAR);
if (tbm==0)
   return(FALSE);


InitRastPort(&trp);

bitmap.BytesPerRow=RASSIZE(x,1);
bitmap.Rows=y;
bitmap.Flags=0;
bitmap.Depth=2;
bitmap.Planes[0]=tbm;
bitmap.Planes[1]=tbm+RASSIZE(w,h);

trp.BitMap=&bitmap;


/** Copy 'Vaders into temporary rastport **/
                                                     /* 0x01, 0 */
ClipBlit(window->RPort, x, y, &trp, 0, 0, w, h, 0xEE,0x01,0);

/** Now clear the vaders **/

SetDrMd(window->RPort,JAM1);
SetAPen(window->RPort,0);
RectFill(window->RPort,x,y,w,h);   /** zap old 'vaders **/

/** Now put them 'vaders back into new position **/

ClipBlit(&trp, 0, 0, window->RPort, x+xoff, y+yoff, w, h, 0xEE,0x01,0);


FreeMem(tbm,RASSIZE(w,h)*2);
#endif
if (xoff < 0)
   x += xoff;
ScrollRaster(window->RPort, -xoff, -yoff, x, y, x+w+xoff, y+h+yoff);
return(TRUE);
}


