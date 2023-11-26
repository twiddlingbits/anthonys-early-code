#include "exec/types.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

void setmenus(s,onoff)

struct Screen *s;
int onoff;

{
extern struct Menu Titles[];
struct Window *w;

for (w=s->FirstWindow; w ; w=w->NextWindow) {
   if (onoff==TRUE)
      SetMenuStrip(w,Titles);
   else
      ClearMenuStrip(w);
   }
}

