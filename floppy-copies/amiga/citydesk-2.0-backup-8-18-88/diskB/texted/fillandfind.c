
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "texted.h"


fillandfind(off,string2,twin,curblock,curline,lastblock,k)
int off,*curline,k;
struct linedata **curblock,**lastblock;
char *string2;
struct textwin *twin;
{

int j,i,test,newblkflag;

struct linedata *block3,*block4;

j = *curline;
i = k;
block3 = (*curblock);

newblkflag = 0;

do {
   (*curblock)->start[j++] = i;
   getnumfit(string2, &i, twin->win->Width-RIGHTOFFSETB - twin->stx);
   if (j == 50) {
      if ((*curblock)->next == 0) {
         newblkflag = 1;
         if ((test = allocblock(&((*curblock)->next))) == OUTOFMEM)
            return(OUTOFMEM);
         }
      (*curblock) = (*curblock)->next;
      j = 0;
      }
   } while (i < off);


(*curblock)->start[j] = i;
if ((*curblock)->next == 0) {
   newblkflag = 1;
   if ((test = allocblock(&((*curblock)->next))) == OUTOFMEM)
      return(OUTOFMEM);
   }

if (newblkflag == 1) {
   (*lastblock) = (*curblock)->next;
   (*lastblock)->next = 0;
   (*lastblock)->start[0] = -1;
   }

j--;

if (j < 0) {
   j = 49;
   while (block3 != (*curblock)) {
      block4 = block3;
      block3 = block3->next;
      }
   (*curblock) = block4;
   }

*curline = j;                                  /*set current top line */

return(AOK);
}



allocblock(block)
struct linedata **block;
{
*block = (struct linedata *) AllocMem(sizeof(struct linedata),MEMF_CLEAR);
if (*block == 0)
   return(OUTOFMEM);
else
   return(AOK);
}

