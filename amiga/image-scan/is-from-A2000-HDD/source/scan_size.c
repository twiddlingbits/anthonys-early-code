#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include "scan.h"

extern struct config config;
extern struct Screen *is_screen;

/********************/
/** change margins **/
/********************/

scan_size()

{
static char *sizes[]={
   "Height (1/1000):",
   "Offset Delay:",
   "Scan Delay:",
   "Return Delay:",""};

static char *m[4];
int i;

for (i=0;i<4;i++) {
   m[i]=(char *)AllocMem(61,MEMF_CLEAR);
   if (m[i]==0) {
      while (--i>=0)
         FreeMem(m[i],61);
      return(FALSE);
      }
   }   

sprintf(m[0],"%d",config.scan_height);
sprintf(m[1],"%d",config.scan_offset);
sprintf(m[2],"%d",config.scan_delay);
sprintf(m[3],"%d",config.return_delay);

   itemlist(is_screen,sizes,m,"Scan Control Parameters");

sscanf(m[0],"%d",&i);
config.scan_height = i;

sscanf(m[1],"%d",&i);
config.scan_offset = i;

sscanf(m[2],"%d",&i);
config.scan_delay = i;

sscanf(m[3],"%d",&i);
config.return_delay = i;

for (i=0;i<4;i++)
   FreeMem(m[i],61);

return(TRUE);
}
