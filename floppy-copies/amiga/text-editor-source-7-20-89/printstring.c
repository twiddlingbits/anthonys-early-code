

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"


main () {

static char string[] = "This is a test string with\nöömlöuts and øþh©r weirdnesses. ";
int i;

openlibs();

printf(&string[0]);

i = 0;
while (i < 59)
   printf("%c - %d\n",string[i],string[i++]);

}


