
#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphics/rastport.h"
#include "graphed.h"


cleanup ( rpt,box,boxflag,twolines,lineflag)
struct RastPort *rpt;
struct Border *box;
int boxflag;
struct Border *twolines;
int lineflag;
{

if (boxflag = YES)
   DrawBorder(rpt,box,0,0);
if (lineflag = YES)
   DrawBorder(rpt,twolines,0,0);


}

