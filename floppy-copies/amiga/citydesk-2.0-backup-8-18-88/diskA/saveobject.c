#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "stdio.h"
#include "fcntl.h"
#include "desk.h"
#include "text.h"

saveobject(obj)

struct object *obj;

{
int f,i;
static char fname[34]={0};
static char drawer[35]={0};
static SHORT cm[] = {0xFFFF,0x0000};
char path[80];
char *text;
int cancel;
struct BitMap bitmap;
struct graphic *g;
UBYTE bsmall[256];
UBYTE *buffer;
SHORT p;

extern struct Window *clipboard;

if (obj->type!=TEXT && obj->type!=GRAPHIC) {
#if GERMAN
   telluser2(clipboard->WScreen,"Objekt sichern ist nur mit",
      "Text und Graphiken möglich.",FALSE);
#else
   telluser2(clipboard->WScreen,"Save object only works",
      "with text & graphics.",FALSE);
#endif
   return(CANCEL);
   }

#if GERMAN
cancel=get_fname(clipboard,clipboard->WScreen,"sichere Objekt als...",fname,drawer);
#else
cancel=get_fname(clipboard,clipboard->WScreen,"Save Object As...",fname,drawer);
#endif

if (cancel==NULL) return(CANCEL);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);


if ((f=open(path,O_CREAT))==-1)
   return(CANTOPENFILE);

if (obj->type==TEXT) {
   text=((struct text_object *)(obj->first->data))->textstart;
   p=0;
   if (text[0]=='\\' && text[1]=='n')
      p=2;
   while (text[p]!=0) {
   if (text[p]=='\\' && text[p+1]=='\\') {
      i=write(f,text+p,2L)-1;
      p=p+2;
      }
   else if (text[p]=='\\' && text[p+1]=='n') {
      i=write(f,"\n",1L);
      p=p+2;
      }
   else {
      i=write(f,text+p,1L);
      p++;
      }
   if (i!=1)
      break;
   }

   close(f);
   if (i!=1)
      return(DISKERROR);
   return(AOK);
   }

else {

   g=(struct graphic *)obj->data;
   bitmap.BytesPerRow=RASSIZE(g->w,1);
   bitmap.Rows=g->h;
   bitmap.Flags=0;
   bitmap.Depth=g->numplanes;
   bitmap.Planes[0]=g->planes[0];
   bitmap.Planes[1]=g->planes[1];
   bitmap.Planes[2]=g->planes[2];
   bitmap.Planes[3]=g->planes[3];

   buffer=(UBYTE *)AllocMem(8192,0);
   if (buffer==NULL) {
      PutPict(f, &bitmap,g->w, g->h, cm, bsmall, 256L);
      }
   else {
      PutPict(f, &bitmap,g->w, g->h, cm, buffer, 8192L);
      FreeMem(buffer,8192);
      }
   close(f);
   i=IffErr();
   if (i==0) {
      return(AOK);
      }
   else {
      return(DISKERROR);
      }
   }
return(CANCEL);
}

