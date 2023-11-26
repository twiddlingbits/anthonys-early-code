
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


caseinplus(cn,wd)
char cn;
struct Window *wd;
{
   extern int objnum[2],objx[2][10],objy[2][10],objlist[2][100],
           noun_pics[MAXLIST];
   extern struct Image *object_images[MAXLIST];
   int i,j,k,ll;

   if (noun_pics[cn] > 0) {
      i = 0;
      while (objlist[0][i] != 0 && i<101)
         i++;
      if (i <objnum[0]) {
         DrawImage(wd->RPort,object_images[noun_pics[cn]],
         objx[0][i],objy[0][i]);
         objlist[0][i] = noun_pics[cn];
         }
      else if (i<101)
         objlist[0][i] = noun_pics[cn];
      }

   if (noun_pics[cn] < 0) {
      j = (noun_pics[cn] * -1);
      i = 0;
      while (objlist[0][i] != 0 && i<101)
         i++;
      if (i <objnum[0]) {
         startsec(wd,j,objx[0][i],objy[0][i]);
         activate(j);
         objlist[0][i] = noun_pics[cn];
         }
      else if (i<101)
         objlist[0][i] = noun_pics[cn];
      }


   return(2);

   }

