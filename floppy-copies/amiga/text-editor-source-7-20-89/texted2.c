

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"

struct screenline line[25];

texted (screen,string,off,wx,wy,ww,wh,status)
struct Screen *screen;
char *string;
int off,wx,wy,ww,wh,*status;
{
struct Window *win;
static struct linedata ld,*curblock,*curblock2,*lastblock,*curblock3;
char *cursor,cursorx,cursory,*tempc1,*tempc2;
char fwhole[100],*string2,*string3;
int len,originallen;
struct textwin twin;
struct RastPort *rp;
struct Menu *Tit1,*textedmenus();
int i,j,k,l,cflag,t,insert;
int charh,charw,curline,y;
LONG mclass;
USHORT mcode[2],mqual;
SHORT Mx,My;
SHORT mn1,mn2,mn3;
struct IntuiMessage *mess;
struct MenuItem *nextitem,*chkitem;
char fname[50];
char fpath[50];
static char key_code_table1[128] =
{0 ,'1','2','3','4','5','6','7','8','9','0','-','=','\\', 0 ,'0',
'q','w','e','r','t','y','u','i','o','p','[',']', 0 ,'1','2','3',
'a','s','d','f','g','h','j','k','l',';', 39, 0 , 0 ,'4','5','6',
 0 ,'z','x','c','v','b','n','m',',','.','/', 0 ,'.','7','8','9',
' ', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

static char key_code_table2[128] = {
'~','!','@','#','$','%','^','&','*','(',')','_','+','|', 0 ,'0',
'Q','W','E','R','T','Y','U','I','O','P','{','}', 0 ,'1','2','3',
'A','S','D','F','G','H','J','K','L',':','"', 0 , 0 ,'4','5','6',
 0 ,'Z','X','C','V','B','N','M','<','>','?', 0 ,'.','7','8','9',
' ', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

*status = AOK;
charh = 8;
charw = 8;
win = ntextwin(screen,wx,wy,ww,wh);
ld.next = 0;
string2 = AllocMem(strlen(string) + MEMEXTRA,0);
if (string2 == NULL) {
   *status = OUTOFMEM;
   goto end;
   }
movmem(string,string2,strlen(string) + 1);
originallen = strlen(string) + MEMEXTRA;
twin.win = win;
twin.x = win->LeftEdge;
twin.y = win->TopEdge;
twin.w = win->Width;
twin.h = win->Height;
twin.maxx = twin.w / charw - 2;
twin.maxy = (twin.h-4) / charh - 2;
twin.stx = 2;
twin.sty = 10;
twin.letw = charw;
twin.leth = charh;

fname[0] = 0;             /* initalize path and filename for disk access */
fpath[0] = 0;

insert = TRUE;
i = 0;                                            /* get line pointers */
j = 0;
ld.start[j++] = i;
curblock = &ld;
do {
   getnumfit(string2,&i,twin.maxx * twin.letw);
   curblock->start[j++] = i;
   if (j == 50) {
      curblock->next = AllocMem(sizeof(struct linedata),0);
      if (curblock->next == 0) {
         *status = OUTOFMEM;
         goto end;
         }
      curblock = curblock->next;
      j = 0;
      }
   } while (i < off);
curblock->next = AllocMem(sizeof(struct linedata),0);
if (curblock->next == 0) {
   *status = OUTOFMEM;
   goto end;
   }
lastblock = curblock->next;
lastblock->next = 0;
lastblock->start[0] = -1;
j = 0;
curblock = &ld;
while (curblock->start[j++] < off) {           /*which line is the offset*/
   if (j == 50) {
      curblock = curblock->next;
      j = 0;
      }
   }                                            /*closest to? */
curline = j-1;                                  /*set current top line */

makewin(&twin,string2,curline,curblock,&len);        /* set up display */
cursor = string2 + off;
cursorx = 2;
cursory = 2;

setcursor(&twin,cursorx,cursory);

Tit1 = textedmenus(&chkitem);
SetMenuStrip(win,Tit1);

ReportMouse(TRUE,win);
ModifyIDCMP(win, NEWSIZE | MENUPICK | RAWKEY | MOUSEBUTTONS);

rp = win->RPort;
j = 0;
l = 0;
do {

   Wait(1<<win->UserPort->mp_SigBit);
   while(mess = GetMsg(win->UserPort)) {

      mclass = mess->Class;
      mcode[0] = mess->Code;
      mqual = mess->Qualifier;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);
            
      if (mclass == NEWSIZE) {
         if (cflag == 1) {
            remakeline(win->RPort,string2,cursory,curblock,
            twin.maxy + curline,&len);
            cflag--;
            }
         twin.x = win->LeftEdge;
         twin.y = win->TopEdge;
         twin.w = win->Width;
         twin.h = win->Height;
         twin.maxx = twin.w / charw - 2;
         twin.maxy = (twin.h-4) / charh - 2;

         off = ld.start[curline];
         i = 0;           /* get line pointers */
         j = 0;
         ld.start[j++] = i;
         do {
            getnumfit(string2,&i,twin.maxx * twin.letw);
            ld.start[j++] = i;
            } while (i < off);
         j = 0;
         while (ld.start[j++] < off)
            ;
         curline = j-1;

         SetAPen(rp,0);
         SetDrMd(rp,JAM2);
         RectFill(rp,2,10,win->Width-3,win->Height-10);
         makewin(&twin,string2,curline,curblock,&len);
         cursorx = 0;
         cursory = 0;
         setcursor(&twin,cursorx,cursory);
         }

      if (mclass == RAWKEY && mcode[0] < 128) {

         if ((mqual & 255) == 0 && key_code_table1[mcode[0]] != 0) {
                                           /*print lowercase letter */
            cflag = 1;
            if (insert == FALSE)
               i = replacelet(&twin,key_code_table1[mcode[0]],cursorx,
                             cursory,string2);
            else
               i = insertlet(&twin,key_code_table1[mcode[0]],cursorx,
                             cursory,string2);
            if(i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy + curline,&len);
               makewin(&twin,string2,curline,curblock,&len);
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
         else if ((mqual & 248) == 0 && key_code_table2[mcode[0]] != 0) {
                                           /* print uppercase letter */
            cflag = 1;
            if (insert == FALSE)
               i = replacelet(&twin,key_code_table2[mcode[0]],cursorx,
                             cursory,string2);
            else
               i = insertlet(&twin,key_code_table2[mcode[0]],cursorx,
                             cursory,string2);
            if(i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy + curline,&len);
               makewin(&twin,string2,curline,curblock,&len);
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

         if (mcode[0] == 67 || mcode[0] == 68) {   /* return */
            setcursor(&twin,cursorx,cursory);
            i = insertlet(&twin,182,cursorx,cursory,string2);
            cursorx = 0;
            cflag = 1;
            goto mdown;
            setcursor(&twin,cursorx,cursory);
            }

         if (mcode[0] == 70 && (line[cursory].l > 1 ||
             line[cursory+1].start != -1)) {
                                                       /* delete */
            setcursor(&twin,cursorx,cursory);
            i = deletelet(&twin,0,cursorx,
                      cursory,string2);
            cflag = 1;
            if (i == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy + curline,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            while (line[cursory].start < 1)
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
            twin.maxy + curline,&len);
            makewin(&twin,string2,curline,curblock,&len);
            cflag = 0;
            while (line[cursory].start <1 && cursory >0)
               cursory--;
            setcursor(&twin,cursorx,cursory);
            }

         if (mcode[0] == 65 && cursorx > 0) {     /* backspace */
            setcursor(&twin,cursorx,cursory);
            cursorx--;
            if (cursorx <0) {
               if (cursory >0) {
                  if (cflag == 1) {
                     remakeline(win->RPort,string2,cursory,curblock,
                     twin.maxy + curline,&len);
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
               twin.maxy + curline,&len);
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
                        twin.maxy + curline,&len);
                        makewin(&twin,string2,curline,curblock,&len);
                        cflag--;
                        }
                     cursorx = line[--cursory].l;
                     }
                  }
               else
                 cursorx = i;
               setcursor(&twin,cursorx,cursory);
               }
            else if (mcode[0] == 76) {  /* top of page */
               setcursor(&twin,cursorx,cursory);
               if (cflag == 1) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy + curline,&len);
                  makewin(&twin,string2,curline,curblock,&len);
                  cflag--;
                  }
               cursory = 0;
               cursorx = 0;
               setcursor(&twin,cursorx,cursory);
               }
            else if (mcode[0] == 78) {                /* word right */
               setcursor(&twin,cursorx,cursory);
               i = cursorx +1;
               while (i<line[cursory].l && line[cursory].letters[i] != ' ')
                  i++;
               if (i >= line[cursory].l) {
                  if (line[cursory+1].start != 0) {
                     cursorx = 0;
                     cursory++;
                     }
                  }
               else
                  cursorx = i;
               setcursor(&twin,cursorx,cursory);
               }
            else if (mcode[0] == 77) {  /* bottom of page */
               setcursor(&twin,cursorx,cursory);
               mdown2:
               if (cflag == 1) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy + curline,&len);
                  makewin(&twin,string2,curline,curblock,&len);
                  cflag--;
                  }
               i = 24;
               while(line[i].start == 0)
                  i--;
               cursory = i;
               cursorx = line[cursory].l -1;
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
                     twin.maxy + curline,&len);
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
            setcursor(&twin,cursorx,cursory);
            if (cflag == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy + curline,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            cursory--;
            if (cursory < 0) {
               cursory++;
               if (curline > 0) {
                  curline--;
                  cursorx = 0;
                  SetBPen(rp,0);
                  SetDrMd(rp,JAM2);
                  ScrollRaster(win->RPort,0,-8,2,10,win->Width-9,
                               line[twin.maxy-1].baseline +1);
                  i = twin.maxy;
                  while (--i >= 0) {
                     movmem(&(line[i]),&(line[i+1]),sizeof(struct
                            screenline));
                     line[i+1].baseline = line[i+1].baseline+8;
                     }
                  printf("%d,",line[1].start);
                  if (line[1].start != 0)
                     curblock2 = curblock;
                  else {
                     curblock3 = &ld;
                     while (curblock3 != curblock) {
                        curblock2 = curblock3;
                        curblock3 = curblock3->next;
                        }
                     }
                  k = curblock2->start[line[0].start -1];
                  if (line[1].start == 0)
                     k = curblock2->start[48];

                  if (string2[k] == ' ')
                     k++;
                  line[0].start = line[1].start -1;
                  if (line[1].start == 0)
                     line[0].start = 49;
                  line[0].l = getnumfit2(string2,&k,twin.maxx * twin.letw,
                                      &line[0]);
                  y = line[1].baseline - 8;
                  line[0].baseline = y;
                  line[0].h = y-6;
                  line[0].d = y+1;

                  redisplayline(&twin,0);

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
               twin.maxy + curline,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            if (cursory == twin.maxy-1) {
            if (line[cursory+1].start != -1) {
               curline++;
               if (curline <50)
                  curblock2 = curblock;
               else {
                  curblock2 = curblock->next;
                  curline = 0;
                  }
               curblock = curblock2;

               SetBPen(rp,0);
               SetDrMd(rp,JAM2);
               ScrollRaster(win->RPort,0,8,2,11,win->Width-9,
                            line[twin.maxy].baseline +1);
               i = 0;
               while (++i < twin.maxy) {
                  movmem(&(line[i]),&(line[i-1]),sizeof(struct
                         screenline));
                  line[i-1].baseline = line[i-1].baseline-8;
                  }
               printf("c%d,",curline);

               k = curblock2->start[line[i].start] + line[i].l;

               if (string2[k] == ' ')
                  k++;
               line[i].start = line[i-1].start +1;
               if (line[i].start >49)
                  line[i].start = 0;
               curblock2->start[line[i].start] = k;
               line[i].l = getnumfit2(string2,&k,twin.maxx * twin.letw,
                                      &line[i]);
               y = line[i-1].baseline + 8;
               line[i].baseline = y;
               line[i].h = y-6;
               line[i].d = y+1;

               redisplayline(&twin,i);

               line[++i].start = line[--i].start +1;
               if (line[++i].start >49) {
                  line[i].start = 0;
                  curblock2 = curblock->next;
                  }
               curblock2->start[line[i].start] = k;
               if (k >= len)
                  line[i].start = -1;

               cursory--;
               }
               }
            if (line[cursory + 1].start != -1) {
               cursory++;
               if (cursorx > line[cursory].l)
                  cursorx = line[cursory].l;
               }
            setcursor(&twin,cursorx,cursory);
            }
         }

      if (mclass == MOUSEBUTTONS) {
         if (mcode[0] == SELECTDOWN) {
            setcursor(&twin,cursorx,cursory);
            if (cflag == 1) {
               remakeline(win->RPort,string2,cursory,curblock,
               twin.maxy + curline,&len);
               makewin(&twin,string2,curline,curblock,&len);
               cflag--;
               }
            i = 0;
            while (i <twin.maxy-1 && line[i+1].start != -1)
               i++;
            while (line[i].baseline - 4 > My)
               i--;
            cursory = i;
            i = line[cursory].l-1;
            while (line[cursory].pos[i]+2 >Mx) {
               i--;
               }
            cursorx = i;
            setcursor(&twin,cursorx,cursory);
            }
         }
      if (mclass == MENUPICK) {
         while (mcode[0] != MENUNULL) {

            mn1 = MENUNUM(mcode[0]);
            mn2 = ITEMNUM(mcode[0]);
            mn3 =  SUBNUM(mcode[0]);

            if (mn1 == 0 && mn2 == 0) {           /* clear string */
               i = telluser(screen,"Are you sure you","want to start over?");
               if (i == TRUE) {
                  curline = 0;
                  cursorx = 0;
                  cursory = 0;
                  string2[0] = -74;
                  string2[1] = 0;
                  SetAPen(rp,0);
                  SetDrMd(rp,JAM2);
                  RectFill(rp,2,10,win->Width-3,win->Height-10);
                  makewin(&twin,string2,curline,curblock,&len);
                  setcursor(&twin,cursorx,cursory);
                  }
               }

            if (mn1 == 0 && mn2 == 1) {           /* load text file */
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
                     curline = 0;
                     cursorx = 0;
                     cursory = 0;
                     SetAPen(rp,0);
                     SetDrMd(rp,JAM2);
                     RectFill(rp,2,10,win->Width-3,win->Height-10);
                     makewin(&twin,string2,curline,curblock,&len);
                     setcursor(&twin,cursorx,cursory);
                     }
                  }
               }

            if (mn1 == 0 && mn2 == 2) {           /* save text file */
               i = get_fname(win,screen,"Save This Text",&fname[0],&fpath[0]);
               if (i != NULL) {
                  fwhole[0] = 0;
                  strncat(&fwhole[0],&fpath[0],49);
                  strncat(&fwhole[0],&fname[0],49);
                  i = textedsavetext(strlen(string2),string2,&fwhole[0]);
                  if (i == FILENOTFOUND)
                     telluser2(screen,"Can't open for saving!",0,FALSE);
                  }
               }

            if (mn1 == 0 && mn2 == 5) {           /* quickie end */
               i = telluser(screen,"Are you sure you","want to quit?");
               if (i == TRUE) {
                  remakeline(win->RPort,string2,cursory,curblock,
                  twin.maxy + curline,&len);
                  makewin(&twin,string2,curline,curblock,&len);
                  CloseWindow(win);
                  string3 = AllocMem(strlen(string2)+1,MEMF_CHIP);
                  if (string3 == NULL) {
                     telluser2(screen,"Alert! Memory","loss imminent!",FALSE);
                     *status = FILENOTFOUND;
                     goto end;
                     }
                  movmem(string2,string3,strlen(string2));
                  FreeMem(string2,originallen);
                  return(string3);
                  }
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

            nextitem = ItemAddress(Tit1,mcode[0]);
            mcode[0] = nextitem->NextSelect;
            }    /* while mcode != MENUNULL end */
         }
      if (lastblock->start[0] != -1) {
         lastblock->next = AllocMem(sizeof(struct linedata),0);
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
if (win != 0)
   CloseWindow(win);
curblock = ld.next;
while (curblock != NULL) {
   curblock2 = curblock->next;   
   FreeMem(curblock,sizeof(struct linedata));
   curblock = curblock2;
   }
if (string2 != 0) {
   if (*status == FILENOTFOUND) {
      *status = AOK;
      return(string2);
      }
   FreeMem(string2,originallen);
   }


}

