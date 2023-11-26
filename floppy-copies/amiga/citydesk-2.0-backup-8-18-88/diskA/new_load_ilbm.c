#include "exec/types.h"
#include "intuition/intuition.h"
#include "ilbm.h"
#include "readpict.h"
#include "desk.h"

int graphicload(g,Window)

struct Window *Window;
struct graphic **g;

{
static char fname[34]={0};
static char drawer[35]={0};
struct Screen *screen;
char path[80];
int cancel;


#if GERMAN
cancel=get_fname(Window,Window->WScreen,"Graphik einlesen",fname,drawer);
#else
cancel=get_fname(Window,Window->WScreen,"Load A Graphic",fname,drawer);
#endif

if (cancel==NULL) return(CANCEL);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

if ((*g=(struct graphic *)AllocMem(sizeof(struct graphic),MEMF_CLEAR))==0)
      return(OUTOFMEM);

screen = Window->WScreen;
newpointer(screen, BUSY);
cancel = load_ilbm (path, *g, screen, FALSE);
newpointer(screen, NORMAL);

if (cancel!=AOK) {
   FreeMem(*g,sizeof(struct graphic));
   return(cancel);
   }
return(AOK);
}


/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_ilbm (path, g, screen, alloc_chip)

struct graphic *g;
char *path;
struct Screen *screen;
int alloc_chip;

{
int i,f,j;
struct BitMap bitmap0;
ILBMFrame iFrame;
long xasp, yasp;
extern ExtAlloc(), ChipAlloc();


if ((f=Open(path,MODE_OLDFILE))==0) {
   return(CANTOPENFILE);
   }

/** The following assumes a grayscale picture. Change so that **/
/** color pictures are converted to true grayscales at this stage **/

if (alloc_chip)
   i=ReadPicture(f, &bitmap0, &iFrame, ChipAlloc);
else
   i=ReadPicture(f, &bitmap0, &iFrame, ExtAlloc);

Close(f);

if (i!=IFF_DONE) {
   return(BADIFF);
   }

for (i=0; i<bitmap0.Depth; i++)
   g->planes[i] = 0;

g->numplanes = bitmap0.Depth;
g->h = iFrame.bmHdr.h;
g->w = iFrame.bmHdr.w;
g->useh = g->h;
g->usew = g->w;
g->usex = 0;
g->usey = 0;

/** Copy bitplane data so that it is alloced with AllocMem() **/

for (i=0; i<g->numplanes; i++) {
   g->planes[i]=(UBYTE *)AllocMem(RASSIZE(g->w,g->h),0);
   if (g->planes[i]==NULL) {
      for (j=0; j<6; j++)
         if (g->planes[j])
            FreeMem(g->planes[j],RASSIZE(g->w,g->h));
      return(OUTOFMEM);
      }
   movmem(bitmap0.Planes[i], g->planes[i], RASSIZE(g->w,g->h));
   }

RemFree(bitmap0.Planes[0]);

xasp = iFrame.bmHdr.xAspect;
yasp = iFrame.bmHdr.yAspect;

g->aspect=((xasp*(ULONG)g->w)<<15)/(yasp*(ULONG)g->h);

if (g->w==320 && g->h==200)
   g->aspect=(ASPLOW*(ULONG)g->w)/((ULONG)g->h);

if (g->w==640 && g->h==200)
   g->aspect=(ASPMED*(ULONG)g->w)/((ULONG)g->h);

if (g->w==320 && g->h==400)
   g->aspect=(ASPLOWLACE*(ULONG)g->w)/((ULONG)g->h);

if (g->w==320 && g->h==200)
   g->aspect=(ASPHI*(ULONG)g->w)/((ULONG)g->h);

invert(g);
return(AOK);
}
