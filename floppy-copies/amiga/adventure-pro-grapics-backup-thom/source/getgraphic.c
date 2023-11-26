
#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
#include "adventure.h"


getgraphic(image,name,cmap)
struct Image *image;
char *name;
WORD *cmap;
{
struct graphic graph1;
int i;
WORD fakemap[32*3];
i = 0;
if (cmap == 0)
   i = load_ilbm(name,&graph1,fakemap);
else
   i = load_ilbm(name,&graph1,cmap);
if (i != 0) {
   printf("bad day!");
   return(-1);
   }

image->LeftEdge = 0;
image->TopEdge = 0;
image->Width = graph1.w;
image->Height = graph1.h;
image->Depth = graph1.planenum;
image->ImageData = graph1.bitmap;
image->PlanePick = 1;
if (image->Depth == 2)
   image->PlanePick = 3;
if (image->Depth == 3)
   image->PlanePick = 7;
if (image->Depth == 4)
   image->PlanePick = 15;
if (image->Depth == 5)
   image->PlanePick = 31;
image->PlaneOnOff = 0;
image->NextImage = NULL;

return(0);
}   /* terminates () */


