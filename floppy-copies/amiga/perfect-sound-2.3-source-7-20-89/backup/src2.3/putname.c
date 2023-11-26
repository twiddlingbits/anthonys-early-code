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
if (location>=MAX_SAMPLES/2) {
   x=320;
   location=location-MAX_SAMPLES/2;
   }
   else x=3;

if (name[0]==0) {
   for (vid=x;vid<x+305;vid=vid+8)
      putxy(Window,vid,11+location*8," ",BLACK,BLACK);
   return(0);
   }

if (vid==REV_VID)
   putxy(Window,x,11+location*8,name,BLACK,WHITE);
else
   putxy(Window,x,11+location*8,name,WHITE,BLACK);
}

