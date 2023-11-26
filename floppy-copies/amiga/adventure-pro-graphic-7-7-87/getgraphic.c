
#include <exec/types.h>
#include <intuition/intuition.h>
#include <lattice/stdio.h>
#include <lattice/ctype.h>
#include "adventure.h"


getgraphic(image,name,yes_no)
struct Image *image;
char *name;
int yes_no;
{
struct graphic graph1;
int i;

i = 0;
i = load_ilbm(name,&graph1);
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
image->PlanePick = 7;
image->PlaneOnOff = 0;
image->NextImage = NULL;
if (yes_no == 1)
   FreeMem(graph1.bitmap,( ( (graph1.w+15) >>4) <<1)*graph1.h*graph1.planenum);
return(0);

}   /* terminates () */


