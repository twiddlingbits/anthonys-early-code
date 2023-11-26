#include "exec/types.h"
#include "intuition/intuition.h"


showobjstat(w,num,max,pg)

struct Window *w;
SHORT num,max,pg;

{
char str[40];

sprintf(str,"Obj %d of %d [pg %d]  ",num,max,pg);
Move(w->RPort,12,144);
Text(w->RPort,str,strlen(str));
}

clearobjstat(w)

struct Window *w;

{
char *str;

str = "                    ";
Move(w->RPort,12,144);
Text(w->RPort,str,strlen(str));
}

