
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casecrms(comcase,rk,l,win1)
int comcase,rk,l;
struct Window *win1;
{

   extern struct sequence *sequences[MAXLIST];
   extern UBYTE *code;
   struct sequence *a;
   int num,r,f;
   int ns,os;

   if (comcase == 0) {
      num = code[rk + 1] - 1 ;
      ns = code[rk + 2] ;
      a = sequences[num];
      if (l == a->room)
         endsec(num+1,win1);
      a->room = ns;
      if (l == a->room) {
         startsec(win1,num+1,0,0);
         activate(num+1);
         }
      }

   return(3);


}


