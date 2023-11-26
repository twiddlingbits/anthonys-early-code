#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

/*

mode=picktool(x,y)

 x,y = position in toolbox window
 mode = item picked, results in a new pointer shape
*/

#define COLSIZE 8

picktool(x,y)

SHORT x,y;

{
SHORT col;
static SHORT tools[]={GRABIT,SIZE,NEWPAGE,EDITFORMAT,ABCEDIT,CHEAPEDIT,EXPLODE,TRASHCAN,
                      NORMAL,CROP,MAGNIFY,EDITOBJ,REFLOWTEXT,LINE,DOBLOCK,DUPOBJECT};

col=-1;
if (x>0 && x<41) col=0;
if (x>41 && x<85) col=1;
if (col==-1) return(NO_TOOL_PICKED);
y=(y-10)/16;
if (y < 0 || y > COLSIZE)
   return(NO_TOOL_PICKED);
return ((int)tools[y+col*COLSIZE]);
}
