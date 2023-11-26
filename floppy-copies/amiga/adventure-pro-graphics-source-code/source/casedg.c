
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casedg(comcase,rk,win1,image)
int comcase,rk;
struct Window *win1;
struct Image *image;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   extern char *extlist[MAXLIST];
   int i,r,f,x,y,x2,y2;
   char *file;

   f = 5;
   file = extlist[code[rk + 1]];
     if (code[rk + 2] == VARIABLE)
        x = varval[code[rk + 3]];
     else {
        x = code[rk + 3];
        x2 = code[rk + 4];
        x = x + (x2<<8);
        f++;
        }
     if (code[rk + f -1] == VARIABLE)
        x = varval[code[rk + f]];
     else {
        y = code[rk + f];
        y2 = code[rk + f + 1];
        y = y + (y2<<8);
        f++;
        }

   if (comcase == 0) {

     i = 0;
     i = getgraphic(image,file,1);
     if (i != 0) {
        return (-1);
        }
     DrawImage(win1->RPort,image,x,y);
     cleargrap(image);
     }

   return(f+1);


}

