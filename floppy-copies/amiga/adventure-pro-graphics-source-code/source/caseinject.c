

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


caseinject(i)
int i;
{
   extern char *typelist[MAXLIST],command[80];

   strcpy(&command[0],typelist[i]);

   }

