
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


makewin (twin,string,cline,ld,len)
struct textwin *twin;
struct linedata *ld;
char *string;
int cline,*len;
{
int i,k,l,y,off;
struct RastPort *rp;
extern struct screenline line[50];
struct linedata *ld2;

rp = twin->win->RPort;
SetAPen(rp,0);
SetBPen(rp,0);
rp->AOlPen = 0;
SetDrMd(rp,JAM2);
*len = strlen(string);

RectFill(rp,2,twin->win->Height-9,twin->win->Width-RIGHTOFFSET,twin->win->Height-2);

i=0;
while(i<twin->maxy)
   setmem(&(line[i++]),sizeof(struct screenline),0);

i = 0;
off = ld->start[cline];
l = cline;
y = 7 + twin->sty;
line[i].start = l;
while (i < twin->maxy) {

   line[i].l = getnumfit2(string, &off, twin->win->Width-RIGHTOFFSETB -
                          4, &line[i]);
   if (line[i].letters[line[i].l - 1] == -74 && string[off-1] == ' ') {
      off--;
      }
/*   printf("s %d ",string);       error trap  */
   line[i].l2 = line[i].l;
   Move(rp,twin->stx,y);
   k = line[i].l;
   SetAPen(rp,GRAY1);
   Text(rp,&(line[i].letters[0]),k);   /* print it */
   SetAPen(rp,0);
   if (k < twin->maxx)
      RectFill(rp,twin->stx+k*twin->letw,y-6,twin->win->Width-RIGHTOFFSET,y+1);

   line[i].baseline = y;
   line[i].h = y-6;
   line[i].d = y+1;
   y = y+8;

   if (ld->start[l++] >= *len) {
      while (i < twin->maxy+1)
         line[i++].start = -1;
      goto ret1;
      }
   if (l == 50) {
      ld2 = ld->next;
      ld = ld2;
      l = 0;
      }
   ld->start[l] = off;
   i++;
   line[i].start = l;
   }
line[i].start = -1;
ret1:
/*   printf("\n\n");    */
return(off);

}


searchforward(string,off,len,t)
char *string;
int off,len,*t;
{

while(string[off] != 32 && string[off] != 10 &&
      string[off] != 13 && off < len)
   off++;
if (off == len)
   return(-1);
*t = off;
while(string[*t] == 32)
   *t++;
*t--;
return(off);

}



searchback(string,off,j)
char *string;
int off,*j;
{

int toff;

toff=/*0*/-1;

while(string[off+toff] != 32 && string[off+toff] != 10 &&
      string[off+toff] != 13 && off+toff >= 0) {
   toff--;
   }

*j = toff;
/* while(string[off + *j] == 32)  {
   *j--;
   }
*j++;  */
if (toff < 0)
   toff++;

*j++;

return(toff);

}



getnumfit(string,off,w)
char *string;
int *off,w;
{

int i,j,k,x,toff,t;
extern char goobleoverflag;

j = *off;
x=j;
k=0;
i=0;
while (string[j] != '\n' && string[j] != 0 && k < w) {
   k = k + 8;
   j++;
   i++;
   }
if (string[j] == '\n') {
   k = k + 8;
   j++;
   i++;
   }

t = j;
if (k >= w) {
   goobleoverflag = 1;
   toff = searchback(string,j,&t);
   if (i + toff < 1) {
      t = j+1;
      }
   else {
      t = j + t;
      j = j + toff;
      }
   }
if (t == x)
   t++;
*off = t;
if (string[j-1] == ' ' && string[j] != ' ')
   (*off)++;

return(j-x);

}




getnumfit2(string,off,w,line)
char *string;
int *off,w;
struct screenline *line;
{

int i,j,letx,x,toff,t;

j = *off;
x=j;
letx=4;  /*   WRONG! should be  'twin->stx'!!    */
i=0;
while (string[j] != '\n' && string[j] != 0 && letx < w) {
   line->letters[i] = string[j];
   line->pos[i] = letx;
   letx = letx + 8;
   j++;
   i++;
   }

if (string[j] == '\n') {
   line->letters[i] = -74;
   line->pos[i] = letx;
   letx = letx + 8;
   j++;
   i++;
   }

t = j;
if (letx >= w && string[j-1] != '\n') {
/*   if (string[j-1] == ' ' && string[j-2] != ' ') {
      j--;
      }    */
   toff = searchback(string,j,&t);
   if (i + toff < 1) {
      t = j;
      }
   else {
      t = j + t;
      j = j + toff;
      }
   }
*off = t;
 if (string[j-1] == ' ' && string[j] != ' ') {
   (*off)++;
   }
return(j-x);

}


bohaha() {
static char string[] = "T'was brillig and the slithey toves,\n Did gire and gimbel in the wabe,\nAll mimsy were the borogroves,\nAnd the mome wrathes outgrabe.";
return( (int) string);
}

