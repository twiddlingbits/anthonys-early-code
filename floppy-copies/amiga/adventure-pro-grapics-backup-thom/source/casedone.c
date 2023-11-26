
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casedone(comcase)
int *comcase;
{
   if (*comcase==0)
     *comcase = -1;
   return(1);

}

casesdone(comcase,nrflag)
int *comcase,*nrflag;
{
   if (*comcase==0) {
      *comcase = -1;
      *nrflag = 1;
      }
   return(1);

}

casendone(comcase,nrflag)
int *comcase,*nrflag;
{
   if (*comcase==0) {
      *comcase = -1;
      *nrflag = -10;
      }
   return(1);

}

