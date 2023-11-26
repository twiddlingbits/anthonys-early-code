/****************/
/*** INVERT.C ***/
/****************/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "graphics/gfx.h"

/* takes a graphic structure and inverts the bitimage **/


invert(g)
struct graphic *g;

{

register short i;
register short wl;
WORD *wp;
unsigned short j;

wl=RASSIZE(g->w,g->h);
wl=wl>>1;     /** convert bytecount to word count **/

for (j=0; j<g->numplanes; j++) {
   wp=(WORD *)g->planes[j];

   for (i=0;i<wl;i++)
      wp[i]=~wp[i];    /* flip them bits */
   }



}

