
/********************************/
/*  Load a new sample from disk */
/********************************/

load_samp (Window,current,samp,path)

int *current;
struct Samp  *samp;
struct Window *Window;
char *path;

{
char name[80],fname[80],line[80];
int cancel;

name[0]=0;

cancel=get_fname(Window,0,"Load a Sample",name,path);
if (cancel==0) return(0);

strcpy(fname,path);
add(fname,name);       /* fname is now path+filename */

sprintf(line,"Loading: %s",fname);
msg(Window,line);
return(load_it(Window,current,samp,fname,name));
}

