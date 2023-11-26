#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"


drawwindow(Window)
struct Window *Window;
{

/* paint window black */

SetAPen(Window->RPort,BLACK);
SetBPen(Window->RPort,BLACK);
RectFill(Window->RPort,2,10,637,198);
RefreshGadgets(Window->FirstGadget,Window,NULL);

/* add a white line seperating status from samples */

drawline(Window,2,GPP-4,637,GPP-4,WHITE);
drawline(Window,2,GPP-5,637,GPP-5,WHITE);

/**** draw the arrows on the left of prop gadgets ****/

drawarrows(Window);

/* print gadget labels */

putxy(Window,4,GSP+2,"START",WHITE,BLACK);
putxy(Window,4,GEP+2,"END",WHITE,BLACK);
putxy(Window,4,GPP+2,"POS'N",WHITE,BLACK);

}
