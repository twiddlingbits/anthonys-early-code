#include "exec/types.h"
#include "intuition/intuition.h"

/******************************/
/** put @ x,y pix pos        **/
/******************************/

putxy(window,x,y,string,fg,bg)

struct Window *window;
int x,y,fg,bg;
char *string;

{

struct IntuiText text;

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

