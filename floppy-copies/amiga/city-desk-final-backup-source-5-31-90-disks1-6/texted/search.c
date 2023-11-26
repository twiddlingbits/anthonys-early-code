
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"

searchtoandset(string2,twin,cursorx,cursory,curline,curblock,len,cflag,
               curblock2,status,ld,lastblock,searchstr,type)
char *string2,*cursorx,*cursory,*searchstr;
struct linedata **curblock,**curblock2,**lastblock,*ld;
int *len,*status,*curline,*cflag,type;
struct textwin *twin;
{

extern struct screenline line[50];
int jumpahead,curlalt,off;
struct linedata *curbalt;
char cursorx2,cursory2,*tempstart;
struct RastPort *rp;

rp = twin->win->RPort;
setcursor(twin,*cursorx,*cursory);

if (*cflag == 1) {
   remakeline(twin->win->RPort,string2,*cursory,*curblock,
   twin->maxy,len);
   makewin(twin,string2,*curline,*curblock,len);
   (*cflag)--;
   }

/* save old */ curlalt = *curline;
/* place    */ curbalt = *curblock;
/* for if   */ cursory2 = *cursory;
/*not found.*/ cursorx2 = *cursorx;

*curblock2 = *curblock;
if (line[*cursory].start < *cursory)
   *curblock2 = (*curblock)->next;
(*cursorx)++;

if (type == 0) {
   tempstart = string2 + (*curblock2)->start[line[*cursory].start] +
            *cursorx;
   jumpahead = patternsearch(tempstart,searchstr,-1);
   }

else {
   tempstart = (char *) (*curblock2)->start[line[*cursory].start] + *cursorx;
   jumpahead = patternschback(string2,searchstr,tempstart);
   }

if (jumpahead != -1) {
   if (type == 0) {
      off = (tempstart + jumpahead) - string2;
      }
   else {
      off = jumpahead;
      }
   *curblock = ld;
   *curline = 0;
   *status = fillandfind(off,string2,twin,curblock,
             curline,lastblock,0);
   if (*status == OUTOFMEM)
      return(-1);
                          
   SetAPen(rp,0);
   SetDrMd(rp,JAM2);
   RectFill(rp,2,10,twin->win->Width-RIGHTOFFSET,twin->win->Height-10);

   makewin(twin,string2,*curline,*curblock,len);        /* set up display */
   *cursory = 0;
   *cursorx = off - (*curblock)->start[line[*cursory].start];
   if (*cursorx-1 == line[*cursory].l ||
       (*cursorx == 1 && line[*cursory].l == 1)) {
      *cursorx = 0;
      *cursory = 1;
      }
   *cflag = 0;
   }
else {
   *curline = curlalt;
   *curblock = curbalt;
   *cursory = cursory2;
   *cursorx = cursorx2;
   flashscreen(&(twin->win->WScreen->ViewPort),4,-1,-1,-1);
   }
makewin(twin,string2,*curline,*curblock,len);        /* set up display */

setcursor(twin,*cursorx,*cursory);

return(off);
}




