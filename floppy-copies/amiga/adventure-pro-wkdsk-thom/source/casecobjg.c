
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casecobjg(comcase,rk,l,win1,winp1)
int comcase,rk,l;
struct Window *win1;
struct winprint *winp1;
{

   extern int objnum[2],objlist[2][100];
   extern UBYTE *code;
   extern char *extlist[MAXLIST];
   extern int noun_pics[MAXLIST];
   extern struct Image *object_images[MAXLIST];
   struct Image *ti;
   int r,f,ll,i,ii,oc;
   int noun;

   f = 4;
   if (comcase == 0) {
      noun = code[rk + 1];
      if (code[rk + 2] == VARIABLE)
         r = code[rk + 3];
      else
         r = code[rk + 3] * -1;

      oc = noun_pics[noun];

      i = 0;
      while (objlist[1][i] != oc && i<objnum[1])
         i++;
      ii = 0;
      while (objlist[0][ii] != oc && ii<objnum[0])
         ii++;
      if (i < objnum[1] )
         eraseroomobjects(win1);
      if (ii < objnum[0] )
         caseinminus(noun,win1);

      ti = object_images[oc];
      if (oc > 0) {
         FreeMem(ti->ImageData,( ( (ti->Width+15) >>4) <<1)
                 *(ti->Height)*(ti->Depth));
         FreeMem(ti,sizeof(struct Image));
         object_images[oc] = 0;
         }

      if (r > 0) {
         ll = 1;
         while(ll < MAXLIST && object_images[ll] != 0)
            ll++;
         object_images[ll] = AllocMem(sizeof(struct Image),MEMF_CHIP);
            if (object_images[ll] == 0) {
            printf("no space for object images, dammit!");
            exit(10);
            }
         ti = object_images[ll];
         noun_pics[noun] = ll;
         ll = 0;
         ll = getgraphic(ti,extlist[r],0);
         if (ll != 0) {
            printf("can't get an  object[2], dammit!");
            exit(10);
            }
         }
      else
         noun_pics[noun] = r;

      if (i < objnum[1] )
         showroomobjects(l,win1);
      if (ii < objnum[0] )
         caseinplus(noun,win1);
      }

   return(f);


}


