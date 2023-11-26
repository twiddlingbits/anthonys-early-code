#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

#define IN(x1,x2,w2)   ((x1)>=(x2) && (x1)<=(x2+w2))

/***************************/
/** do two boxes overlap? **/
/***************************/

/** return TRUE if box(x1,y1,h1,w2) overlaps box(x2,y2,h2,w2),
    else   FALSE is returned **/

overlap(x1,y1,h1,w1,x2,y2,h2,w2)

int  x1,y1,x2,y2;
int  h1,h2,w1,w2;

{

SHORT casey,casex;

/*printf("entered overlap:\nx=%d,y=%d,h=%d,w=%d\nx=%d,y=%d,h=%d,w=%d\n",
        x1,y1,h1,w1,x2,y2,h2,w2); */


casey=FALSE;
casex=FALSE;

if (IN(x1,x2,w2))    casex=TRUE;
if (IN(x1+w1,x2,w2)) casex=TRUE;
if (IN(x2,x1,w1))    casex=TRUE;
if (IN(x2+w2,x1,w1)) casex=TRUE;

if (IN(y1,y2,h2))    casey=TRUE;
if (IN(y1+h1,y2,h2)) casey=TRUE;
if (IN(y2,y1,h1))    casey=TRUE;
if (IN(y2+h2,y1,h1)) casey=TRUE;

return (casey&&casex);
}
