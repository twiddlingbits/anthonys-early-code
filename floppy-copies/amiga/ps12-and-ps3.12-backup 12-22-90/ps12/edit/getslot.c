#include "exec/types.h"
#include "/include/psound.h"

/*******************/
/* find a new slot */
/*******************/

getslot(Window)

struct Window *Window;

{
int i=0;
extern struct Disk_Samp samp[MAX_SAMPLES];

while(samp[i].flags&SAMPLE_OPEN && i<MAX_SAMPLES) i++;
if (i==MAX_SAMPLES) {
   show_msg(Window,198);
printf("can't get slot\n");
   return(-1);
   }
printf("got slot %d\n",i);
return(i);
}
