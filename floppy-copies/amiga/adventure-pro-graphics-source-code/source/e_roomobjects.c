
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


eraseroomobjects(wd)
struct Window *wd;
{
   extern int objnum[2],objx[2][10],objy[2][10],objlist[2][100],
           noun_pics[MAXLIST];
   extern struct Image *object_images[MAXLIST];
   struct Image *tpoint;
   int i,a,j;

   SetAPen(wd->RPort,0);
   i = 0;
   while (i<objnum[1] ) {
      a = objlist[1][i];
      if (a > 0) {
         tpoint = object_images[a];
         RectFill(wd->RPort, objx[1][i], objy[1][i],
         objx[1][i] + tpoint->Width, objy[1][i] + tpoint->Height);
         objlist[1][i] = 0;
         }
      if (a < 0) {
         j = (a * -1);
         
         endsec(j,wd);
         objlist[1][i] = 0;
         }
      i++;
      }
   }

