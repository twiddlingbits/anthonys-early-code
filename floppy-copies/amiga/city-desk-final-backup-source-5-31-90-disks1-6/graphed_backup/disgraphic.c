
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"


disgraphic(rpt,g,scnw,scnh,scnd)
struct RastPort *rpt;
struct graphic *g;
int scnw,scnh,scnd;
{
   LONG workbyt,X1,Y1,OFFX,OFFY,X2,Y2,X3,Y3;
   int i;

   X1 = g->w;
   Y1 = g->h;
   X2 = g->usex;
   Y2 = g->usey;
   OFFX = g->usew;
   OFFY = g->useh;
   X3 = OFFX;
   if (X3 >scnw-10)
      X3 = scnw-10;
   Y3 = OFFY;
   if (Y3 >scnh-20)
      Y3 = scnh-20;
   workbyt = rpt->BitMap->BytesPerRow;

   i = 0;
   do {
   fit (g->planes[i]+ Y2 * X1 / 8,X2,OFFY,OFFX,RASSIZE(X1,1),
   rpt->BitMap->Planes[i]+workbyt *((scnh-Y3)/2),(scnw-X3)/2,
   Y3,X3,workbyt);
   } while (++i < g->numplanes && i < scnd);
   }



