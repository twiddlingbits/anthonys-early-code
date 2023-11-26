
/********************************/
/*  Load a new sample from disk */
/********************************/

load_samp (Window,path)

struct Window *Window;
char *path;

{
char name[80],fname[80],line[80];
int cancel;
extern struct Screen *screen;

name[0]=0;

cancel=get_fname(Window,screen,get_msg_ptr(243),name,path);
if (cancel==0) return(0);

strcpy(fname,path);
add(fname,name);       /* fname is now path+filename */

sprintf(line,get_msg_ptr(219),fname);
msg(Window,line);
return(load_it(Window,fname,name));
}

