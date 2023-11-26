#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "fcntl.h"
#include "desk.h"



/**********************************************************

   Window == pointer to current window            (entry)
   text   == pointer to block of text loaded,     (exit)
          == zero if there was an error.
   len    == length of text loaded
***********************************************************/

loadtext(text,Window,len)

struct Window *Window;
char **text;
LONG *len;

{
int f;
static char fname[34]={0};
static char drawer[35]={0};
char path[80];
int cancel;

#if GERMAN
cancel=get_fname(Window,Window->WScreen,"Text einlesen",fname,drawer);
#else
cancel=get_fname(Window,Window->WScreen,"Load Some Text",fname,drawer);
#endif

if (cancel==NULL) return(CANCEL);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);


if ((f=open(path,O_RDONLY))==-1) return(FILENOTFOUND);
*len=lseek(f,0L,2);    /* how long is file? */
lseek(f,0L,0);        /* rewind to start of file */
if ((*text=(char *)AllocMem(*len+1,0))==0) {
   close(f);
   return(OUTOFMEM);
   }

if(read(f,*text,*len)!=*len) {
   close(f);
   return(DISKERROR);
   }
close(f);
*(*text + *len)=0;   /** Note that this is not needed now that len is **/
return(AOK);         /** passed. But, we alloc len+1, and free len+1 **/
}
