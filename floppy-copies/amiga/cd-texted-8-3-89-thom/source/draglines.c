
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


draglines (twin,c1,c2,curline,curblock,ld,curblock2,curblock3,string2,off2,
           len,holdbm)
struct textwin *twin;
int *c1,*c2;
struct linedata **curblock,*ld,**curblock2,**curblock3;
char *string2;
int *off2,len,*curline;
struct BitMap **holdbm;
{
struct Window *win;
struct linedata *curblock4;
extern struct screenline line[50];
struct RastPort *rp;
int tx,ty,cc1,cc2;
LONG mclass;
USHORT mcode;
SHORT Mx,My;
SHORT mn1,mn2,mn3;
struct IntuiMessage *mess,bogusmess;
int down,offset,oldy,oldx,holdx,fstline,fstpos,oldfl;
int status,ysize,bogusflag,templine,templine2,templine3;

bogusflag = 0;
ysize = twin->maxy;
win = twin->win;

ReportMouse(TRUE,win);
ModifyIDCMP(win, INTUITICKS | RAWKEY | MOUSEMOVE | MOUSEBUTTONS);

rp = win->RPort;
down = 0;
while (TRUE) {

   Wait(1<<win->UserPort->mp_SigBit);
   do {
      if (bogusflag == 0) {
         if (mess = (struct IntuiMessage *)GetMsg(win->UserPort))
            ;
         else
            goto endwhile;

         mclass = mess->Class;
         mcode = mess->Code;
         Mx = mess->MouseX;
         My = mess->MouseY;
         movmem(mess,&bogusmess,sizeof(struct IntuiMessage));
         ReplyMsg(mess);
         }
      else {
         mclass = bogusmess.Class;
         mcode = bogusmess.Code;
         Mx = bogusmess.MouseX;
         My = bogusmess.MouseY;
         bogusflag = 0;
         }

      while (mclass == MOUSEMOVE) {
         mess = (struct IntuiMessage *)GetMsg(win->UserPort);
         if (mess == NULL)
            goto exit1;
         if (mess->Class != MOUSEMOVE) {
            mclass = bogusmess.Class;
            mcode = bogusmess.Code;
            Mx = bogusmess.MouseX;
            My = bogusmess.MouseY;
            movmem(mess,&bogusmess,sizeof(struct IntuiMessage));
            bogusflag = 1;
            }
         ReplyMsg(mess);
         }
      exit1:
            
      if (mclass == INTUITICKS) {
         if (ty < 0) {
            mclass = MOUSEMOVE;
            }
         }

      if (mclass == MOUSEBUTTONS) {
         if (mcode == SELECTDOWN) {
            findpointer(twin,Mx,My,&tx,&ty);
            if (tx>-1 &&ty>-1) {
               down = 1;
               oldy = ty;
               oldx = tx;
               setline(win,ty,ty,tx,tx);
               fstline = ty;
               fstpos = tx;
               oldfl = fstline;
               offset = 0;
               curblock4 = *curblock;
               if (line[ty].start < ty)
                  curblock4 = (struct linedata *) (*curblock)->next;
               *c1 = curblock4->start[line[ty].start] + fstpos;
               cc1 = curblock4->start[line[ty].start] + fstpos;
               }
            }
         else if (mcode == SELECTUP) {
            if (down == 1) {
               down = 0;
               findpointer(twin,Mx,My,&tx,&ty);
               setline(win,-10);
               if (ty >-1 && tx>-1) {
                  curblock4 = *curblock;
                  if (line[ty].start < ty)
                     curblock4 = (struct linedata *) (*curblock)->next;
                  *c2 = curblock4->start[line[ty].start] + tx;
                  cc2 = curblock4->start[line[ty].start] + tx;
                  status = AOK;
                  }
               else
                  status = CANCEL;
               goto end;
               }
            }
         }
      if (mclass == MOUSEMOVE && down == 1) {
         findpointer(twin,Mx,My,&tx,&ty);
         if (tx>-1 &&ty>-1) {
            setline(win,-10);
            offset = offset - (ty - oldy); /* standard offset changer line */
            oldy = ty;
            oldx = tx;
            templine = fstpos;
            setline(win,fstline,ty,fstpos,tx);
            }

         else if (tx>-1 &&ty == -1) {     /* drag up */
            if (*curline != 0 || *curblock != ld) {
               setline(win,-10);
               offset = offset - (0 - oldy); /* standard offset changer line */
               offset = offset + 1;
               oldy = 0;
               fstline = offset;
               templine = fstpos;
               if (fstline < 0) {
                  fstline = 0;
                  templine = -1;
                  }
/*               if (fstline < twin->maxy -1 && offset > 0)
                  fstline++;     */
               scrollup(twin,curline,curblock,curblock2,curblock3,
                        string2,off2,len,ld);
               if (fstline >twin->maxy -1) {
                  fstline = twin->maxy -1;
                  templine = -1;
                  }
               setline(win,fstline,0,-1,templine);
               }
            }

         else if (tx>-1 &&ty == -2) {     /* drag down */
            if (-1 != line[twin->maxy-1].start && *off2< len) {
               setline(win,-10);
               offset = offset - (twin->maxy - 1 - oldy); /* standard offset changer line */
               offset = offset - 1;
               oldy = twin->maxy-1;
               fstline = oldy + offset;
               templine = fstpos;
               if (fstline < 0) {
                  fstline = 0;
                  templine = -1;
                  }
               scrolldown(twin,curline,curblock,curblock2,curblock3,
                        string2,off2,len,holdbm);
               if (fstline >twin->maxy -1) {
                  fstline = twin->maxy -1;
                  templine = -1;
                  }
               setline(win,fstline,oldy,templine,-1);
               }
            }
         }

      if (mclass == RAWKEY) {
         if (down == 1)
            setline(win,-10);
         down = 0;
         status = CANCEL;
         goto end;
         }

      } while (TRUE);  /* end of while(IntuiMessage) */
   endwhile:
   } /* end of while(TRUE) */

end:
if (*c1 > *c2) {
   *c2 = cc1;
   *c1 = cc2;
   }
(*c2)++;

return(status);
}




setline(win,line1,line2,pos1,pos2)
struct Window *win;
int line1,line2,pos1,pos2;
{

extern struct screenline line[50];
int x,y,h,w,line3,pos3;
int x2,y2,h2,w2;
int x3,y3,h3,w3;
static int linea,lineb,posa,posb;

SetAPen(win->RPort,GRAY1);
SetDrMd(win->RPort,COMPLEMENT);

if (line1 == -10) {
   line1 = linea;
   line2 = lineb;
   pos1 = posa;
   pos2 = posb;
   }
else {
   linea = line1;
   lineb = line2;
   posa = pos1;
   posb = pos2;
   }

if (line1 > line2) {
   line3 = line1;
   line1 = line2;
   line2 = line3;
   pos3 = pos1;
   pos1 = pos2;
   pos2 = pos3;
   }

if (line1 == line2) {
   if (pos1 > pos2) {
      pos3 = pos1;
      pos1 = pos2;
      pos2 = pos3;
      }
   }


x = line[line1].pos[0]+1;
y = line[line1+1].h;
w = win->Width - RIGHTOFFSET;
h = line[line2-1].d;

if (pos1 == -1)
   x2 = line[line1].pos[0]+1;
else
   x2 = line[line1].pos[pos1]+1;
y2 = line[line1].h;
w2 = win->Width - RIGHTOFFSET;
h2 = line[line1].d;

x3 = line[line2].pos[0]+1;
y3 = line[line2].h;
if (pos1 == -1)
   w3 = win->Width - RIGHTOFFSET;
else
   w3 = line[line2].pos[pos2]+9;
h3 = line[line2].d;

if (line1 == line2) {
   RectFill(win->RPort,x2,y2,w3,h3);
   }
else {
   if (line1+1 != line2) {
      RectFill(win->RPort,x,y,w,h);
      }
   RectFill(win->RPort,x2,y2,w2,h2);
   RectFill(win->RPort,x3,y3,w3,h3);
   }


SetDrMd(win->RPort,JAM2);

}


