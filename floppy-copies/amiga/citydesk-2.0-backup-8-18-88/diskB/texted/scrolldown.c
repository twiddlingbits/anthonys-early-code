
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


scrolldown(twin,curline,curblock,curblock2,curblock3,string2,off2,len,holdbm)
struct textwin *twin;
struct linedata **curblock,**curblock2,**curblock3;
int *off2,*curline,len;
char *string2;
struct BitMap *holdbm;
{

int i,k,y,wx,wy;
extern struct screenline line[25];


  (*curline)++;
   if (*curline <50)
      *curblock2 = *curblock;
   else {
      *curblock2 = (*curblock)->next;
      *curline = 0;
      }
   *curblock = *curblock2;

   SetBPen(twin->win->RPort,0);
   SetDrMd(twin->win->RPort,JAM1);

/*
   wx = twin->win->LeftEdge;
   wy = twin->win->TopEdge;

   WaitBOVP(&(twin->win->WScreen->ViewPort));
   BltBitMap(twin->win->RPort->BitMap,wx+2,wy+19,holdbm,wx+2,wy+11,
             twin->win->Width-9,
             line[twin->maxy-1].baseline +1, 0xC0,0xFF,NULL);

   WaitBOVP(&(twin->win->WScreen->ViewPort));
   BltBitMap(holdbm,wx+2,wy+11,twin->win->RPort->BitMap,
             wx+2,wy+11,twin->win->Width-9,
             line[twin->maxy-1].baseline +1, 0xC0,0xFF,NULL);
*/

   ScrollRaster(twin->win->RPort,0,8,2,11,twin->win->Width-RIGHTOFFSET,
             line[twin->maxy-1].baseline +1);
   i = 0;
   *curblock3 = *curblock2;
   while (++i < twin->maxy) {
      movmem(&(line[i]),&(line[i-1]),sizeof(struct screenline));
      line[i-1].baseline = line[i-1].baseline-8;
      line[i-1].d = line[i-1].d-8;
      line[i-1].h = line[i-1].h-8;
                                     /* the last three lines need big-
                                        time changing when I go to mult-
                                        iple fonts! */
      if (line[i].start == 49) {
         *curblock3 = (*curblock2)->next;
         }
      if (line[i].start == 0) {
         *curblock2 = *curblock3;
         }
      }
   i--;

   k = (*curblock2)->start[line[i-1].start] + line[i-1].l;

   if (string2[k] == ' ')
      k++;

   line[i].start = line[i-1].start +1;
   if (line[i].start == 50)
      line[i].start = 0;
   *curblock2 = *curblock3;
   (*curblock2)->start[line[i].start] = k;
   line[i].l = getnumfit2(string2, &k, twin->win->Width-RIGHTOFFSETB -
                          twin->stx, &line[i]);

   y = line[i-1].baseline + 8;
   line[i].baseline = y;
   line[i].h = y-6;
   line[i].d = y+1;
   *off2 = k;

   redisplayline(twin,i);

   line[++i].start = -1;


}



scrollup(twin,curline,curblock,curblock2,curblock3,string2,off2,len,ld)
struct textwin *twin;
struct linedata **curblock,**curblock2,**curblock3,*ld;
int *off2,*curline,*len;
char *string2;
{

int i,k,y,wx,wy;
extern struct screenline line[25];
struct Window *win;
struct RastPort *rp;

   win = twin->win;
   rp = win->RPort;

   (*curline)--;
   if ((*curline) < 0)
      (*curline) = 49;
   SetBPen(rp,0);
   SetDrMd(rp,JAM2);
   i = twin->maxy;
   WaitBOVP(&(win->WScreen->ViewPort));
   ScrollRaster(win->RPort,0,-8,2,10,win->Width-RIGHTOFFSET,
                10 + i * 8);
   while (--i >= 0) {
      movmem(&(line[i]),&(line[i+1]),sizeof(struct
             screenline));
      line[i+1].baseline = line[i+1].baseline+8;
      line[i+1].d = line[i+1].d+8;
      line[i+1].h = line[i+1].h+8;
      }
   if (line[1].start != 0) {
      (*curblock2) = (*curblock);
      }
   else {
      (*curblock3) = ld;
      while ((*curblock3) != (*curblock)) {
         (*curblock2) = (*curblock3);
         (*curblock3) = (*curblock3)->next;
         }
      }
   k = (*curblock2)->start[line[0].start -1];
   if (line[1].start == 0)
      k = (*curblock2)->start[49];
   (*off2) = k;
   line[0].start = line[1].start -1;
   if (line[1].start == 0)
      line[0].start = 49;
   (*curblock) = (*curblock2);
   line[0].l = getnumfit2(string2, &k, twin->win->Width-RIGHTOFFSETB -
                          twin->stx, &line[0]);
   y = line[1].baseline - 8;
   line[0].baseline = y;
   line[0].h = y-6;
   line[0].d = y+1;

   redisplayline(twin,0);

}


