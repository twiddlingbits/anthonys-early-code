#include "exec/types.h"
#include "intuition/intuition.h"

extern SHORT std_width;
extern SHORT std_height;

get_screen_data()

{
struct Screen wbdata;

GetScreenData(&wbdata,sizeof(struct Screen),WBENCHSCREEN,0L);

std_width  = wbdata.Width;

if (wbdata.Height >=400 && wbdata.Height < 750)
   std_height = wbdata.Height/2;
else
   std_height = wbdata.Height;

return(wbdata.Height);
}

