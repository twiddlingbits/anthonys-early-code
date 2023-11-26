#include "exec/types.h"
#include "graphics/gfx.h"
#include "desk.h"

void free_graphic_data(g)

struct graphic *g;

{
short i;

for (i=0; i<g->numplanes; i++) 
   FreeMem(g->planes[i],RASSIZE(g->w,g->h));

/* RemFree(g->planes[0]); */
}

