#include "exec/types.h"
#include "df1:sed.h"

/********************************************/
/** insert a range from one samp to another */
/********************************************/

move(Window,samp,start,end,position,from,to)

struct Window *Window;
struct Samp *samp;
int position,from,to,start,end;

{

char *y,*x;
int len;

   if (start>=end) {
      msg(Window,"Markers not set.");
      return(0);
      }

   if ((samp->type[to]==STEREO || samp->type[from]==STEREO)
        && (samp->type[to]!=samp->type[from])) {
      msg(Window,"Both samples are not stereo.");
      return(0);
      }

   msg(Window,"Working...");

   len=samp->length[to]+end-start;

   if ((x=AllocMem(len,0))==0) {
      nomem(Window);
      return(0);
      }

   if (samp->type[to]==STEREO)
      if ((y=AllocMem(len,0))==0) {
         nomem(Window);
         FreeMem(x,len);
         return(0);
         }

   movmem(samp->lmem[to],x,position);
   movmem(samp->lmem[from]+start,x+position,end-start);
   movmem(samp->lmem[to]+position,x+position+end-start,
          samp->length[to]-position);
   FreeMem(samp->lmem[to],samp->length[to]);
   samp->lmem[to]=x;

   if (samp->type[to]==STEREO) {
      movmem(samp->rmem[to],y,position);
      movmem(samp->rmem[from]+start,y+position,end-start);
      movmem(samp->rmem[to]+position,y+position+end-start,
          samp->length[to]-position);
      FreeMem(samp->rmem[to],samp->length[to]);
      samp->rmem[to]=y;
      }
    else samp->rmem[to]=x;

   samp->length[to]=len;
   msg(Window,"");
   }

