

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


caseinminus(cn,wd)
char cn;
struct Window *wd;
{
   extern int objnum[2],objx[2][10],objy[2][10],objlist[2][100],
           noun_pics[MAXLIST];
   extern struct Image *object_images[MAXLIST];
   int i,ii,j,k;
   struct Image *tpoint;

   if (noun_pics[cn] != 0) {
      i = 0;
      while (objlist[0][i] != noun_pics[cn] && i<100)
         i++;
      if (i <objnum[0]) {
         if (noun_pics[cn] < 0) {
            j = (noun_pics[cn] * -1);
            endsec(j,wd);
            }
         else {
            SetAPen(wd->RPort,0);
            tpoint = object_images[noun_pics[cn]];
            RectFill(wd->RPort, objx[0][i], objy[0][i],
            objx[0][i] + tpoint->Width, objy[0][i] + tpoint->Height);
            }
         objlist[0][i] = 0;
         ii = objnum[0];
         while (objlist[0][ii] == 0 && ii<100)
            ii++;
         if (ii < 100) {
            objlist[0][i] = objlist[0][ii];
            objlist[0][ii] = 0;
            if (objlist[0][i] < 0) {
               j = (objlist[0][i] * -1);
               startsec(j,objx[0][i],objy[0][i],wd);
               activate(j);
               }
            else {
               DrawImage(wd->RPort,object_images[objlist[0][i]],
               objx[0][i],objy[0][i]);
               }
            }
         }
      else if (i<101)
         objlist[0][i] = 0;
      }

   }


