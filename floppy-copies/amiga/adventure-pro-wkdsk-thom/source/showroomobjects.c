
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


showroomobjects(room,wd)
int room;
struct Window *wd;
{
   extern int objnum[2],objx[2][10],objy[2][10],objlist[2][100],
           noun_pics[MAXLIST];
   extern struct Image *object_images[MAXLIST];
   extern struct loc2 rm;
   int i,ii,a,j;

   i = 0;
   ii = 1;
   while (ii < MAXLIST && i < objnum[1]) {
      if (rm.item[room][ii] != 0) {
         a = noun_pics[rm.item[room][ii]];
         if (a > 0) {
            DrawImage(wd->RPort,object_images[a],
            objx[1][i],objy[1][i]);
            objlist[1][i] = a;
            i++;
            }
         if (a < 0) {
            j = (a * -1);
            startsec(wd,j,objx[1][i],objy[1][i]);
            activate(j);
            objlist[1][i] = a;
            i++;
            }
         }
         ii++;
      }
   }


