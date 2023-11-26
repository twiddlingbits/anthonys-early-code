#include "/include/psound.h"

/********************************/
/*  Load a new sample from disk */
/********************************/

load_samp (Window,path,ptrptr)

struct Window *Window;
char *path;
struct Disk_Samp **ptrptr;

{
extern struct studioBase *studioBase;
char name[80],fname[80],line[80];
int cancel;

name[0]=0;

cancel=get_fname(Window,studioBase->screen,get_msg_ptr(243),name,path);
if (cancel==0) return(0);

strcpy(fname,path);
add(fname,name);       /* fname is now path+filename */

/*sprintf(line,get_msg_ptr(219),fname);
msg(Window,line);*/
return(load_it(Window,fname,name,ptrptr));
}

