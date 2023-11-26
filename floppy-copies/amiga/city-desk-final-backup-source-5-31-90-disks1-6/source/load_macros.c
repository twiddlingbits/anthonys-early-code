#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "fcntl.h"
#include "desk.h"
#include "text.h"


/**********************************************************

   Window == pointer to current window            (entry)
   text   == pointer to block of text loaded,     (exit)
          == zero if there was an error.
   len    == length of text loaded
***********************************************************/

load_macros(passed_fname)

char *passed_fname;

{
int f;
static char fname[34]={0};
static char drawer[35]={0};
char path[80];
int cancel,len;
extern struct Window *clipboard;
extern struct text_state defaultstate;
struct text_state dummy_state;
char *text;

if (passed_fname==0) {
   cancel=get_fname(clipboard,clipboard->WScreen,"Load Macro File",fname,drawer);

   if (cancel==NULL) return(CANCEL);

   path[0]=0;
   add(path,drawer);  /* connect drawer and filename into a path */
   add(path,fname);
   if ((f=open(path,O_RDONLY))==-1) return(FILENOTFOUND);
   }

else {
   if ((f=open(passed_fname,O_RDONLY))==-1) return(FILENOTFOUND);
   }

len=lseek(f,0L,2);    /* how long is file? */
lseek(f,0L,0);        /* rewind to start of file */
if ((text=(char *)AllocMem(len+1,0))==0) {
   close(f);
   return(OUTOFMEM);
   }

if(read(f,text,len)!=len) {
   close(f);
   FreeMem(text, len+1);
   return(DISKERROR);
   }

close(f);
*(text + len)=0;
dummy_state = defaultstate;
set_default_state(&dummy_state, text);
FreeMem(text, len+1);
return(AOK);
}

