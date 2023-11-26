#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "libraries/dos.h"
#include "stdio.h"
#include "fcntl.h"
#include "desk.h"
#include "text.h"

saveobject(w,obj)

struct object *obj;
struct Window *w;

{
int f,i;
static char fname[34]={0};
static char drawer[35]={0};
static SHORT cm1[] = {0xFFFF,0x0000};
static SHORT cm2[] = {0xFFFF,0xEEEE,0xDDDD,0x0CCCC,0xBBBB,0xAAAA,0x9999,
                      0x8888,0x7777,0x6666,0x5555,0x4444,0x3333,0x2222,
                      0x1111,0x0000};
static SHORT *cm;
char path[80];
char *text;
int cancel;
struct BitMap bitmap;
struct graphic *g;
UBYTE bsmall[256];
UBYTE *buffer;
SHORT p;

if (obj->type!=TEXT && obj->type!=GRAPHIC) {
#if GERMAN
   telluser2(w->WScreen,"Objekt sichern ist nur mit",
      "Text und Graphiken möglich.",FALSE);
#else
   telluser2(w->WScreen,"Save object only works",
      "with text & graphics.",FALSE);
#endif
   return(CANCEL);
   }

#if GERMAN
cancel=get_fname(w,w->WScreen,"sichere Objekt als...",fname,drawer);
#else
cancel=get_fname(w,w->WScreen,"Save Object As...",fname,drawer);
#endif

if (cancel==NULL) return(CANCEL);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);


if (obj->type==TEXT) {
   if ((f=open(path,O_CREAT))==-1)
      return(CANTOPENFILE);

   text = tobj_text(obj);
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
   if ((f=Open(path,MODE_NEWFILE))==-1)
      return(CANTOPENFILE);

   g=(struct graphic *)obj->data;
   bitmap.BytesPerRow=RASSIZE(g->w,1);
   bitmap.Rows=g->h;
   bitmap.Flags=0;
   bitmap.Depth=g->numplanes;
   bitmap.Planes[0]=g->planes[0];
   bitmap.Planes[1]=g->planes[1];
   bitmap.Planes[2]=g->planes[2];
   bitmap.Planes[3]=g->planes[3];

   if (g->numplanes > 1)
      cm = cm2;
   else
      cm = cm1;

   buffer=(UBYTE *)AllocMem(8192,0);
   if (buffer==NULL) {
      PutPict(f, &bitmap,g->w, g->h, cm, bsmall, 256L);
      }
   else {
      PutPict(f, &bitmap,g->w, g->h, cm, buffer, 8192L);
      FreeMem(buffer,8192);
      }
   Close(f);
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

