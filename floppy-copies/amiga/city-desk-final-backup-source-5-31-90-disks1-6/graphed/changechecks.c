
#include "exec/types.h"
#include "intuition/intuition.h"


changechecks(gmode,rmode)
int gmode,rmode;
{
extern struct MenuItem *jackandbean1[6];

jackandbean1[0]->Flags &=(~CHECKED);
jackandbean1[1]->Flags &=(~CHECKED);
jackandbean1[2]->Flags &=(~CHECKED);
jackandbean1[3]->Flags &=(~CHECKED);
jackandbean1[4]->Flags &=(~CHECKED);
jackandbean1[5]->Flags &=(~CHECKED);

jackandbean1[gmode]->Flags |=CHECKED;

jackandbean1[rmode + 4]->Flags |=CHECKED;

}




