
#include "exec/types.h"
#include "intuition/intuition.h"


changechecks(gmode,qdraw,brush)
int gmode,qdraw,brush;
{
extern struct MenuItem *jackandbean1[11];
extern struct MenuItem *jackandbean2[11];

jackandbean1[0]->Flags &=(~CHECKED);
jackandbean1[1]->Flags &=(~CHECKED);
jackandbean1[2]->Flags &=(~CHECKED);
jackandbean1[3]->Flags &=(~CHECKED);
jackandbean1[4]->Flags &=(~CHECKED);
jackandbean1[5]->Flags &=(~CHECKED);
jackandbean1[6]->Flags &=(~CHECKED);
jackandbean1[7]->Flags &=(~CHECKED);
jackandbean1[8]->Flags &=(~CHECKED);
jackandbean1[9]->Flags &=(~CHECKED);
jackandbean1[10]->Flags &=(~CHECKED);

jackandbean2[0]->Flags &=(~CHECKED);
jackandbean2[1]->Flags &=(~CHECKED);
jackandbean2[2]->Flags &=(~CHECKED);
jackandbean2[3]->Flags &=(~CHECKED);
jackandbean2[4]->Flags &=(~CHECKED);
jackandbean2[5]->Flags &=(~CHECKED);
jackandbean2[6]->Flags &=(~CHECKED);
jackandbean2[7]->Flags &=(~CHECKED);
jackandbean2[8]->Flags &=(~CHECKED);
jackandbean2[9]->Flags &=(~CHECKED);
jackandbean2[10]->Flags &=(~CHECKED);

brush = brush+6;
jackandbean1[brush]->Flags |=CHECKED;
jackandbean2[brush]->Flags |=CHECKED;

qdraw =qdraw+3;
if (qdraw == 3)
   qdraw = 5;
jackandbean1[qdraw]->Flags |=CHECKED;
jackandbean2[qdraw]->Flags |=CHECKED;

jackandbean1[gmode]->Flags |=CHECKED;
jackandbean2[gmode]->Flags |=CHECKED;


}




