
#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
#include "adventure.h"


cleargrap(image)
struct Image *image;
{
FreeMem(image->ImageData,( ( (image->Width+15) >>4) <<1) *
        image->Height*image->Depth);

return(0);

}   /* terminates () */


