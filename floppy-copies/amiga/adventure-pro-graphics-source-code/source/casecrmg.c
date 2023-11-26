
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casecrmg(comcase,rk,l,win1,image,picx,picy)
int comcase,rk,l;
struct Window *win1;
struct Image *image;
int picx,picy;
{

   extern UBYTE *code;
   extern char *ext2list[MAXLIST];
   extern struct loc2 rm;
   int r,f,i;
   char *ng;

   f = 3;
   if (comcase == 0) {
      r = code[rk + 1] ;
      ng = ext2list[code[rk + 2]];
      if (r == l) {
         endrmsec(l,win1);

         i = 0;
         i = getgraphic(image,ng,1);
         if (i != 0) {
            exit(10);
            }
         DrawImage(win1->RPort,image,picx,picy);
         cleargrap(image);

         startrmsec(l,win1);
         }
      rm.pic[r] = code[rk + 2];
      }

   return(f);


}


