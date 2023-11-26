#include "exec/types.h"
#include "intuition/intuition.h"

static short do_msgs = TRUE;

/******************************/
/** put @ x,y pix pos        **/
/******************************/

void putxy(window,x,y,string,fg,bg)

struct Window *window;
int x,y,fg,bg;
char *string;

{
struct IntuiText text;

if (!do_msgs)
   return;

text.FrontPen=fg;
text.BackPen=bg;
text.DrawMode=JAM2;
text.LeftEdge=x;
text.TopEdge=y;
text.ITextFont=NULL;
text.IText=string;
text.NextText=NULL;


PrintIText (window->RPort,&text,0,0);

}

void msg_on()
{
do_msgs = TRUE;
}

void msg_off()
{
do_msgs = FALSE;
}

