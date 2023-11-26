#include "exec/types.h"
#include "psound.h"

/*******************/
/* find a new slot */
/*******************/

getslot(Window)

struct Window *Window;

{
int i=0;
extern struct Samp samp[MAX_SAMPLES];

while(samp[i].lmem!=0 && i<MAX_SAMPLES) i++;
if (i==MAX_SAMPLES) {
   show_msg(Window,198);
   return(-1);
   }
return(i);
}
