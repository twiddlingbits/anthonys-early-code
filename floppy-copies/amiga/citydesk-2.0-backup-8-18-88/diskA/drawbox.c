#include "exec/types.h"
#include "graphics/rastport.h"
#include "stdio.h"

/*****************/
/**  draw a box **/
/*****************/

drawbox(RPort,x,y,h,w)

struct RastPort *RPort;
SHORT x,y,h,w;

{
h=max(h,2);                /** make sure we draw a box, not a line **/
w=max(w,2);
Move(RPort,x,y);
Draw(RPort,x+w-1,y);
Draw(RPort,x+w-1,y+h-1);
Draw(RPort,x,y+h-1);
Draw(RPort,x,y);
}


/*************************************/
/**  draw box and clip to screen size*/
/*************************************/

#define inx(val) min(std_width-1,max(val,0))
#define iny(val) min(screensize-1,max(val,0))


clipdrawbox(RPort,x,y,h,w)

struct RastPort *RPort;
SHORT x,y,h,w;

{
extern SHORT screensize;
extern SHORT std_width;

h=max(h,2);                /** make sure we draw a box, not a line **/
w=max(w,2);
Move(RPort,inx(x),iny(y));
Draw(RPort,min(x+w,std_width-1),max(y,0));
Draw(RPort,min(x+w,std_width-1),max(min(y+h,screensize-1),0));
Draw(RPort,max(x,0),min(y+h,screensize-1));
Draw(RPort,max(x,0),max(y,0));
}
