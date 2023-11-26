#include "exec/types.h"
#include "intuition/intuition.h"

zap_title(s)

struct Screen *s;

{
SetDrMd(&s->RastPort,JAM1);
SetAPen(&s->RastPort,0);
RectFill(&s->RastPort,0,0,s->Width,25);
}

