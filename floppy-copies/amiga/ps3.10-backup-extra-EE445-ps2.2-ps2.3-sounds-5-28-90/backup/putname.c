#include "exec/types.h"
#include "psound.h"

/*****************************************************/
/* PutName -- put a sample name in the correct place */
/*****************************************************/

PutName(Window,name,location,vid)

struct Window *Window;
char *name;
int location;
int vid;
{
int x;

x = 30;

if (name[0]==0) {
   for (vid=x;vid<x+140;vid=vid+8)
      putxy(Window,vid,STAT_Y+location*8," ",BLACK,BLACK);
   return(0);
   }

if (vid==REV_VID)
   putxy(Window,x,STAT_Y+location*8,name,BLACK,WHITE);
else
   putxy(Window,x,STAT_Y+location*8,name,WHITE,BLACK);
}

