
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


setcursor(twin,cx,cy)
struct textwin *twin;
int cx,cy;
{

int i,j,k;
int x,y;
extern struct screenline line[50];
struct RastPort *rp;

rp = twin->win->RPort;
SetAPen(rp,GRAY1);
SetBPen(rp,1);
SetDrMd(rp,COMPLEMENT);

x = line[cy].pos[cx];
y = line[cy].baseline;
if (x == 0)
  x = cx*8+twin->stx;
if (y == 0)
  y = cy*8+twin->sty+7;
Move(rp,x,y);
Draw(rp,x+8,y);
Move(rp,x,y-1);
Draw(rp,x+8,y-1);
Move(rp,x,y-2);
Draw(rp,x+8,y-2);
Move(rp,x,y-3);
Draw(rp,x+8,y-3);
Move(rp,x,y-4);
Draw(rp,x+8,y-4);
Move(rp,x,y-5);
Draw(rp,x+8,y-5);
Move(rp,x,y-6);
Draw(rp,x+8,y-6);


}


void findpointer(twin,x,y,x2,y2)
struct textwin *twin;
int x,y;
int *x2,*y2;
{

int i,j,k;
extern struct screenline line[50];

*x2 = 0;
*y2 = 0;

if (x <2 || x >(twin->win->Width - RIGHTOFFSET))
   *x2 = -1;
if (y <10)
   *y2 = -1;
if (y > (twin->win->Height -10))
   *y2 = -2;
if (*x2 <0 || *y2 < 0)
   ;
else{

i = 0;
while (i <twin->maxy-1 && line[i+1].start != -1)
   i++;
while (line[i].baseline - 4 > y && i > 0)
   i--;
*y2 = i;
i = line[*y2].l-1;
while (line[*y2].pos[i]+2 > x && i > 0)
   i--;
*x2 = i;

}
}

