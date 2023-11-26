#ifndef  GRAPHICS_REGIONS_H
#define GRAPHICS_REGIONS_H

#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif
/********************************************************************/
/*               Commodore-Amiga, Inc.                              */
/*               regions.h                                          */
/********************************************************************/

struct RegionRectangle
{
    struct RegionRectangle *Next,*Prev;
    struct Rectangle bounds;
};

struct Region
{
    struct Rectangle bounds;
    struct RegionRectangle *RegionRectangle;
};

#endif
