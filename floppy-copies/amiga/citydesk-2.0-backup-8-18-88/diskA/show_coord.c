#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

extern struct Window *coord_window;


/** mx   Mouse X Position **/
/** my   Mouse Y Position **/
/** dx   Offset to add to obj->x to get position **/
/** dy   Offset to add to obj->y to get position **/
/** aw   Width of object in pixels. Zero means use obj->w **/
/** ah   Height of object in pixels. Zero means use obj->h **/

void show_coord(obj, mx, my, dx, dy, aw, ah)

struct object *obj;
short dx,dy;
short aw,ah;
short mx, my;

{
extern struct pageinfo page[MAXPAGES];
char buf[20];
float yval,xval;
float h, w;
COORD xc,yc;
COORD wc,hc;
short pg;

if (coord_window==0)
   return;

pg = obj->pn;
xc = stocx(dx)/obj->mag;
yc = stocy(dy)/obj->mag;

if (aw==0)
   wc = obj->w;
else
   wc = stocx(aw)/obj->mag;

if (ah==0)
   hc = obj->h;
else
   hc = stocy(ah)/obj->mag;

w = ((float)(wc))/576.0;
h = ((float)(hc))/576.0;

xval= ((float)(obj->x+xc))/576.0;
yval= ((float)(obj->y+yc))/576.0;

sprintf(buf,"%6.2f %5.2f",xval,yval);

SetAPen(coord_window->RPort,BLACK);
SetDrMd(coord_window->RPort,JAM2);
Move(coord_window->RPort,104,31);
Text(coord_window->RPort,buf,strlen(buf));

sprintf(buf,"%6.2f %5.2f",h,w);

Move(coord_window->RPort,104,41);
Text(coord_window->RPort,buf,strlen(buf));

xval= ((float)mousetocx(&page[pg],mx))/576.0;
yval= ((float)mousetocy(&page[pg],my))/576.0;

sprintf(buf,"%6.2f %5.2f",xval,yval);

Move(coord_window->RPort,104,21);
Text(coord_window->RPort,buf,strlen(buf));
}

