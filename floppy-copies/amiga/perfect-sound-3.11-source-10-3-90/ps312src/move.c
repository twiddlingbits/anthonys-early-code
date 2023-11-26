#include "exec/types.h"
#include "psound.h"

/********************************************/
/** insert a range from one samp to another */
/********************************************/

move(Window,start,end,position,from,to)

struct Window *Window;
int position,from,to,start,end;

{
extern struct Samp samp[MAX_SAMPLES];
char *y,*x;
int len;

   if (start>=end) {
      show_msg(Window,195);
      return(0);
      }

   if ((samp[to].type==STEREO || samp[from].type==STEREO)
        && (samp[to].type!=samp[from].type)) {
      show_msg(Window,196);
      return(0);
      }

   show_msg(Window,112);

   len=samp[to].length+end-start;

   if ((x=AllocMem(len,0))==0) {
      nomem(Window);
      return(0);
      }

   if (samp[to].type==STEREO)
      if ((y=AllocMem(len,0))==0) {
         nomem(Window);
         FreeMem(x,len);
         return(0);
         }

   movmem(samp[to].lmem,x,position);
   movmem(samp[from].lmem+start,x+position,end-start);
   movmem(samp[to].lmem+position,x+position+end-start,
          samp[to].length-position);
   FreeMem(samp[to].lmem,samp[to].length);
   samp[to].lmem=x;

   if (samp[to].type==STEREO) {
      movmem(samp[to].rmem,y,position);
      movmem(samp[from].rmem+start,y+position,end-start);
      movmem(samp[to].rmem+position,y+position+end-start,
          samp[to].length-position);
      FreeMem(samp[to].rmem,samp[to].length);
      samp[to].rmem=y;
      }
    else samp[to].rmem=x;

   samp[to].length=len;
   msg(Window,"");
   }

