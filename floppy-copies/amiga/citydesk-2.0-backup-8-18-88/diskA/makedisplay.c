#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "graphics/text.h"
#include "desk.h"
#include "laser.h"
     
/*********************/
/*** makedisplay *****/
/*********************/
     
makedisplay(obj)
     
struct object *obj;
     
{
extern struct Window *clipboard;
struct graphic *g;
int len;
int pixwidth, pixheight, planesize;
     
if (obj->type==RULE)   /** rules have no bitmap **/
   return(AOK);
     
freedata(obj);  /** make sure screendata has been released **/
     
if (obj->type==GRAPHIC) {
   g=(struct graphic *)obj->data;

   if (g->numplanes > 1)
      obj->numplanes = 2;
   else
      obj->numplanes=1;

   len=DATASIZE(obj);

   if (len == 0) {
      showstat(clipboard->WScreen,TOOSMALL);
      return(TOOSMALL);
      }
   obj->screendata=(UBYTE *)AllocMem(len,MEMF_CHIP|MEMF_CLEAR);
   if (obj->screendata==0) {
      showstat(clipboard->WScreen,OUTOFMEM);
      return (OUTOFMEM);
      }

   pixwidth  = ctosx(obj->w*obj->mag);
   pixheight = ctosy(obj->h*obj->mag);
   planesize = RASSIZE(pixwidth, pixheight);

/* dither(g, obj->screendata, 0, pixwidth, pixheight, RASSIZE(pixwidth,1),printerprefs.flags); */

   fit(g->planes[g->numplanes-1]+RASSIZE(g->w,g->usey),g->usex,g->useh,g->usew,
       RASSIZE(g->w,1),  /* end src parms, start dest */
       obj->screendata,0,pixheight,pixwidth,
       RASSIZE(pixwidth,1));

   if (obj->numplanes==2) {
      fit(g->planes[g->numplanes-2]+RASSIZE(g->w,g->usey),g->usex,g->useh,g->usew,
          RASSIZE(g->w,1),   /* end src parms, start dest */
          obj->screendata + planesize,0,pixheight,pixwidth,
          RASSIZE(pixwidth,1));
         }
   return(AOK);
   }
     
else if (obj->type==TEXT) {
   len=DATASIZE(obj);
   if (len == 0)  {
      showstat(clipboard->WScreen,TOOSMALL);
      return(TOOSMALL);
      }
     
   obj->screendata=(UBYTE *)AllocMem(len,MEMF_CHIP|MEMF_CLEAR);
   if (obj->screendata==0) {
      showstat(clipboard->WScreen,OUTOFMEM);
      return(OUTOFMEM);
      }
     
   /** Note text should already be formated **/
   draw_text(obj,obj->screendata,ctosx(obj->w*obj->mag),
             ctosy(obj->h*obj->mag),0,
             RASSIZE(ctosx(obj->w*obj->mag),1),
             ctosx(obj->mag<<16), ctosy(obj->mag<<16));
   return(AOK);
   }
return(CANCEL);
}

