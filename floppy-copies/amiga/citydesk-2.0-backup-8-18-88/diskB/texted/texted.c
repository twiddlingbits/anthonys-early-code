
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"

struct screenline line[25];
char goobleoverflag;
USHORT *goobleimage1;
USHORT *goobleimage2;
struct ConsoleDevice *ConsoleDevice;
struct Gadget *gooblegadget[3];
struct PropInfo *goobleprop;

texted (screen,string,off,wx,wy,ww,wh,status)
struct Screen *screen;
char *string;
int off,wx,wy,ww,wh,*status;
{
struct Window *win;
static struct linedata ld,*curblock,*curblock2,*lastblock,*curblock3;
struct linedata *curbalt;
char *cursor,cursorx,cursory,cursorx2,cursory2;
char fwhole[100],*string2,*string3;
static char searchstr[80],repstr[80];
int len,originallen,off2,off3,*f;
struct textwin twin;
struct RastPort *rp;
struct Menu *Tit1,*textedmenus();
int jumpahead,tempstart,i,j,k,l,cflag,insert;
int y,charh,charw,curline,curlalt,tx,ty;
LONG mclass;
USHORT mcode[2],mqual;
APTR maddress;
SHORT Mx,My,pasteflag;
SHORT mn1,mn2,mn3;
struct IntuiMessage *mess;
struct MenuItem *nextitem,*chkitem;
static char fname[50],temp1[2];
char fpath[50],*cutstring;
int cc1,cc2;
static struct BitMap holdbm;
static struct IOStdReq ioreq;
static struct InputEvent ievent = {NULL,IECLASS_RAWKEY,0,0,0};

goobleimage1 = 0;
goobleimage2 = 0;

string2 = 0;
if (OpenDevice("console.device",-1L,&ioreq,0L))  {
   *status = FILENOTFOUND;
   goto end;
   }
else
   ConsoleDevice = ioreq.io_Device;

*status = AOK;
charh = 8;
charw = 8;
off2 = 0;
cutstring = 0;
pasteflag = 0;

win = (struct Window *) ntextwin(screen,wx,wy,ww,wh);  /* open window! */

if ((win) < (struct Window *) 1) {
   *status = OUTOFMEM;
   goto end;
   }

/*  This stuff is for a failed attempt at getting the 'flash' out of
    the scroll routines when in color 3.

printf(" <%d> ",win->RPort->BitMap->BytesPerRow *
       win->RPort->BitMap->Rows * win->RPort->BitMap->Depth);

movmem(win->RPort->BitMap,&holdbm,sizeof(struct BitMap));

printf(" <%d> ",holdbm.BytesPerRow * holdbm.Rows * holdbm.Depth);
holdbm.Planes[0] = (struct PLANEPTR *) AllocMem(holdbm.BytesPerRow *
                     holdbm.Rows * holdbm.Depth,MEMF_CLEAR);
if (holdbm.Planes[0] == NULL) {
   *status = OUTOFMEM;
   goto end;
   }
holdbm.Planes[1] = (struct PLANEPTR *) (holdbm.Planes[0] +
                    (holdbm.BytesPerRow * holdbm.Rows));
*/

ld.next = 0;
if (string != 0) {
   string2 = (char *) AllocMem(strlen(string) + MEMEXTRA,0);
   if (string2 == NULL) {
      *status = OUTOFMEM;
      goto end;
      }
   movmem(string,string2,strlen(string) + 1);
   originallen = strlen(string) + MEMEXTRA;
   }
else {
   string2 = (char *) AllocMem(10000 + MEMEXTRA,MEMF_CLEAR);
   if (string2 == NULL) {
      *status = OUTOFMEM;
      goto end;
      }
   originallen = 10000 + MEMEXTRA;
   movmem("\n",string2,1);
   }
twin.win = win;
twin.x = win->LeftEdge;
twin.y = win->TopEdge;
twin.w = win->Width;
twin.h = win->Height;
twin.maxx = twin.w / charw - 4;
twin.maxy = (twin.h-2) / charh - 2;
twin.stx = 4;
twin.sty = 10;
twin.letw = charw;
twin.leth = charh;

fname[0] = 0;             /* initalize path and filename for disk access */
fpath[0] = 0;

insert = TRUE;
curline = 0;
i = 0;
curblock = &ld;
*status = fillandfind(off,string2,&twin,&curblock,&curline,&lastblock,0);
if (*status == OUTOFMEM)
      goto end;

makewin(&twin,string2,curline,curblock,&len);        /* set up display */
cursor = string2 + off;
cursorx = 0;
cursory = 0;
cflag = 0;

ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags, goobleprop->HorizPot,
           (0xffff*(0))/(len/PROPSPACE+1), goobleprop->HorizBody,
            (0xffff / (len/PROPSPACE+1)));

setcursor(&twin,cursorx,cursory);

Tit1 = textedmenus(&chkitem);
SetMenuStrip(win,Tit1);

ReportMouse(TRUE,win);
ModifyIDCMP(win, CLOSEWINDOW | NEWSIZE | GADGETUP | MENUPICK | RAWKEY |
            MOUSEBUTTONS);

rp = win->RPort;
j = 0;
l = 0;
do {

   Wait(1<<win->UserPort->mp_SigBit);
   while(mess = (struct IntuiMessage *)GetMsg(win->UserPort)) {

      mclass = mess->Class;
      mcode[0] = mess->Code;
      mqual = mess->Qualifier;
      maddress = mess->IAddress;
      if (mclass == RAWKEY)
         ievent.ie_position.ie_addr = *((APTR *)mess->IAddress);
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

            
      if (mclass == NEWSIZE) {
         FreeMem(holdbm.Planes[0], (holdbm.BytesPerRow *
                           holdbm.Rows * holdbm.Depth));
         if (cflag == 1) {
            remakeline(win->RPort,string2,cursory,curblock,
            twin.maxy,&len);
            cflag--;
            }
         twin.x = win->LeftEdge;
         twin.y = win->TopEdge;
         twin.w = win->Width;
         twin.h = win->Height;
         twin.maxx = twin.w / charw - 4;
         twin.maxy = (twin.h-4) / charh - 2;

         off = ld.start[curline];
         curline = 0;
         i = 0;
         curblock = &ld;
         *status = fillandfind(off,string2,&twin,&curblock,&curline,
                               &lastblock,0);
         if (*status == OUTOFMEM) {
               goto end;
               }

         SetAPen(rp,0);
         SetDrMd(rp,JAM2);
         RectFill(rp,2,10,win->Width-RIGHTOFFSET,win->Height-10);

         makewin(&twin,string2,curline,curblock,&len);        /* set up display */
         cursor = string2 + off;
         cursorx = 0;
         cursory = 0;
         cflag = 0;
      /* remake hold-bitmap data and holding area */
/*      movmem(win->RPort->BitMap,&holdbm,sizeof(struct BitMap));
      holdbm.Planes[0] = (struct PLANEPTR *) AllocMem(holdbm.BytesPerRow *
                           holdbm.Rows * holdbm.Depth,MEMF_CLEAR);
      if (holdbm.Planes[0] == NULL) {
         *status = OUTOFMEM;
         goto end;
         }
      holdbm.Planes[1] = (struct PLANEPTR *) (holdbm.Planes[0] +
                          (holdbm.BytesPerRow * holdbm.Rows));
*/
         i = curblock->start[curline] / PROPSPACE;
         ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                    goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));

         setcursor(&twin,cursorx,cursory);
         }

      if (mclass == RAWKEY && mcode[0] < 128) {

         ievent.ie_Code = mcode[0];
         ievent.ie_Qualifier = mqual;
         k = RawKeyConvert(&ievent,&temp1[0],1,0);
        if (temp1[0] == 7 ||temp1[0] == 8 ||temp1[0] == 9 ||
                           temp1[0] == 13 ||temp1[0] == 127)
           goto jumppast;
         if ((mqual & 255) == 0 && k > 0) {
                                           /*print lowercase letter */
            cflag = 1;
            if (insert == FALSE)
               i = replacelet(&twin,temp1[0],cursorx,
                             cursory,string2);
            else
               i = insertlet(&twin,temp1[0],cursorx,
                             cursory,string2);
            if(i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag = 0;
               }
            cursorx++;
            if (cursorx > line[cursory].l) {
               cursorx = cursorx - line[cursory].l-1;
               if (cursorx <0)
                  cursorx = 0;
               goto mdown;
               }
            setcursor(&twin,cursorx,cursory);

            }
         else if ((mqual & 248) == 0 && k > 0) {
                                           /* print uppercase letter */
            cflag = 1;
            if (insert == FALSE)
               i = replacelet(&twin,temp1[0],cursorx,
                             cursory,string2);
            else
               i = insertlet(&twin,temp1[0],cursorx,
                             cursory,string2);
            if(i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag = 0;
               }
            cursorx++;
            if (cursorx > line[cursory].l) {
               if (line[cursory + 1].start != 0) {
                  cursorx = cursorx - line[cursory].l - 1;
                  if (cursorx <0)
                     cursorx = 0;
                  cursory++;
                  }
               else
                  cursorx--;
               }
            setcursor(&twin,cursorx,cursory);

            }
         jumppast:
         if (temp1[0] == 13 && k > 0) {   /* return */
            setcursor(&twin,cursorx,cursory);
            i = insertlet(&twin,182,cursorx,cursory,string2);
            cursorx = 0;
            cflag = 1;
            goto mdown;
            setcursor(&twin,cursorx,cursory);
            }

         if (temp1[0] == 127 && k > 0 && (line[cursory].l > 1 ||
             line[cursory+1].start != -1)) {
                                                       /* delete */
            setcursor(&twin,cursorx,cursory);
            i = deletelet(&twin,0,cursorx,
                      cursory,string2);
            cflag = 1;
            if (i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            while (line[cursory].start < 0)
               cursory--;
            while (cursory <0)
               cursory++;
            setcursor(&twin,cursorx,cursory);
            }

         if ((mcode[0] == 50 && (mqual & 128)) && (line[cursory].l > 1 ||
            line[cursory+1].start != -1)) {     /* right amiga-x */
            setcursor(&twin,cursorx,cursory);
            line[cursory].l = 0;
            remakeline(win->RPort,string2,cursory,curblock,
            twin.maxy,&len);
            makewin(&twin,string2,curline,curblock,&len);
            cflag = 0;
            while (line[cursory].start <1 && cursory >0)
               cursory--;
            setcursor(&twin,cursorx,cursory);
            }

         if (k > 0 && temp1[0] == 8 && cursorx > 0) {     /* backspace */
            setcursor(&twin,cursorx,cursory);
            cursorx--;
            if (cursorx <0) {
               if (cursory >0) {
                  if (cflag == 1) {
                     remakeline(win->RPort,string2,cursory,curblock,
                     twin.maxy,&len);
                     makewin(&twin,string2,curline,curblock,&len);
                     cflag--;
                     }
                  cursorx = line[--cursory].l;
                  }
               else
                  cursorx++;
               }
            i = deletelet(&twin,0,cursorx,
                      cursory,string2);
            cflag = 1;
            if (i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               while (line[cursory].start == 0)
                  cursory--;
               }
            setcursor(&twin,cursorx,cursory);
            }

         if ((mqual & 1) || (mqual & 2)) {
            if (mcode[0] == 79) {   /* word left */
               setcursor(&twin,cursorx,cursory);
               i = cursorx -1;
               while (i>-1 && line[cursory].letters[i] != ' ')
                  i--;
               if (i <0) {
                  if (cursory >0) {
                     if (cflag == 1) {
                        remakeline(win->RPort,string2,cursory,curblock,
                        twin.maxy,&len);
                        makewin(&twin,string2,curline,curblock,&len);
                        cflag--;
                        }
                     cursorx = line[--cursory].l;
                     }
                  }
               else
                  cursorx = i;
               if (cursorx > 0)
                  cursorx--;
               setcursor(&twin,cursorx,cursory);
               }
            else if (mcode[0] == 76) {  /* top of page */
               setcursor(&twin,cursorx,cursory);
               if (cflag == 1) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy,&len);
                  makewin(&twin,string2,curline,curblock,&len);
                  cflag--;
                  }
               cursory = 0;
               cursorx = 0;
               setcursor(&twin,cursorx,cursory);
               }
            else if (mcode[0] == 78) {                /* word right */
               setcursor(&twin,cursorx,cursory);
               /* set up pointer to 1 more than cursor pos. */
               i = cursorx +1;
               /* find end of line OR a space */
               while (i<line[cursory].l && line[cursory].letters[i] != ' ')
                  i++;
               if (i >= line[cursory].l) {
                  if (cursory < twin.maxy - 1 && line[cursory + 1].start != -1) {
                     cursorx = 0;
                     cursory++;
                     }
                  else {
                     if (cursorx > 0)
                        cursorx--;
                     }
                  }
               else
                  cursorx = i;
               if (cursorx > 0)
                  cursorx++;
               setcursor(&twin,cursorx,cursory);
               }
            else if (mcode[0] == 77) {  /* bottom of page */
               setcursor(&twin,cursorx,cursory);
               mdown2:
               if (cflag == 1) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy,&len);
                  makewin(&twin,string2,curline,curblock,&len);
                  cflag--;
                  }
               i = twin.maxy - 1;
               while(line[i].start == 0)
                  i--;
               cursory = i;
               cursorx = line[cursory].l -1;
               if (cursorx < 0)
                  cursorx = 0; 
               setcursor(&twin,cursorx,cursory);
               }

            }
         else if (mcode[0] == 79) {   /* move left */
            setcursor(&twin,cursorx,cursory);
            cursorx--;
            if (cursorx <0) {
               if (cursory >0) {
                  if (cflag == 1) {
                     remakeline(win->RPort,string2,cursory,curblock,
                     twin.maxy,&len);
                     makewin(&twin,string2,curline,curblock,&len);
                     cflag--;
                     }
                  cursorx = line[--cursory].l;
                  }
               else
                  cursorx++;
               }
            setcursor(&twin,cursorx,cursory);
            }
         else if (mcode[0] == 76) {  /* move up */
            mup:
            if (mclass != 0)
               setcursor(&twin,cursorx,cursory);
            if (cflag == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               off2 = makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            cursory--;
            if (cursory < 0) {
               cursory++;
               if (curline != 0 || curblock != &ld) {

               scrollup(&twin,&curline,&curblock,&curblock2,&curblock3,
                          string2,&off2,len,&ld);

                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));
                  }
               }

            if (cursorx > line[cursory].l)
               cursorx = line[cursory].l;
            setcursor(&twin,cursorx,cursory);
            }

         else if (mcode[0] == 78) {                /* move right */
            setcursor(&twin,cursorx,cursory);
            cursorx++;
            if (cursorx > line[cursory].l) {
               cursorx = 0;
               goto mdown;
               }
            setcursor(&twin,cursorx,cursory);
            }
         else if (mcode[0] == 77) {                     /*move down*/
            setcursor(&twin,cursorx,cursory);
            mdown:
            if (cflag == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               off2 = makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }

            if (cursory == twin.maxy-1 && off2 < len) {

               scrolldown(&twin,&curline,&curblock,&curblock2,&curblock3,
                          string2,&off2,len,&holdbm);
               cursory--;

                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));

               }
            if (line[cursory + 1].start != -1) {
               cursory++;
               if (cursorx > line[cursory].l)
                  cursorx = line[cursory].l;
               }
            setcursor(&twin,cursorx,cursory);
            }
         }

      if (mclass == CLOSEWINDOW) {
         *status = BADCD;
         goto qukend;
         }

      if (mclass == MOUSEBUTTONS) {
         if (mcode[0] == SELECTDOWN) {
            setcursor(&twin,cursorx,cursory);
            if (cflag == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            if (pasteflag == 1) {
               findpointer(&twin,Mx,My,&tx,&ty);
               curblock2 = curblock;
               if (line[ty].start < ty)
                  curblock2 = (struct linedata *) curblock->next;
               cc1 = curblock2->start[line[ty].start]+tx;
               replace(string2,cc1,NULL,cutstring,&len);
               makewin(&twin,string2,curline,curblock,&len);
               pasteflag = 0;
               }
            else {
               findpointer(&twin,Mx,My,&tx,&ty);
               cursorx = tx;
               cursory = ty;
               }
            setcursor(&twin,cursorx,cursory);
            }
         }

      if (mclass == GADGETUP) {
         if (maddress == (APTR) gooblegadget[2]) {  /* deal with slider hit */

            i = ((goobleprop->VertPot * (len/PROPSPACE) + (1<<15)) >>16);
            off = i * PROPSPACE;

            if (cflag == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy,&len);
               off2 = makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }

            curblock = &ld;
            curline = 0;
            *status = fillandfind(off,string2,&twin,&curblock,
                      &curline,&lastblock,0);
            if (*status == OUTOFMEM) {
            telluser2(screen,"Oops! Out of","memory!",FALSE);
               goto end;
               }
            SetAPen(rp,0);
            SetDrMd(rp,JAM2);
            RectFill(rp,2,10,twin.win->Width-RIGHTOFFSET,twin.win->Height-10);

            makewin(&twin,string2,curline,curblock,&len);
            cursory = 0;
            cursorx = 0;
            cflag = 0;
            setcursor(&twin,cursorx,cursory);
            }

         else if (maddress == (APTR) gooblegadget[1]) {  /* deal with down arrow hit */
            setcursor(&twin,cursorx,cursory);
            mclass = 0;
            i = 0;
            while (i <twin.maxy-1 && line[i+1].start != -1)
               i++;
            cursory = i;
            goto mdown;
            }

         else if (maddress == (APTR) gooblegadget[0]) {  /* deal with up arrow hit */
            setcursor(&twin,cursorx,cursory);
            mclass = 0;
            cursory = 0;
            goto mup;
            }
         }

      if (mclass == MENUPICK) {
         while (mcode[0] != MENUNULL) {

            mn1 = MENUNUM(mcode[0]);
            mn2 = ITEMNUM(mcode[0]);
            mn3 =  SUBNUM(mcode[0]);

            if (mn1 == 0 && mn2 == 0) {           /* clear string */
               ClearMenuStrip(win,Tit1);
               i = telluser(screen,"Are you sure you","want to start over?");
               if (i == TRUE) {
                  curline = 0;
                  curblock = &ld;
                  cursorx = 0;
                  cursory = 0;
                  string2[0] = -74;
                  setmem(string2+1,originallen-3,0);
                  SetAPen(rp,0);
                  SetDrMd(rp,JAM2);
                  RectFill(rp,2,10,win->Width-RIGHTOFFSET,win->Height-10);
                  off2 = makewin(&twin,string2,curline,curblock,&len);
                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(1 /PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (1 /PROPSPACE+1)));

                  setcursor(&twin,cursorx,cursory);
                  }
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 0 && mn2 == 1) {           /* load text file */
               ClearMenuStrip(win,Tit1);
               i = get_fname(win,screen,"Load Some Text",&fname[0],&fpath[0]);
               if (i != NULL) {
                  fwhole[0] = 0;
                  strncat(&fwhole[0],&fpath[0],49);
                  strncat(&fwhole[0],&fname[0],49);
                  i = textedloadtext(&originallen,&string2,&fwhole[0]);
                  if (i == OUTOFMEM)
                     telluser2(screen,"Not enough memory",
                               "to load this file!",FALSE);
                  else if (i == FILENOTFOUND)
                     telluser2(screen,"Can't get that file!",0,FALSE);
                  else {
                     off2 = 0;
                     curline = 0;
                     curblock = &ld;
                     cursorx = 0;
                     cursory = 0;
                     SetAPen(rp,0);
                     SetDrMd(rp,JAM2);
                     RectFill(rp,2,10,win->Width-RIGHTOFFSET,win->Height-10);
                     makewin(&twin,string2,curline,curblock,&len);
                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));

                     setcursor(&twin,cursorx,cursory);
                     }
                  }
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 0 && mn2 == 2) {           /* save text file */
               ClearMenuStrip(win,Tit1);
               i = get_fname(win,screen,"Save This Text",&fname[0],&fpath[0]);
               if (i != NULL) {
                  fwhole[0] = 0;
                  strncat(&fwhole[0],&fpath[0],49);
                  strncat(&fwhole[0],&fname[0],49);
                  i = textedsavetext(strlen(string2),string2,&fwhole[0]);
                  if (i == FILENOTFOUND)
                     telluser2(screen,"Can't open for saving!",0,FALSE);
                  }
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 0 && mn2 == 3) {           /* printer dump */
               ClearMenuStrip(win,Tit1);
               f = (int *)Open("prt:",1006);
               if (f == NULL)
                  telluser2(screen,"Printer not available!",0,FALSE);
               else {
                  l = strlen(string2);
                  j = 0;
                  i = 0;
                  while (i < l) {
                     j = i;
                     goobleoverflag = 0;
                     getnumfit(string2,&i,550);
                     Write(f,string2 + j,i-j);
                     if (goobleoverflag == 1)
                        Write(f,"\n",1);
                     }
                  Close(f);
                  }
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 0 && mn2 == 5) {           /* quickie end */
               *status = DOREFLOW;
               goto qukend;
               }

            if (mn1 == 0 && mn2 == 6) {           /* quickie end */
               *status = AOK;
               qukend:
               ClearMenuStrip(win,Tit1);
               if (*status !=BADCD)
                  i = telluser(screen,"Are you sure you","want to quit?");
               else {
                  *status = AOK;
                  i = TRUE;
                  }
               if (i == TRUE) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy,&len);
                  makewin(&twin,string2,curline,curblock,&len);
                  string3 = (char *)AllocMem(strlen(string2)+1,MEMF_CHIP);
                  if (string3 == NULL) {
                     telluser2(screen,"Alert! Memory","loss imminent!",FALSE);
                     i = FILENOTFOUND;
                     goto end;
                     }
                  movmem(string2,string3,strlen(string2)+1);
                  goto end;
                  }
               SetMenuStrip(win,Tit1);
               goto getonwithit;
               }

            if (mn1 == 0 && mn2 == 7) {           /* quickie end */
               *status = JUSTQUIT;
               goto qukend;
               }

            if (mn1 == 2) {           /* insert toggle */
               if (insert == TRUE) {
                  insert = FALSE;
                  chkitem->Flags |=CHECKED;
                  }
               else {
                  insert = TRUE;
                  chkitem->Flags &=(~CHECKED);
                  }
               }

            if (mn1 == 1 && mn2 == 0) {       /* get search string */
               ClearMenuStrip(win,Tit1);
               textedgetstr(screen,searchstr,"Input search string?",250);
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 1 && mn2 == 1) {       /* search for. */
               off = searchtoandset(string2,&twin,&cursorx,&cursory,
                                    &curline,&curblock,&len,&cflag,
                                    &curblock2,status,&ld,&lastblock,
                                    searchstr,0);
                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));

               }

            if (mn1 == 1 && mn2 == 2) {       /* search back */
               off = searchtoandset(string2,&twin,&cursorx,&cursory,
                                    &curline,&curblock,&len,&cflag,
                                    &curblock2,status,&ld,&lastblock,
                                    searchstr,1);
                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));

               }

            if (mn1 == 1 && mn2 == 3) {       /* get replace. string */
               ClearMenuStrip(win,Tit1);
               textedgetstr(screen,repstr,"Input replacement?",250);
               SetMenuStrip(win,Tit1);
               ;
               }

            if (mn1 == 1 && mn2 == 4 && searchstr[0] != 0) {       /* replace */
               ClearMenuStrip(win,Tit1);
               if (cflag == 1) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy,&len);
                  off2 = makewin(&twin,string2,curline,curblock,&len);
                  cflag--;
                  }

               curblock2 = curblock;
               if (line[cursory].start < cursory)
                  curblock2 = (curblock)->next;

               tempstart = curblock2->start[line[cursory].start] + cursorx;
               off3 = tempstart;
               do {
                  jumpahead = patternsearch((string2+tempstart),searchstr,-1);
                  if (jumpahead != -1) {
                     tempstart = tempstart+jumpahead;
                     replace(string2,tempstart,searchstr,repstr,&len);
                     }
                  }while (jumpahead != -1);
               if (off3 != tempstart) {
                  makewin(&twin,string2,curline,curblock,&len);
                  setcursor(&twin,cursorx,cursory);
                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));
                  }
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 1 && mn2 == 5) {       /* replace w/query */
               ClearMenuStrip(win,Tit1);
               if (cflag == 1) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy,&len);
                  off2 = makewin(&twin,string2,curline,curblock,&len);
                  cflag--;
                  }

               curblock2 = curblock;
               if (line[cursory].start < cursory)
                  curblock2 = (curblock)->next;

               tempstart = curblock2->start[line[cursory].start] + cursorx;
               off3 = tempstart;
               do {
                  jumpahead = patternsearch((string2+tempstart),searchstr,-1);
                  if (jumpahead != -1) {
                     tempstart = tempstart+jumpahead;

                     off = tempstart;
                     curblock = &ld;
                     curline = 0;
                     *status = fillandfind(off,string2,&twin,&curblock,
                               &curline,&lastblock,0);
                     if (*status == OUTOFMEM) {
                     telluser2(screen,"Oops! Out of","memory!",FALSE);
                        goto end;
                        }
                     SetAPen(rp,0);
                     SetDrMd(rp,JAM2);
                     RectFill(rp,2,10,twin.win->Width-RIGHTOFFSET,twin.win->Height-10);

                     makewin(&twin,string2,curline,curblock,&len);        /* set up display */
                     cursory = 0;
                     cursorx = off - (curblock)->start[line[cursory].start];
                     if (cursorx-1 == line[cursory].l ||
                         (cursorx == 1 && line[cursory].l == 1)) {
                        cursorx = 0;
                        cursory = 1;
                        }
                     cflag = 0;
                     setcursor(&twin,cursorx,cursory);

                     i = telluser(screen,"    Replace?    ",0);
                     if (i == TRUE) {
                        replace(string2,tempstart,searchstr,repstr,&len);
                        }
                     else
                        tempstart++;
                     }
                  }while (jumpahead != -1);
               if (off3 != tempstart) {
                  makewin(&twin,string2,curline,curblock,&len);
                  setcursor(&twin,cursorx,cursory);
                     i = curblock->start[curline] / PROPSPACE;
                     ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                                goobleprop->HorizPot,
                    (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                    (0xffff / (len/PROPSPACE+1)));
                  }
               SetMenuStrip(win,Tit1);
               }

            if (mn1 == 1 && mn2 == 7) {       /* cut */
               setcursor(&twin,cursorx,cursory);
               i = draglines (&twin,&cc1,&cc2,&curline,&curblock,&ld,&curblock2,
                          &curblock3,string2,&off2,len,&holdbm);
               if (i == AOK) {
               if (cutstring != 0)
                  FreeMem(cutstring,strlen(cutstring)+1);
               i = cc1 - cc2;
               if (i < 0)
                  i = cc2 - cc1;
               cutstring = AllocMem(i+1,MEMF_CLEAR);
               if (cutstring == NULL)
                  telluser2(screen,"No memory ","for cut.",FALSE);
               else {
                  movmem((string2+cc1),cutstring,i);
                  cut(string2,cc1,cc2,&len);
                  off = cc1;
                  curblock = &ld;
                  curline = 0;
                  *status = fillandfind(off,string2,&twin,&curblock,
                            &curline,&lastblock,0);
                  SetAPen(rp,0);
                  SetDrMd(rp,JAM2);
                  RectFill(rp,2,10,twin.win->Width-RIGHTOFFSET,twin.win->Height-10);
                  makewin(&twin,string2,curline,curblock,&len);

                  i = curblock->start[curline] / PROPSPACE;
                  ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                             goobleprop->HorizPot,
                             (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                             (0xffff / (len/PROPSPACE+1)));
                  }
                  }
               setcursor(&twin,cursorx,cursory);
               ModifyIDCMP(win, CLOSEWINDOW | NEWSIZE | MENUPICK |
                           RAWKEY | MOUSEBUTTONS | GADGETUP);
               }

            if (mn1 == 1 && mn2 == 8) {       /* copy */
               setcursor(&twin,cursorx,cursory);
               i = draglines (&twin,&cc1,&cc2,&curline,&curblock,&ld,&curblock2,
                          &curblock3,string2,&off2,len,&holdbm);
               if (i == AOK) {
               if (cutstring != 0)
                  FreeMem(cutstring,strlen(cutstring)+1);
               i = cc1 - cc2;
               if (i < 0)
                  i = cc2 - cc1;
               cutstring = AllocMem(i+1,MEMF_CLEAR);
               if (cutstring == NULL)
                  telluser2(screen,"No memory ","for copy.",FALSE);
               else {
                  movmem((string2+cc1),cutstring,i);
                  }
                  }
               setcursor(&twin,cursorx,cursory);
               ModifyIDCMP(win, CLOSEWINDOW | NEWSIZE | MENUPICK |
                           RAWKEY | MOUSEBUTTONS | GADGETUP);
               }

            if (mn1 == 1 && mn2 == 9) {       /* paste */
               if (cutstring == NULL)
                  telluser2(screen,"Nothing has been","cut or copied yet.",FALSE);
               else if (len + strlen(cutstring) > originallen - 10)
                  telluser2(screen,"Not enough text","memory.",FALSE);
               else
                  pasteflag = 1;
               }

            if (mn1 == 1 && mn2 == 10) {       /* erase */
               i = draglines (&twin,&cc1,&cc2,&curline,&curblock,&ld,&curblock2,
                          &curblock3,string2,&off2,len,&holdbm);
               if (i == AOK) {
                  i = telluser(screen,"Are you sure you","want to erase this section?");
                  if (i == TRUE) {
                     cut(string2,cc1,cc2,&len);
                     off = cc1;
                     curblock = &ld;
                     curline = 0;
                     *status = fillandfind(off,string2,&twin,&curblock,
                               &curline,&lastblock,0);
                     makewin(&twin,string2,curline,curblock,&len);
                     setcursor(&twin,cursorx,cursory);
                     }
                  i = curblock->start[curline] / PROPSPACE;
                  ModifyProp(gooblegadget[2],win,NULL,goobleprop->Flags,
                             goobleprop->HorizPot,
                             (0xffff*(i))/(len/PROPSPACE+1), goobleprop->HorizBody,
                             (0xffff / (len/PROPSPACE+1)));
                  }
               ModifyIDCMP(win, CLOSEWINDOW | NEWSIZE | MENUPICK |
                           GADGETUP | RAWKEY | MOUSEBUTTONS);
               }


            nextitem = (struct MenuItem *) ItemAddress(Tit1,mcode[0]);
            mcode[0] = nextitem->NextSelect;
            }    /* while mcode != MENUNULL end */
         }
      getonwithit:
      if (lastblock->start[0] != -1) {
         lastblock->next = (struct linedata *) AllocMem(sizeof(struct linedata),0);
         if (lastblock->next == 0) {
            *status = OUTOFMEM;
            goto end;
            }
         lastblock = lastblock->next;
         lastblock->next = 0;
         lastblock->start[0] = -1;
         }
      }
   } while(TRUE) ;

end:
if (win > 0)
   CloseWindow(win);
if (cutstring != 0)
   FreeMem(cutstring,strlen(cutstring)+1);

curblock = ld.next;
while (curblock != NULL) {
   curblock2 = curblock->next;   
   FreeMem(curblock,sizeof(struct linedata));
   curblock = curblock2;
   }
/*           *   more about that 'flash' buisness   *
if (holdbm.Planes[0] != NULL)  {
   FreeMem(holdbm.Planes[0], (holdbm.BytesPerRow * holdbm.Rows *
           holdbm.Depth));
   }
*/
end2:
if (goobleimage1 != 0)
   FreeMem(goobleimage1,48);
if (goobleimage2 != 0)
   FreeMem(goobleimage2,48);

CloseDevice(&ioreq);
if (string2 != 0) {
   if (i == FILENOTFOUND) {
      return( (int) string2);
      }
   FreeMem(string2,originallen);
   return( (int) string3);
   }

return(0);
}

