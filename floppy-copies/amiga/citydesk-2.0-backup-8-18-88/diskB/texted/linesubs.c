
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


insertlet(twin,letter,cx,cy,string)
struct textwin *twin;
char letter,string;
int cx,cy;
{

int i,j,k;
int x,y;
extern struct screenline line[25];
struct RastPort *rp;
char temp1[130];
int temp2[130];

rp = twin->win->RPort;
SetAPen(rp,GRAY1);
SetBPen(rp,0);
SetDrMd(rp,JAM2);

if (cx == line[cy].l) {
   line[cy].letters[cx] = letter;
   line[cy].pos[cx] = line[cy].pos[cx-1] + twin->letw;
   line[cy].l++;
   }
else {
   i = line[cy].l + 2;
   while (i>cx) {
      line[cy].letters[i] = line[cy].letters[--i];
      line[cy].pos[++i] = line[cy].pos[--i]+twin->letw;
      }
   line[cy].letters[cx] = letter;
   line[cy].pos[cx] = line[cy].pos[cx-1] + twin->letw;
   if (cx == 0) {
      i=0;
      while(i < line[cy].l + 2)
         line[cy].pos[i] = twin->letw * i++;
      }
   line[cy].l++;
   }
if (line[cy].l > twin->maxx -1) {
   return(1);
   }
redisplayline(twin,cy);
return(0);

}





replacelet(twin,letter,cx,cy,string)
struct textwin *twin;
char letter,string;
int cx,cy;
{

int i,j,k;
int x,y;
extern struct screenline line[25];
struct RastPort *rp;
char temp1[130];
int temp2[130];

rp = twin->win->RPort;
SetAPen(rp,GRAY1);
SetBPen(rp,0);
SetDrMd(rp,JAM2);

line[cy].letters[cx] = letter;
line[cy].pos[cx] = line[cy].pos[cx-1] + twin->letw;
if (cx == 0)
   line[cy].pos[cx] = 0;

if (cx >=line[cy].l)
   line[cy].l++;

i = cx;
while(++i<twin->maxx)
   line[cy].pos[i] = line[cy].pos[i-1] + twin->letw;

if (line[cy].letters[twin->maxx -1] != 0)
   return(1);
redisplayline(twin,cy);
return(0);

}





deletelet(twin,letter,cx,cy,string)
struct textwin *twin;
char letter,string;
int cx,cy;
{

int i,j,k;
int x,y;
extern struct screenline line[25];
struct RastPort *rp;
char temp1[130];
int temp2[130];

rp = twin->win->RPort;
SetAPen(rp,GRAY1);
SetBPen(rp,0);
SetDrMd(rp,JAM2);

j=0;
if (line[cy].letters[cx] == -74 || (line[cy].l == 0)) {
   j++;
   }
if (cx >= line[cy].l) {
   line[cy].letters[cx] = 0;
   line[cy].pos[cx] = line[cy].pos[cx-1]+twin->letw;
   }
else {
   i = cx;
   while (i<line[cy].l+1) {
      line[cy].letters[i] = line[cy].letters[++i];
      line[cy].pos[--i] = line[cy].pos[++i]-twin->letw;
      }
   line[cy].letters[i] = 0;
   line[cy].l--;
   }

redisplayline(twin,cy);
return(j);

}






redisplayline(twin,l)
struct textwin *twin;
int l;
{
int y,k;
struct RastPort *rp;
extern struct screenline line[25];

rp = twin->win->RPort;
SetAPen(rp,GRAY1);
SetBPen(rp,0);
SetDrMd(rp,JAM2);

y = 7+twin->sty+8*l;
Move(rp,twin->stx,y);
k = line[l].l;
if (k>0)
   Text(rp,&(line[l].letters[0]),k);   /* print it */
SetAPen(rp,0);
RectFill(rp,twin->stx+k*twin->letw,y-6,twin->win->Width-RIGHTOFFSET,y+1);

}





