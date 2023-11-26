#include "exec/types.h"
#include "df1:sed.h"
/*******************/
/* find a new slot */
/*******************/

getslot(Window,samp)

struct Window *Window;
struct Samp *samp;

{
int i=0;

while(samp->lmem[i]!=0 && i<MAX_SAMPLES) i++;
if (i==MAX_SAMPLES) {
   msg(Window,"No slots available... Delete a sample.");
   return(-1);
   }
return(i);
}
